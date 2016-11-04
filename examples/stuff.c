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

#include <stdint.h>
#include <stdlib.h>

#include <drm/drm_fourcc.h>
#include <vulkan/vulkan_chromium.h>

#define unused __attribute__((unused))

static void
check_vk_result(VkResult res)
{
    if (res != VK_SUCCESS)
        abort();
}

static VkFormat
get_vk_format_for_drm_fourcc(uint32_t drm_fourcc)
{
    // STUB
    return VK_FORMAT_UNDEFINED;
}

static void
draw_stuff(VkCommandBuffer cmd)
{
    // STUB
}

static void unused
example_import_dma_buf_memory(VkDevice dev, int fd, size_t size,
                              VkDeviceMemory *mem)
{
    VkResult res;

    res = vkImportDmaBufMemoryCHROMIUM(dev,
            &(VkDmaBufMemoryImportInfoCHROMIUM) {
                .sType = VK_STRUCTURE_TYPE_DMA_BUF_MEMORY_IMPORT_INFO_CHROMIUM,
                .pNext = NULL,
                .fd = fd,
                .size = size,
            },
            /*pAllocator*/ NULL,
            mem);
    check_vk_result(res);
}

static void unused
example_bind_dma_buf_image(
        VkDevice dev,
        VkDeviceMemory dma_buf_memory,
        size_t offset,
        uint32_t width,
        uint32_t height,
        VkImage *external_image)
{
    VkResult res;

    // This example hard-codes the image's external layout.  In real usage, the
    // image's external layout, as well as its offset into the dma_buf, would
    // be negotiated between the consumer and producer during an initial setup
    // phase.
    const VkDrmExternalImageCreateInfoCHROMIUM external_info = {
        .sType = VK_STRUCTURE_TYPE_DRM_EXTERNAL_IMAGE_CREATE_INFO_CHROMIUM,
        .pNext = NULL,
        .externalLayout = &(VkDrmExternalImageLayoutCHROMIUM) {
            // Observe that this struct omits stride. Stride is omitted
            // because it's meaningless for many layouts, such as layouts
            // that contain auxiliary surfaces. The ABI version defines the
            // stride.
            .sType = VK_STRUCTURE_TYPE_DRM_EXTERNAL_IMAGE_LAYOUT_CHROMIUM,
            .pNext = NULL,
            .abi = &(VkDrmExternalImageAbiCHROMIUM) {
                .sType = VK_STRUCTURE_TYPE_DRM_EXTERNAL_IMAGE_ABI_CHROMIUM,
                .pNext = NULL,

                // Vendors are defined in drm_fourcc.h.
                .vendor = DRM_FORMAT_MOD_VENDOR_INTEL,

                // The ABI is defined separately by each vendor and is a
                // driver-private detail.
                .version = 1,
            },
            .drmFourCC = DRM_FORMAT_RGBA8888,
            .drmModifier = I915_FORMAT_MOD_Yf_TILED,
        },
    };

    VkFormat format = get_vk_format_for_drm_fourcc(
            external_info.externalLayout->drmFourCC);

    // Create an external VkImage. Observe that the image, like any
    // non-external VkImage, is initially unbound to memory; that
    // VkDrmExternalImageCreateInfoCHROMIUM extends VkImageCreateInfo; and that
    // 'initialLayout' and 'tiling' refer to VkDrmExternalImageCreateInfoCHROMIUM.
    //
    // TODO: Move the below text to the extension specs.
    //
    // An "external image" is any VkImage created with
    // VkImageCreateInfo::initalLayout ==
    // VK_IMAGE_LAYOUT_DRM_EXTERNAL_CHROMIUM.  The image's external layout is
    // defined by VkDrmExternalImageCreateInfoCHROMIUM.
    //
    // An external image has two layouts at all times: an external layout,
    // which was defined during image creation and is immutable; and a current
    // layout, which can be either the image's external layout, referenced by
    // VK_IMAGE_LAYOUT_DRM_EXTERNAL_CHROMIUM, or a normal Vulkan layout, such
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
    res = vkCreateImage(dev,
            &(VkImageCreateInfo) {
                .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                .pNext = &external_info,
                .flags = 0,
                .imageType = VK_IMAGE_TYPE_2D,
                .format = format,
                .extent = { width, height, 1 },
                .mipLevels = 1,
                .arrayLayers = 1,
                .samples = 1,
                .tiling = VK_IMAGE_TILING_DRM_EXTERNAL_CHROMIUM,
                .usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                         VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                         VK_IMAGE_USAGE_SAMPLED_BIT |
                         VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                .queueFamilyIndexCount = 1,
                .pQueueFamilyIndices = (uint32_t[]) { 0 },
                .initialLayout = VK_IMAGE_LAYOUT_DRM_EXTERNAL_CHROMIUM,
            },
            /*pAllocator*/ NULL,
            external_image);
    check_vk_result(res);

    // To bind a VkImage to a dma_buf-imported VkDeviceMemory, the image must
    // have been created with
    //     VkImageCreateInfo::initialLayout == VK_IMAGE_LAYOUT_DRM_EXTERNAL_CHROMIUM.
    res = vkBindImageMemory(dev, *external_image, dma_buf_memory, offset);
    check_vk_result(res);
}

