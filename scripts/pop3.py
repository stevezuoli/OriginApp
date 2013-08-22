#!/usr/bin/python
#coding=utf-8
'''
# 20120531
#=============================================================================
#     FileName:         pop_imap_recv_mail.py
#     Desc:                 Fetch mail by POP and IMAP
#     Author:           forrest
#     Email:            hongsun924@gmail.com
#     HomePage:         NULL
#     Version:          0.0.1
#     LastChange:       2011-08-09 17:51:49
#     History:          
#=============================================================================
'''

import os, sys, re, string, getpass, email, poplib, imaplib
import logging, traceback


# 如果get_charsets()查找不到编码信息, 则采用下面方式:
def check_code(msg):
    #print "check_code(msg) start:"
    for code in ('gb2312', 'gb18030', 'gbk', 'big5', 'utf-8', 'utf16', 'utf32', 'jp', 'euc_kr'):
        try:
            #print "code = ", code
            return unicode(msg, code, 'ignore' )
        except:
            pass
    return msg

# 检查编码信息
def check_sub(sub, code, msg):
    #print "check_sub(msg) start :"
    if code != None:
        logging.info("code: %s" % code)
        f = open('filename.txt', 'wb')
        f.write(sub)
        f.close()
        cmd= 'iconv  -f '+ code +' -t UTF-8 filename.txt  >  filename2.txt '
        os.system(cmd)
        f = open('filename2.txt', 'r')        
        filename = f.read()
        f.close()
        subject = filename
        logging.info("return in check_sub #1")
        return subject
    elif msg.get_charsets()[0] !=None:
        subject = unicode(sub, msg.get_charsets()[0], 'ignore')
        logging.info("code: %s" % msg.get_charsets()[0])
        logging.info("return in check_sub #2")
        return subject


# 解析邮件头以及附件名称
def mail_parser(msg, obj):
    logging.info("mail_parser(msg) start:")
    obj = ' '.join(obj.split())
    logging.info("obj: %s" % obj)
    obj = obj.replace("?=", "?= ")
    reobj = re.compile(r'q\?= ', re.I)
    obj = reobj.sub(r'Q?=', obj)
    try:
        decode_obj = email.Header.decode_header(obj)
        obj_list = []
        for decode_obj_item in decode_obj:
            #obj_sub, obj_code = decode_obj_item
            obj_sub, obj_code = decode_obj_item
            test_obj = check_sub(obj_sub, obj_code, msg)
            obj_list.append(test_obj)

        logging.info("return in mail_parser #1")
        return ''.join(obj_list)
    except:
        exc_type, exc_value, exc_traceback = sys.exc_info()
        #print dir(exc_traceback)
        logging.info("%s" % traceback.format_exc(exc_traceback))
        logging.info("return in mail_parser #2")
        return obj


# 解析text/plain信息, 如果 part.get_charsets()不能获取编码信息, 则使用check_content_code来检测编码
def text_plain_msg(part):
    text_plain = part.get_payload(decode=True)
    charset = part.get_charsets()[0]
    if charset == "x-gbk":
        charset = "gbk"

    if len(text_plain):
        if charset != None:
            try:
                return unicode(text_plain, charset, 'ignore')
            except:
                pass


# 解析text/html信息, 如果 part.get_charsets()不能获取编码信息, 则使用check_content_code来检测编码
def text_html_msg(part):
    text_html = part.get_payload(decode=True)
    charset = part.get_charsets()[0]
    if charset == "x-gbk":
        charset = "gbk"

    if len(text_html):
        if charset != None:
            try:
                return unicode(text_html, charset, 'ignore')
            except:
                pass


# 解析附件, 分析两种情况:
    #1. Content-ID: 如果使用了Content-ID内嵌资源, 则以该ID命名,获取此资源
    #2. Content-Disposition: 如果使用了Content-Disposition,则分为两种情形, inline资源 和 attachment资源:
        #先尝试使用正则分隔解析出文件名, 如果不能, 则使用part.get_filename()来获取文件名
