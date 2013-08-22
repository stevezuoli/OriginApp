///////////////////////////////////////////////////////////////////////
// Dictionary.cpp
// Contact:wang mingyin
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////



#include "Utility/Dictionary.h"
#include<fstream>
#include<iostream>
#include<stdlib.h>
#include <string.h>
#include<sys/types.h>
#include<dirent.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<unistd.h>
#include<sys/mman.h>
#include <ctype.h>
#include <arpa/inet.h>
#include<algorithm>

using namespace std;

#define  DICTDEFAULTPATH     "/mnt/us/DK_System/xKindle/res/dict"


namespace Dictionary
{
    vector<Dict_Info> vec_dict_info;

    void LoadDict()
    {
        GetDictListFromFile();
        vector<Dict_Info> vec_tmp=GetDictListFromHardDisk();
        string filename(DICTDEFAULTPATH);
        filename+="/dict.dat";
        ofstream fout(filename.c_str());
        if(!fout)
        {
            cerr << "Error write to file" << endl;
            return ;
        }
              
        for(int i=0; i<(int)vec_dict_info.size(); i++)
        {
            vector<Dict_Info>::iterator iter=vec_tmp.begin();
            for(int j=0; j<(int)vec_tmp.size(); j++)
            {
                if(vec_dict_info[i].filename==vec_tmp[j].filename)
                {
                    //写入文件
                    vec_tmp.erase(iter+j);
                    fout << vec_dict_info[i].filename << endl;
                    break;
                }
            }
        }
        
        for(int k=0; k<(int)vec_tmp.size(); k++)
        {
            fout << vec_tmp[k].filename << endl;
        }
        fout.close();
           GetDictListFromFile();
           CreateSecondIndex();
    }
    int GetDictListFromFile()
    {
        string dict_path(DICTDEFAULTPATH);
        string filename=dict_path+"/dict.dat";
        ifstream fin(filename.c_str());
        vector<Dict_Info> vec_tmp_dict;
        if(!fin)
        {
            cerr << "open dict.dat file error!" << endl;
            return -1;
        }
        while(fin)
        {
            //打开文件，开始读取其中的字典相关信息
            string dict_file_name;
            getline(fin,dict_file_name);
            if(dict_file_name.size()<=1)
                continue;

            if((int)dict_file_name.find(dict_path)==-1)
            {
                dict_file_name=dict_path+"/"+dict_file_name;          //包含路径名
            }
            string dictname=GetPrivateString("bookname",dict_file_name.c_str());
			string strDictFormat = GetPrivateString("sametypesequence", dict_file_name.c_str());
            if(dictname.size() > 1)        //不为空
            {
                Dict_Info dict;
                dict.dictname = dictname;
                dict.filename = dict_file_name;
				if(strDictFormat == "h")
				{
					dict.dictFormart = DICT_HTML;
				}
				else
				{
					dict.dictFormart = DICT_TEXT;
				}
                vec_tmp_dict.push_back(dict);
            }
        }
        fin.close();
        vec_dict_info=vec_tmp_dict;   //避免重复写入
    
        return vec_dict_info.size();
    }
    vector<Dict_Info> GetDictListFromHardDisk()
    {
        DIR * dir=NULL;
        vector<Dict_Info> dict_from_disk;
        struct dirent * ptr=NULL;
        char dictPath[MAX_NAME_SIZE]={0};
        strncpy(dictPath,DICTDEFAULTPATH,MAX_NAME_SIZE-1);
        dir =opendir(dictPath);
        if(dir!=NULL)
        {
            char oldPath[128]={0};
            getcwd(oldPath,128);         //进入到当前目录，测试文件是否存在
            chdir(dictPath);
            while((ptr = readdir(dir))!=NULL)
            {
                if(strcmp(".", ptr->d_name) == 0 || strcmp("..", ptr->d_name) == 0)
                    continue;
                //开始检查三个文件的完整性
                char filename[MAX_NAME_SIZE]={0};
                strncpy(filename,ptr->d_name,MAX_NAME_SIZE-1);
                char temp[MAX_NAME_SIZE]={0};
            
                if(!strcmp(filename + strlen(filename) - 4, ".ifo"))
                {
                    int temp_index1=-1,temp_index2=-1;
                    memset(temp,0,MAX_NAME_SIZE);
                    strcpy(temp, filename);
                    temp[strlen(temp) - 4] = '\0';
                    strcat(temp, ".idx");
                    temp_index1 = access(temp, 0);

                    memset(temp, 0, MAX_NAME_SIZE);
                    strcpy(temp, filename);
                    temp[strlen(temp) - 4] = '\0';
                    strcat(temp, ".dict");
                    temp_index2 = access(temp, 0);
                    if(!temp_index1 && !temp_index2)       //全部存在
                    {
                        Dict_Info dict;
                        string tmp(DICTDEFAULTPATH);
                        tmp=tmp+"/"+filename;
                        dict.filename=tmp;
                        dict_from_disk.push_back(dict);
                    }
                }
            }
            chdir(oldPath);
        }
        closedir(dir);
        return dict_from_disk;
    }
    int GetDictCount()
    {
        return vec_dict_info.size();
    }