static void unused
example_acquire_external_drm_image(
        VkDevice dev,
        VkQueue queue,
        uint32_t queue_family_index,
        VkCommandBuffer cmd,
        VkImage external_image,
        VkSemaphore acquire_semaphore) {
    VkResult res;

    res = vkBeginCommandBuffer(cmd,
            &(VkCommandBufferBeginInfo) {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .pNext = NULL,
                .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
                .pInheritanceInfo = NULL,
            });
    check_vk_result(res);

    // This barrier acquires ownership of the image through a layout
    // transition.  The source layout is the external layout defined by
    // VkDrmExternalImageCreateInfoCHROMIUM::externalLayout; the destination
    // layout is a normal Vulkan layout.
    //
    // Observe that 'srcAccessMask' and 'oldLayout' refer to external usage.
    const VkImageMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext = NULL,
        .srcAccessMask = VK_ACCESS_DRM_EXTERNAL_CHROMIUM,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                         VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_DRM_EXTERNAL_CHROMIUM,
        .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = queue_family_index,
        .image = external_image,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
    };

    vkCmdPipelineBarrier(cmd,
        /*srcStageMask*/ VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, // TODO: Maybe allow something looser?
        /*dstStageMask*/ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        /*dependencyFlags*/ 0,
        /*memoryBarrierCount*/ 0,
        /*pMemoryBarriers*/ NULL,
        /*bufferMemoryBarrierCount*/ 0,
        /*pBufferMemoryBarriers*/ NULL,
        /*imageMemoryBarrierCount*/ 1,
        /*pImageMemoryBarriers*/ &barrier);

    draw_stuff(cmd);

    res = vkEndCommandBuffer(cmd);
    check_vk_result(res);

    res = vkQueueSubmit(queue,
            /*submitCount*/ 1,
            (VkSubmitInfo[]) {
                {
                    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                    .pNext = NULL,
                    .waitSemaphoreCount = 1,
                    .pWaitSemaphores = (VkSemaphore[]) {
                        acquire_semaphore,
                    },
                    .pWaitDstStageMask = (VkPipelineStageFlags[]) {
                        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                    },
                    .commandBufferCount = 1,
                    .pCommandBuffers = (VkCommandBuffer[]) {
                        cmd,
                    },
                    .signalSemaphoreCount = 0,
                    .pSignalSemaphores = NULL,
                },
            },
            /*fence*/ VK_NULL_HANDLE);
    check_vk_result(res);
}

static void unused
example_release_external_drm_image(
        VkDevice dev,
        VkQueue queue,
        uint32_t queue_family_index,
        VkCommandBuffer cmd,
        VkImage external_image,
        VkSemaphore release_semaphore)
{
    VkResult res;

    res = vkBeginCommandBuffer(cmd,
            &(VkCommandBufferBeginInfo) {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .pNext = NULL,
                .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
                .pInheritanceInfo = NULL,
            });
    check_vk_result(res);

    draw_stuff(cmd);

    // This barrier releases ownership of the image through a layout
    // transition.  The source layout is a normal Vulkan layout; the
    // destination layout the external layout defined by
    // VkDrmExternalImageCreateInfoCHROMIUM::externalLayout.
    //
    // Observe that 'dstAccessMask' and 'newLayout' refer to external usage.
    const VkImageMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext = NULL,
        .srcAccessMask = VK_ACCESS_DRM_EXTERNAL_CHROMIUM,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                         VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_DRM_EXTERNAL_CHROMIUM,
        .srcQueueFamilyIndex = queue_family_index,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = external_image,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
    };

    vkCmdPipelineBarrier(cmd,
        /*srcStageMask*/ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        /*dstStageMask*/ VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, // TODO: Maybe allow something looser?
        /*dependencyFlags*/ 0,
        /*memoryBarrierCount*/ 0,
        /*pMemoryBarriers*/ NULL,
        /*bufferMemoryBarrierCount*/ 0,
        /*pBufferMemoryBarriers*/ NULL,
        /*imageMemoryBarrierCount*/ 1,
        /*pImageMemoryBarriers*/ &barrier);

    res = vkEndCommandBuffer(cmd);
    check_vk_result(res);

    res = vkQueueSubmit(queue,
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
                        cmd,
                    },
                    .signalSemaphoreCount = 0,
                    .pSignalSemaphores = NULL,
                    .pWaitSemaphores = (VkSemaphore[]) {
                        release_semaphore,
                    },
                },
            },
            /*fence*/ VK_NULL_HANDLE);
    check_vk_result(res);
}
