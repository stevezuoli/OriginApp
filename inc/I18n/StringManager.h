#ifndef _LRCMANAGE_H_
#define _LRCMANAGE_H_

#include "Utility/CString.h"
#include "DkSPtr.h"
#include <string>
using namespace std;

//using namespace DkFormat;

#define STRING_LIST(MACRO_NAME)\
    MACRO_NAME(APPLICATION_NAME,                        "多看阅读")\
    MACRO_NAME(UNKNOWN_ERROR,                        "未知错误")\
    MACRO_NAME(SURE_DELETE_BOOK,                        "确认删除此书?")\
    MACRO_NAME(SURE_DELETE_DIRECTORY,                   "确认删除此文件夹?")\
    MACRO_NAME(SURE_DELETE_PUSHEDMESSAGE,               "确认删除此条消息?")\
    MACRO_NAME(SURE_UPDATE_ALL,                         "确认更新所有书籍?")\
    MACRO_NAME(SURE_DOWNLOAD_ALL,                       "确认下载所有书籍?")\
    MACRO_NAME(SURE_STOP_UPDATE_ALL,                    "系统不会保存正在更新的书籍，确认停止?")\
    MACRO_NAME(SURE_STOP_DOWNLOAD_ALL,                  "系统不会保存正在下载的书籍，确认停止?")\
    MACRO_NAME(BOOK_NAME,                               "书名:")\
    MACRO_NAME(BOOK_AUTHOR,                             "作者:")\
    MACRO_NAME(BOOK_EDITORS,                            "编者:")\
    MACRO_NAME(BOOK_SIZE,                               "大小:")\
    MACRO_NAME(DELETE_BOOK,                             "删除此书")\
    MACRO_NAME(READ_BOOK,                               "阅读此书")\
    MACRO_NAME(SHARE_BOOK_TO_SINAWEIBO,                 "分享至新浪微博")\
    MACRO_NAME(SERIALIZEDBOOK,                          "连载书更新")\
    MACRO_NAME(FAILEDTOCHECKNEWVERSION,                 "检测新版本失败，网络可能有问题，请稍后重试！")\
    MACRO_NAME(ADD_BOOKMARK,                            "加书签")\
    MACRO_NAME(SEARCH,                                  "查找")\
    MACRO_NAME(DOWNLOAD_ALL,                            "全部下载")\
    MACRO_NAME(BOOK_CONTENT_SEARCH,                     "搜索")\
    MACRO_NAME(BOOK_SEARCH_KEYWORD,                    "全文查找")\
    MACRO_NAME(NO_TEXT_SEARCH_RESULT,                   "无法找到相对应内容，请重新输入想要查找项")\
    MACRO_NAME(NO_BOOK_SEARCH_RESULT,                   "没有找到您要查询的内容，请重新输入")\
    MACRO_NAME(NO_KEYWORD_FOR_TEXT_SEARCHING,           "您没有输入任何内容，无法查找")\
    MACRO_NAME(TOC_CONTENT_ANALYZING,                   "开始解析目录,请耐心等待")\
    MACRO_NAME(JUMP,                                    "跳转")\
    MACRO_NAME( AUTO_PAGE_DOWN_EVERY,                   "每隔")\
    MACRO_NAME( DO_AUTO_PAGE_DOWN,                      "秒自动翻页。(%d～%d秒)")\
    MACRO_NAME(SYSTEM_SETTING,                          "系统设置")\
    MACRO_NAME(DELETE_BOOKMARK,                         "去书签")\
    MACRO_NAME(DOWNLOAD_FINISH,                         "下载完成")\
    MACRO_NAME(DOWNLOADING,                             "正在下载")\
    MACRO_NAME(WAITING,                                 "正在等待")\
    MACRO_NAME(PAUSE,                                   "暂停")\
    MACRO_NAME(NOT_SUPPORT,                             "暂不支持")\
    MACRO_NAME(ACTION_OK,                               "确认")\
    MACRO_NAME(CANNOT_OPEN_BOOK,                        "无法打开此书")\
    MACRO_NAME(INPUT_PASSWORD,                          "请输入密码：")\
    MACRO_NAME(SET_FONT_SIZE,                           "字号设置")\
    MACRO_NAME(DUOKAN,                                  "多看")\
	MACRO_NAME(LAYOUT_OPTIONS,                          "版式选择")\
	MACRO_NAME(READERLAYOUT,                            "界面布局")\
	MACRO_NAME(READERLAYOUT_FULLSCREEN,                 "        全屏        ")\
    MACRO_NAME(READERLAYOUT_SHOWREADERMESSAGE,          "显示书籍信息")\
	MACRO_NAME(LAYOUT,                                  "阅读版式")\
    MACRO_NAME(LAYOUT_ELEGANT,                          "优雅")\
    MACRO_NAME(LAYOUT_STANDARD,                         "标准")\
    MACRO_NAME(LAYOUT_COMPACT,                          "紧凑")\
    MACRO_NAME(LAYOUT_ORIGINAL,                         "原版")\
    MACRO_NAME(BOOK_INFO,                               "书籍信息")\
    MACRO_NAME(SEARCHING,                               "搜索")\
    MACRO_NAME(DOWNLOAD_BOOK,                           "下载此书")\
    MACRO_NAME(NEXT_PAGE,                               "下一页")\
    MACRO_NAME(BOOK_NAME_KEY_WORD,                      "书城搜索")\
    MACRO_NAME(OPEN_WIFI_NET,                           "打开WiFi网络")\
    MACRO_NAME(DOWNLOAD_BOOK_MANAGER,                   "下载管理")\
    MACRO_NAME(MANAGE_WIFI,                             "管理WiFi网络")\
    MACRO_NAME(CLOSE_WIFI_NET,                          "关闭WiFi网络")\
    MACRO_NAME(WIFI_IDENTIFY,                           "身份：")\
    MACRO_NAME(SEARCH_RESULT,                           "搜索结果")\
    MACRO_NAME(TOTAL_OF_12,                             "共12条")\
    MACRO_NAME(HALF_PAGE,                               "1/2页")\
    MACRO_NAME(BOOKS_LEFT_IN_QUEUE,                     "还有%d本书在等待下载...")\
    MACRO_NAME(DOWNLOAD_NOT_ENOUGH_SPACE,               "停止下载，请检查存储空间和网络连接")\
    MACRO_NAME(DOWNLOAD_DRM_ERROR,                      "获取版权数据失败！")\
    MACRO_NAME(FAILED_CONN_NET_ERROR,                   "连接失败，网络错误!")\
    MACRO_NAME(BOOK_TOTAL,                              "共")\
    MACRO_NAME(BOOK_BEN,                                "本")\
    MACRO_NAME(BOOK_PAGE,                               "页")\
    MACRO_NAME(BOOK_00_PAGE,                            "0/0")\
    MACRO_NAME(BOOK_DELETE,                             "删除")\
    MACRO_NAME(SURE_DELETE,                             "确认删除?")\
    MACRO_NAME(COMMENTS,                                "批注")\
    MACRO_NAME(BOOKMARK,                                "书签")\
    MACRO_NAME(SHUZHAI,                                 "书摘")\
    MACRO_NAME(DUOKANSHUZHAI,                           "#多看书摘# ")\
    MACRO_NAME(WHOLE,                                   "全部")\
    MACRO_NAME(BOOK_TIAO,                               "条")\
    MACRO_NAME(START,                                   "开始")\
    MACRO_NAME(SORT,                                    "排序")\
    MACRO_NAME(RECENT_READING,                          "最近阅读")\
    MACRO_NAME(RECENT_ADDED,                            "最近加入")\
    MACRO_NAME(BY_BOOK_TITLE,                           "按书名")\
    MACRO_NAME(BY_BOOK_DIRECTORY,                       "按目录")\
    MACRO_NAME(FIRST_TITLE_KEY_WORD,                    "书名首字母、书名关键字、标签")\
    MACRO_NAME(ACTION_CANCEL,                           "取消")\
    MACRO_NAME(ACTION_YES,                              "是")\
    MACRO_NAME(ACTION_NO,                               "否")\
    MACRO_NAME(ACTION_SUBMIT,                           "提交")\
    MACRO_NAME(MEMORY_REMAIN,                           "剩余")\
    MACRO_NAME(SURE_RESTART,                            "确定重启？")\
    MACRO_NAME(SURE_SWITCH_KINDLE_SYSTEM,               "确定切换到KINDLE系统？")\
    MACRO_NAME(SYSTEM_REBOOTING_PLEASE_WAIT,            "系统正在重启，请稍候...")\
    MACRO_NAME(LOW_POWER_NO_DOWNLOAD,                   "当前电量低，无法下载，请及时充电")\
    MACRO_NAME(ALREADY_LATEST_VERSION,                  "已经是最新版本")\
    MACRO_NAME(SURE_UNINSTALL,                          "确定卸载？")\
    MACRO_NAME(DUOKAN_READER_COPYRIGHT,                 "多看阅读器软件版权归北京多看科技有限公司所有。多看阅读器软件及其所包含所有组件仅提供给个人出于非商业目的的使用，所有将其用于任何商业目的的行为必须事先获得多看科技甚至各组件开发者的许可。\r\n感谢文泉驿微米黑")\
    MACRO_NAME(REFRESH_NETWORK,                         "刷新网络")\
    MACRO_NAME(ADD_WIFI,								"添加网络...")\
    MACRO_NAME(ADD_WIFI_MANUALLY,						"添加WiFi网络")\
    MACRO_NAME(WIFI_TITLE,								"无线和网络")\
    MACRO_NAME(WIFI_STATUS,								"WiFi网络状态")\
    MACRO_NAME(WIFI_CHOICE,								"选取附近的网络...")\
    MACRO_NAME(WIFI_IGNORE,								"忽略此网络")\
	MACRO_NAME(CONNECT_ASSIGN_IP,                       "正在分配IP...")\
    MACRO_NAME(CONNECT_ASSIGN_IP_FAIL,                  "IP分配失败")\
	MACRO_NAME(CONNECT_ASSIGN_IP_LOST,                  "IP未分配，信号弱或者盲区")\
	MACRO_NAME(CONNECT_ASSIGN_IP_NO_DNS,                "IP未分配，DNS配置错误")\
	MACRO_NAME(CONNECT_ASSIGN_IP_NO_GW,                 "IP未分配，网关配置错误")\
	MACRO_NAME(CONNECT_ASSIGN_IP_DHCP_OFFER,            "IP未分配，DHCP Offer错误")\
    MACRO_NAME(CONNECT_AUTO,                            "自动连接中")\
    MACRO_NAME(CONNECT_WORKING,                         "正在连接")\
    MACRO_NAME(CONNECT_ALREADY,                         "已连接")\
    MACRO_NAME(CONNECT_NULL,							"WiFi未连接")\
    MACRO_NAME(CONNECT_POWEROFF,						"WiFi未启用")\
    MACRO_NAME(CONNECT_FAILED,                          "连接失败")\
    MACRO_NAME(CONNECT_TO,								"连接到")\
    MACRO_NAME(CONNECT_RETRY,                           "空密码连接失败，再次尝试?")\
    MACRO_NAME(CONNECT_FAILED_TRY_AGAIN,                "连接失败，再次尝试")\
    MACRO_NAME(WIFI_DELETE_CONFIRM,                     "确认删除此手工配置的WiFi吗?") \
    MACRO_NAME(WIFI_DELETE_PWD,                         "确认清除此WiFI密码?") \
    MACRO_NAME(WIFINOTDELETABLE,                        "该WiFi非手工配置，不能删除，请尝试刷新") \
    MACRO_NAME(CONNECTEDWIFINOTDELETABLE,               "不能删除已连接或正在连接的WiFi") \
    MACRO_NAME(USABLE_NETWORKS,                         "个可用网络")\
    MACRO_NAME(REFRESH_WIFI_NET,                        "刷新WiFi...")\
	MACRO_NAME(REFRESH_WIFI_FAIL,                       "刷新WiFi失败")\
    MACRO_NAME(REFRESH_WIFI_NOMEM,                      "刷新WiFi失败,内存不足")\
    MACRO_NAME(INPUT_PASSWORD_NO_COLON,                 "请输入密码")\
    MACRO_NAME(INPUT_NUMBER_SYMBOL_SYM_PRESS,           "输入数字及符号，请按Sym键")\
    MACRO_NAME(NAME_OF_NETWORK,                         "网络名称：")\
    MACRO_NAME(PASSWORD_WITH_COLON,                     "密码：")\
    MACRO_NAME(CONTINUE_READ_BOOK,                      "继续阅读")\
    MACRO_NAME(DOWNLOAD_FAILED,                         "下载失败")\
    MACRO_NAME(BOOKSTORE_LOADING_INFO,                  "载入中...")\
    MACRO_NAME(OPEN_WORKING_IN,                         "正在打开...")\
    MACRO_NAME(CLOSE_WORKING_IN,                        "正在关闭...")\
    MACRO_NAME(CONNECT_WORKING_IN,                      "正在连接...")\
    MACRO_NAME(REFRESH_WORKING_IN,                      "正在刷新...")\
    MACRO_NAME(BOOK_READING,                            "阅读")\
    MACRO_NAME(NET_FUNCTION_NEED_NET,                   "此功能需联网,是否现在联网?")\
    MACRO_NAME(RESTART_SWITCH_SYS,                      "系统重启和切换系统")\
    MACRO_NAME(SYS_RESTART,                             "系统重启")\
    MACRO_NAME(RESTART,                                 "重启")\
    MACRO_NAME(SWI_KINDLE,                              "切换到KINDLE系统")\
    MACRO_NAME(SWITCH_SWITCH,                           "切换")\
    MACRO_NAME(READ_SETTING,                            "阅读设置")\
    MACRO_NAME(LAYOUT_SETTING,                            "版式设置")\
    MACRO_NAME(ALL_REFRESH_SPAN,                        "全刷间隔")\
    MACRO_NAME(BOOK_FONT_SIZE,                          "字号")\
    MACRO_NAME(BOOK_FONT_SIZE_POSTFIX,      "号")\
    MACRO_NAME(FONT_ONE_SIZE,                           "1号")\
    MACRO_NAME(FONT_TWO_SIZE,                           "2号")\
    MACRO_NAME(FONT_THREE_SIZE,                         "3号")\
    MACRO_NAME(FONT_FOUR_SIZE,                          "4号")\
    MACRO_NAME(FONT_FIVE_SIZE,                          "5号")\
    MACRO_NAME(BOOK_LINE_GAP,                           "行间距")\
    MACRO_NAME(BOOK_PARA_SPACING,                       "段间距")\
    MACRO_NAME(BOOK_INDENT,                             "缩进")\
    MACRO_NAME(INPUT_METHOD,                            "输入法")\
    MACRO_NAME(LANGUAGE_INPUT,                          "语言和输入法")\
    MACRO_NAME(JIANTI_PINYIN,                           "拼音简体")\
    MACRO_NAME(FANTI_PINYIN,                            "拼音繁体")\
    MACRO_NAME(PINYIN,                                  "拼音")\
    MACRO_NAME(WUBI,                                    "五笔简体")\
    MACRO_NAME(FANTI,                                   "繁体")\
    MACRO_NAME(CANGJIE,                                 "倉頡")\
    MACRO_NAME(LATIN,                                   "拉丁")\
    MACRO_NAME(DATE_AND_TIME,                           "日期和时间")\
    MACRO_NAME(SET_DATE,                                "设置日期")\
    MACRO_NAME(SET_TIME,                                "设置时间")\
    MACRO_NAME(TIME_FORMAT,                             "时间格式")\
    MACRO_NAME(TWELVE_HOUR,                             "12小时制")\
    MACRO_NAME(TWENTYFOUR_HOUR,                         "24小时制")\
    MACRO_NAME(TIME_ZONE,                               "设置时区")\
    MACRO_NAME(TIME_ZONE_EAST,                          "东半球")\
    MACRO_NAME(TIME_ZONE_WEST,                          "西半球")\
    MACRO_NAME(ABOUT_DUOKAN,                            "关于多看")\
    MACRO_NAME(ABOUT_KINDLE,                            "关于Kindle")\
    MACRO_NAME(SYSTEM_UPDATE,                           "在线升级")\
    MACRO_NAME(CHECK_UPDATE,                            "检查更新")\
    MACRO_NAME(SYSTEM_VERSION,                          "系统版本")\
    MACRO_NAME(SYSTEM_UNINSTALL,                        "系统卸载")\
    MACRO_NAME(SYS_UNINSTALL,                           "卸载")\
    MACRO_NAME(STORAGE_CAPACITY,                        "存储容量")\
    MACRO_NAME(COPYRIGHT,                               "版权声明")\
    MACRO_NAME(SYS_CHECK,                               "查看")\
    MACRO_NAME(ADD,                                     "添加")\
    MACRO_NAME(PUT_SPACE_BETWEEN_LABEL,                 "标签间请用空格分隔")\
    MACRO_NAME(LANGUAGE_OPTION,                         "语言")\
    MACRO_NAME(CHINESE,                                 "中文")\
    MACRO_NAME(ENGLISH,                                 "英文")\
    MACRO_NAME(CUSTOM_CNS_FONT,                         "中文字体")\
    MACRO_NAME(CUSTOM_ENG_FONT,                         "英文字体")\
    MACRO_NAME(CUSTOM_FONT_SAME_AS_CNS,                 "使用中文字体设置")\
    MACRO_NAME(IMAGE_ROTATE_OPTIONS,                    "屏幕旋转")\
    MACRO_NAME(IMAGE_CARTOON_MODE,                      "漫画")\
    MACRO_NAME(ZOOM_IN,                                 "放大")\
    MACRO_NAME(ZOOM_OUT,                                "缩小")\
    MACRO_NAME(BOOK_OPEN_FAILED,                        "打开文件失败")\
    MACRO_NAME(FINDNEWRELEASE,                          "发现新版本")\
    MACRO_NAME(CURRENTVERSION,                          "当前版本")\
    MACRO_NAME(LATESTVERSION,                           "最新版本")\
	MACRO_NAME(UPDATE_NEW_FUNCTION,						"新增功能")\
	MACRO_NAME(UPDATE_NEW_REPAIR,						"修复问题")\
    MACRO_NAME(AUTOREBOOT,                              "升级过程系统可能自动重启。")\
    MACRO_NAME(UPGRADENOW,                              "是否现在升级?")\
    MACRO_NAME(UPGRADEFAILED,                           "升级失败，请稍后重试。")\
    MACRO_NAME(HAVEDOWNLOADNEWVERSION,                  "已在之前下载成功，但尚未升级。")\
    MACRO_NAME(UPGRADE_DISK_FULL,                       "磁盘容量不足,请释放空间后再尝试升级!")\
    MACRO_NAME(UPGRADE_OTA_DIFF_PACKAGE,                "当前为增量升级包，大小")\
    MACRO_NAME(UPGRADE_OTA_FULL_PACKAGE,                "当前为完整升级包，大小")\
    MACRO_NAME(UPGRADE_OTA_NEED_SPACE,                  "解压所需临时空间")\
    MACRO_NAME(UPGRADE_OTA_FREE_SPACE,                  "当前磁盘可用空间")\
    MACRO_NAME(UPGRADEFAILED_FILE_SYSTEM_ERROR,         "文件系统错误，请连接电脑，检查Kindle USB磁盘。")\
    MACRO_NAME(UPGRADEFAILED_CDDIR,                     "原因:不能进入系统目录。")\
    MACRO_NAME(UPGRADING,                               "正在升级系统，请稍等...")\
    MACRO_NAME(UPGRADE_REBOOTING,                       "系统升级成功，正在重启...")\
    MACRO_NAME(UPGRADE_NewVersionDOWNLOADING,           "新版本正在下载，请稍候...")\
    MACRO_NAME(UPGRADE_GettingNewRelease,               "正在获取新版本...")\
	MACRO_NAME(ACCOUNT_XIAOMI_MIGRATE_TITLE,            "帐号迁移")\
    MACRO_NAME(ACCOUNT_XIAOMI_MIGRATE_TIP,              "为了向读友们提供更强大的云端服务，多看帐号将会迁移至小米帐号系统。\n\n您的所有信息都将被保留。\n\n只需要简单三步，迁移完成后您将同时获得免费云空间。随时随地云书架随行，享受精品阅读时光。\n\n为了保证安全、快速地迁移数据，强烈建议您在网络条件好的环境开始帐号迁移流程。")\
    MACRO_NAME(ACCOUNT_DUOKAN_CLOSED_TIP,               "多看账号登录已经关闭。\n\n您可以选择迁移多看账号到小米帐户，或者登录小米账户。")\
    MACRO_NAME(ACCOUNT_DUOKAN_REGISTER_CLOSED_TIP,      "多看账号注册已经关闭，请注册小米账号以使用多看服务")\
    MACRO_NAME(ACCOUNT_XIAOMI_MIGRATE,                  "立即迁移至小米帐号")\
    MACRO_NAME(ACCOUNT_XIAOMI_NOT_MIGRATE,              "暂不迁移，以后再说")\
	MACRO_NAME(ACCOUNT_FEEDBACK,                        "帐号迁移反馈")\
	MACRO_NAME(ACCOUNT_FEEDBACK_DUOKAN_ACCOUNT,         "请输入您的原多看帐号")\
	MACRO_NAME(ACCOUNT_FEEDBACK_XIAOMI_ACCOUNT,         "请输入您的小米帐号")\
	MACRO_NAME(ACCOUNT_FEEDBACK_PHONE,                  "能联系到您的手机号：")\
	MACRO_NAME(ACCOUNT_FEEDBACK_PHONE_TIP,              "请输入联系到您的手机号")\
	MACRO_NAME(ACCOUNT_FEEDBACK_ERROR,                  "请您选择错误类型：")\
	MACRO_NAME(ACCOUNT_FEEDBACK_ERROR_PURCHASED,        "已购图书出错")\
	MACRO_NAME(ACCOUNT_FEEDBACK_ERROR_NOTE,             "笔记出错")\
	MACRO_NAME(ACCOUNT_FEEDBACK_ERROR_FAVOURITE,        "收藏数据出错")\
	MACRO_NAME(ACCOUNT_FEEDBACK_ERROR_EXPERIENCE,       "阅历数据出错")\
	MACRO_NAME(ACCOUNT_FEEDBACK_ERROR_OTHER,            "其他错误类型，请详细描述")\
    MACRO_NAME(NETWORK_NAME_CANNOT_BE_EMPTY,            "网络名称不能为空，请输入")\
    MACRO_NAME(LABEL_FENLAN,                            "分栏")\
    MACRO_NAME(LABEL_FENLAN_CLOSE,                      "关闭分栏")\
    MACRO_NAME(AUDIO_PLAYER,                            "音频播放器")\
    MACRO_NAME(SONG_NOT_FOUND,                          "请使用方向键控制")\
    MACRO_NAME(AUTHOR_UNKOWN,                           "")\
    MACRO_NAME(TEXT_COUNT,                              "个")\
    MACRO_NAME(INPUTISNULL_REPEAT,                      "您输入的内容为空，请重新输入。")\
    MACRO_NAME(DICTIONARY,                              "查字典")\
    MACRO_NAME(WORD_SET,                                "设置")\
    MACRO_NAME(NO_INSTALL_DICT,                         "尚未安装词典")\
    MACRO_NAME(NO_INSTALL_DICT_INFO,                    "\n\n格式支持:\n通过7-zip解压为包含.dz、.idx、.ifo这三个文件的字典。\n\n大小限制:40M \n\n保存目录:DK_System\\xKindle\\res\\dict\n")\
    MACRO_NAME(DICT_TIPS,                               "直接输入自己所查内容，按确定即可查询该词解释")\
    MACRO_NAME(DICT_NO_RESULT  ,                        "很抱歉，没有对应的结果")\
    MACRO_NAME(BACK_TO_DICT,                            "返回到字典")\
    MACRO_NAME(FAILED_ADD_WORD,                         "添加失败")\
    MACRO_NAME(DICT_NAME_UP,                            "上移")\
    MACRO_NAME(DICT_NAME_DOWN,                          "下移")\
    MACRO_NAME(DICT_SEE_MORE,                           "查看更多...")\
    MACRO_NAME(LEFT_TITLE_NUMBER,                       "《")\
    MACRO_NAME(RIGHT_TITLE_NUMBER,                      "》")\
    MACRO_NAME(BOOKINFODLG_LATEST_SECTION,              "最新章节：")\
    MACRO_NAME(SCREENSAVER_USER,                        "关闭")\
    MACRO_NAME(SCREENSAVER_OUT_MESSAGE,                 "       正在处理中...")\
    MACRO_NAME(INPUT_UNLOCK_PASSWD,                     "请输入解锁密码")\
    MACRO_NAME(DATA_WIFI_TRANSFER,                      "WiFi传输")\
    MACRO_NAME(DATA_WIFI_TRANSFER_OPEN,                 "开启")\
    MACRO_NAME(DATA_WIFI_TRANSFER_HLEP,                 "请用FTP客户端软件打开：\r\nftp://")\
    MACRO_NAME(EMBOLDEN_LEVEL,                          "加黑")\
    MACRO_NAME(EMBOLDEN_LEVEL_ONE,                      "较黑")\
    MACRO_NAME(EMBOLDEN_LEVEL_TWO,                      "特黑")\
    MACRO_NAME(ACCOUNT,                                 "帐号")\
    MACRO_NAME(ACCOUNTCENTER,                           "帐号中心")\
    MACRO_NAME(ACCOUNT_EMAIL,                           "邮箱")\
    MACRO_NAME(ACCOUNT_PASSWORD,                        "密码")\
    MACRO_NAME(ACCOUNT_LOGIN,                           "登录")\
    MACRO_NAME(ACCOUNT_LOGIN_SUCCESS,                   "登录成功")\
    MACRO_NAME(ACCOUNT_LOGIN_LOGINING,                  "正在登录中，请稍候...")\
	MACRO_NAME(ACCOUNT_LOGINING_TITLEBAR_TIP,            "登录中")\
    MACRO_NAME(ACCOUNT_LOGINING,                        "正在获取个人信息，请稍候...")\
    MACRO_NAME(ACCOUNT_NOTLOGGEDIN,                     "未登录")\
    MACRO_NAME(ACCOUNT_LOGOUT,                          "退出")\
    MACRO_NAME(ACCOUNT_REGISTER,                        "注册")\
    MACRO_NAME(ACCOUNT_USERNAME,                        "用户名")\
    MACRO_NAME(ACCOUNT_MODIFYUSER,                      "修改")\
    MACRO_NAME(ACCOUNT_OLDPASSWORD,                     "旧密码")\
    MACRO_NAME(ACCOUNT_NEWPASSWORD,                     "新密码")\
    MACRO_NAME(ACCOUNT_PASSWORDMODIFYSUCCEED,           "密码修改成功")\
    MACRO_NAME(ACCOUNT_FORGETPASSWORD,                  "忘记密码?")\
    MACRO_NAME(ACCOUNT_FORGETPASSWORD2,                 "忘记密码")\
    MACRO_NAME(ACCOUNT_EMPTY_MAIL_PWD,                  "邮箱帐号和密码不能为空")\
    MACRO_NAME(ACCOUNT_USERNAMEMODIFYSUCCEED,           "用户名已成功修改")\
    MACRO_NAME(ACCOUNT_EMPTYPARAMETER,                  "输入不能为空")\
    MACRO_NAME(ACCOUNT_NETWORKISSUE,                    "连接网络超时，请稍后重试")\
    MACRO_NAME(ACCOUNT_EMAIL_INVALID,					"Email格式错误")\
    MACRO_NAME(ACCOUNT_YOURDKMAIL,                      "免费邮箱: ")\
    MACRO_NAME(ACCOUNT_DKMAILFUNC,                      ".它可以实现书籍推送、RSS订阅等功能，欢迎使用！")\
    MACRO_NAME(READINGHISTORY_TOTALHOURS,               "阅读总小时数")\
    MACRO_NAME(READINGHISTORY_AVERAGEHOURSPERBOOK,      "平均每本阅读时间")\
    MACRO_NAME(READINGHISTORY_PAGESPERHOUR,             "每小时阅读页数")\
    MACRO_NAME(READINGHISTORY_TOTALCOMPLETEDEDBOOKS,    "阅读完成书籍数量")\
    MACRO_NAME(LOW_POWER_NO_TTS,                        "当前电量不足，是否取消朗读？")\
    MACRO_NAME(LOW_POWER_NO_AUDIOPLAYER,                "当前电量不足，是否取消播放音乐？")\
    MACRO_NAME(LOW_POWER_PLEASE_CHARGE,                 "当前电量低，请及时充电")\
    MACRO_NAME(EMAILCANNOTEMPTY,                        "邮件帐号不能为空")\
    MACRO_NAME(CLEARREADINGSTAT,                        "重置")\
    MACRO_NAME(WIFITRANSFER_CLOSE,                      "断开")\
    MACRO_NAME(WIFIIPTYPE,                              "IP类型:")\
    MACRO_NAME(WIFIDYNAMICIP,                           "动态")\
    MACRO_NAME(WIFISTATICIP,                            "静态")\
    MACRO_NAME(WIFIIPADDRESS,                           "IP地址:")\
    MACRO_NAME(WIFIIPMASK,                              "子网掩码:")\
    MACRO_NAME(WIFIGATEWAY,                             "网关:")\
    MACRO_NAME(WIFIDNSIP,                               "DNS地址:")\
    MACRO_NAME(WIFI_EMPTYIP,                            "静态IP设置参数不完整")\
    MACRO_NAME(WIFI_CONNECT,                            "连接")\
    MACRO_NAME(PROGRESSBAR_SELECT_TYPE,                 "进度条模式")\
    MACRO_NAME(PROGRESSBAR_NORMAL_TYPE,                 "完整版")\
    MACRO_NAME(PROGRESSBAR_MINI_TYPE,                   "迷你版")\
	MACRO_NAME(FONT_RENDER,								"字体渲染")\
    MACRO_NAME(FONT_RENDER_SHARP,						"锐利")\
    MACRO_NAME(FONT_RENDER_SMOOTH,						"平滑")\
	MACRO_NAME(EPUB_MOBI_METADATA,						"EPUB/MOBI")\
	MACRO_NAME(EPUB_MOBI_METADATA_OPEN,					"元数据")\
	MACRO_NAME(EPUB_MOBI_METADATA_CLOSE,				"文件名")\
    MACRO_NAME(POWER_MANAGEMENT,                       "电源管理")\
    MACRO_NAME(BATTERYRATIO_MODE,                       "电量百分比")\
    MACRO_NAME(PDF_SMART_STRIP_EDGE,                    "智能切边")\
    MACRO_NAME(PDF_CUSTOM_STRIP_EDGE,                   "手动切边")\
    MACRO_NAME(PDF_STRIP_EDGE,                          "切边")\
    MACRO_NAME(PDF_NORMAL,                              "正常") \
    MACRO_NAME(BOOK_PARSE_ERROR,                        "本书解析错误") \
    MACRO_NAME(BOOK_PARSE_ISPARSING,                    "正在解析本书，请稍后重试") \
    MACRO_NAME(PDF_REARRANGE_FAILED,                    "当前为扫描版，无法提取到文字，将返回到原版")\
    MACRO_NAME(TOUCH_FIRST_TITLE_KEY_WORD,              "书名、书名首字母、关键字")\
	MACRO_NAME(TOUCH_ACTION_CANCEL,                     "取消")\
    MACRO_NAME(TOUCH_ACTION_SAVE,                       "保存")\
    MACRO_NAME(TOUCH_TITLEBAR_FRONTLIGHT,               "调节亮度")\
    MACRO_NAME(TOUCH_TITLEBAR_WLAN,                     "WLAN")\
    MACRO_NAME(TOUCH_TITLEBAR_WLANOFF,                  "关闭WLAN")\
    MACRO_NAME(TOUCH_TITLEBAR_WIFITRANSFER,             "WiFi传输")\
    MACRO_NAME(TOUCH_TITLEBAR_USERLOGIN,                "用户登录")\
    MACRO_NAME(TOUCH_TITLEBAR_WEB_BROWSER,              "浏览器")\
    MACRO_NAME(TOUCH_TITLEBAR_USERLOGOUT,               "退出")\
    MACRO_NAME(TOUCH_TITLEBAR_SUBBOOKINFO,             "订阅书讯")\
    MACRO_NAME(TOUCH_TITLEBAR_QUITSUBBOOKINFO,          "退订书讯")\
    MACRO_NAME(TOUCH_PUSH_UP_CLOSE,						"收起通知栏")\
    MACRO_NAME(TOUCH_BOOKSETTING_TOC,                   "目录")\
    MACRO_NAME(TOUCH_BOOKSETTING_TTS,                   "朗读")\
    MACRO_NAME(TOUCH_BOOKSETTING_CLOSETTS,              "关闭")\
	MACRO_NAME(TOUCH_BOOKSETTING_VOLUME,                "音量")\
    MACRO_NAME(TOUCH_BOOKSETTING_FONTSIZE,              "字号")\
    MACRO_NAME(TOUCH_BOOKSETTING_FONTSETTING,           "字体")\
    MACRO_NAME(TOUCH_BOOKSETTING_FORMAT,                "版式")\
    MACRO_NAME(TOUCH_BOOKSETTING_BOLD,                  "加黑")\
    MACRO_NAME(TOUCH_BOOKSETTING_SETTING,               "设置")\
    MACRO_NAME(TOUCH_BOOKSETTING_ROTATE,                "转屏")\
    MACRO_NAME(TOUCH_BOOKSETTING_TRIM,                  "切边")\
    MACRO_NAME(TOUCH_BOOKSETTING_TRIM_SETTING,          "切边设置")\
    MACRO_NAME(TOUCH_BOOKSETTING_SMART_TRIM,            "自动切边")\
    MACRO_NAME(TOUCH_BOOKSETTING_MANUAL_TRIM,           "手工切边")\
    MACRO_NAME(TOUCH_BOOKSETTING_CLOSETRIM,             "关闭切边")\
    MACRO_NAME(TOUCH_PDFTRIM_TIPS,						 "移动确认完毕后点击屏幕中央")\
	MACRO_NAME(TOUCH_BOOKSETTING_CONFIRM,				"保存")\
	MACRO_NAME(TOUCH_BOOKSETTING_CANCEL,				"关闭")\
	MACRO_NAME(TOUCH_BOOKSETTING_REARRANGE,             "智能重排")\
    MACRO_NAME(TOUCH_BOOKSETTING_CLOSEREARRANGE,        "关闭重排")\
	MACRO_NAME(TOUCH_BOOKSETTING_READERSETTING,			"阅读设置")\
    MACRO_NAME(TOUCH_BOOKSETTING_MAINTEXT_FONTSETTING,	"正文字体设置")\
    MACRO_NAME(TOUCH_BOOKSETTING_OTHER_FONTSETTING,     "其他字体设置")\
	MACRO_NAME(TOUCH_BOOKSETTING_TAP_LEFT,			"轻点屏幕左侧")\
	MACRO_NAME(TOUCH_BOOKSETTING_TAP_RIGHT,			"轻点屏幕右侧")\
	MACRO_NAME(TOUCH_BOOKSETTING_TURN_DOWN,				"下翻页")\
	MACRO_NAME(TOUCH_BOOKSETTING_TURN_UP,			"上翻页")\
	MACRO_NAME(TOUCH_BOOKSETTING_NEXT_CHAPTER,			"下一章")\
	MACRO_NAME(TOUCH_BOOKSETTING_PREV_CHAPTER,			"上一章")\
	MACRO_NAME(TOUCH_BOOKSETTING_READINGUPDOWN_GUSTURE_CUSTOM,	"上下滑动")\
	MACRO_NAME(TOUCH_BOOKSETTING_READINGUPDOWN_GESTURE_CHAPTER,	"换章")\
	MACRO_NAME(TOUCH_BOOKSETTING_READINGUPDOWN_GESTURE_PAGE,	"换页")\
	MACRO_NAME(TOUCH_BOOKSETTING_DISPLAY_CONTENT,		"内容显示")\
	MACRO_NAME(TOUCH_BOOKSETTING_DISPLAY_NORMAL,		"正常")\
	MACRO_NAME(TOUCH_BOOKSETTING_DISPLAY_ADAPTWIDE,		"宽度自适应")\
	MACRO_NAME(TOUCH_BOOKSETTING_JUMPTITLE,				"页面跳转")\
	MACRO_NAME(TOUCH_BOOKSETTING_JUMP,					"跳转")\
	MACRO_NAME(TOUCH_BOOKSETTING_CLOSE,					"取消")\
	MACRO_NAME(TOUCH_BOOKSETTING_ENTRY,					"开头")\
	MACRO_NAME(TOUCH_BOOKSETTING_TAG,					"末尾")\
    MACRO_NAME(TOUCH_BOOKSETTING_PAGE,					"页")\
    MACRO_NAME(TOUCH_BOOKSETTING_TOC_COMMENTANDDIGEST,  "书摘批注")\
	MACRO_NAME(TOUCH_BOOKSETTING_TOC_BOOKTOC,			"目录")\
	MACRO_NAME(TOUCH_BOOKSETTING_TOC_BOOKMARK,			"书签")\
    MACRO_NAME(TOUCH_DATESETTING,                        "日期设置")\
    MACRO_NAME(TOUCH_TIMESETTING,                        "时间设置")\
    MACRO_NAME(TOUCH_SYSTEMSETTING,                      "系统设置")\
    MACRO_NAME(TOUCH_SYSTEMSETTING_ACCOUNTINFO,          "帐号信息")\
    MACRO_NAME(TOUCH_SYSTEMSETTING_ACCOUNT_REGISTER,     "注册")\
    MACRO_NAME(TOUCH_SYSTEMSETTING_INPUT_MAILADDRESS,    "请输入您注册的邮箱")\
    MACRO_NAME(TOUCH_SYSTEMSETTING_REGISTER_MAILADDRESS, "需要注册的邮箱")\
    MACRO_NAME(TOUCH_SYSTEMSETTING_INPUT_PASSWORD,       "请输入您的密码")\
    MACRO_NAME(TOUCH_SYSTEMSETTING_ALIAS,				 "昵称:")\
    MACRO_NAME(TOUCH_SYSTEMSETTING_ALIAS_MUSTNOT_NULL,	 "昵称不能为空")\
    MACRO_NAME(TOUCH_SYSTEMSETTING_PWD_MUSTNOT_NULL,	 "密码不能为空")\
    MACRO_NAME(TOUCH_SYSTEMSETTING_REGISTER_ACCOUNT,     "注册多看帐号")\
	MACRO_NAME(TOUCH_SYSTEMSETTING_REGISTER_XIAOMI_ACCOUNT, "注册小米帐号")\
    MACRO_NAME(TOUCH_SYSTEMSETTING_LOGIN_ACCOUNT,        "登录多看帐号")\
	MACRO_NAME(TOUCH_SYSTEMSETTING_LOGIN_XIAOMI_ACCOUNT, "登录小米帐号")\
    MACRO_NAME(TOUCH_SYSTEMSETTING_ACCOUNT_MODIFYUSER,   "修改帐号信息")\
	MACRO_NAME(TOUCH_SYSTEMSETTING_SUBMIT_FEEDBACK,      "提交反馈") \
    MACRO_NAME(TOUCH_SYSTEMSETTING_SEND_PASSWORD,		 "发送密码")\
    MACRO_NAME(TOUCH_SYSTEMSETTING_MODIFY_PASSWORD,		 "修改密码")\
    MACRO_NAME(TOUCH_SYSTEMSETTING_RESET_PWD_EMAIL,		 "重置密码邮件已发送")\
    MACRO_NAME(TOUCH_SYSTEMSETTING_CONTACT_EMAIL,		 "如有疑问可联系 kindleuser@duokan.com")\
    MACRO_NAME(TOUCH_SYSTEMSETTING_PUSHES, "推送")\
    MACRO_NAME(TOUCH_SYSTEMSETTING_PUSHBOOK_MAIL,		 "推送邮箱")\
    MACRO_NAME(TOUCH_SYSTEMSETTING_PUSHBOOK_TIPS,		 "什么是推送邮箱")\
    MACRO_NAME(TOUCH_SYSTEMSETTING_DK_BOOKINFO,			 "多看书讯")\
    MACRO_NAME(TOUCH_SYSTEMSETTING_CLOSE,		         "关闭")\
    MACRO_NAME(TOUCH_SYSTEMSETTING_TIME_AM,              "AM")\
    MACRO_NAME(TOUCH_SYSTEMSETTING_TIME_PM,              "PM")\
	MACRO_NAME(TOUCH_SYSTEMUPDATE_OK,					 "立即升级")\
	MACRO_NAME(TOUCH_SYSTEMUPDATE_CANCEL,				 "下次再说")\
    MACRO_NAME(TOUCH_OPEN_FOLDER,                        "打开文件夹")\
    MACRO_NAME(TOUCH_DELETE_FOLDER,                      "删除文件夹")\
    MACRO_NAME(TOUCH_RESTART_SWITCH_SYS,                 "系统重启和切换")\
    MACRO_NAME(TOUCH_SWI_KINDLE,                         "Kindle原系统")\
    MACRO_NAME(TOUCH_SYSTEM_UPDATE,                      "系统更新")\
    MACRO_NAME(TOUCH_SET_DATE,                           "日期")\
    MACRO_NAME(TOUCH_SET_TIME,                           "时间")\
    MACRO_NAME(TOUCH_BOOK_FONT_SIZE,                     "字号选择")\
    MACRO_NAME(TOUCH_PROGRESSBAR_NORMAL_TYPE,            "完整")\
    MACRO_NAME(TOUCH_PROGRESSBAR_MINI_TYPE,              "迷你")\
	MACRO_NAME(TOUCH_SYSTEMSETTING_COMMENT_EXPORT_TITLE, "书摘批注")\
	MACRO_NAME(TOUCH_SYSTEMSETTING_COMMENT_EXPORT,		 "导出")\
    MACRO_NAME(TOUCH_CUSTOM_CNS_FONT,                    "自定义中文字体")\
    MACRO_NAME(TOUCH_CUSTOM_ENG_FONT,                    "自定义英文字体")\
    MACRO_NAME(TOUCH_GET_TOC_FAILURE,                    "自动断章失败，本书无章节信息。")\
    MACRO_NAME(TOUCH_UPPER_FLODER,                       "返回")\
    MACRO_NAME(TOUCH_GET_BOOKMARK_FAILURE,               "当前没有添加书签")\
    MACRO_NAME(TOUCH_GET_COMMENT_FAILURE,                "当前没有添加书摘批注")\
	MACRO_NAME(TOUCH_GET_ADD_BOOKMARK_GUIDE,             "阅读中点击菜单区域，右上角出现    ,轻点可给所在页加上书签")\
    MACRO_NAME(TOUCH_GET_ADD_COMMENT_GUIDE,              "手指轻点长按，移动选中需要做书摘或批注的文字区域，结束后，抬手弹出提示添加书摘或批注，选择即可。")\
    MACRO_NAME(TOUCH_CUTTINGEDGE_NORMAL,                 "正常")\
    MACRO_NAME(TOUCH_CUTTINGEDGE_ODDEVEN,                "奇偶页")\
    MACRO_NAME(TOUCH_DELETE,                             "删除")\
    MACRO_NAME(TOUCH_EXIT,                               "退出")\
    MACRO_NAME(TOUCH_CANCEL,                             "取消")\
    MACRO_NAME(TOUCH_SAVE,                               "保存")\
    MACRO_NAME(TOUCH_COMMENT,                            "批注")\
    MACRO_NAME(TOUCH_ADD_COMMENT,                        "添加批注")\
    MACRO_NAME(TOUCH_EDIT_COMMENT,                       "编辑")\
    MACRO_NAME(TOUCH_DELETE_COMMENT,                     "删除")\
    MACRO_NAME(TOUCH_ADD_DIGEST,                         "书摘")\
    MACRO_NAME(TOUCH_DELETE_DIGEST,                      "删除书摘")\
    MACRO_NAME(TOUCH_GOTO_COMMENTORDIGEST,               "查看书摘批注")\
    MACRO_NAME(TOUCH_COMMENTITEM,                        "注：")\
    MACRO_NAME(TOUCH_DOWNLOAD,							 "下载")\
    MACRO_NAME(BOOKSTORE_LOADINGDATAFAILED,				 "加载数据失败")\
	MACRO_NAME(TOUCH_DISCOUNT,							 "特价")\
	MACRO_NAME(TOUCH_TODAYDISCOUNT,						 "今日特价")\
	MACRO_NAME(TOUCH_FREEZONE,							 "免费专区")\
	MACRO_NAME(TOUCH_ALSOBUY,							 "购买了此书的人还买了")\
    MACRO_NAME(TOUCH_ALSOVIEW,							 "看了此书的人还看了")\
	MACRO_NAME(TOUCH_SCREENSHOTTIPS,					 "图片已保存至DK_ScreenShots目录下")\
	MACRO_NAME(BOOK_PUBLISH,							 "版权")\
    MACRO_NAME(BOOK_ISSUED,                              "出版")\
	MACRO_NAME(BOOK_UPDATE,								 "更新")\
    MACRO_NAME(BOOK_TRANSLATORS,                         "译者")\
    MACRO_NAME(BOOK_TAG,                                 "标签")\
    MACRO_NAME(BOOK_IDENTIFIER,                          "书号")\
    MACRO_NAME(BOOK_WORDCOUNT,                           "字数:\t%d 万字")\
    MACRO_NAME(BOOK_ORIGIN_PRICE,                        "原价")\
    MACRO_NAME(BOOK_PAPER_PRICE,                         "纸书")\
	MACRO_NAME(BOOKSTORE_BUY,							 "购买")\
	MACRO_NAME(BOOKSTORE_BUY_WHOLE,						 "购买完整版，后面的内容更精彩...")\
    MACRO_NAME(LAST_UPDATE,                              "上次记录")\
    MACRO_NAME(BOOKSTORE_TRIALVERSION,					 "试读本")\
    MACRO_NAME(BOOKSTORE_FREECHAPTERS,                   "免费试读")\
	MACRO_NAME(BOOKSTORE_READTRIAL,					     "试读")\
    MACRO_NAME(BOOKSTORE_READING_PROGRESS,			     "已读%d%%")\
    MACRO_NAME(BOOKSTORE_READING_FEW,   			     "已读不到1%")\
    MACRO_NAME(BOOKSTORE_BOOK_COUNT,			         "共%d本")\
    MACRO_NAME(BOOKSTORE_ADD_FAVOURITE,					 "收藏")\
    MACRO_NAME(BOOKSTORE_ADDING_FAVOURITE,			     "收藏中")\
    MACRO_NAME(BOOKSTORE_ADD_TO_CART,                    "加入")\
    MACRO_NAME(BOOKSTORE_ADDING_TO_CART,                 "加入中")\
    MACRO_NAME(BOOKSTORE_ALREADY_ADDED_TO_CART,          "已成功将图书添加到购物车")\
    MACRO_NAME(BOOKSTORE_CART_DISCOUNT_NOTICE,           "可享特惠：")\
    MACRO_NAME(BOOKSTORE_CART_DISCOUNT_DETAILS,          "总共为您优惠￥%g")\
    MACRO_NAME(BOOKSTORE_SPECIAL_EVENT_NOTICE_NOT_REACH,         "您还差￥%g，即可享受更大幅度的满%g减%g元优惠 ！")\
    MACRO_NAME(BOOKSTORE_SPECIAL_EVENT_NOTICE_REACHED,           "其中，满%g减%g元，已优惠￥%g ！")\
    MACRO_NAME(BOOKSTORE_CART_PURCHASING_NOTICE,         "请登录Web书城book.duokan.com，然后去购物车结算")\
    MACRO_NAME(BOOKSTORE_REMOVE_FROM_CART,               "已加入")\
    MACRO_NAME(BOOKSTORE_CONFIRM_REMOVE_CART,            "确定从购物车移除《%s》？")\
    MACRO_NAME(BOOKSTORE_HAD_ADDED_FAVOURITE,		     "已收藏")\
    MACRO_NAME(BOOKSTORE_REMOVE_FAVOURITE,  		     "取消")\
    MACRO_NAME(BOOKSTORE_REMOVING_FAVOURITE,  		     "取消中")\
    MACRO_NAME(BOOKSTORE_SHARE,  		                 "分享")\
    MACRO_NAME(BOOKSTORE_FREECHARTS,					 "免费榜")\
    MACRO_NAME(BOOKSTORE_FREE,					         "免费")\
    MACRO_NAME(BOOKSTORE_PAYCHARTS,						 "畅销榜")\
    MACRO_NAME(BOOKSTORE_MONTHLYPAY,                     "月度榜")\
    MACRO_NAME(BOOKSTORE_FAVOURPAY,                      "好评榜")\
    MACRO_NAME(BOOKSTORE_RANKING_DOUBAN,                 "豆瓣好评")\
    MACRO_NAME(BOOKSTORE_RANKING_DANGDANG,               "当当热销")\
    MACRO_NAME(BOOKSTORE_RANKING_AMAZON,                 "亚马逊热销")\
    MACRO_NAME(BOOKSTORE_RANKING_JINGDONG,               "京东热销")\
    MACRO_NAME(BOOKSTORE_RMB,					         "￥")\
    MACRO_NAME(BOOKSTORE_PERSONALALLBOOKS,               "全部书籍")\
    MACRO_NAME(BOOKSTORE_DOWNLOADED,					 "已下载")\
    MACRO_NAME(BOOKSTORE_NON_DOWNLOADED,				 "不在本地")\
	MACRO_NAME(BOOKSTORE_HIDEBOOK_FROM_LIST,			 "从列表中隐藏")\
    MACRO_NAME(BOOKSTORE_DOWNLOAD,					     "下载")\
    MACRO_NAME(BOOKSTORE_UPDATE,					     "更新")\
    MACRO_NAME(BOOKSTORE_CREATEORDER_FAILED,             "创建订单失败!")\
    MACRO_NAME(BOOKSTORE_NEWBOOK,                        "新书上架")\
    MACRO_NAME(BOOKSTORE_NEWBOOK_ABUSTRUCT,              "最新最热门书籍")\
    MACRO_NAME(BOOKSTORE_PUBLISH,                        "出版社")\
    MACRO_NAME(BOOKSTORE_PUBLISH_ABUSTRUCT,              "出版社精选推荐")\
    MACRO_NAME(BOOKSTORE_RECOMMEND,                      "精品推荐")\
    MACRO_NAME(BOOKSTORE_RECOMMEND_ABUSTRUCT,            "多看精选书籍推荐")\
    MACRO_NAME(BOOKSTORE_DISCOUNT,                       "特价免费")\
    MACRO_NAME(BOOKSTORE_DISCOUNT_ABUSTRUCT,             "每日特价和免费专区")\
    MACRO_NAME(BOOKSTORE_CHARTS,                         "热门排行")\
    MACRO_NAME(BOOKSTORE_CHARTS_ABUSTRUCT,               "热门榜单")\
    MACRO_NAME(BOOKSTORE_RANKING,                        "榜单")\
    MACRO_NAME(BOOKSTORE_CATEGORYVIEW,                   "分类")\
    MACRO_NAME(BOOKSTORE_CATEGORY_ABUSTRUCT,			 "小说、经管、计算机…")\
	MACRO_NAME(PERSON_BUY_OR_DOWNLOAD,					 "购买和下载")\
    MACRO_NAME(BOOKSTORE_SELECT_PAYMENT_METHOD,		     "选择支付方式")\
    MACRO_NAME(BOOKSTORE_PAYMENT_METHOD_SENDMAIL,		 "发送购买订单到邮箱")\
    MACRO_NAME(BOOKSTORE_PAYMENT_METHOD_OTHREDEVICE,	 "通过其他设备购买")\
    MACRO_NAME(BOOKSTORE_PAYMENT_METHOD_WEBSITE,         "网页端购买")\
    MACRO_NAME(BOOKSTORE_MAILORDER_SUCCEEDED,	         "发送购买邮件成功")\
    MACRO_NAME(BOOKSTORE_MAILORDER_FAILED,	             "发送购买邮件失败")\
    MACRO_NAME(BOOKSTORE_DUOKAN_CHOICEST_BOOK,	         "多看出品")\
    MACRO_NAME(BOOKSTORE_VIEW_ALL,	                     "查看全部")\
    MACRO_NAME(BOOKSTORE_PAY_FINISHED,	                 "完成支付")\
    MACRO_NAME(BOOKSTORE_I_KNOW,	                     "我知道了")\
    MACRO_NAME(BOOKSTORE_BOOKREVIEW,                     "评论")\
    MACRO_NAME(BOOKSTORE_BOOKABSTRACT_AND_COPYRIGHT,	 "书籍简介和其他信息")\
    MACRO_NAME(BOOKSTORE_BOOKABSTRACT,                   "内容提要")\
    MACRO_NAME(BOOKSTORE_BOOKCOPYRIGHTS,                 "版权")\
    MACRO_NAME(BOOKSTORE_TITLE_ORDER_MAIL_SENT, "发送购买订单到注册邮箱") \
    MACRO_NAME(BOOKSTORE_HINT_ORDER_MAIL_SENT, "1.选择“发送链接”；\n2.到%s收取邮件，如收件箱中没有可到垃圾箱中查看；\n3.点击邮箱中链接网上完成支付。") \
    MACRO_NAME(BOOKSTORE_HINT_BUY_FROM_OTHER_DEVICE, "1.iOS/Android设备打开多看阅读；\n2.确保登录和kindle同一个帐号；\n3.进入书城购物车购买即可。") \
    MACRO_NAME(BOOKSTORE_HINT_BUY_FROM_WEBSITE, "1.浏览器地址栏输入book.duokan.com；\n2.登录帐号通过购物车结算即可。") \
    MACRO_NAME(BOOKSTORE_TITLE_BUY_FROM_OTHER_DEVICE, "通过其它设备购买") \
    MACRO_NAME(BOOKSTORE_TITLE_BUY_FROM_WEBSITE, "网页端购买") \
    MACRO_NAME(BOOKSTORE_USERDATA_SYNC_GOTO_PAGE,     "您正在阅读第%d页，从服务器获取的最新阅读进度为第%d页，是否要跳转到新位置？") \
	MACRO_NAME(BOOKSTORE_USERDATA_SYNC_PAGING,		  "您正在阅读 %s，从服务器获取的最新阅读进度为 %s，是否要跳转到新位置？") \
	MACRO_NAME(BOOKSTORE_USERDATA_SYNC_BOOKCOVER,	  "封面") \
    MACRO_NAME(BOOKSTORE_SELLER,                      "畅销书")\
    MACRO_NAME(BOOKSTORE_NEWARIVALS,                  "新书上架")\
    MACRO_NAME(BOOKSTORE_NONSEARCHRESULT_INFO,        "抱歉，书城中没有您要查询的\"%s\"相关书籍")\
    MACRO_NAME(BOOKSTORE_GOTOTOPIC,                   "进入专题")\
    MACRO_NAME(BOOKSTORE_BOOK_UPDATETIME,             "更新时间：")\
    MACRO_NAME(BOOKSTORE_BOOK_DUOKAN,                 "多看兑换码")\
    MACRO_NAME(BOOKSTORE_BOOK_DUOKANKEY_ENTRY,        "兑换码换书")\
    MACRO_NAME(BOOKSTORE_BOOK_DUOKANKEY_INPUT,        "请输入多看兑换码")\
	MACRO_NAME(TOUCH_TRANSLATE,                       "翻译")\
	MACRO_NAME(TOUCH_HAVE_NO_CHAPTER,                 "该文件没有章节")\
	MACRO_NAME(TOUCH_HAVE_NO_PRE_CHAPTER,             "当前已是首章节")\
	MACRO_NAME(TOUCH_HAVE_NO_NEXT_PAGE,               "当前已是最后一页")\
    MACRO_NAME(BOOKSTORE_RETRY,                       "重试")\
    MACRO_NAME(CURRENTPAGE_IS_PICTURE,                 "图（%ld / %ld 页）")\
	MACRO_NAME(TOUCH_COMMENT_LINK,					  "跳转")\
	MACRO_NAME(TOUCH_COMMENT_EXPORT_TITLE,			  "导出书摘批注")\
	MACRO_NAME(TOUCH_COMMENT_EXPORT_PROCESSING,		   "正在处理中，请稍候...")\
	MACRO_NAME(TOUCH_COMMENT_EXPORT_FILESAVE,		   "文件保存至：")\
	MACRO_NAME(TOUCH_COMMENT_EXPORT_PROGRESS,		   "正在导出   ")\
	MACRO_NAME(TOUCH_COMMENT_EXPORT_OK,				   "导出成功")\
	MACRO_NAME(TOUCH_COMMENT_EXPORT_ABUSTRUCT,		   "多看笔记 来自多看阅读 for Kindle")\
    MACRO_NAME(TOUCH_BACK_HOME,            	           "返回书城首页")\
    MACRO_NAME(TOUCH_BACK_PREV_PAGE,    	           "返回上一页")\
    MACRO_NAME(BOOKSTORE_BOOKCOMMENT_USEFULL,          "有用")\
    MACRO_NAME(BOOKSTORE_BOOKCOMMENT_REPLY,            "回复")\
    MACRO_NAME(BOOKSTORE_BOOKCOMMENT_ADDCOMMENT,       "添加评论")\
    MACRO_NAME(BOOKSTORE_BOOKCOMMENT_REQUIRED,         "(*为必选项)")\
    MACRO_NAME(BOOKSTORE_BOOKCOMMENT_STARLEVEL,        "* 星 级")\
    MACRO_NAME(BOOKSTORE_BOOKCOMMENT_TITLE,            "* 标 题")\
    MACRO_NAME(BOOKSTORE_BOOKCOMMENT_CONTENT,          "* 评 论")\
    MACRO_NAME(BOOKSTORE_BOOKCOMMENT_DEFAULTCONTENT,   "不少于20字")\
    MACRO_NAME(BOOKSTORE_BOOKCOMMENT_NOCOMMENT,        "还没有评论")\
    MACRO_NAME(BOOKSTORE_BOOKCOMMENT_NOCOMMENT_INFO,   "咦！抢个沙发？")\
    MACRO_NAME(BOOKSTORE_BOOKCOMMENT,                  "用户评论")\
    MACRO_NAME(BOOKSTORE_BOOKCOMMENT_SORT_USEFULL,     "最有用")\
    MACRO_NAME(BOOKSTORE_BOOKCOMMENT_SORT_LATEST,      "最新")\
    MACRO_NAME(BOOKSTORE_GETCOMMENTINFO,               "正在获取评论信息，请稍后...")\
    MACRO_NAME(BOOKSTORE_GETCOMMENTINFO_FAIL,          "获取评论信息失败，请稍后重试")\
    MACRO_NAME(BOOKSTORE_MYBOOKSCORE,                    "我的评分")\
    MACRO_NAME(BOOKSTORE_BOOKSCORE,                    "评分")\
    MACRO_NAME(BOOKSTORE_BOOKSCORE_NOSCORE,            "当前暂没有评分")\
    MACRO_NAME(BOOKSTORE_BOOKSCORE_NOLOGGIN,           "请登录后添加评分")\
    MACRO_NAME(BOOKSTORE_BOOKSCORE_CANNOTADDSCORE,     "请购买后评分")\
    MACRO_NAME(BOOKSTORE_BOOKCOMMENT_EXITCOMMENTINFO,  "确认要退出评论?")\
    MACRO_NAME(BOOKSTORE_BOOKSCORE_ADDSCOREFAIL,       "添加评分失败，请稍后重试！")\
    MACRO_NAME(BOOKSTORE_BOOKSCORE_ADDSCORE_FULLMARK,  "多谢了")\
    MACRO_NAME(BOOKSTORE_BOOKSCORE_ADDSCORE_NOTFULLMARK,"我们会继续努力")\
    MACRO_NAME(BOOKSTORE_BOOKCOMMENT_ADDCOMMENTFAILE,  "添加评论失败，请稍后重试！")\
    MACRO_NAME(BOOKSTORE_SUBMITTING,                   "正在提交")\
    MACRO_NAME(BOOKSTORE_BOOKSCORE_SUCCESS,            "评分已提交")\
    MACRO_NAME(BOOKSTORE_BOOKCOMMENT_VOTE_SUCCESS,     "投票成功")\
    MACRO_NAME(BOOKSTORE_BOOKCOMMENT_VOTE_FAIL,        "投票失败")\
    MACRO_NAME(BOOKSTORE_BOOKCOMMENT_VOTE_REPEATED,    "您已经投过票了")\
    MACRO_NAME(BOOKSTORE_VIEW_UPDATE_ALL,              "查看更新历史")\
    MACRO_NAME(BOOKSTORE_BOOK_DUOKANKEY_EXCHANGE,      "兑换")\
    MACRO_NAME(BOOKSTORE_FETCH_NOELEM,                 "没有相关的数据")\
    MACRO_NAME(BOOKSTORE_REDEEM_TOO_LONG_WORDS,        "赠言太长了")\
    MACRO_NAME(BOOKSTORE_REDEEM_WRONG_DEST_EMAIL,      "寄送邮箱地址不正确")\
    MACRO_NAME(BOOKSTORE_REDEEM_ILLIGEL_CODE,          "非法的兑换码")\
    MACRO_NAME(BOOKSTORE_REDEEM_INVALID_CODE,          "无效的兑换码")\
    MACRO_NAME(BOOKSTORE_REDEEM_CAN_NOT_USE_FREE_BOOK, "不能为免费图书购买兑换码")\
    MACRO_NAME(BOOKSTORE_REDEEM_BOOK_PURCHASED,        "您已购买了该图书，不能使用该兑换码")\
    MACRO_NAME(BOOKSTORE_REDEEM_BOOK_NOT_EXIST,        "未找到图书信息")\
    MACRO_NAME(BOOKSTORE_DUOKANKEY_TIPS,               "请输入12位兑换码")\
    MACRO_NAME(BOOKSTORE_DUOKANKEY_HELP1,               "注：1. 多看兑换码可直接兑换多看书城中指定一本书籍；\n")\
    MACRO_NAME(BOOKSTORE_DUOKANKEY_HELP2,               "2. 多看兑换码可通过参加活动获得或是购买获赠，详情请见book.duokan.com")\
    MACRO_NAME(BOOKSTORE_BOOKCATALOGUE_NOPREVIEW,       "本章节没有预览信息")\
    MACRO_NAME(BOOKSTORE_MY_CART,                       "我的购书车")\
    MACRO_NAME(BOOKSTORE_MY_CART_DISCOUNT,              " %d折 ")\
    MACRO_NAME(BOOKSTORE_MY_CART_SUMMARY_DETAILS,       "共 %d 本书， ￥ %g")\
    MACRO_NAME(BOOKSTORE_MY_CART_SUMMARY,               "实际支付: ￥ %g")\
    MACRO_NAME(BOOKSTORE_MY_CART_SUMMARY_FULL,          "您的购物车已达50本上限，共计￥ %g")\
    MACRO_NAME(BOOKSTORE_MY_CART_AUTHOR,                "作者: %s")\
    MACRO_NAME(BOOKSTORE_MY_CART_PRICE,                 "￥ %g 元")\
    MACRO_NAME(BOOKSTORE_MY_CART_SUMMARY_EMPTY,         "您的购书车是空的！")\
    MACRO_NAME(BOOKSTORE_MY_CART_PURCHASE,              "去结算")\
    MACRO_NAME(BOOKSTORE_MY_CART_ORIGIN_PRICE,          "原价￥%.2f")\
    MACRO_NAME(BOOKSTORE_MY_CART_PAPER_PRICE,           "纸书￥%.2f")\
    MACRO_NAME(BOOKSTORE_ADD_FAVOURITE_FAILED,          "加入收藏夹失败，请检查网络~~")\
    MACRO_NAME(BOOKSTORE_ADD_FAVOURITE_SUCCEEDED,       "已成功加入收藏夹！")\
    MACRO_NAME(BOOKSTORE_FAVOURITES_FULL,               "收藏夹已满，暂时加不了啊，去清理一下吧~~")\
    MACRO_NAME(BOOKSTORE_REMOVE_FAVOURITE_FAILED,       "取消收藏失败，请检查网络~~")\
    MACRO_NAME(BOOKSTORE_REMOVE_FAVOURITE_SUCCEEDED,    "已取消收藏!")\
    MACRO_NAME(BOOKSTORE_ADD_TO_CART_FAILED,            "加入购书车失败，请检查网络~~")\
    MACRO_NAME(BOOKSTORE_CART_FULL,                     "购物车已满，去清理一下吧~~")\
    MACRO_NAME(BOOKSTORE_ADD_TO_CART_SUCCEEDED,         "已成功加入购书车！")\
    MACRO_NAME(BOOKSTORE_REMOVE_FROM_CART_FAILED,       "无法从购书车移除，请检查网络~~")\
    MACRO_NAME(BOOKSTORE_REMOVE_FROM_CART_SUCCEEDED,    "已从购书车移除!")\
    MACRO_NAME(BOOKSTORE_UPDATE_LIST_PANEL_FAILED,      "更新列表失败，请检查网络！")\
	MACRO_NAME(BOOKSTORE_LOW_VERSION,      "当前软件版本过低，不支持该书版本，需升级软件！")\
	MACRO_NAME(BOOKSTORE_REED_UPGRADE,      "现在升级")\
    MACRO_NAME(LOGIN_EXPIRED,					        "您的登录已过期，请重新登录")\
    MACRO_NAME(SUBMITSUCCESS_PENDINGREVIEW,					  "已提交，等待审核")\
    MACRO_NAME(BOOKSTORE_BOOKCOMMENT_ADDCOMMENT_UNPURCHASED, "不能对未购买书籍添加评论！")\
    MACRO_NAME(AFTER_LOGIN, "登录后")\
    MACRO_NAME(SEND_LINK, "发送链接")\
    MACRO_NAME(SENDING_LINK, "发送中…")\
    MACRO_NAME(SENDED_LINK, "发送完成")\
    MACRO_NAME(MAIL_ORDER_FOOT_NOTE, "注：如长时间没有收到邮件，可尝试再发送链接")\
    MACRO_NAME(FONT_STYLE_GBK_OR_BIG5, "简繁")\
    MACRO_NAME(FONT_STYLE_GBK, "简体")\
    MACRO_NAME(FONT_STYLE_BIG5, "繁体")\
    MACRO_NAME(SN, "序列号")\
    MACRO_NAME(MAC, "网卡地址")\
	MACRO_NAME(PAGINT, "分页中...")\
	MACRO_NAME(SYNCING, "正在排序中...")\
    MACRO_NAME(SETTING_SYSTEM, "系统")\
    MACRO_NAME(SETTING_READING, "阅读")\
    MACRO_NAME(SETTING_ACCOUNT, "帐号")\
    MACRO_NAME(TEXT_CONVERT, "字体样式")\
    MACRO_NAME(MAILCHECKSTART, "推送接收中...")\
    MACRO_NAME(MAILCHECKNORESULT, "无推送")\
    MACRO_NAME(MAILCHECKGOTRESULT, "推送完成")\
    MACRO_NAME(MAILCANTCONNECTED, "网络异常，稍后重试")\
    MACRO_NAME(MAILPYTHONNOFOUND, "未发现推送程序，重装多看")\
    MACRO_NAME(BOOKCORRECTION, "纠错")\
    MACRO_NAME(BOOKCORRECTION_ORIENTCONTENT, "原文信息")\
    MACRO_NAME(BOOKCORRECTION_MODIFY1,       "修改建议")\
    MACRO_NAME(BOOKCORRECTION_MODIFY2,       "(书籍位置不用标明，系统自动处理)")\
    MACRO_NAME(BOOKCORRECTION_SEND,          "发送")\
    MACRO_NAME(BOOKCORRECTION_ONLINE_SEND,   "在线发送")\
    MACRO_NAME(BOOKCORRECTION_OFFLINE_SEND,  "离线发送")\
    MACRO_NAME(BOOKCORRECTION_SENDING,        "发送中...")\
    MACRO_NAME(BOOKCORRECTION_SAVED,          "离线发送成功，谢谢反馈！下次联网时会自动同步消息")\
    MACRO_NAME(BOOKCORRECTION_SEND_FAILED,    "网络有误，已保存为离线消息，恢复联网将自动发布离线消息")\
    MACRO_NAME(BOOKCORRECTION_SEND_CANCELED,  "感谢反馈，已保存为离线消息，恢复联网将自动发布离线消息")\
    MACRO_NAME(BOOKCORRECTION_SEND_SUCCEED,   "发送成功，谢谢反馈")\
    MACRO_NAME(DUOKAN_NEWS_DISPLAY_NAME,      "多看推送")\
    MACRO_NAME(UNEXPECTED_ERROR, "未知错误")\
    MACRO_NAME(CATEGORY_NEW, "新建分组")\
    MACRO_NAME(CATEGORY_ADD_FILE, "添加文件")\
    MACRO_NAME(CATEGORY_RENAME_BUTTON, "重命名")\
    MACRO_NAME(ADD_BOOK_TO_CART, "添加到购物车")\
    MACRO_NAME(REMOVE_FAVOURITE, "取消收藏")\
    MACRO_NAME(ADD_BOOK_TO_FAVOURITE, "移至收藏夹")\
    MACRO_NAME(REMOVE_FROM_CART, "从购物车中删除")\
    MACRO_NAME(OPEN_BOOK_DETAILS, "书籍信息")\
    MACRO_NAME(CATEGORY_ADD_FILE_HINT, "当前没有任何文件，请点右上角“添加文件”")\
    MACRO_NAME(CATEGORY_EXISTING, "分组已存在")\
    MACRO_NAME(CATEGORY_ADD_FILE_TO_CATEGORY, "选取文件至分组")\
    MACRO_NAME(CATEGORY_DELETE_FILE_FROM_CATEGORY, "从分组中移除")\
    MACRO_NAME(CATEGORY_ALREADY_SELECTED, "已选中%d本")\
    MACRO_NAME(CATEGORY_INPUT_NAME, "请输入分组名称")\
    MACRO_NAME(CATEGORY_RENAME, "请输入新的分组名称")\
    MACRO_NAME(CATEGORY_DELETE_CATEGORY_CONFIRM, "此操作将删除分组，是否继续？")\
    MACRO_NAME(CATEGORY_DELETE_CATEGORY_FILES_CONFIRM, "同时彻底删除此分组下的所有文件")\
    MACRO_NAME(CATEGORY_HINT_FOR_NEW_CATEGORY, "创建成功，是否现在添加书籍？")\
    MACRO_NAME(ELEGANT_READING_TIME, "#精品阅读时光# ")\
    MACRO_NAME(SHAREWEIBO_HINT, "我正在使用多看阅读Kindle版看《%s》，Kindle上的精品阅读时光，与我的朋友齐分享。@多看")\
	MACRO_NAME(SHAREWEIBO_PERSONALEXPERIENCE, "#多看阅历# 我使用 @多看阅读 超过了%g%%的读书人。总计阅读时间%d小时，平均每日阅读%s。")\
    MACRO_NAME(TYPE_WORD_COUNTS, "还可以输入%d字")\
    MACRO_NAME(TYPE_WORD_COUNTS_OVER, "已经超过%d字")\
    MACRO_NAME(SINAWEIBO_USERNAME_TYPE_TIPS, "请用你的新浪微博帐号登录")\
    MACRO_NAME(SINAWEIBO_PASSWORD_TYPE_TIPS, "输入密码")\
    MACRO_NAME(BIND_SINAWEIBO, "绑定新浪微博")\
    MACRO_NAME(SHARE, "分享")\
    MACRO_NAME(SHARED_SUCCESS, "分享成功")\
    MACRO_NAME(BIND, "绑定")\
    MACRO_NAME(BIND_SINAWEIBO_SUCCESS, "绑定新浪微博成功")\
    MACRO_NAME(NOBIND, "未绑定")\
    MACRO_NAME(UNBIND, "解绑")\
    MACRO_NAME(UNBIND_SUCCESS, "解绑成功")\
    MACRO_NAME(SWITCH, "更换")\
    MACRO_NAME(SINAWEIBO, "新浪微博")\
    MACRO_NAME(EVERNOTE, "印象笔记")\
    MACRO_NAME(YINXIANG, "印象笔记")\
    MACRO_NAME(EVERNOTE_INTERNATIONAL, "Evernote International")\
    MACRO_NAME(EVERNOTE_BIND_SUCCEED, "印象笔记绑定成功！")\
    MACRO_NAME(EVERNOTE_BIND_FAILED, "未绑定印象笔记！")\
    MACRO_NAME(SANDBOX, "Sandbox")\
    MACRO_NAME(SHARE_ACCOUNT_SETTING, "分享帐号设置")\
    MACRO_NAME(EXPORT_ACCOUNT_SETTING, "导出帐号设置")\
    MACRO_NAME(EXPORT_FETCHING_USER_INFO, "正在获取用户信息")\
    MACRO_NAME(EXPORT_FETCHING_USER_INFO_FAILED, "获取用户信息失败")\
    MACRO_NAME(EXPORT_WAIT_FOR_LAST_TASK, "请等待上个导出任务结束")\
    MACRO_NAME(EXPORT_AUTHORIZING, "正在认证")\
    MACRO_NAME(EXPORT_AUTHORIZING_FAILED, "认证失败，请检查网络")\
    MACRO_NAME(EXPORT_IS_EXPORTING, "正在导出...")\
    MACRO_NAME(EXPORT_EXPORTING_FAILED, "导出失败，请检查网络及帐号")\
    MACRO_NAME(EXPORT_SUCCEEDED, "笔记成功导出！")\
    MACRO_NAME(WEIBO_ERROR_USERNAME_PASSWORD_MISMATCH, "用户名或密码错误")\
    MACRO_NAME(WEIBO_ERROR_REPEAT_CONTENT, "相同内容请间隔10分钟再发布哦！")\
	MACRO_NAME(PERSONAL_NOTLOGIN, "未登录用户")\
	MACRO_NAME(PERSONAL_BOOK, "我的书籍")\
	MACRO_NAME(PERSONAL_BOOK_SUBTITLE, "查看已购图书")\
	MACRO_NAME(PERSONAL_COMMENT_SUBTITLE, "热门评论和我的评论")\
	MACRO_NAME(PERSONAL_EXPERIENCE, "我的阅历")\
	MACRO_NAME(PERSONAL_EXPERIENCE_SUBTITLE, "读书时间统计")\
	MACRO_NAME(PERSONAL_EXPERIENCE_INDEX, "您的阅历排名超过%g%%的人")\
	MACRO_NAME(PERSONAL_EXPERIENCE_BOOKNUM, "读过的书籍")\
	MACRO_NAME(PERSONAL_EXPERIENCE_COMPELTE_BOOKNUM, "读完的书籍")\
	MACRO_NAME(PERSONAL_EXPERIENCE_TOTAL_TIME, "总阅读小时")\
	MACRO_NAME(PERSONAL_EXPERIENCE_TOTAL_DAY, "使用的天数")\
	MACRO_NAME(PERSONAL_EXPERIENCE_TIME_SPAN, "时间分布")\
	MACRO_NAME(PERSONAL_EXPERIENCE_READING_TIME, "您更喜欢在%d:00-%d:00看书")\
	MACRO_NAME(PERSONAL_EXPERIENCE_EVERAGE, "平均 %s/天")\
	MACRO_NAME(PERSONAL_EXPERIENCE_SHARE, "分享")\
	MACRO_NAME(PERSONAL_EXPERIENCE_REFRESH, "刷新")\
	MACRO_NAME(PERSONAL_EXPERIENCE_REFRESHING, "刷新中...")\
	MACRO_NAME(PERSONAL_EXPERIENCE_SYNC, "同步于%d年%02d月%02d日 %02d:%02d")\
	MACRO_NAME(PERSONAL_EXPERIENCE_SYNC_NOT, "尚未进行同步")\
    MACRO_NAME(PERSONAL_MESSAGE, "消息中心")\
	MACRO_NAME(PERSONAL_MESSAGE_SUBTITLE, "个人信息和系统消息")\
	MACRO_NAME(PERSONAL_MESSAGE_SYSTEM, "系统消息")\
	MACRO_NAME(PERSONAL_KEY, "我的码")\
    MACRO_NAME(PERSONAL_COMMENTSSQUARE, "评论广场")\
    MACRO_NAME(PERSONAL_HOTCOMMENTS, "热门评论")\
    MACRO_NAME(PERSONAL_OWNCOMMENTS, "我的评论")\
    MACRO_NAME(PERSONAL_MINUTE_IN, "分钟内")\
    MACRO_NAME(PERSONAL_MINUTE_BEFORE, "分钟前")\
    MACRO_NAME(PERSONAL_HOUR_BEFORE, "小时前")\
	MACRO_NAME(PERSONAL_MYBOOK_PURCHASED, "购买于 %d年%d月%d日")\
	MACRO_NAME(PERSONAL_MYBOOK_UNHIDE, "取消隐藏")\
	MACRO_NAME(PERSONAL_MYBOOK_HIDE, "确认从已购中隐藏此书?")\
	MACRO_NAME(PERSONAL_MYBOOK_HIDE_TIP, "此书隐藏后将无法在我的书籍中显示，本地已下载书籍不影响阅读，如要在列表中恢复显示，需要到书城我的已购图书中的已隐藏书籍中恢复。")\
    MACRO_NAME(PERSONAL_MYBOOK_HIDE_FAIL_TIP, "隐藏失败，请检查网络~~")\
	MACRO_NAME(PERSONAL_MESSAGES_COMMENT_REPLY_TITLE, "回复了我")\
    MACRO_NAME(PERSONAL_MESSAGES_MYCOMMENT_REPLY, "回复%s：%s")\
    MACRO_NAME(PERSONAL_MESSAGES_COMMENTPAGE_TITLE, "评论详情")\
    MACRO_NAME(COME_FROM_WHERE, "来自%s版")\
    MACRO_NAME(COME_FROM_WEB, "来自网页版")\
    MACRO_NAME(VIEW_BOOKINFO, "查看书籍")\
	MACRO_NAME(QUANTIFUER_HOUR, "小时")\
	MACRO_NAME(QUANTIFUER_MINUTE, "分钟")\
    MACRO_NAME(PERSONAL_NOTES, "我的笔记")\
	MACRO_NAME(PERSONAL_NOTES_SUBTITLE, "书摘、批注汇总")\
    MACRO_NAME(PERSONAL_FAVOURITES, "我的收藏")\
    MACRO_NAME(PERSONAL_FAVOURITES_SUBTITLE, "感兴趣的书籍")\
    MACRO_NAME(PERSONAL_FAVOURITES_NO_ELEMENT_NOTICE, "没有发现藏书，到书城逛逛有惊喜呦~~")\
    MACRO_NAME(PERSONAL_CART_NO_ELEMENT_NOTICE, "购书车是空的，到书城逛逛有惊喜呦~~")\
    MACRO_NAME(PERSONAL_CART_NO_ELEMENT_STROLL, "到书城逛逛")\
    MACRO_NAME(PERSONAL_GOTO_CART, "去购物车")\
	MACRO_NAME(NOTE, "笔记")\
	MACRO_NAME(ADD_DATE, "添加时间")\
	MACRO_NAME(NOTE_DELETE_TIPS, "是否删除本条笔记")\
	MACRO_NAME(NO_NOTE_TIPS, "当前还没有记笔记，读书时不妨把喜欢的言语做成书摘或批注吆~~")\
	MACRO_NAME(EMPTY_ALL_NOTES, "删除所有笔记")\
    MACRO_NAME(EXPORT_ALL_NOTES, "导出笔记")\
	MACRO_NAME(EMPTY_ALL_NOTES_TIPS, "是否删除本书所有笔记")\
	MACRO_NAME(EXPORT_TO_EVERNOTE, "导出至印象笔记")\
	MACRO_NAME(CHECK_NOTE_IN_BOOK, "跳至所在书中位置")\
	MACRO_NAME(SHARE_NOTE, "分享笔记")\
    MACRO_NAME(LOGIN_ERROR_0, "成功")\
    MACRO_NAME(LOGIN_ERROR_1, "此用户当前未登录")\
    MACRO_NAME(LOGIN_ERROR_2, "服务忙")\
    MACRO_NAME(LOGIN_ERROR_3, "昵称不存在")\
    MACRO_NAME(LOGIN_ERROR_4, "该e-mail尚未注册，请先注册")\
    MACRO_NAME(LOGIN_ERROR_5, "e-mail格式错误")\
    MACRO_NAME(LOGIN_ERROR_6, "待搜索的e-mail不能为空")\
    MACRO_NAME(LOGIN_ERROR_7, "待搜索的e-mail不能含有空格")\
    MACRO_NAME(LOGIN_ERROR_8, "待搜索的昵称不能为空")\
    MACRO_NAME(LOGIN_ERROR_9, "待搜索的昵称不能含有空格")\
    MACRO_NAME(LOGIN_ERROR_10, "待搜索的e-mail格式错误")\
    MACRO_NAME(LOGIN_ERROR_11, "e-mail不能为空")\
    MACRO_NAME(LOGIN_ERROR_12, "e-mail不能含有空格")\
    MACRO_NAME(LOGIN_ERROR_13, "e-mail已存在")\
    MACRO_NAME(LOGIN_ERROR_14, "昵称已存在")\
    MACRO_NAME(LOGIN_ERROR_15, "昵称不能为空")\
    MACRO_NAME(LOGIN_ERROR_16, "昵称不能含有空格")\
    MACRO_NAME(LOGIN_ERROR_17, "设备ID不能为空")\
    MACRO_NAME(LOGIN_ERROR_18, "设备ID不能含有空格")\
    MACRO_NAME(LOGIN_ERROR_19, "AppID不能为空")\
    MACRO_NAME(LOGIN_ERROR_20, "AppID不能含有空格")\
    MACRO_NAME(LOGIN_ERROR_21, "时间戳不能为空")\
    MACRO_NAME(LOGIN_ERROR_22, "时间戳格式错误")\
    MACRO_NAME(LOGIN_ERROR_23, "密码不能为空")\
    MACRO_NAME(LOGIN_ERROR_24, "密码不能含有空格")\
    MACRO_NAME(LOGIN_ERROR_25, "访问令牌为空，请重新登录")\
    MACRO_NAME(LOGIN_ERROR_26, "访问令牌不能含有空格")\
    MACRO_NAME(LOGIN_ERROR_27, "登录密码错误")\
    MACRO_NAME(LOGIN_ERROR_28, "令牌错误")\
    MACRO_NAME(LOGIN_ERROR_29, "结果为空")\
    MACRO_NAME(LOGIN_ERROR_30, "令牌已过期，请尝试重新登录")\
    MACRO_NAME(LOGIN_ERROR_31, "昵称和e-mail不匹配")\
    MACRO_NAME(LOGIN_ERROR_32, "服务不可用")\
    MACRO_NAME(LOGIN_ERROR_33, "该请求已过期")\
    MACRO_NAME(LOGIN_ERROR_34, "e-mail地址无效")\
    MACRO_NAME(LOGIN_ERROR_35, "旧密码错误")\
    MACRO_NAME(LOGIN_ERROR_36, "旧密码不能为空")\
    MACRO_NAME(LOGIN_ERROR_37, "旧密码不能含有空格")\
    MACRO_NAME(LOGIN_ERROR_38, "旧密码长度不对")\
    MACRO_NAME(LOGIN_ERROR_39, "新密码不能为空")\
    MACRO_NAME(LOGIN_ERROR_40, "新密码不能含有空格")\
    MACRO_NAME(LOGIN_ERROR_41, "新密码长度不对")\
    MACRO_NAME(LOGIN_ERROR_42, "新旧密码不能相同")\
    MACRO_NAME(LOGIN_ERROR_43, "密码长度错误")\
    MACRO_NAME(LOGIN_ERROR_44, "性别输入错误")\
    MACRO_NAME(LOGIN_ERROR_45, "生日日期格式错误")\
    MACRO_NAME(LOGIN_ERROR_46, "临时密码请求已经生效，请勿再次点击")\
    MACRO_NAME(LOGIN_ERROR_47, "请填写真实邮箱,iduokan邮箱不能作为多看帐号")\
    MACRO_NAME(LOGIN_ERROR_48, "服务器鸭梨很大,不要太着急吆:-)")\
    MACRO_NAME(LOGIN_ERROR_49, "服务器异常,创建iduokan邮箱失败")\
    MACRO_NAME(LOGIN_ERROR_50, "邮件服务器繁忙，请稍后重试")\
    MACRO_NAME(LOGIN_ERROR_51, "发送临时密码失败，请确认邮件是否真实存在")\
    MACRO_NAME(LOGIN_ERROR_52, "该e-mail帐号不存在")\
    MACRO_NAME(LOGIN_ERROR_53, "该昵称帐号不存在")\
    MACRO_NAME(LOGIN_ERROR_54, "临时密码格式错误")\
    MACRO_NAME(LOGIN_ERROR_55, "设备ID类型错误")\
    MACRO_NAME(LOGIN_ERROR_56, "应用类型错误")\
    MACRO_NAME(LOGIN_ERROR_57, "该请求被拒绝")\
    MACRO_NAME(LOGIN_ERROR_58, "非期望结果出现")\
	MACRO_NAME(LOGIN_ERROR_65, "-该帐号已迁移，请用小米帐号登录")\
    MACRO_NAME(GOTO_MESSAGES_COMMENTDETAILINFO, "查看评论详情")\
    MACRO_NAME(GOTO_MESSAGES_USERDETAILINFO, "查看 %s")\
    MACRO_NAME(BOXMESSAGES_MORE, "正在载入更多未读消息...")\
    MACRO_NAME(BOXMESSAGES_MORE1, "您有%d条未读消息，请刷新")\
    MACRO_NAME(BOXMESSAGES_MORE2, "您有%d条未读消息，正在刷新，请稍后...")\
	MACRO_NAME(WEBBROSER_START, "正在启动体验版浏览器...")\
	MACRO_NAME(WEBBROSER_START_TIP, "系统在浏览器运行时不会自动休眠，请在不用时关闭浏览器。")\
	MACRO_NAME(ACCOUNT_MIGRATE_SUCCEED, "帐号迁移成功！")\
	MACRO_NAME(ACCOUNT_MIGRATE_CANCELED, "帐号迁移已取消！")\
	MACRO_NAME(ACCOUNT_MIGRATE_FAILED, "帐号迁移失败！")\
    MACRO_NAME(WEB_SEARCH, "Web搜索")\
    MACRO_NAME(WEB_SEARCH_BAIDU, "百度搜索")\
    MACRO_NAME(WEB_SEARCH_GOOGLE, "谷歌搜索")\
    MACRO_NAME(WEB_SEARCH_CIBA, "金山词霸")\
    MACRO_NAME(WEB_SEARCH_WIKI, "维基百科")\
    MACRO_NAME(BATCH_UPDATE_SUCCEED, "您的书都已经是最新的啦！")\
    MACRO_NAME(BATCH_DOWNLOAD_SUCCEED, "您的书都已经下载啦！")\
    MACRO_NAME(BATCH_UPDATE_EMPTY, "没有找到可以更新的书！")\
    MACRO_NAME(BATCH_DOWNLOAD_EMPTY, "没有找到可以下载的书！")\
    MACRO_NAME(BATCH_UPDATE_FAILED, "更新失败了，请检查网络~")\
    MACRO_NAME(BATCH_DOWNLOAD_FAILED, "下载失败了，请检查网络~")\
    MACRO_NAME(BATCH_UPDATE_PENDING, "开始更新...")\
    MACRO_NAME(BATCH_DOWNLOAD_PENDING, "开始下载...")\
    MACRO_NAME(BATCH_UPDATE, "全部更新")\
    MACRO_NAME(BATCH_DOWNLOAD, "全部下载")\
    MACRO_NAME(BATCH_STOP_DOWNLOAD, "停止下载")\
    MACRO_NAME(ASK_OPEN_EXTERNAL_LINK, "是否打开链接: %s ?")\
    MACRO_NAME(BOOKSTORE_WAITING,      "等待")\

// TODO: 将该部分改为从文件读取。
#define LAYOUT_DEMO_FILEPATH "/DuoKan/LayoutDemo.txt"




void InitLanguage();

enum SOURCESTRINGID
{

#define STRING_LIST_INDEX_ENUM(CommandId, StringName)\
            CommandId,
    STRING_LIST(STRING_LIST_INDEX_ENUM)
    STRING_COUNT
#undef STRING_LIST_INDEX_ENUM
};

class StringManager
{
private:
    StringManager();

public:
    ~StringManager();

public:
    static CString  GetStringById(SOURCESTRINGID stringId);
    static  LPCSTR GetPCSTRById(SOURCESTRINGID stringId);
    static StringManager* GetInstance();
    static bool SetLanguage(string);
    static void InitMap();
    static void ReloadLang();
    static const std::string &GetDemoString();

    static const char* GetLoginErrorMessage(int errorCode, const char* msg);
private:
    static StringManager* s_pStringManager;

private:

};

#endif
