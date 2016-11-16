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

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <drm/drm_fourcc.h>
#include <vulkan/vk_chromium.h>

#define unused __attribute__((unused))

#define ARRAY_LENGTH(a) (sizeof(a) / sizeof(a[0]))

#define for_each_bit(b, bits) \
    for (__typeof__(bits) __bits = (bits); \
         (b) = __builtin_ffs(__bits) - 1, __bits != 0; \
         __bits &= ~(1 << (b)))

static void
checkVkResult(VkResult result)
{
    if (result != VK_SUCCESS)
        abort();
}

static void
drawStuff(VkCommandBuffer commandBuffer)
{
    // STUB
}

static VkResult unused
exampleImportDmaBufMemory(
        VkDevice device,
        int dmaBufFd,
        VkDeviceMemory *pMemory,
        uint32_t *pMemoryTypeIndex,
        VkDeviceSize *pMemorySize)
{
    VkResult result;

    // Query the dma_buf's size and the set of VkMemoryType it supports.
    VkDmaBufPropertiesCHROMIUM dmaBufProperties = {
        .sType = VK_STRUCTURE_TYPE_DMA_BUF_PROPERTIES_CHROMIUM,
        .pNext = NULL,
        // Remainder will be filled by vkDeviceGetDmaBufMemoryTypeProperties.
    };

    result = vkGetDmaBufPropertiesCHROMIUM(device, dmaBufFd,
                                           &dmaBufProperties);
    if (result != VK_SUCCESS)
        return result;

    assert(dmaBufProperties.memoryTypeIndexCount > 0);

    // Print info about dma_buf.
    printf("dma_buf size: %lu\n", dmaBufProperties.size);
    for (uint32_t i = 0; i < dmaBufProperties.memoryTypeIndexCount; ++i) {
        printf("dma_buf supports memory type index %u\n",
               dmaBufProperties.memoryTypeIndices[i]);
    }

    // When importing a dma_buf as VkDeviceMemory, it must be imported
    // with a VkMemoryType returned by vkDeviceGetDmaBufPropertiesCHROMIUM.
    // The resultant VkDeviceMemory will span the dma_buf's address range
    // given by VkDmaBufMemoryImportInfoCHROMIUM::allocationOffset and
    // VkMemoryAllocateInfo::allocationSize. `offset + size` must not exceed
    // VkDmaBufPropertiesCHROMIUM::size.
    VkMemoryAllocateInfo allocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .memoryTypeIndex = dmaBufProperties.memoryTypeIndices[0],
        .allocationSize = dmaBufProperties.size,
        .pNext =
            &(VkDmaBufMemoryImportInfoCHROMIUM) {
                .sType = VK_STRUCTURE_TYPE_DMA_BUF_MEMORY_IMPORT_INFO_CHROMIUM,
                .pNext = NULL,
                .dmaBufFd = dmaBufFd,
                .allocationOffset = 0,
            },
    };

    result = vkAllocateMemory(device,
            &allocateInfo,
            /*pAllocator*/ NULL,
            pMemory);
    if (result != VK_SUCCESS)
        return result;

    *pMemoryTypeIndex = allocateInfo.memoryTypeIndex;
    *pMemorySize = allocateInfo.allocationSize;
    return VK_SUCCESS;
}

static VkResult unused
exampleGetDrmFormatModifierCount(
        VkPhysicalDevice physicalDevice,
        uint32_t *pCount)
{
    VkResult result;

    VkDmaBufImageFormatPropertiesCHROMIUM dmaBufProperties = {
        .sType = VK_STRUCTURE_TYPE_DMA_BUF_IMAGE_FORMAT_PROPERTIES_CHROMIUM,
        .pNext = NULL,

        // Parameters 'drmFormatModifierCount' and 'pDrmFormatModifiers' behave
        // similarly to other array-length/array pairs in the core Vulkan API,
        // such as that in vkEnumeratePhysicalDevices.
        .drmFormatModifierCount = 0,
        .pDrmFormatModifiers = NULL,
    };

    VkImageFormatProperties2CHROMIUM baseProperties = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES2_CHROMIUM,
        .pNext = &dmaBufProperties,
    };

    // This example hard-codes the query's assertions (format, usage, etc). In
    // real usage, the query's assertions would be application-dependent.
    //
    // When querying VkDmaBufImageFormatPropertiesCHROMIUM, the given
    // tiling must be VK_IMAGE_TILING_EXTERNAL_DMA_BUF_CHROMIUM.
    result = vkGetPhysicalDeviceImageFormatProperties2CHROMIUM(
            physicalDevice,
            &(VkImageFormatPropertiesInfo2CHROMIUM) {
                .sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_INFO2_CHROMIUM,
                .pNext = NULL,
                .format = VK_FORMAT_R8G8B8A8_UNORM,
                .type = VK_IMAGE_TYPE_2D,
                .tiling = VK_IMAGE_TILING_EXTERNAL_DMA_BUF_CHROMIUM,
                .usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                         VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                         VK_IMAGE_USAGE_SAMPLED_BIT |
                         VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                .flags = 0,
            },
            &baseProperties);
    if (result < 0)
        return result;

    *pCount = dmaBufProperties.drmFormatModifierCount;
    return VK_SUCCESS;
}

