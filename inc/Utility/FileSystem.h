////////////////////////////////////////////////////////////////////////
// Copyright (c) Duokan Corporation. All rights reserved.
// 
////////////////////////////////////////////////////////////////////////

#ifndef __FILESYSTEM_H_
#define __FILESYSTEM_H_

#include "dkBaseType.h"
#include <vector>
#include <string>

int GetFilesInDir(std::string dir, std::vector<std::string> &files);
int GetImagesFilesInDir(std::string dir, std::vector<std::string> &files);
int GetSubDirsInDir(std::string dir, std::vector<std::string> &dirNames);
int MoveFilesInDir(std::string src_dir, std::string des_dir, std::vector<std::string>& movedFiles);
BOOL HasMusicInDir(std::string dir);
int GetBookFilesInDir(std::string dir, std::vector<std::string> &files);
std::string CheckForSameFilename(std::string _filename);
bool IsMacOSFileInfo(const char * filePath);
bool UppercaseString(const char *_pInput,int _iInputLen,char *_pOutput,int _iOutputLen);
bool IsDuoKanBookStoreBook(const std::string& path);
std::string InterceptFileName(const std::string& path);

class FileSystem
{
public:
    static bool DeleteFile(const char* fileName);
    static bool DeleteFile(const std::string& fileName)
    {
        return DeleteFile(fileName.c_str());
    }
    static bool DeleteFolder(const char* folder);
    static bool DeleteFolder(const std::string& folder)
    {
        return DeleteFolder(folder.c_str());
    }

    static bool SaveFile(const char* buffer, const int length, const char* absoluteFilePath);
    static bool SaveLocalNoteToEvernoteFile(int bookId,
                                            std::string& bookName,
                                            std::string& author);
    static bool SaveCloudNoteToEvernoteFile(const std::string& text,
                                            const std::string& bookName,
                                            const std::string& author);
    static bool IsEvernoteFileExist(const std::string& bookName,
                                    const std::string& author,
                                    bool internal);
private:
    FileSystem();
    static bool SaveToEvernoteFile(const std::string& text,
                                   const std::string& bookName,
                                   const std::string& author,
                                   bool internal);
};
#endif /*__FILESYSTEM_H_*/
