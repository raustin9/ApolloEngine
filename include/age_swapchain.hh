#pragma once
#ifndef AGE_SWAP_CHAIN
#define AGE_SWAP_CHAIN

#include "age_device.hh"
#include <vulkan/vulkan_core.h>
#include <vector>

namespace age {
    class age_swapchain {
        public:
            age_swapchain(age_device &device, VkExtent2D extent);
            age_swapchain(const age_swapchain&) = delete;
            age_swapchain& operator= (const age_swapchain&) = delete;
            ~age_swapchain();

        private:

            VkSurfaceFormatKHR _choose_swap_surface_format(
                    const std::vector<VkSurfaceFormatKHR>& available_formats);
            VkPresentModeKHR _choose_swap_present_mode(
                    const std::vector<VkPresentModeKHR>& available_present_modes);
            VkExtent2D _choose_swap_extent(
                    const VkSurfaceCapabilitiesKHR& capabilities);

            void _init();
            void _create_swapchain();

            // Member fields
            age_device& _device;
            VkExtent2D _extent;
            VkSwapchainKHR _swapchain;
            std::vector<VkImage> _swapchain_images;
            VkFormat _swapchain_image_format;
            VkExtent2D _swapchain_extent;
    };
}

#endif /* AGE_SWAP_CHAIN */