def attach_msg(part, mail_num, file_path):

    content_dis = part.get_all('Content-Disposition')
    content_id = part.get_all('Content-ID')

    #print "attach_msg part =",part;
    file_name = str(mail_num) + "_" + "Unknown_file_name"

    if content_id != None:
        file_name = "cid_" + str(mail_num) + "_" + content_id[0] 

    if content_dis != None:
        file_data = re.findall(r'(?<=")[\s\S]*?(?=")', content_dis[0])
        if file_data:
            file_name = file_data[0].strip()
        else:
            file_name =  part.get_filename()

    pre_filename = "none"
    #print "file_name to write: #1", file_name		

    if file_name:
        file_name2 = mail_parser(part, file_name)
        #print "file_name2 to write:", file_name2		
        pre_filename = file_path + file_name2
        logging.info("  -Start to download attachment file %s." % pre_filename)
        data = part.get_payload(decode=True)
        pre_filename = re.sub('[\:*?"<>|]','',pre_filename)
        logging.info("filename to write: %s" % pre_filename)
        if (isinstance(pre_filename, type(u'a'))):
            pre_filename = pre_filename.encode("utf-8")
        existed = os.path.exists(pre_filename); 
        #basename = os.path.basename(pre_filename);
        basename = os.path.splitext(pre_filename)[0];
        extname = os.path.splitext(pre_filename)[1];
        #print "file ext =", extname;
        i = 0;
        while existed:
            pre_filename = os.path.join("",basename+"_" + str(i) + extname);
            existed = os.path.exists( pre_filename); 
            i += 1;
        
        try:
            print "#############################file name to write: ", pre_filename;
            logging.info("filename to write final: %s" % pre_filename)
            f = open(pre_filename, 'wb')
            f.write(data)
            f.close()
            logging.info("  -End to download attachment file.")
        except:
            exc_type, exc_value, exc_traceback = sys.exc_info()
            #print dir(exc_traceback)
            logging.info("%s" % traceback.format_exc(exc_traceback))

    return pre_filename

# 处理邮件头信息, 包括From, To, Subject, Date
def header_msg(mail_num, msg):
    from_addr = ''
    to_addr = ''
    subject = ''
    date = ''

    if msg['From'] != None:
        from_addr = mail_parser(msg, msg['From'])

    if msg['To'] != None:
        to_addr = mail_parser(msg, msg['To'])

    if msg['Subject'] != None:
        subject = mail_parser(msg, msg['Subject'])

    if msg['Date'] != None:
        date = msg['Date']

    return from_addr, to_addr, subject, date


# 处理邮件正文内容信息, 先判断是否是一个multipart, 如果是, 则for出每一个part的内容(plain, html, attach)
def content_msg(mail_num, msg, file_path):
    #print "content_msg", msg
    txt_plain = []
    txt_html = []
    atta_data = []

    if msg.is_multipart():
        #print "  -Multipart Mail."
        count = 0;
        for part in msg.walk():
            #print "count = ", count;
            count += 1;
            if not part.is_multipart():
                #print "@@@@@@@@@@sub-part is not multi-part"
                if part.get_filename():
                    attach_msg(part, mail_num, file_path);
    else:
        #print "  -Not Multipart Mail."
        if msg.get_filename():
            attach_msg(msg, mail_num, file_path)

    return txt_plain, txt_html, atta_data
    
# pop.retr()提取邮件
def pop_retr_mail(num, uid, file_path):
    hdr, message, octet = pop.retr(num)
    #print "get mail content pop_retr_mail pop.retr(num)"
    msg = email.message_from_string(string.join(message, '\n'))
    mail_text, mail_html, mail_atta = content_msg(num, msg, file_path)
    print "done in pop_retr_mail"

