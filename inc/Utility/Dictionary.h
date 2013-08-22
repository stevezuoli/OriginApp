///////////////////////////////////////////////////////////////////////
// Dictionary.h
// Contact:wang mingyin
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////


#ifndef _DICTIONARY_H_
#define _DICTIONARY_H_

#include<vector>
#include<string>
using std::vector;
using std::string;


#define MAX_NAME_SIZE         128

enum DictFormart
{
	DICT_HTML,
	DICT_TEXT
};

struct Dict_Info
{
    string dictname;//字典名字
    string filename;//字典存储路径名
    DictFormart dictFormart; //字典格式
};

namespace Dictionary
{
    void LoadDict();
    void InitDict();
    int GetDictCount();
    vector<Dict_Info> GetDictInfo();
    string TranslateForFuzzyMatch(const char* pWord,int index,string *pstrOutWord);
    string Translate(const char* pWord,int iIndex);         //字典序列
    string TranslateInMultiDict(const char* pWord);
    void DeleteDict(int iIndex);

    //生词的相关操作
    int AddNewWord(const char* pWord);
       bool IsNewWordExist();
    void DeleteAllNewWords();
    vector<string> GetAllNewWords();
    
    int GetDictListFromFile();
    vector<Dict_Info> GetDictListFromHardDisk();
    string GetPrivateString(const char* pKey,const char* pFilename);
    string FindWordIndex(const char* pWord,int index,string *pstrOutWord,bool bFuzzyMatch);
    string FindMeanInDict(string indexName,int offset,int size);
    void CreateSecondIndex();
    std::vector<const char*> MemoryFind(string& indexName,const char* pIdxMapAddr,int iIdxLen,const char* pWord,bool bFuzzyMatch);
    vector<string> FindCandidateWord(const char* pWord,int iIndex,int iMaxCandidateCount);
}


#endif
