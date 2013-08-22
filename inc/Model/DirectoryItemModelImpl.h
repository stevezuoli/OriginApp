////////////////////////////////////////////////////////////////////////
// DirectoryItemModelImpl.h
// Contact: liujt
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __DIRECTORYITEMMODELIMPL_H__
#define __DIRECTORYITEMMODELIMPL_H__

#include "Model/IDirectoryItemModel.h"

class CDirectoryItemModelImpl : public IDirectoryItemModel
{
public:
    CDirectoryItemModelImpl(const char* path, const char* name, ModelType type);
    CDirectoryItemModelImpl(const char* path, ModelType type);
    virtual ~CDirectoryItemModelImpl() {}

    bool SetFullPath(const char* path);
    bool SetFullPath(const std::string& path)
    {
        return SetFullPath(path.c_str());
    }

    virtual std::string GetItemPath() const;
    virtual std::string GetItemName() const;
    virtual bool SetItemName(const std::string& name);
    virtual std::string GetItemGBKDisplayName() const;
    virtual std::string GetItemDisplayName() const;

    virtual ModelType GetModelType() const;
    virtual std::string GetFullPath() const;
    virtual bool IsSelected() const;
    virtual void SetSelected(bool selected);

private:
    std::string m_path;
    std::string m_name;
    mutable std::string m_gbkDisplayName;
    ModelType m_modelType;
    bool m_selected;
};

typedef CDirectoryItemModelImpl* PCDirectoryItemModelImpl;
#endif

