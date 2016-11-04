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

#define VK_CHROMIUM_dma_buf_memory_import
#define VK_CHROMIUM_DMA_BUF_MEMORY_IMPORT_SPEC_VERSION 0 // TODO
#define VK_CHROMIUM_DMA_BUF_MEMORY_IMPORT_EXTENSION_NAME "VK_CHROMIUM_dma_buf_memory_import"

#define VK_STRUCTURE_TYPE_DMA_BUF_MEMORY_IMPORT_INFO_CHROMIUM ((VkStructureType) 0) // TODO

typedef struct VkDmaBufMemoryImportInfoCHROMIUM {
    VkStructureType                             sType;
    const void*                                 pNext;
    int                                         fd;
    VkDeviceSize                                size;
} VkDmaBufMemoryImportInfoCHROMIUM;

#ifndef VK_NO_PROTOTYPES
VKAPI_ATTR VkResult VKAPI_CALL vkImportDmaBufMemoryCHROMIUM(
    VkDevice                                    device,
    const VkDmaBufMemoryImportInfoCHROMIUM*     pImportInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDeviceMemory*                             pMemory);
#endif

#define VK_CHROMIUM_drm_external_image 0
#define VK_CHROMIUM_DRM_EXTERNAL_IMAGE_SPEC_VERSION 0
#define VK_CHROMIUM_DRM_EXTERNAL_IMAGE_EXTENSION_NAME "VK_CHROMIUM_drm_external_image"

#define VK_STRUCTURE_TYPE_DRM_EXTERNAL_IMAGE_ABI_CHROMIUM ((VkStructureType) 0) // TODO
#define VK_STRUCTURE_TYPE_DRM_EXTERNAL_IMAGE_CREATE_INFO_CHROMIUM ((VkStructureType) 0) // TODO
#define VK_STRUCTURE_TYPE_DRM_EXTERNAL_IMAGE_LAYOUT_CHROMIUM ((VkStructureType) 0) // TODO
#define VK_IMAGE_TILING_DRM_EXTERNAL_CHROMIUM ((VkImageTiling) 0) // TODO
#define VK_IMAGE_LAYOUT_DRM_EXTERNAL_CHROMIUM ((VkImageLayout) 0) // TODO
#define VK_ACCESS_DRM_EXTERNAL_CHROMIUM ((VkAccessFlagBits) 0) // TODO

typedef struct VkDrmExternalImageAbiCHROMIUM {
    VkStructureType                             sType;
    const void*                                 pNext;
    uint32_t                                    vendor;
    uint32_t                                    version;

} VkDrmExternalImageAbiCHROMIUM;

typedef struct VkDrmExternalImageLayoutCHROMIUM {
    VkStructureType                             sType;
    const void*                                 pNext;
    const VkDrmExternalImageAbiCHROMIUM*        abi;
    uint32_t                                    drmFourCC;
    uint64_t                                    drmModifier;
} VkDrmExternalImageLayoutCHROMIUM;

// Extends VkImageCreateInfo
typedef struct VkDrmExternalImageCreateInfoCHROMIUM {
    VkStructureType                             sType;
    const void*                                 pNext;
    const VkDrmExternalImageLayoutCHROMIUM*     externalLayout;
} VkDrmExternalImageCreateInfoCHROMIUM;

// TODO: Functions that query support for external images and layouts.

#ifdef __cplusplus
}
#endif