static VkResult unused
exampleAllocateAndExportDmaBuf(
        VkPhysicalDevice physicalDevice,
        VkDevice device,
        VkMemoryPropertyFlags requiredFlags,
        VkDeviceSize allocationSize,
        VkDeviceMemory *pMemory,
        int *pDmaBufFd)
{
    VkResult result;
    VkPhysicalDeviceMemoryProperties memoryProperties;
    uint32_t memoryTypeIndex = UINT32_MAX;

    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    // Choose a memory type that supports dma_buf-export as well the given required flags.
    requiredFlags &= VK_MEMORY_PROPERTY_DMA_BUF_EXPORT_CHROMIUM;

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
        VkMemoryPropertyFlags actualFlags =
            memoryProperties.memoryTypes[i].propertyFlags;

        if ((actualFlags & requiredFlags) == requiredFlags) {
            memoryTypeIndex = i;
            break;
        }
    }

    if (memoryTypeIndex == UINT32_MAX)
        return VK_ERROR_FEATURE_NOT_PRESENT;

    result = vkAllocateMemory(device,
            &(VkMemoryAllocateInfo) {
                .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                .pNext = NULL,
                .allocationSize = allocationSize,
                .memoryTypeIndex = memoryTypeIndex,
            },
            /*pAllocator*/ NULL,
            pMemory);
    if (result < 0)
        return result;

    result = vkGetMemoryDmaBufCHROMIUM(device, *pMemory, pDmaBufFd);
    if (result < 0)
        goto fail;

    return VK_SUCCESS;

  fail:
    vkFreeMemory(device, *pMemory, /*pAllocator*/ NULL);
    return result;
}

static VkResult unused
examplePrintDmaBufImageProperties(VkPhysicalDevice physicalDevice)
{
    VkResult result;

    // If the number available drm modifier count exceeds the array length,
    // vkGetPhysicalDeviceImageFormatProperties2CHROMIUM will return
    // VK_INCOMPLETE. Instead of hard-coding an array length, we could have
    // first queried the available modifier count and allocated
    // accordingly; see exampleGetDrmFormatModifierCount.
    uint64_t drmFormatModifiers[64];

    VkDmaBufImageFormatPropertiesCHROMIUM dmaBufProperties = {
        .sType = VK_STRUCTURE_TYPE_DMA_BUF_IMAGE_FORMAT_PROPERTIES_CHROMIUM,
        .pNext = NULL,

        // Parameters 'drmFormatModifierCount' and 'pDrmFormatModifiers' behave
        // similarly to other array-length/array pairs in the core Vulkan API,
        // such as that in vkEnumeratePhysicalDevices.
        .drmFormatModifierCount = ARRAY_LENGTH(drmFormatModifiers),
        .pDrmFormatModifiers = drmFormatModifiers,
    };

    VkImageFormatProperties2CHROMIUM baseProperties = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES2_CHROMIUM,
        .pNext = &dmaBufProperties,
        // Remainder will be filled by
        // vkGetPhysicalDeviceImageFormatProperties2CHROMIUM.
    };

    // This example hard-codes the query's assertions (format, usage, etc). In
    // real usage, the query's assertions would be application-dependent.
    //
    // When querying VkDmaBufImageFormatPropertiesCHROMIUM, the given
    // tiling must be VK_IMAGE_TILING_EXTERNAL_DMA_BUF_CHROMIUM.
    result = vkGetPhysicalDeviceImageFormatProperties2CHROMIUM(
            physicalDevice,
            &(VkImageFormatPropertiesInfo2CHROMIUM) {
                .sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_INFO2_CHROMIUM,
                .pNext = NULL,
                .format = VK_FORMAT_R8G8B8A8_UNORM,
                .type = VK_IMAGE_TYPE_2D,
                .tiling = VK_IMAGE_TILING_EXTERNAL_DMA_BUF_CHROMIUM,
                .usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                         VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                         VK_IMAGE_USAGE_SAMPLED_BIT |
                         VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                .flags = 0,
            },
            &baseProperties);
    if (result < 0)
        return result;

    printf("max width: %u\n", baseProperties.imageFormatProperties.maxExtent.width);
    printf("max height: %u\n", baseProperties.imageFormatProperties.maxExtent.height);
    printf("max depth: %u\n", baseProperties.imageFormatProperties.maxExtent.depth);
    printf("max miplevels: %u\n", baseProperties.imageFormatProperties.maxMipLevels);
    printf("max array layers: %u\n", baseProperties.imageFormatProperties.maxArrayLayers);

    uint32_t b;
    printf("samples:");
    for_each_bit(b, baseProperties.imageFormatProperties.sampleCounts) {
        printf(" %u", 1 << b);
    }
    printf("\n");

    printf("drm modifiers:");
    for (uint32_t i = 0; i < dmaBufProperties.drmFormatModifierCount; ++i) {
        printf(" 0x%08lx", dmaBufProperties.pDrmFormatModifiers[i]);
    }
    if (result == VK_INCOMPLETE) {
        printf(" ...");
    }
    printf("\n");

    return result;
}

