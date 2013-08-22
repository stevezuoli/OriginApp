#include "DKXManager/DKX/DKXManager.h"
#include "DKXManager/dkx/DKXReadingDataItemFactory.h"
#include<iostream.h>
#include <vector>
using namespace std;

void WriteADkx()
{
    printf("begin 111111111111111111\n");
    string strBook1 = "a.txt";
    DKXManager *TmpManager = DKXManager::GetInstance();
    // add book information
    printf("begin 2222222222222\n");
    TmpManager->SetBookFilePath(strBook1);
    printf("begin 33333333333333333\n");
    TmpManager->SetBookFileType(strBook1,"txt");
    TmpManager->SetBookName(strBook1,"a.txt");
    TmpManager->SetBookLastReadTime(strBook1,"1970-01-01");
    TmpManager->SetBookReadingOrder(strBook1,-1);
    TmpManager->SetBookAuthor(strBook1,"bbb");
    TmpManager->SetBookCover(strBook1,"/mnt/us/a.png");
    TmpManager->SetBookSource(strBook1,"xinlang");
    TmpManager->SetBookUrl(strBook1,"www.baidu.com");
    TmpManager->SetBookAbstract(strBook1,"weoiiddddddddddddddddddddddddddddddddddaaaaaaaaaaaaaaaaaaaaaa");
    TmpManager->SetBookCreateTime(strBook1,"2010-02-13");
    TmpManager->SetBookAddOrder(strBook1,-1);
    TmpManager->SetBookISBN(strBook1,"111111111");
    TmpManager->SetBookSize(strBook1,"1.1M");
    vector<string> vTags;
    vTags.push_back("A");
    vTags.push_back("C");
    vTags.push_back("D");
    vTags.push_back("B");
    TmpManager->SetFileTag(strBook1,vTags);
    printf("begin 44444444444444444\n");
    
    CT_RefPos clsPos;
    clsPos.SetAtomIndex(100000);
    CT_ReadingDataItemImpl clsDataItem;
    clsDataItem.SetBeginPos(clsPos);
    clsDataItem.SetEndPos(clsPos);

    ICT_ReadingDataItem * pItem = DKXReadingDataItemFactory::CreateReadingDataItem(ICT_ReadingDataItem::BOOKMARK);
    printf("GenerateGUID 11111111111111 : %s\n",pItem->GetID().c_str());
    pItem->SetCreateTime("2010-10-10");
    pItem->SetLastModTime("2010-10-10");
    pItem->SetBookContent("22222222222222222222222222222");
    TmpManager->AddBookmark(strBook1,pItem);
    DKXReadingDataItemFactory::DestoryReadingDataItem(pItem);
    
    pItem = DKXReadingDataItemFactory::CreateReadingDataItem(ICT_ReadingDataItem::COMMENT);
    printf("GenerateGUID 2222222222 : %s\n",pItem->GetID().c_str());
    pItem->SetCreateTime("2010-10-10");
    pItem->SetLastModTime("2010-10-10");
    pItem->SetBookContent("3333333333333333333333333");
    pItem->SetUserContent("aaaaaaaaaaaaaaaaaaaaaaa");
    TmpManager->AddBookmark(strBook1,pItem);
    DKXReadingDataItemFactory::DestoryReadingDataItem(pItem);

    pItem = DKXReadingDataItemFactory::CreateReadingDataItem(ICT_ReadingDataItem::DIGEST);
    printf("GenerateGUID 33333333333333 : %s\n",pItem->GetID().c_str());
    pItem->SetCreateTime("2010-10-10");
    pItem->SetLastModTime("2010-10-10");
    pItem->SetBookContent("4444444444444444444444");
    pItem->SetUserContent("bbbbbbbbbbbbbbbbbbbbbbbbbbbb");
    TmpManager->AddBookmark(strBook1,pItem);
    DKXReadingDataItemFactory::DestoryReadingDataItem(pItem);

    TmpManager->SetProgress(strBook1,&clsDataItem);
    TmpManager->SetPDFSettingFieldValue(strBook1,"xuanzhuanjiaodu","90");
    TmpManager->SetPDFSettingFieldValue(strBook1,"xuanzhuanmoshi","1");
    TmpManager->SetPDFSettingFieldValue(strBook1,"wenbenmoshi","0");
    TmpManager->SaveCurDkx();

    DKXManager *TmpManager1 = DKXManager::GetInstance(); 
    printf("5555555555555555555555555555\n");
    CT_ReadingDataItemImpl clsDataItem1;
    TmpManager1->FetchProgress(strBook1,&clsDataItem1);

    CT_RefPos clsPos1 = clsDataItem1.GetBeginPos();
    int iAtom = clsPos.GetAtomIndex();
    printf("ATom is %d\n",iAtom);
    printf("6666666666666666666666666\n");
}

void ReadADkx()
{
    string strBook1 = "a.txt";
    DKXManager TmpManager;

    cout << TmpManager.FetchBookFilePath(strBook1) << endl;
    cout << TmpManager.FetchBookFileType(strBook1) << endl;
    cout << TmpManager.FetchBookName(strBook1) << endl;
    cout << TmpManager.FetchBookAuthor(strBook1) << endl;
    cout << TmpManager.FetchBookCover(strBook1) << endl;
    cout << TmpManager.FetchBookSource(strBook1) << endl;
    cout << TmpManager.FetchBookUrl(strBook1) << endl;
    cout << TmpManager.FetchBookAbstract(strBook1) << endl;
    cout << TmpManager.FetchBookLatestChapter(strBook1) << endl;


}


void WriteAzipDkxForB()
{
    string strBook1 = "a.zip/b.txt";
    DKXManager TmpManager;

    TmpManager.SetBookFilePath(strBook1);
    TmpManager.SetBookFileType(strBook1,"txt");
    TmpManager.SetBookName(strBook1,"b.txt");
    TmpManager.SetBookAuthor(strBook1,"zhangzhangxiaoxiao");
    TmpManager.SetBookCover(strBook1,"/mnt/us/b.jpg");
    TmpManager.SetBookSource(strBook1,"xiaomi");
    TmpManager.SetBookUrl(strBook1,"www.163.com");
    TmpManager.SetBookAbstract(strBook1,"rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr");
    TmpManager.SetBookLatestChapter(strBook1,98);



    printf("5555555555555555555555555555\n");
    TmpManager.SaveCurDkx();
    printf("6666666666666666666666666\n");
}

void WriteAzipDkxForC()
{
    string strBook1 = "a.zip/c.epub";
    DKXManager TmpManager;

    TmpManager.SetBookFilePath(strBook1);
    TmpManager.SetBookFileType(strBook1,"epub");
    TmpManager.SetBookName(strBook1,"c.epub");
    TmpManager.SetBookAuthor(strBook1,"jinyo");
    TmpManager.SetBookCover(strBook1,"/mnt/us/c.png");
    TmpManager.SetBookSource(strBook1,"baidu");
    TmpManager.SetBookUrl(strBook1,"www.126.com");
    TmpManager.SetBookAbstract(strBook1,"tttttttttttttttttttttttttttttttttt");


    // add book mark
    printf("5555555555555555555555555555\n");
    TmpManager.SaveCurDkx();
    printf("6666666666666666666666666\n");
}

int main()
{
    WriteADkx();
    //ReadADkx();
    //WriteAzipDkxForB();
//    WriteAzipDkxForC();
}