    vector<Dict_Info> GetDictInfo()
    {
        return vec_dict_info;
    }

    string GetPrivateString(const char* pKey,const char* pFilename)
    {
        if(NULL==pKey || pKey[0]==0)
            return "";
        ifstream fin(pFilename);
        int length=strlen(pKey);
        string retVal="";
        if(!fin)
        {
            cerr << "GetPrivateString open file error!" << endl;
            return "";
        }
        while(fin)
        {
            string line;    
            getline(fin,line);
            if(line.substr(0,length)==pKey)
            {
                retVal=line.substr(length+1);
                break;
            }
        
        }
        fin.close();
        return retVal;
    }

	//模糊查找
    string TranslateForFuzzyMatch(const char* pWord,int index,string *pstrOutWord)
    {
        string mean=FindWordIndex(pWord,index,pstrOutWord,true);
        return mean;
    }

    string Translate(const char* pWord,int index)
    {
        string mean=FindWordIndex(pWord,index,NULL,false);
        return mean;
    }
    string TranslateInMultiDict(const char* pWord)
    {
            string mean;
            for(int i=0; i<(int)vec_dict_info.size(); i++)
            {
            mean+=FindWordIndex(pWord,i,NULL,false);
            mean+="\n\n\n";
            }
            return mean;
    }
    void DeleteDict(int iIndex)
    {
        if(iIndex<0 || iIndex>=(int)vec_dict_info.size())
            return ;
        if(vec_dict_info.empty())
            return;
        vector<Dict_Info>::iterator it=vec_dict_info.begin();
        for(int i=0; i<iIndex; i++)
        {
            ++it;          //much better
        }
        string index_name=(it->filename).substr(0,it->filename.size()-4);
        vec_dict_info.erase(it);

        string delete_name=index_name+".idx";
        unlink(delete_name.c_str());
        delete_name=index_name+".ifo";
        unlink(delete_name.c_str());
        delete_name=index_name+".idx.oft";
        unlink(delete_name.c_str());
        delete_name=index_name+".dict";
        unlink(delete_name.c_str());

    }

    string FindWordIndex(const char* pWord,int iIndex,string *pstrOutWord,bool bFuzzyMatch)
    {
        if(pWord==NULL)
            return "";
        if(iIndex<0 || iIndex>=(int)vec_dict_info.size())
            return "";
        Dict_Info dict=vec_dict_info[iIndex];
        string index_name=dict.filename;
        index_name=index_name.substr(0,index_name.size()-4)+".idx";
        int fd=-1;
        struct stat f_stat;
        char *map_address = NULL;
        const char *word_address = NULL;

        fd=open(index_name.c_str(),O_RDONLY,S_IRUSR);
        if(fd < 0)
        {
            return "" ;
        }
        if(fstat(fd, &f_stat) < 0)
        {
            close(fd);
            return "" ;
        }
        map_address =(char *)mmap(NULL,f_stat.st_size, PROT_READ, MAP_SHARED, fd, 0);
        if(map_address == MAP_FAILED)
        {
            close(fd);
            return "";
        }
        std::vector<const char*> addresses = MemoryFind(index_name,map_address, f_stat.st_size, pWord,bFuzzyMatch);

        string mean;
        for (size_t i = 0; i < addresses.size(); ++i)
        {
            word_address = addresses[i];
            if(NULL == word_address)
            {
                close(fd);
                munmap(map_address, f_stat.st_size);
                return "";
            }
            const char* pTmpWord=word_address;
            if(pTmpWord!=NULL)
                word_address+=strlen(pTmpWord)+1;

            int *word_addrd = NULL;
            word_addrd=(int*)word_address;
            int offset=ntohl(*word_addrd++);
            int size= ntohl(*word_addrd);
            if(bFuzzyMatch || 0 == strcasecmp(pTmpWord,pWord))
            {
                std::string curMean = FindMeanInDict(index_name,offset,size);
                mean += curMean;
            }
            if(pstrOutWord)
            {
                *pstrOutWord = pTmpWord;
            }
            if (i + 1 != addresses.size())
            {
                mean += "\r\n";
            }
        }
        munmap(map_address, f_stat.st_size);
        close(fd);
        return mean ;
    }

