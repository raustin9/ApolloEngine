#pragma once
#include <iostream>
#include <vulkan/vulkan_core.h>
#ifndef AGE_DEVICE
#define AGE_DEVICE

#include <vector>
#include <vulkan/vulkan.h>

namespace age {

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
            void _create_instance();
            void _setup_debug_messenger();
            VkResult _create_debug_utils_messenger(
                VkInstance instance,
                const VkDebugUtilsMessengerCreateInfoEXT *debug_info,
                const VkAllocationCallbacks *p_allocator,
                VkDebugUtilsMessengerEXT *p_debug_messenger
            );
            
            std::vector <const char*> _get_required_extensions(); // get list of the required extensions
            bool _check_validation_layer_support();               // checks that we are able to use the validation layers we specify
            
            // Private memeber fields
            VkInstance _instance; // Vulkan instance
            VkDebugUtilsMessengerEXT _debug_messenger; // debug messenger
            const std::vector <const char*> _validation_layers = {
                "VK_LAYER_KHRONOS_validation"
            };
    };
}

#endif /* AGE_DEVICE */
