#pragma once
#include <iostream>
#include <optional>
#include <vulkan/vulkan_core.h>
#ifndef AGE_DEVICE
#define AGE_DEVICE

#include <vector>
#include <vulkan/vulkan.h>

namespace age {
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphics_family;

        bool is_complete();
    };

    class age_device {
        public:
#ifdef NDEBUG
            const bool enable_validation_layers = false;
#else
            const bool enable_validation_layers = true;
#endif

            age_device();
            age_device(const age_device&) = delete;
            age_device& operator= (const age_device&) = delete;
            ~age_device();

        private:
            static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
                VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,     
                VkDebugUtilsMessageTypeFlagsEXT message_type,                
                const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data, 
                void* p_user_data);
            static void destroy_debug_messenger(
                VkInstance instance,
                VkDebugUtilsMessengerEXT debug_messenger,
                const VkAllocationCallbacks *p_allocator);

            // Private Member Functions
            void _init_vulkan();
            void _pick_physical_device();
            void _create_instance();
            void _setup_debug_messenger();
            VkResult _create_debug_utils_messenger(
                VkInstance instance,
                const VkDebugUtilsMessengerCreateInfoEXT *debug_info,
                const VkAllocationCallbacks *p_allocator,
                VkDebugUtilsMessengerEXT *p_debug_messenger
            );
            void _populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT &debug_info);
            
            std::vector <const char*> _get_required_extensions(); // get list of the required extensions
            bool _check_validation_layer_support();               // checks that we are able to use the validation layers we specify
            bool _is_device_suitable(VkPhysicalDevice device); // check if an available device is suitable for operations we need to perform
            int _rate_device_suitability(VkPhysicalDevice device); // rate the desirability of a GPU to choose from
            QueueFamilyIndices _find_queue_families(VkPhysicalDevice device);
            
            // Private memeber fields
            VkInstance _instance; // Vulkan instance
            VkPhysicalDevice _physical_device; // the physical GPU
            VkDebugUtilsMessengerEXT _debug_messenger; // debug messenger
            const std::vector <const char*> _validation_layers = {
                "VK_LAYER_KHRONOS_validation"
            };
    };
}

#endif /* AGE_DEVICE */