static void unused
exampleBindDmaBufImage(
        VkDevice device,
        VkDeviceMemory dmaBufMemory,
        VkImage *pDmaBufImage)
{
    VkResult result;

    // This example hard-codes the image's drm format modifier.  In real usage,
    // the drm format modifier would be negotiated between the consumer and
    // producer during an initial setup phase.
    //
    // Observe that this struct omits many details that would be needed to
    // define the memory layout of a complex (mipmapped, array, multisampled,
    // etc) image. It merely defines a drm format modifier per image, and
    // offset and pitch per plane. That information should be sufficient to
    // define the memory layout of simple (non-mipmapped, non-array,
    // single-sampled, etc) image. The larger problem of how to import the
    // memory layout of complex images during vkCreateImage is not solved here;
    // that feature is deferred to future extensions.
    // memory layout of imported images
    //
    const VkDmaBufImageImportInfoCHROMIUM importInfo = {
        .sType = VK_STRUCTURE_TYPE_DMA_BUF_IMAGE_IMPORT_INFO_CHROMIUM,
        .pNext = NULL,
        .drmFormatModifier = I915_FORMAT_MOD_Yf_TILED,
        .planeCount = 1,
        .pPlanes = (VkDmaBufImageImportPlaneInfoCHROMIUM[]) {
            {
                .sType = VK_STRUCTURE_TYPE_DMA_BUF_IMAGE_IMPORT_PLANE_INFO_CHROMIUM,
                .pNext = NULL,
                .offset = 0,
                .rowPitch = 1 << 16,
            },
        },
    };

    // This example hard-codes much of VkImageCreateInfo. In real usage, this
    // hard-coded info would be negotiated between the image's consumer and
    // producer.  Observe that this example creates a "simple" image (that is,
    // a 2D, non-mipmapped, non-array, single-sample image); but nothing in the
    // extension API enforces that the image be "simple". The complexity of the
    // dma_buf image is determined by the negotiation between the consumer and
    // producer, as well as constraints defined in any the vendor-specific
    // extension documents.
    //
    // TODO: Move the below text to the extension specs.
    //
    // An "external dma_buf image" is any VkImage created with
    //     .
    //     VkImageCreateInfo::pNext is VkDmaBufImageImportInfoCHROMIUM
    //     VkImageCreateInfo::initalLayout == VK_IMAGE_LAYOUT_EXTERNAL_DMA_BUF_CHROMIUM
    //     VkImageCreateInfo::tiling == VK_IMAGE_TILING_EXTERNAL_DMA_BUF_CHROMIUM
    //     .
    // The image's tiling and 'external layout' is defined by
    // VkDmaBufImageImportInfoCHROMIUM.
    //
    // An external dma_buf image has two layouts at all times: an 'external layout',
    // which was defined during image creation and is immutable; and a current
    // layout, which can be either the image's external layout, referenced by
    // VK_IMAGE_LAYOUT_EXTERNAL_DMA_BUF_CHROMIUM, or a normal Vulkan layout, such
    // as VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL.
    //
    // When the image's current layout is a non-external layout, Vulkan "owns"
    // the image and can access it as if it were a normal, non-external image.
    // When the image's current layout is an external layout, the image is
    // "externally owned". When the image is externally owned, access to the
    // image is severely restricted.  Work submitted to a queue must not access
    // an externally owned image except work that transitions the image from
    // its external layout to a non-external layout (for example, a transition
    // effected by VkAttachmentReference::layout or by vkCmdPipelineBarrier).
    //
    // Vulkan "acquires ownership" of the image when it transitions from an
    // external to a non-external layout. Conversely, Vulkan "releases
    // ownership" of the image when it transitions from a non-external layout
    // to an external layout.
    //
    const VkImageCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = &importInfo,
        .flags = 0,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = VK_FORMAT_R8G8B8A8_UNORM,
        .extent = { 1920, 1080, 1 },
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = 1,
        .tiling = VK_IMAGE_TILING_EXTERNAL_DMA_BUF_CHROMIUM,
        .usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                 VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                 VK_IMAGE_USAGE_SAMPLED_BIT |
                 VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = (uint32_t[]) { 0 },
        .initialLayout = VK_IMAGE_LAYOUT_EXTERNAL_DMA_BUF_CHROMIUM,
    };

    // Create the external dma_buf VkImage. Observe that the image, like any
    // non-external VkImage, is initially unbound to memory.
    result = vkCreateImage(device,
            &createInfo,
            /*pAllocator*/ NULL,
            pDmaBufImage);
    checkVkResult(result);

    // To bind a VkImage to a dma_buf-imported VkDeviceMemory, the image must
    // have been created with
    //     .
    //     VkImageCreateInfo::pNext is VkDmaBufImageImportInfoCHROMIUM
    //     VkImageCreateInfo::initalLayout == VK_IMAGE_LAYOUT_EXTERNAL_DMA_BUF_CHROMIUM
    //     VkImageCreateInfo::tiling == VK_IMAGE_TILING_EXTERNAL_DMA_BUF_CHROMIUM
    //
    // This example hard-code the image's offset into the dma_buf. In real
    // usage, the offset would be negotiated between consumer and producer.
    //
    result = vkBindImageMemory(device, *pDmaBufImage,
                               dmaBufMemory, /*memoryOffset*/ 4096);
    checkVkResult(result);
}

