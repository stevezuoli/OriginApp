#ifndef EBOOK_QIYITEST_DK_XMLPARSE_H
#define EBOOK_QIYITEST_DK_XMLPARSE_H
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>

xmlXPathObjectPtr getNodeSet(xmlDocPtr doc, const xmlChar *szXpath);

#endif