    vector<const char*> SearchCandidateWord(string& indexName,const char* pIdxMapAddr,int iIdxLen,const char* pWord,int iMaxCandidateCount)
    {
        vector<const char*> vctCandidate;
        vctCandidate.clear();
        int fd=-1;
        int iSrcWordLen = strlen(pWord);
        string sec_index=indexName+".oft";
        fd=open(sec_index.c_str(),O_RDONLY,S_IRUSR);
        if(fd < 0)
        {
            return vctCandidate;
        }
        struct stat f_stat;
        if(fstat(fd, &f_stat) < 0)
        {
            close(fd);
            return vctCandidate;
        }
        char *map_address = NULL;
        map_address =(char *)mmap(NULL,f_stat.st_size, PROT_READ, MAP_SHARED, fd, 0);
        if(map_address == MAP_FAILED)
        {
            close(fd);
            return vctCandidate;
        }
        //二分查找开始        因为是32位长度  4个字节
        int* map_addr = NULL;
        map_addr=(int*)map_address;
        const char *cp = NULL;
        int begin=0,end=0,mid=0;
        int offset=0;
        cp=map_address;
        end=f_stat.st_size>>2;
        
        //  mid=(end-begin)>>1;
        //  const char* pSimilarWord=NULL;
        //查找不到默认返回最相近的
        while(begin<=end)
        {
            mid=begin+((end-begin)>>1);
            offset=*(map_addr+mid);
            const char* pTmpWord=pIdxMapAddr+offset;
            cp=pTmpWord;
            //      pSimilarWord=pTmpWord;
            int result=strcasecmp(pTmpWord,pWord);
            if(0==result)
            {
                break;
            }
            else if(result>0)
            {
                end=mid-1;
            }
            else
            {
                begin=mid+1;
            }
        }
        int iVctLen = iMaxCandidateCount;
        if(!strncasecmp(cp,pWord,iSrcWordLen))
        {
            vctCandidate.push_back(cp);
            iVctLen--;
        }
        map_addr += mid;
        for(int i = 0; i < iVctLen; i++)
        {
            map_addr++;
            offset = *(map_addr);
            const char *pCandidateWord = pIdxMapAddr + offset;
            if(!strncasecmp(pCandidateWord,pWord,iSrcWordLen))
            {
                vctCandidate.push_back(pCandidateWord);
            }
            else
            {
                break;
            }
        }


        if(map_address)
        {
            munmap(map_address, f_stat.st_size);
        }

        close(fd);


        return vctCandidate;
    }

	//查看是否有后缀，并返回后缀的长度
    bool HavePostfix(char *pSrcWord,int *iPostfixLen)
    {
        int iLen = strlen(pSrcWord);
        if(iLen < (*iPostfixLen + 1))
        {
            return false;
        }
        switch(*iPostfixLen)
        {
        case 0:
            return true;
        case 1:
            if(pSrcWord[iLen - 1] == 's' || pSrcWord[iLen - 1] == 'd')
            {
                pSrcWord[iLen - 1] = 0;
                return true;
            }
            break;
        case 2:
            if(pSrcWord[iLen - 2] == 'e' && (pSrcWord[iLen - 1] == 's' || pSrcWord[iLen - 1] == 'd'))
            {
                pSrcWord[iLen - 2] = 0;
                return true;
            }
            break;
        case 3:
            if(pSrcWord[iLen - 3] == 'i' && pSrcWord[iLen - 2] == 'e' && pSrcWord[iLen - 1] == 's')
            {
                pSrcWord[iLen - 3] = 'y';
                pSrcWord[iLen - 2] = 0;
                return true;
            }
            else if(pSrcWord[iLen - 3] == 'i' && pSrcWord[iLen - 2] == 'n' && pSrcWord[iLen - 1] == 'g')
            {
                pSrcWord[iLen - 3] = 'e';
                pSrcWord[iLen - 2] = 0;
                return true;
            }
            else if(pSrcWord[iLen - 3] == 'i' && pSrcWord[iLen - 2] == 'e' && pSrcWord[iLen - 1] == 'd')
            {
                pSrcWord[iLen - 3] = 'y';
                pSrcWord[iLen - 2] = 0;
                return true;
            }
            break;
        case 4:
            if(pSrcWord[iLen - 3] == 'i' && pSrcWord[iLen - 2] == 'n' && pSrcWord[iLen - 1] == 'g')
            {
                pSrcWord[iLen - 3] = 0;
                return true;
            }
            break;
        default:
            break;
        }
        return false;
    }