static void unused
exampleAcqureDmaBufImageWithPipelineBarrier(
        VkDevice device,
        VkQueue queue,
        uint32_t queueFamilyIndex,
        VkCommandBuffer commandBuffer,
        VkImage dmaBufImage,
        VkSemaphore acquireSemaphore) {
    VkResult result;

    result = vkBeginCommandBuffer(commandBuffer,
            &(VkCommandBufferBeginInfo) {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .pNext = NULL,
                .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
                .pInheritanceInfo = NULL,
            });
    checkVkResult(result);

    // This barrier acquires ownership of the image through a layout
    // transition.  The source layout is the external layout defined by
    // VkDmaBufImageImportInfoCHROMIUM::drmFormatModifier; the destination
    // layout is a normal Vulkan layout.
    //
    // Observe that 'srcAccessMask' and 'oldLayout' refer to external usage.
    const VkImageMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext = NULL,
        .srcAccessMask = VK_ACCESS_EXTERNAL_DMA_BUF_CHROMIUM,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                         VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_EXTERNAL_DMA_BUF_CHROMIUM,
        .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = queueFamilyIndex,
        .image = dmaBufImage,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
    };

    vkCmdPipelineBarrier(commandBuffer,
        // TODO: Maybe allow a looser srcStageMask?
        /*srcStageMask*/ VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        /*dstStageMask*/ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        /*dependencyFlags*/ 0,
        /*memoryBarrierCount*/ 0,
        /*pMemoryBarriers*/ NULL,
        /*bufferMemoryBarrierCount*/ 0,
        /*pBufferMemoryBarriers*/ NULL,
        /*imageMemoryBarrierCount*/ 1,
        /*pImageMemoryBarriers*/ &barrier);

    drawStuff(commandBuffer);

    result = vkEndCommandBuffer(commandBuffer);
    checkVkResult(result);

    result = vkQueueSubmit(queue,
            /*submitCount*/ 1,
            (VkSubmitInfo[]) {
                {
                    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                    .pNext = NULL,
                    .waitSemaphoreCount = 1,
                    .pWaitSemaphores = (VkSemaphore[]) {
                        acquireSemaphore,
                    },
                    .pWaitDstStageMask = (VkPipelineStageFlags[]) {
                        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                    },
                    .commandBufferCount = 1,
                    .pCommandBuffers = (VkCommandBuffer[]) {
                        commandBuffer,
                    },
                    .signalSemaphoreCount = 0,
                    .pSignalSemaphores = NULL,
                },
            },
            /*fence*/ VK_NULL_HANDLE);
    checkVkResult(result);
}

