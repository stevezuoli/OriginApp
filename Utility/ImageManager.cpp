////////////////////////////////////////////////////////////////////////
// ImageManager.cpp
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "Utility/ImageManager.h"
#include "Utility/CImageLoader.h"
#include "GUI/ITpGraphics.h"
#include "drivers/DeviceInfo.h"
#include "interface.h"

#define RESOURCE_PATH "res/"

extern char g_szAppPath[];

std::vector<std::string> ImageManager::s_imageIds;
std::vector<SPtr<ITpImage> > ImageManager::s_images;

// TODO: Memory problems?
static std::string gpImageName[IMAGE_COUNT] =
{
#define IMAGE_LIST_NAME(CommandId, ImageNameOther, ImageNameKP) \
            ImageNameOther,
    IMAGE_LIST(IMAGE_LIST_NAME)
#undef IMAGE_LIST_NAME
};

static std::string gpKPImageName[IMAGE_COUNT] =
{
#define KP_IMAGE_LIST_NAME(CommandId, ImageNameOther, ImageNameKP) \
            ImageNameKP,
    IMAGE_LIST(KP_IMAGE_LIST_NAME)
#undef KP_IMAGE_LIST_NAME
};

SPtr<ITpImage> ImageManager::GetImage(INT32 iImageId)
{
    std::string SrcPath = GetImagePath(iImageId);
    return GetCachedImage(SrcPath);
}

SPtr<ITpImage> ImageManager::GetImage(const std::string& imageName)
{
    return GetCachedImage(imageName);
}

SPtr<ITpImage> ImageManager::GetStretchedImage(INT32 /*iImgId*/, INT32 /*iWidth*/, INT32 /*iHeight*/)
{
    TRACE("not implement");
    TP_ASSERT(FALSE);
    return SPtr<ITpImage>();
//    return GetCachedStretchedImage(iImgId, iWidth, iHeight);
}

SPtr<ITpImage> ImageManager::GetCachedImage(const std::string& imageName)
{
    SPtr<ITpImage> spCachedImage = FindCachedImage(imageName);
    if (spCachedImage)
    {
        return spCachedImage;
    }
    else
    {
//TODO:Load defualt Image
        if (imageName.empty())
        {
            return SPtr<ITpImage>();
        }
        SPtr<ITpImage> spImage = CImageLoader::GetImage(imageName.c_str());
        AddCachedImage(imageName, spImage);
        return spImage;
    }
}

SPtr<ITpImage> ImageManager::GetCachedStretchedImage(INT32 /*iImgId*/, INT32 /*iWidth*/, INT32 /*iHeight*/)
{
    TRACE("not implement");
    TP_ASSERT(FALSE);
    return SPtr<ITpImage>();
}

void ImageManager::AddCachedImage(const std::string& rstrImageId, SPtr<ITpImage> spImage)
{
    if (spImage)
    {
        s_imageIds.push_back(rstrImageId);
        s_images.push_back(spImage);
    }
    return;
}

SPtr<ITpImage> ImageManager::FindCachedImage(const std::string& rstrImageId)
{
   for (size_t i = 0; i < s_imageIds.size(); i++)
    {
        if (s_imageIds[i] == rstrImageId)
        {
            return s_images[i];
        }
    }

    return SPtr<ITpImage>();
}

void ImageManager::FreeCachedImages()
{
//    TRACE("not implement");
//    TP_ASSERT(FALSE);
//    return ;
    for (size_t i = 0; i < s_images.size(); i++)
    {
        s_images[i] = SPtr<ITpImage>();
    }

    s_images.clear();
    s_imageIds.clear();
}

std::string ImageManager::GetImagePath(INT32 iImageId)
{
    std::string str1(g_szAppPath);
    if (DeviceInfo::IsKPW())
    {
        const std::string& strImgName = gpKPImageName[iImageId];
        if (!strImgName.empty() > 0)
        {
            return str1 + RESOURCE_PATH + strImgName;
        }
    }

    return str1 + RESOURCE_PATH + gpImageName[iImageId];
}

