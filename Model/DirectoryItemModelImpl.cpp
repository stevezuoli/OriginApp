////////////////////////////////////////////////////////////////////////
// CDirectoryItemModelImpl.cpp
// Contact: liujt
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include <string.h>
#include "Model/DirectoryItemModelImpl.h"
#include "Utility/StringUtil.h"
#include "Utility/EncodeUtil.h"
#include "Utility/PathManager.h"
#include "Common/FileManager_DK.h"

using namespace dk::utility;
using namespace std;

CDirectoryItemModelImpl::CDirectoryItemModelImpl(const char* path, const char* name, ModelType modelType)
    : m_path(path)
    , m_name(name)
    , m_modelType(modelType)
    , m_selected(false)
{
}

CDirectoryItemModelImpl::CDirectoryItemModelImpl(const char* path, ModelType modelType)
    : m_modelType(modelType)
    , m_selected(false)
{
    if (StringUtil::IsNullOrEmpty(path))
    {
        return;
    }
    SetFullPath(path);
}

bool CDirectoryItemModelImpl::SetFullPath(const char* path)
{
    PathManager::SplitLastPath(path, &m_path, &m_name);
    return true;
}

string CDirectoryItemModelImpl::GetItemPath() const
{
    return m_path;
}

string CDirectoryItemModelImpl::GetItemName() const
{
    return m_name;
}

bool CDirectoryItemModelImpl::SetItemName(const std::string& name)
{
    if (name.empty())
        return false;
    m_name = name;
    return true;
}

string CDirectoryItemModelImpl::GetItemGBKDisplayName() const
{
    if (m_gbkDisplayName.empty())
    {
        m_gbkDisplayName= EncodeUtil::UTF8ToGBKString(GetItemDisplayName());
    }
    return m_gbkDisplayName;
}

IDirectoryItemModel::ModelType CDirectoryItemModelImpl::GetModelType() const
{
    return m_modelType;
}

std::string CDirectoryItemModelImpl::GetFullPath() const
{
    return PathManager::ConcatPath(m_path.c_str(), m_name.c_str());
}

bool CDirectoryItemModelImpl::IsSelected() const
{
    return m_selected;
}

void CDirectoryItemModelImpl::SetSelected(bool selected)
{
    m_selected = selected;
}


std::string CDirectoryItemModelImpl::GetItemDisplayName() const
{
    if (MT_CATEGORY == m_modelType)
    {
        return GetItemName();
    }
    if (MT_DIRECTORY == m_modelType)
    {
        return PathManager::GetDirectoryDisplayName(GetFullPath()); 
    }

    CDKFileManager* fileManager = CDKFileManager::GetFileManager();
    PCDKFile file = fileManager->GetFileByPath(GetFullPath());
    if (NULL == file)
    {
        return "";
    }
    file->SyncFile();
    return file->GetFileName();
}