static void unused
exampleReleaseDmaBufImageWithPipelineBarrier(
        VkDevice device,
        VkQueue queue,
        uint32_t queueFamilyIndex,
        VkCommandBuffer commandBuffer,
        VkImage dmaBufImage,
        VkSemaphore releaseSemaphore)
{
    VkResult result;

    result = vkBeginCommandBuffer(commandBuffer,
            &(VkCommandBufferBeginInfo) {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .pNext = NULL,
                .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
                .pInheritanceInfo = NULL,
            });
    checkVkResult(result);

    drawStuff(commandBuffer);

    // This barrier releases ownership of the image through a layout
    // transition.  The source layout is a normal Vulkan layout; the
    // destination layout the external layout defined by
    // VkDmaBufImageImportInfoCHROMIUM::drmFormatModifier.
    //
    // Observe that 'dstAccessMask' and 'newLayout' refer to external usage.
    const VkImageMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext = NULL,
        .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dstAccessMask = VK_ACCESS_EXTERNAL_DMA_BUF_CHROMIUM,
        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_EXTERNAL_DMA_BUF_CHROMIUM,
        .srcQueueFamilyIndex = queueFamilyIndex,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = dmaBufImage,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
    };

    vkCmdPipelineBarrier(commandBuffer,
        // TODO: Maybe allow a looser dstStageMask?
        /*srcStageMask*/ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        /*dstStageMask*/ VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        /*dependencyFlags*/ 0,
        /*memoryBarrierCount*/ 0,
        /*pMemoryBarriers*/ NULL,
        /*bufferMemoryBarrierCount*/ 0,
        /*pBufferMemoryBarriers*/ NULL,
        /*imageMemoryBarrierCount*/ 1,
        /*pImageMemoryBarriers*/ &barrier);

    result = vkEndCommandBuffer(commandBuffer);
    checkVkResult(result);

    result = vkQueueSubmit(queue,
            /*submitCount*/ 1,
            (VkSubmitInfo[]) {
                {
                    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                    .pNext = NULL,
                    .waitSemaphoreCount = 0,
                    .pWaitSemaphores = NULL,
                    .pWaitDstStageMask = (VkPipelineStageFlags[]) {
                        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                    },
                    .commandBufferCount = 1,
                    .pCommandBuffers = (VkCommandBuffer[]) {
                        commandBuffer,
                    },
                    .signalSemaphoreCount = 0,
                    .pSignalSemaphores = NULL,
                    .pWaitSemaphores = (VkSemaphore[]) {
                        releaseSemaphore,
                    },
                },
            },
            /*fence*/ VK_NULL_HANDLE);
    checkVkResult(result);
}

