

#include<stdio.h>
#include <libxml/HTMLparser.h>
#include<libxml/xpath.h>
xmlXPathObjectPtr getNodeSet(xmlDocPtr doc, const xmlChar *szXpath)
{
    if(doc == NULL || szXpath == NULL)
    {
        return NULL;
    }
    xmlXPathContextPtr context = NULL;    //XPATH上下文指针
    xmlXPathObjectPtr result = NULL;       //XPATH对象指针，用来存储查询结果
    context = xmlXPathNewContext(doc);     //创建一个XPath上下文指针

    if (context == NULL)
    {
       return NULL;
    }

    result = xmlXPathEvalExpression(szXpath, context); //查询XPath表达式，得到一个查询结果
    xmlXPathFreeContext(context);             //释放上下文指针

    if (result == NULL)
    {
       return NULL;
    }

    if (xmlXPathNodeSetIsEmpty(result->nodesetval))   //检查查询结果是否为空
    {
       xmlXPathFreeObject(result);
       return NULL;
    }

    return result;
}

