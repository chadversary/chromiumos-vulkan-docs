// Copyright 2016 The Chromium OS Authors. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#include "vulkan.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VK_CHROMIUM_image_format_properties2
#define VK_CHROMIUM_IMAGE_FORMAT_PROPERTIES2_SPEC_VERSION 0 // TODO
#define VK_CHROMIUM_IMAGE_FORMAT_PROPERTIES2_EXTENSION_NAME "VK_CHROMIUM_image_format_properties2"

#define VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_INFO2_CHROMIUM ((VkStructureType) 0) // TODO
#define VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES2_CHROMIUM ((VkStructureType) 0) // TODO

typedef struct VkImageFormatPropertiesInfo2CHROMIUM {
    VkStructureType             sType;
    const void*                 pNext;
    VkFormat                    format;
    VkImageType                 type;
    VkImageTiling               tiling;
    VkImageUsageFlags           usage;
    VkImageCreateFlags          flags;
} VkImageFormatPropertiesInfo2CHROMIUM;

typedef struct VkImageFormatProperties2CHROMIUM {
    VkStructureType             sType;
    void*                       pNext;
    VkImageFormatProperties     imageFormatProperties;
} VkImageFormatProperties2CHROMIUM;

typedef VkResult (VKAPI_CALL *PFN_vkGetPhysicalDeviceImageFormatProperties2CHROMIUM)(VkPhysicalDevice physicalDevice, const VkImageFormatPropertiesInfo2CHROMIUM* pInfo, VkImageFormatProperties2CHROMIUM* pImageFormatProperties);

#ifndef VK_NO_PROTOTYPES
VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceImageFormatProperties2CHROMIUM(
    VkPhysicalDevice                                physicalDevice,
    const VkImageFormatPropertiesInfo2CHROMIUM*     pInfo,
    VkImageFormatProperties2CHROMIUM*               pImageFormatProperties);
#endif

#define VK_CHROMIUM_dma_buf_memory_import
#define VK_CHROMIUM_DMA_BUF_MEMORY_IMPORT_SPEC_VERSION 0 // TODO
#define VK_CHROMIUM_DMA_BUF_MEMORY_IMPORT_EXTENSION_NAME "VK_CHROMIUM_dma_buf_memory_import"

#define VK_STRUCTURE_TYPE_DMA_BUF_PROPERTIES_CHROMIUM ((VkStructureType) 0) // TODO
#define VK_STRUCTURE_TYPE_DMA_BUF_MEMORY_IMPORT_INFO_CHROMIUM ((VkStructureType) 0) // TODO

typedef struct VkDmaBufPropertiesCHROMIUM {
    VkStructureType     sType;
    const void*         pNext;
    VkDeviceSize        size;
    uint32_t            memoryTypeIndexCount;
    uint32_t            memoryTypeIndices[VK_MAX_MEMORY_TYPES];
} VkDmaBufPropertiesCHROMIUM;

// Extends VkMemoryAllocateInfo
typedef struct VkDmaBufMemoryImportInfoCHROMIUM {
    VkStructureType     sType;
    const void*         pNext;
    int                 dmaBufFd;
    VkDeviceSize        allocationOffset;
} VkDmaBufMemoryImportInfoCHROMIUM;

typedef VkResult VKAPI_PTR (*PFN_vkGetDmaBufPropertiesCHROMIUM)(VkDevice device, int dmaBufFd, VkDmaBufPropertiesCHROMIUM* pProperties);

#ifndef VK_NO_PROTOTYPES
VKAPI_ATTR VkResult VKAPI_CALL vkGetDmaBufPropertiesCHROMIUM(
    VkDevice                        device,
    int                             dmaBufFd,
    VkDmaBufPropertiesCHROMIUM*     pProperties);
#endif

#define VK_CHROMIUM_dma_buf_memory_export
#define VK_CHROMIUM_DMA_BUF_MEMORY_EXPORT_SPEC_VERSION 0 // TODO
#define VK_CHROMIUM_DMA_BUF_MEMORY_EXPORT_EXTENSION_NAME "VK_CHROMIUM_dma_buf_memory_export"

#define VK_MEMORY_PROPERTY_DMA_BUF_EXPORT_CHROMIUM ((VkMemoryPropertyFlagBits) 0) // TODO

typedef VkResult VKAPI_PTR (*PFN_vkGetMemoryDmaBufCHROMIUM)(VkDevice device, VkDeviceMemory memory, int* pDmaBufFd);

#ifndef VK_NO_PROTOTYPES
VKAPI_ATTR VkResult VKAPI_CALL vkGetMemoryDmaBufCHROMIUM(
    VkDevice            device,
    VkDeviceMemory      memory,
    int*                pDmaBufFd);
#endif

#define VK_CHROMIUM_external_drm_image
#define VK_CHROMIUM_EXTERNAL_DRM_IMAGE_SPEC_VERSION 0
#define VK_CHROMIUM_EXTERNAL_DRM_IMAGE_EXTENSION_NAME "VK_CHROMIUM_external_drm_image"

#define VK_STRUCTURE_TYPE_EXTERNAL_DRM_IMAGE_CREATE_INFO_CHROMIUM ((VkStructureType) 0) // TODO
#define VK_STRUCTURE_TYPE_EXTERNAL_DRM_IMAGE_FORMAT_PROPERTIES_CHROMIUM ((VkStructureType) 0) // TODO
#define VK_IMAGE_TILING_EXTERNAL_DRM_CHROMIUM ((VkImageTiling) 0) // TODO
#define VK_IMAGE_LAYOUT_EXTERNAL_DRM_CHROMIUM ((VkImageLayout) 0) // TODO
#define VK_ACCESS_EXTERNAL_DRM_CHROMIUM ((VkAccessFlagBits) 0) // TODO

// Extends VkImageFormatProperties2CHROMIUM
typedef struct VkExternalDrmImageFormatPropertiesCHROMIUM {
    VkStructureType     sType;
    void*               pNext;
    uint32_t            drmFormatModifierCount;
    uint64_t*           pDrmFormatModifiers;
} VkExternalDrmImageFormatPropertiesCHROMIUM;

// Extends VkImageCreateInfo
typedef struct VkExternalDrmImageCreateInfoCHROMIUM {
    VkStructureType     sType;
    const void*         pNext;
    uint64_t            drmFormatModifier;
} VkExternalDrmImageCreateInfoCHROMIUM;

#ifdef __cplusplus
}
#endif