static void unused
exampleAcquireDmaBufImageWithSubpassTransition(
        VkDevice device,
        VkQueue queue,
        uint32_t queueFamilyIndex,
        VkCommandBuffer commandBuffer,
        VkImageView dmaBufImageView,
        uint32_t viewWidth,
        uint32_t viewHeight,
        VkSemaphore acquireSemaphore)
{
    VkResult result;
    VkRenderPass pass;
    VkFramebuffer framebuffer;

    result = vkCreateRenderPass(device,
            &(VkRenderPassCreateInfo) {
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .attachmentCount = 1,
                .pAttachments = (VkAttachmentDescription[]) {
                    {
                        .flags = 0,
                        .format = VK_FORMAT_R8G8B8A8_UNORM,
                        .samples = 1,
                        .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
                        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                        .initialLayout = VK_IMAGE_LAYOUT_EXTERNAL_DMA_BUF_CHROMIUM,
                        .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                    },
                },
                .subpassCount = 1,
                .pSubpasses = (VkSubpassDescription[]) {
                    {
                        .flags = 0,
                        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
                        .inputAttachmentCount = 0,
                        .pInputAttachments = NULL,
                        .colorAttachmentCount = 1,
                        .pColorAttachments = (VkAttachmentReference[]) {
                            {
                                // This layout transition acquires ownership of
                                // the external dma_buf image.  The Vulkan 1.0 spec
                                // ensures that the image transitions from the
                                // previous layout
                                // (VkAttachmentDescription::initialLayout =
                                // VK_IMAGE_LAYOUT_EXTERNAL_DMA_BUF_CHROMIUM) to
                                // the subpass's layout before the subpass
                                // begins execution.
                                .attachment = 0,
                                .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                            },
                        },
                        .pResolveAttachments = NULL,
                        .pDepthStencilAttachment = NULL,
                        .preserveAttachmentCount = 0,
                        .pPreserveAttachments = NULL,
                    },
                },
                .dependencyCount = 1,
                .pDependencies = (VkSubpassDependency[]) {
                    {
                        // TODO: To ensure a well-defined transition from
                        // external layout to non-external layout, is an
                        // explicit VkSubpassDependency really required? I'm
                        // (chadv) unsure because Vulkan is hard.
                        //
                        // TODO: Maybe allow a looser srcStageMask?
                        .srcSubpass = VK_SUBPASS_EXTERNAL,
                        .srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                        .srcAccessMask = VK_ACCESS_EXTERNAL_DMA_BUF_CHROMIUM,

                        .dstSubpass = 0, // this subpass
                        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                                         VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,

                        // VK_DEPENDENCY_BY_REGION_BIT is illegal for external dma_buf images.
                        .dependencyFlags = 0,
                    },
                },
            },
            /*pAllocator*/ NULL,
            &pass);
    checkVkResult(result);

    result = vkCreateFramebuffer(device,
            &(VkFramebufferCreateInfo) {
                .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .renderPass = pass,
                .attachmentCount = 1,
                .pAttachments = (VkImageView[]) {
                    dmaBufImageView,
                },
                .width = viewWidth,
                .height = viewHeight,
                .layers = 1,
            },
            /*pAllocator*/ NULL,
            &framebuffer);
    checkVkResult(result);

    result = vkBeginCommandBuffer(commandBuffer,
            &(VkCommandBufferBeginInfo) {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .pNext = NULL,
                .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
                .pInheritanceInfo = NULL,
            });
    checkVkResult(result);

    vkCmdBeginRenderPass(commandBuffer,
            &(VkRenderPassBeginInfo) {
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                .pNext = NULL,
                .renderPass = pass,
                .framebuffer = framebuffer,
                .renderArea = (VkRect2D) {
                    .offset = { 0, 0 },
                    .extent = { viewWidth, viewHeight },
                },
                .clearValueCount = 0,
                .pClearValues = NULL,
            },
            VK_SUBPASS_CONTENTS_INLINE);

    drawStuff(commandBuffer);

    vkCmdEndRenderPass(commandBuffer);

    result = vkEndCommandBuffer(commandBuffer);
    checkVkResult(result);

    result = vkQueueSubmit(queue,
            /*submitCount*/ 1,
            (VkSubmitInfo[]) {
                {
                    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                    .pNext = NULL,
                    .waitSemaphoreCount = 1,
                    .pWaitSemaphores = (VkSemaphore[]) {
                        acquireSemaphore,
                    },
                    .pWaitDstStageMask = (VkPipelineStageFlags[]) {
                        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                    },
                    .commandBufferCount = 1,
                    .pCommandBuffers = (VkCommandBuffer[]) {
                        commandBuffer,
                    },
                    .signalSemaphoreCount = 0,
                    .pSignalSemaphores = NULL,
                },
            },
            /*fence*/ VK_NULL_HANDLE);
    checkVkResult(result);
}

