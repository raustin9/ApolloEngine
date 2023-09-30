#include "age_swapchain.hh"
#include "age_device.hh"
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <vulkan/vulkan_core.h>
#include <iostream>

namespace age {

    // Constructor //
    age_swapchain::age_swapchain(age_device &device, VkExtent2D extent)
    : _device{device}, _extent{extent} {
        this->_init();
    }

    // Destrcutor //
    age_swapchain::~age_swapchain() {
        // Image views are explicitly created by us, so we clean them up
        for (VkImageView image_view : this->_swapchain_image_views) {
            vkDestroyImageView(this->_device.get_device(), image_view, nullptr);
        }

        // Destroy the swapchain
        if (this->_swapchain != nullptr) {
            vkDestroySwapchainKHR(this->_device.get_device(), this->_swapchain, nullptr);
            this->_swapchain = nullptr;
        }
    }

    void
    age_swapchain::_init() {
        this->_create_swapchain();
        this->_create_image_views();
    }

    // Create the swapchain
    void
    age_swapchain::_create_swapchain() {
        SwapChainSupportDetails swap_chain_support = this->_device.get_swapchain_support();
        
        VkSurfaceFormatKHR surface_format = this->_choose_swap_surface_format(
                swap_chain_support.formats);
        VkPresentModeKHR present_mode = this->_choose_swap_present_mode(
                swap_chain_support.present_modes);
        VkExtent2D extent = this->_choose_swap_extent(
                swap_chain_support.capabilities);

        // Choose one more image than the minimum so that we don't have to wait for the driver
        // to complete internal operations before we can acquire another image to render to
        uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;
 
        // Make sure that we do not exceed the max number of images that the driver is capable of
        // 0 is a special case here where it means there is no maximum number of images
        if (swap_chain_support.capabilities.maxImageCount > 0
                && image_count > swap_chain_support.capabilities.maxImageCount) {
            image_count = swap_chain_support.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        create_info.surface = this->_device.get_surface();
        create_info.minImageCount = image_count;
        create_info.imageFormat = surface_format.format;
        create_info.imageColorSpace = surface_format.colorSpace;
        create_info.imageExtent = extent;
        create_info.imageArrayLayers = 1; // specifies the amount of layers each image consists of. Always 1 unless developing stereoscopic 3D application
        create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // specifies the kinds of operations we'll use the images in swap chain for. We can edit this to do post-processing

        QueueFamilyIndices indices = this->_device.find_physical_device_queue_families();
        uint32_t queue_family_indices[] = {
            indices.graphics_family.value(),
            indices.present_family.value()
        };

        if (indices.graphics_family != indices.present_family) {
            create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT; // images can be used across multiple queue families without
                                                                       // explicit ownership transfers
            create_info.queueFamilyIndexCount = 2;
            create_info.pQueueFamilyIndices = queue_family_indices;
        } else {
            create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // image is owned by one queue family at a time
                                                                      // and ownership has to be explicitly transferred 
                                                                      // before being used by another queue family
            create_info.queueFamilyIndexCount = 2;     // optional -- good to be explicit
            create_info.pQueueFamilyIndices = nullptr; // optional -- good to be explicit
        }

        create_info.preTransform = swap_chain_support.capabilities.currentTransform; // specify that we do not want any special transforms
        create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // specifies if alpha channe should
                                                                        // be used for blending with other
                                                                        // windows in the system
                                                                        // -- almost always ignore this
        create_info.presentMode = present_mode; // we do not want to render pixels that are covered
                                                // -- obscured by a window
        create_info.clipped = VK_TRUE;
        create_info.oldSwapchain = VK_NULL_HANDLE;
        
        if (vkCreateSwapchainKHR(this->_device.get_device(), &create_info, nullptr, &this->_swapchain) != VK_SUCCESS) {
            throw std::runtime_error("Error: unable to create swapchain");
        }

        vkGetSwapchainImagesKHR(
                this->_device.get_device(),
                this->_swapchain,
                &image_count,
                nullptr);
        this->_swapchain_images.resize(image_count);
        vkGetSwapchainImagesKHR(
                this->_device.get_device(),
                this->_swapchain,
                &image_count,
                this->_swapchain_images.data());

        this->_swapchain_image_format = surface_format.format;
        this->_swapchain_extent = extent;
    }

    void 
    age_swapchain::_create_image_views() {
        this->_swapchain_image_views.resize(this->_swapchain_images.size());

        for (size_t i = 0; i < this->_swapchain_images.size(); i++) {
            VkImageViewCreateInfo create_info{};
            create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            create_info.image = this->_swapchain_images[i]; 
            
            // treat images as 2D textures
            create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;       
            create_info.format = this->_swapchain_image_format;

            // Swizzle the colors 
            // for example you can map all channels to the red channel for monochrome texture
            // or map constant values of 0 or 1 to a channel
            create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            // Describes the image purpose and which part of the image should be accessed
            // These images will be used as color targets without any mipmapping levels or multiple layers
            create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            create_info.subresourceRange.baseMipLevel = 0;
            create_info.subresourceRange.levelCount = 1;
            create_info.subresourceRange.baseArrayLayer = 0;
            create_info.subresourceRange.layerCount = 1;

            if (vkCreateImageView(this->_device.get_device(), &create_info, nullptr, &this->_swapchain_image_views[i])
                != VK_SUCCESS) {
                throw std::runtime_error("Error: unable to create image views");
            }

        }
    }

    // Choose the surface format
    // This is the color depth of the surface
    VkSurfaceFormatKHR
    age_swapchain::_choose_swap_surface_format(
            const std::vector<VkSurfaceFormatKHR>& available_formats) {
        for (const auto& available_format : available_formats) {
            if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB
                && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return available_format;
            }
        }

        return available_formats[0];
    }

    // Choose the present mode
    // This is the conditions for swapping
    // the images to the screen
    // Choose mailbox swapping preferably
    VkPresentModeKHR
    age_swapchain::_choose_swap_present_mode(
            const std::vector<VkPresentModeKHR>& available_present_modes) {
        for (const VkPresentModeKHR available_present_mode : available_present_modes) {
            if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return available_present_mode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    // Get the resolution of the swap chain images
    // This is normally equal to the resolution of the window,
    // but that is not always true (high retina displays can differ in
    // pixel count from the resolution of image)
    VkExtent2D
    age_swapchain::_choose_swap_extent(
            const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {
            VkExtent2D actual_extent = this->_extent;
            actual_extent.width = std::clamp(
                    actual_extent.width, 
                    capabilities.minImageExtent.width, 
                    capabilities.maxImageExtent.width);
            actual_extent.height = std::clamp(
                    actual_extent.height, 
                    capabilities.minImageExtent.height, 
                    capabilities.maxImageExtent.height);

            return actual_extent;
        }
    }
} // end age namespace