	//返回所有查找到的该词或最近的那个(有可能有多个相同的词)
    std::vector<const char*> MemoryFind(string& indexName,const char* pIdxMapAddr,int iIdxLen,const char* pSrcWord,bool bFuzzyMatch)
    {
        std::vector<const char*> results;
        int fd=-1;
        string sec_index=indexName+".oft";
        fd=open(sec_index.c_str(),O_RDONLY,S_IRUSR);
        if(fd < 0)
        {
            return results;
        }
        struct stat f_stat;
        if(fstat(fd, &f_stat) < 0)
        {
            close(fd);
            return results;
        }

        int iSrcLen = strlen(pSrcWord);
        char* pWord = (char *) malloc(iSrcLen + 1);
        if(NULL == pWord)
        {
            close(fd);
            return results;
        }
        sprintf(pWord,"%s",pSrcWord);

        char *map_address = NULL;
        map_address =(char *)mmap(NULL,f_stat.st_size, PROT_READ, MAP_SHARED, fd, 0);
        if(map_address == MAP_FAILED)
        {
            free(pWord);
            close(fd);
            return results;
        }

        int iPostfixLen = 0;
        do{
            if(HavePostfix(pWord,&iPostfixLen))
            {
                //二分查找开始        因为是32位长度  4个字节
                int* map_addr = NULL;
                map_addr=(int*)map_address;
                int begin=0,end=0,mid=0;
                int offset=0;
                end=f_stat.st_size>>2;
                //  mid=(end-begin)>>1;
                //  const char* pSimilarWord=NULL;
                //查找不到默认返回最相近的
                while(begin<=end)
                {
                    mid=begin+((end-begin)>>1);
                    offset=*(map_addr+mid);
                    const char* pTmpWord=pIdxMapAddr+offset;
                    //      pSimilarWord=pTmpWord;
                    int result=strcasecmp(pTmpWord,pWord);
                    if(0==result)
                    {
                        results.push_back(pTmpWord);
                        int savedMid = mid;
                        while (--mid >= begin)
                        {
                            offset = *(map_addr + mid);

                            pTmpWord = pIdxMapAddr + offset;
                            if (0 == strcasecmp(pTmpWord, pWord))
                            {
                                results.push_back(pTmpWord);
                            }
                            else
                            {
                                break;
                            }
                        }
                        mid = savedMid;
                        while (++mid  < end)
                        {
                            offset = *(map_addr + mid);

                            pTmpWord = pIdxMapAddr + offset;
                            if (0 == strcasecmp(pTmpWord, pWord))
                            {
                                results.push_back(pTmpWord);
                            }
                            else
                            {
                                break;
                            }
                        }
                        break;
                    }
                    else if(result>0)
                    {
                        end=mid-1;
                    }
                    else
                    {
                        begin=mid+1;
                    }
                }
				//如果已经有词了，则返回
				if(results.size() > 0)
				{
					break;
				}
            }
            sprintf(pWord,"%s",pSrcWord);
            iPostfixLen++;
        }while(bFuzzyMatch && iPostfixLen < 5);
        if(map_address)
        {
            munmap(map_address, f_stat.st_size);
        }

        free(pWord);
        close(fd);

        return results;
    }

    vector<string> FindCandidateWord(const char* pWord,int iIndex,int iMaxCandidateCount)
    {
        vector<string> vctCandidateWord;
        vctCandidateWord.clear();
        if(pWord==NULL || iMaxCandidateCount <= 0)
            return vctCandidateWord;
        if(iIndex<0 || iIndex>=(int)vec_dict_info.size())
            return vctCandidateWord;
        Dict_Info dict=vec_dict_info[iIndex];
        string index_name=dict.filename;
        index_name=index_name.substr(0,index_name.size()-4)+".idx";
        int fd=-1;
        struct stat f_stat;
        char *map_address = NULL;

        fd=open(index_name.c_str(),O_RDONLY,S_IRUSR);
        if(fd < 0)
        {
            return vctCandidateWord ;
        }
        if(fstat(fd, &f_stat) < 0)
        {
            close(fd);
            return vctCandidateWord ;
        }
        map_address =(char *)mmap(NULL,f_stat.st_size, PROT_READ, MAP_SHARED, fd, 0);
        if(map_address == MAP_FAILED)
        {
            close(fd);
            return vctCandidateWord;
        }
        vector<const char *>vctCandidatePointer = SearchCandidateWord(index_name,map_address, f_stat.st_size, pWord,iMaxCandidateCount);
        if(vctCandidatePointer.empty())
        {
            close(fd);
            munmap(map_address, f_stat.st_size);
            return vctCandidateWord;
        }

        for(vector<const char *>::iterator itr = vctCandidatePointer.begin();itr != vctCandidatePointer.end();itr++)
        {
            vctCandidateWord.push_back(*itr);
        }
        munmap(map_address, f_stat.st_size);
        close(fd);
        return vctCandidateWord ;

    }

