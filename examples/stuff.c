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
example_dma_buf_image_layout(void)
{
    VkDrmExternalImageLayoutCHROMIUM layout unused = {
        // Required when binding an image to dma_buf-imported memory.
        //
        // Observe that this struct omits stride. Stride is omitted
        // because it's meaningless for many layouts, such as layouts
        // that contain auxiliary surfaces. The ABI version defines the
        // stride.
        .sType = VK_STRUCTURE_TYPE_DRM_EXTERNAL_IMAGE_LAYOUT_CHROMIUM,
        .pNext = NULL,
        .abi = &(VkDrmExternalImageAbiCHROMIUM) {
            .sType = VK_STRUCTURE_TYPE_DRM_EXTERNAL_IMAGE_ABI_CHROMIUM,
            .pNext = NULL,

            // Vendors are defined drm_fourcc.h.
            .vendor = DRM_FORMAT_MOD_VENDOR_INTEL,

            // The ABI is defined separately by each vendor and is a
            // driver-private detail.
            .version = 1,
        },
        .drmFourCC = DRM_FORMAT_RGBA8888,
        .drmModifier = I915_FORMAT_MOD_Yf_TILED,
    };
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
        const VkDrmExternalImageLayoutCHROMIUM *external_layout,
        VkImage *external_image)
{
    VkResult res;
    VkFormat format = get_vk_format_for_drm_fourcc(external_layout->drmFourCC);

    res = vkCreateImage(dev,
            &(VkImageCreateInfo) {
                .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                .pNext = external_layout, // XXX
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
        const VkDrmExternalImageLayoutCHROMIUM *external_layout,
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

    // The image's previous external owner last accessed the image using an
    // external layout. That owner passed a description of that external layout
    // to us using some external API, perhaps over IPC. Now we are the image's
    // owner. Despite being the current owner, we must not access the image's
    // memory with Vulkan until we transition the image from that external
    // layout to a normal Vulkan layout.
    const VkImageMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext = external_layout, // XXX
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
        const VkDrmExternalImageLayoutCHROMIUM *external_layout,
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

    // We are the external image's current owner, and the image's current
    // layout is a normal Vulkan layout.  During some initial negotiation
    // phase, the next owner provided us a set of external layouts through
    // which it is able to access the image. We must transition the image from
    // its current layout to one of the pre-negotiated external layouts before
    // releasing ownership to the next owner.
    const VkImageMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext = external_layout, // XXX
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
