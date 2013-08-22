////////////////////////////////////////////////////////////////////////
// IDirectoryItemModel.h
// Contact: liujt
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __IDIRECTORYITEMMODEL_H__
#define __IDIRECTORYITEMMODEL_H__

#include "dkBaseType.h"
#include <string>

class IDirectoryItemModel
{
public:
    enum ModelType
    {
        MT_BOOK,
        MT_DIRECTORY,
        MT_CATEGORY
    };
    IDirectoryItemModel() {}
    virtual ~IDirectoryItemModel() {}
    virtual std::string GetItemPath() const = 0;
    virtual std::string GetItemName() const = 0;
    virtual bool SetItemName(const std::string& name) = 0;
    virtual std::string GetItemGBKDisplayName() const = 0;
    virtual std::string GetItemDisplayName() const = 0;
    virtual std::string GetFullPath() const = 0;
    virtual ModelType GetModelType() const = 0;
    virtual bool IsSelected() const = 0;
    virtual void SetSelected(bool selected) = 0;
};

#endif
