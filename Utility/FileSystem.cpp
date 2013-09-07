#include "Utility/FileSystem.h"
#include "Utility/PathManager.h"
#include "Utility/ReaderUtil.h"
#include "BookStore/BookStoreInfoManager.h"
#include "Common/FileManager_DK.h"
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <iostream>
#include <cctype>
#include <unistd.h>
#include <sstream>
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include "interface.h"

using namespace std;
using namespace dk::utility;

#define THUMB_DB "Thumbs.db"
#define PARENT_DIR '.'
#define TOUCH_DATA_DIR ".sdr"

int GetFilesInDir(string dir, vector<string> &files)
{
    DIR *dp = NULL;
    struct dirent *dirp = NULL;
    files.clear();
    if((dp  = opendir(dir.c_str())) == NULL) {
        return 0;
    }

    while ((dirp = readdir(dp)) != NULL)
    if (DT_REG == dirp->d_type)
    {
        files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return files.size();
}


int GetImagesFilesInDir(string dir, vector<string> &files)
{
    static const string FILEEXT[5] = {".JPEG",".JPG",".GIF",".PNG",".BMP"};
    
    DIR *dp = NULL;
    struct dirent *dirp = NULL;
    files.clear();

    if((dp  = opendir(dir.c_str())) == NULL) {
        return 0;
    }

    while ((dirp = readdir(dp)) != NULL)
        if (DT_REG == dirp->d_type)
        {
            string filename(dirp->d_name);
            transform(filename.begin(), filename.end(), filename.begin(), ::toupper);
									
			if (IsMacOSFileInfo(filename.c_str()))
			{
				continue;
			}

            for (int i =0; i<5; i++)
            {
                if (filename.rfind(FILEEXT[i]) != string::npos)
                    {
                        files.push_back(dirp->d_name);
                        break;
                    }
            }
        }

    closedir(dp);
    return files.size();
}

BOOL HasMusicInDir(string dir)
{
    static const string FILEEXT[13] = {".MP3",".WAV",".WMA", ".AIFF", ".RA", ".AU", ".RM", ".AAC", ".AC3", ".MKA", ".M4A", ".MP2", ".OGG"};
    
    DIR *dp = NULL;
    struct dirent *dirp = NULL;

    if((dp  = opendir(dir.c_str())) == NULL) {
        return 0;
    }

    while ((dirp = readdir(dp)) != NULL)
        if (DT_REG == dirp->d_type)
        {
            string filename(dirp->d_name);
						
			if (IsMacOSFileInfo(filename.c_str()))
			{
				continue;
			}

            for (int i =0; i<13; i++)
            {
                UINT uLen = FILEEXT[i].size();
                if(uLen > filename.size())
                {
                    continue;
                }
                string suffix = filename.substr(filename.size()-uLen, uLen);
                transform(suffix.begin(), suffix.end(), suffix.begin(), ::toupper);
                if (suffix == FILEEXT[i])
                {
                    closedir(dp);
                    return TRUE;
                }
            }
        }

    closedir(dp);
    return FALSE;
}


int GetBookFilesInDir(string dir, vector<string> &files)
{
    static const string  FILEEXT[6]= {".EPUB",".PDF",".TXT",".ZIP",".RAR",".MOBI"};
    
    DIR *dp = NULL;
    struct dirent *dirp = NULL;
    files.clear();

    if((dp  = opendir(dir.c_str())) == NULL) {
        return 0;
    }

    while ((dirp = readdir(dp)) != NULL)
        if (DT_REG == dirp->d_type)
        {
            string filename(dirp->d_name);
									
			if (IsMacOSFileInfo(filename.c_str()))
			{
				continue;
			}

            for (int i =0; i<6; i++)
            {
                UINT uLen = FILEEXT[i].size();
                if(uLen > filename.size())
                {
                    continue;
                }

                string suffix = filename.substr(filename.size()-uLen, uLen);
                transform(suffix.begin(), suffix.end(), suffix.begin(), ::toupper);
                if (suffix == FILEEXT[i])
                {
                    files.push_back(filename);
                    break;
                }
            }
        }

    closedir(dp);
    sort(files.begin(), files.end());
    return files.size();
}

//将给定的小写字符串转化为大写字符串，只限于英文字母的转化。

bool UppercaseString(const char *_pInput, int _iInputLen, char *_pOutput, int _iOutputLen)
{
    if(_pInput == NULL || _pOutput == NULL || _iInputLen <= 0 || _iOutputLen <= 0)
    {
        return false;
    }
    int iMaxLen = _iInputLen > (_iOutputLen - 1) ? (_iOutputLen - 1) : _iInputLen;
    int i = 0;
    while(*_pInput && i < iMaxLen)
    {
        if(*_pInput >= 'a' && *_pInput <= 'z')
        {
            *_pOutput = *_pInput - 32;
        }
        else
        {
            *_pOutput = *_pInput;
        }
        _pOutput++;
        _pInput++;
        i++;
    }
    *_pOutput = 0;
    return true;
}

string CheckForSameFilename(string _filename)
{
        DebugPrintf(DLC_XU_KAI,"_filename is %s",_filename.c_str());
        string _testname = _filename;
        int _tmpoffset = _testname.find_last_of('.');
        int count = 0;
        string _tmpadd = "";
        stringstream spd;
        DebugPrintf(DLC_XU_KAI,"_testname.c_str() is %s",_testname.c_str());
        while(!access(_testname.c_str(),0))
        {
            spd.clear();
            spd.str("");
            spd << "(" << count << ")";
            _tmpadd = spd.str();
            _testname = _filename;
            _testname.insert(_tmpoffset,_tmpadd);
            count++;
        }

        DebugPrintf(DLC_XU_KAI,"_testname.c_str() 22 is %s",_testname.c_str());
        return _testname;
}

int MoveFilesInDir(std::string src_dir, std::string des_dir, std::vector<std::string>& movedFiles)
{
    DIR *dp = NULL;

    if((dp  = opendir(src_dir.c_str())) == NULL) {
        return 0;
    }
    closedir(dp);

    if((dp  = opendir(des_dir.c_str())) == NULL) {
        int status = mkdir(des_dir.c_str(), S_IRWXO | S_IRWXG | S_IRWXU);
        if ( 0 != status)
            return 0;
        else if((dp  = opendir(des_dir.c_str())) == NULL) 
        {
            return 0;
        }
    }
    closedir(dp);

    if ( src_dir[ src_dir.size() -1 ]  != '/' )
        src_dir.append("/");

    if ( des_dir[ des_dir.size() -1 ] != '/')
        des_dir.append("/");

    int move_count =0;
     vector<string> files;
     movedFiles.clear();
    if (GetBookFilesInDir(src_dir, files) >0)
    {
        for(unsigned int i=0; i< files.size();i++)
        {
            string filename = files[i];
            string sourcename = src_dir + filename;
            string targetname = des_dir + filename;

            int tmp =0;
            while ( access(targetname.c_str(), F_OK) ==0 )
            {
                tmp++;
                std::string tempfilename = filename;
                std::string key = ".";
                size_t found=tempfilename.rfind(key);

                std::stringstream ss;
                ss << tmp;

                std::string tmpNString;
                ss >> tmpNString;

                if (found !=string::npos)
                    tempfilename.replace (found,key.length(), std::string("_") + tmpNString + key);
                targetname = des_dir + tempfilename;
            }

            if (0 == rename(sourcename.c_str(), targetname.c_str()))
                movedFiles.push_back(targetname);
                move_count++;
            }
    }

    return move_count;
}

bool IsMacOSFileInfo(const char * filePath)
{
    LPCSTR lpBaseName =NULL;
    lpBaseName = strrchr(filePath , '/');
    if (lpBaseName)
    {
        lpBaseName++;
    }
    else
    {
        lpBaseName = filePath;
    }
    
    if (strlen(lpBaseName) > 2 && lpBaseName[0] == '.' && lpBaseName[1] == '_')
    {
        return true;
    }
    return false;
}

bool IsDuoKanBookStoreBook(const string& path)
{
    if (PathManager::IsUnderBookStore(path.c_str()))
    {
        string bookId = InterceptFileName(path);
        dk::bookstore::BookStoreInfoManager *pManager =  dk::bookstore::BookStoreInfoManager::GetInstance();
        if (!bookId.empty() && 
            !pManager->GetLocalFileTitle(bookId.c_str()).empty())
        {
            return true;
        }
    }
    return false;
}

string InterceptFileName(const string& path)
{
    size_t pos = path.rfind('/');
    string fileName(path);
    if(pos != string::npos)
    {
        fileName = path.substr(pos + 1);
    }
    pos = fileName.find('.');
    if (pos != string::npos)
    {
        fileName = fileName.substr(0,pos);
    }
    return fileName;
}

bool FileSystem::DeleteFile(const char* fileName)
{
    return unlink(fileName) == 0;
}

bool FileSystem::DeleteFolder(const char* fileName)
{
    return rmdir(fileName) == 0;
}

static bool isInvalidFileNameCharactor(unsigned int c)
{
    return (c == ';' ||
            c == '%' ||
            c == ',' ||
            c == ':' ||
            c == '\''||
            c == '/' ||
            c == '&' ||
            c == '\n'||
            c == '\r');
}

static std::string FilterFileName(const char* s)
{
    if (NULL == s)
    {
        return "";
    }

    const unsigned char* us = (const unsigned char*)s;
    std::string result;
    while (unsigned int c = *us++)
    {
        if (!isInvalidFileNameCharactor(c))
        {
            result.push_back(c);
        }
        else
        {
            result.push_back(' ');
        }  
    }
    return result;
}

bool FileSystem::SaveLocalNoteToEvernoteFile(int bookId, std::string& bookName, std::string& author)
{
    CDKFileManager *pFileManager = CDKFileManager::GetFileManager();
    PCDKFile pDkFile = pFileManager->GetFileById(bookId);
    if(0 == pDkFile)
    {
        return false;
    }

    std::string localBookID = pDkFile->GetBookID();
    ReadingBookInfo readingInfo;
    if (ReaderUtil::GenerateReadingBookInfo(localBookID.c_str(), &readingInfo))
    {
        bookName = pDkFile->GetFileName();
        author = pDkFile->GetFileArtist();
        //DebugPrintf(DLC_DIAGNOSTIC, "SaveLocalNoteToEvernoteFile: %s, %s", bookName.c_str(), author.c_str());
        return SaveToEvernoteFile(readingInfo.ToPostData(), bookName, author, true);
    }

    return false;
}

bool FileSystem::SaveCloudNoteToEvernoteFile(const std::string& text,
                                            const std::string& bookName,
                                            const std::string& author)
{
    return SaveToEvernoteFile(text, bookName, author, false);
}

bool FileSystem::SaveToEvernoteFile(const std::string& text,
                                    const std::string& bookName,
                                    const std::string& author,
                                    bool internal)
{
    std::string filteredBookName = FilterFileName(bookName.c_str());
    std::string filteredAuthor   = FilterFileName(author.c_str());
    std::string fileName = internal ? PathManager::GetEvernoteInternalPath() : PathManager::GetEvernotePath();
    fileName.append(1, '/');
    fileName.append(filteredBookName).append(1, '_').append(filteredAuthor).append(".xml");
    if (PathManager::MakeDirIfEvernoteDirNotExisting())
    {
        return SaveFile(text.c_str(), text.length(), fileName.c_str());
    }
}

bool FileSystem::IsEvernoteFileExist(const std::string& bookName,
                                     const std::string& author,
                                     bool internal)
{
    std::string fileName = internal ? PathManager::GetEvernoteInternalPath() : PathManager::GetEvernotePath();
    fileName.append(1, '/');
    fileName.append(bookName).append(1, '_').append(author).append(".xml");
    return PathManager::IsFileExisting(fileName);
}

bool FileSystem::SaveFile(const char* buffer, const int length, const char* absoluteFilePath)
{
    if (buffer && absoluteFilePath && length>0)
    {
        FILE* fp = fopen(absoluteFilePath, "w");
        if (!fp)
        {
            DebugPrintf(DLC_DIAGNOSTIC, "FileSystem::SaveFile error[%s]", strerror(errno));
            return false;
        }

        size_t writeLength = fwrite(buffer, 1, length, fp);
        if (writeLength != (unsigned int)length)
        {
            fclose(fp);
            DebugPrintf(DLC_DIAGNOSTIC, "FileSystem::SaveFile error[writeLength: %d, expectedLength: %d]", writeLength, length);
            return false;
        }

        fclose(fp);
        return true;
    }

    return false;
}