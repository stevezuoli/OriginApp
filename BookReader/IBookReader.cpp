#include "BookReader/IBookReader.h"
#include "BookReader/TextBookReader.h"
#include "BookReader/EpubBookReader.h"
#include "BookReader/MobiBookReader.h"
#include "BookReader/PdfBookReader.h"
IBookReader* IBookReader::CreateBookReaderFromFormat(DkFileFormat fileFormat)
{
	switch (fileFormat)
	{
	case DFF_Text:
		return new TextBookReader();
	case DFF_ElectronicPublishing:
		return new EpubBookReader();
	case DFF_MobiPocket:
		return new MobiBookReader();
	case DFF_PortableDocumentFormat:
		return new PdfBookReader();
	default:
        return NULL;
	}
}

const char* UIBookParseStatusListener::EventBookParseFinished = "EventBookParseFinished";
UIBookParseStatusListener* UIBookParseStatusListener::GetInstance()
{
    static UIBookParseStatusListener listner;
    return &listner;
}

void  UIBookParseStatusListener::FireBookParseFinishedEvent()
{
    ParseStatusEventArgs args;
    FireEvent(EventBookParseFinished, args);
}