static void unused
exampleReleaseDmaBufImageWithSubpassTransition(
        VkDevice device,
        VkQueue queue,
        uint32_t queueFamilyIndex,
        VkCommandBuffer commandBuffer,
        VkImageView dmaBufImageView,
        uint32_t viewWidth,
        uint32_t viewHeight,
        VkSemaphore releaseSemaphore)
{
    VkResult result;
    VkRenderPass pass;
    VkFramebuffer framebuffer;

    result = vkCreateRenderPass(device,
            &(VkRenderPassCreateInfo) {
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .attachmentCount = 1,
                .pAttachments = (VkAttachmentDescription[]) {
                    {
                        .flags = 0,
                        .format = VK_FORMAT_R8G8B8A8_UNORM,
                        .samples = 1,
                        .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
                        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,

                        // This layout transition releases ownership of the
                        // external image.
                        .initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                        .finalLayout = VK_IMAGE_LAYOUT_EXTERNAL_DMA_BUF_CHROMIUM,
                    },
                },
                .subpassCount = 1,
                .pSubpasses = (VkSubpassDescription[]) {
                    {
                        .flags = 0,
                        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
                        .inputAttachmentCount = 0,
                        .pInputAttachments = NULL,
                        .colorAttachmentCount = 1,
                        .pColorAttachments = (VkAttachmentReference[]) {
                            {
                                .attachment = 0,
                                .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                            },
                        },
                        .pResolveAttachments = NULL,
                        .pDepthStencilAttachment = NULL,
                        .preserveAttachmentCount = 0,
                        .pPreserveAttachments = NULL,
                    },
                },
                .dependencyCount = 1,
                .pDependencies = (VkSubpassDependency[]) {
                    {
                        // TODO: To ensure a well-defined transition from
                        // non-external layout to external layout, is an
                        // explicit VkSubpassDependency really required? I'm
                        // (chadv) unsure because Vulkan is hard.
                        //
                        // TODO: Maybe allow a looser dstStageMask?
                        .srcSubpass = 0, // self
                        .srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                        .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,

                        .dstSubpass = VK_SUBPASS_EXTERNAL,
                        .dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                        .dstAccessMask = VK_ACCESS_EXTERNAL_DMA_BUF_CHROMIUM,

                        // VK_DEPENDENCY_BY_REGION_BIT is illegal for external images.
                        .dependencyFlags = 0,
                    },
                },
            },
            /*pAllocator*/ NULL,
            &pass);
    checkVkResult(result);

    result = vkCreateFramebuffer(device,
            &(VkFramebufferCreateInfo) {
                .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .renderPass = pass,
                .attachmentCount = 1,
                .pAttachments = (VkImageView[]) {
                    dmaBufImageView,
                },
                .width = viewWidth,
                .height = viewHeight,
                .layers = 1,
            },
            /*pAllocator*/ NULL,
            &framebuffer);
    checkVkResult(result);

    result = vkBeginCommandBuffer(commandBuffer,
            &(VkCommandBufferBeginInfo) {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .pNext = NULL,
                .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
                .pInheritanceInfo = NULL,
            });
    checkVkResult(result);

    vkCmdBeginRenderPass(commandBuffer,
            &(VkRenderPassBeginInfo) {
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                .pNext = NULL,
                .renderPass = pass,
                .framebuffer = framebuffer,
                .renderArea = (VkRect2D) {
                    .offset = { 0, 0 },
                    .extent = { viewWidth, viewHeight },
                },
                .clearValueCount = 0,
                .pClearValues = NULL,
            },
            VK_SUBPASS_CONTENTS_INLINE);

    drawStuff(commandBuffer);

    vkCmdEndRenderPass(commandBuffer);

    result = vkEndCommandBuffer(commandBuffer);
    checkVkResult(result);

    result = vkQueueSubmit(queue,
            /*submitCount*/ 1,
            (VkSubmitInfo[]) {
                {
                    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                    .pNext = NULL,
                    .waitSemaphoreCount = 0,
                    .pWaitSemaphores = NULL,
                    .pWaitDstStageMask = NULL,
                    .commandBufferCount = 1,
                    .pCommandBuffers = (VkCommandBuffer[]) {
                        commandBuffer,
                    },
                    .signalSemaphoreCount = 1,
                    .pSignalSemaphores = (VkSemaphore[]) {
                        releaseSemaphore,
                    },
                },
            },
            /*fence*/ VK_NULL_HANDLE);
    checkVkResult(result);
}