    string FindMeanInDict(string indexName,int offset,int size)
    {
        string dict_name=indexName.substr(0,indexName.size()-4)+".dict";
    
        ifstream fin(dict_name.c_str());
        if(!fin)
        {
            cerr << "Can't open the dict_name!" << endl;
            return "";
        }
        fin.seekg(offset,ios::beg);

        char* pSearchWord=new char[size+1];

        if (NULL == pSearchWord)
            return "";

        memset(pSearchWord,0,size+1);
        fin.read(pSearchWord,size);
        string search_word(pSearchWord);
        if(pSearchWord)
        {
            delete[] pSearchWord;
            pSearchWord=NULL;
        }
        fin.close();
        return search_word;
    }
    void InitDict()
    {
        GetDictListFromFile();
        //CreateSecondIndex();
    }

    void CreateSecondIndex()
    {
        int iSize=vec_dict_info.size();
        for(int i=0; i<iSize; i++)
        {
            string index_name=vec_dict_info[i].filename;
            index_name=index_name.substr(0,index_name.size()-4)+".idx";
            string second_index=index_name+".oft";
            int fd=-1;
            struct stat f_stat;
            char *map_address = NULL;

            fd=open(index_name.c_str(),O_RDONLY,S_IRUSR);
            if(fd < 0)
            {
                return ;
            }
            if(fstat(fd, &f_stat) < 0)
            {
                close(fd);
                return ;
            }
            FILE* pFile = fopen(second_index.c_str(), "wb+");//建立二级索引文件
            if(pFile == NULL)
            {
                close(fd);
                return ;
            }
            map_address = (char *)mmap(NULL,f_stat.st_size, PROT_READ, MAP_SHARED, fd, 0);//映射原索引
            if(map_address == MAP_FAILED)
            {
                close(fd);
                fclose(pFile);
                return ;
            }
            char *m = NULL;
            m = map_address;
            int offset=0;
            int len=0;

            while(offset < (f_stat.st_size))//写入二级索引
            {
                fwrite(&offset, sizeof(int), 1, pFile);
                len = strlen(m) + 9;
                offset += len;
                m += len;
            }
            munmap(map_address, f_stat.st_size);
            offset = 0;
            close(fd);
            fclose(pFile);
        }
    }

    //生词相关操作

    int AddNewWord(const char* pWord)
    {
        if(NULL==pWord)
            return -1;
        string dict_path(DICTDEFAULTPATH);
        string filename=dict_path+"/New_word.data";
        if(0==access(filename.c_str(),F_OK))   //0 是否存在
        {
            ifstream fin(filename.c_str());
            if(!fin)
            {
                cerr << "Can't open the new_word.dat file!" << endl;
                return -1;            //error
            }
            while(fin)
            {
                string word;
                fin >> word;
                if(word==pWord)
                {
                    fin.close();
                    return 1;           //重复添加
                }
            }
            fin.close();
        }
        //没有相等的或者文件还没有打开
        ofstream fout(filename.c_str(),ios::app);
        if(!fout)
        {
            cerr << "Can't write the new_word.dat file!" << endl;
            return -1;            //error
        }
        //fout.seekp(ios::end);
        fout << pWord << endl;
        fout.close();
    
        return 0;
    }

       bool IsNewWordExist()
       {
            string dict_path(DICTDEFAULTPATH);
         string filename=dict_path+"/New_word.data";
         if(0==access(filename.c_str(),F_OK))   //0 是否存在
         {
                return true;
         }
            else
            {
                return false;
            }
       }
    void DeleteAllNewWords()
    {
        string dict_path(DICTDEFAULTPATH);
        string filename=dict_path+"/New_word.data";
        remove(filename.c_str());
    }

    vector<string> GetAllNewWords()
    {
        string dict_path(DICTDEFAULTPATH);
        string filename=dict_path+"/New_word.data";
        vector<string> vec_str;

        ifstream fin(filename.c_str());
        if(!fin)
        {
            cerr << "Can't open the new_word.dat file!" << endl;
            return vec_str;            
        }
           string word;
        while(fin >> word)
        {
            vec_str.push_back(word);
        }
        fin.close();
        return vec_str;
    }
}