# imap.uid('fetch', uid, '(RFC822)')提取邮件
def imap_fetch_mail(db, db_name, uid, file_path):
    #typ, msg_data = imap.fetch('403' , '(BODY.PEEK[HEADER])')
    #typ, msg_data = imap.fetch('403' , '(BODY.PEEK[TEXT])')

    result, data = imap.uid('fetch', uid, '(RFC822)')
    raw_email = data[0][1]

    msg = email.message_from_string(raw_email)

    logging.info("  -Start to fetch mail header message.")
    mail_from, mail_to, mail_subject, mail_date = header_msg(uid, msg)
    logging.info("  -End to fetch Header message!")

    logging.info("  -Start to fetch mail content message.")
    mail_text, mail_html, mail_atta = content_msg(uid, msg, file_path)
    logging.info("  -End to fetch Content message!")

    if len(mail_text):
        mail_text = "\n".join(mail_text)

    if len(mail_html):
        mail_html = "\n".join(mail_html)


    # tb_mail_atta表中添加邮件信息
    if len(mail_atta):
        logging.info("  -Start to insert mail attachment to mysql tb_mail_atta.")
        for file_name in mail_atta:
            logging.info("  -Insert Pass!")

# 主程序开始
if __name__ == "__main__":

    logging.basicConfig(filename="pop3.log", format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    rootLog = logging.getLogger()
    rootLog.setLevel(logging.DEBUG)

    if len(sys.argv) < 6:  
        logging.debug("please input the server port username password filepath, return \n")
        sys.exit(11)

        # config Mail Server
    logging.debug("======== Please Config mail server: server, port, username ,password ,filepath ========")
    server = sys.argv[1]
    port = sys.argv[2]
    pop_user = sys.argv[3]
    pop_pass = sys.argv[4]
    file_path = sys.argv[5]
    logging.debug("server: %s, port: %s, pop_user: %s, pop_pass: %s, file_path: %s" % (server, port, pop_user, pop_pass, file_path))

    if not os.path.exists(file_path):
        os.makedirs(file_path)
    
    # if it's 110, using POP3 to get the mail attachment
    if port == "110":
        print "RecvMail Begin"
        try:
            logging.info("begin pop3")
            pop = poplib.POP3(server, port, timeout = 15)
            pop.set_debuglevel(1)
            pop.user(pop_user)
            pop.pass_(pop_pass)
        except:
            exc_type, exc_value, exc_traceback = sys.exc_info()
            #print dir(exc_traceback)
            print "MailDisConnected"
            logging.info("%s" % traceback.format_exc(exc_traceback))
            #print "please check the mail server and port, also check the username and password again! "
            sys.exit(1)

        # pop.uidl ????uid???        
        print "MailConnected"
        result, data, oct = pop.uidl()
        for item in data:
            try:
                uid = item.split()
                mail_num = uid[0]
                mail_uid = uid[1]
                logging.info("refore pop_retr_mail mail %s %s" % (mail_num, mail_uid))
                print "RecvMail:%s/%d" % (mail_num, len(data))
                #if not db.select_uid(db_name, mail_uid):
                pop_retr_mail(mail_num, mail_uid, file_path)
                logging.info("after pop_retr_mail mail %s %s" % (mail_num, mail_uid))
                #pop.dele(mail_num)
                logging.info("after delete mail")
                logging.info("End to fetch mail, num: %s, uid: %s" %(mail_num, mail_uid))
            except:
                exc_type, exc_value, exc_traceback = sys.exc_info()
                #print dir(exc_traceback)
                logging.error("%s" % traceback.format_exc(exc_traceback))
        print "RecvMail End"
        print "Mail fetch finished!"
        pop.quit()

    # IMAP get the mail content
    # port = 143
    elif port == "143":
        try:
            imap = imaplib.IMAP4_SSL(server, port)
            imap.debug = 4
            imap.login(imap_user, imap_pass)
            logging.info("Mail server: %s:%s init pass." %(server,port))
        except:
            print "please check the mail server and port, also check the username and password again! "
            logging.info("Mail server: %s:%s init failed." %(server,port))
            sys.exit(1)

        #print imap.select('inbox')
        inbox_result, inbox_data = imap.search(None, "ALL")

        uid_result, uid_data = imap.uid('search' , None, "ALL")
        uid_list = uid_data[0].split()

        for uid in uid_list :
            if not db.select_uid(db_name, uid):
                logging.info("Start to fetch mail, uid: %s" %uid)
                imap_fetch_mail(db, db_name, uid, file_path)
                logging.info("End to fetch mail, passed, uid: %s" %uid)

        #print "Mail fetch finished!"
        imap.quit()
    else:
        print "Don't know mail server."

