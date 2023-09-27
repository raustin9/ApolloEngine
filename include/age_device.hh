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
            static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback( // static member callback function for debug error messages
                VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,     
                VkDebugUtilsMessageTypeFlagsEXT message_type,                
                const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data, 
                void* p_user_data);
            static void destroy_debug_messenger( // static member function to be used to destroy the debug messenger at cleanup
                VkInstance instance,
                VkDebugUtilsMessengerEXT debug_messenger,
                const VkAllocationCallbacks *p_allocator);

            // Private Member Functions
            void _init_vulkan();                    // initialize vulkan
            void _pick_physical_device();           // pick the GPU that we are going to use
            void _create_logical_device();          // create the logical device to interface with
            void _create_instance();                // create the vulkan instance
            void _setup_debug_messenger();          // setup necessary steps to create the debug messenger
            void _populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT &debug_info); // fill in debug create info struct
            VkResult _create_debug_utils_messenger( // create the debug messenger that can send the messages
                VkInstance instance,
                const VkDebugUtilsMessengerCreateInfoEXT *debug_info,
                const VkAllocationCallbacks *p_allocator,
                VkDebugUtilsMessengerEXT *p_debug_messenger
            );
            
            std::vector <const char*> _get_required_extensions(); // get list of the required extensions
            bool _check_validation_layer_support();               // checks that we are able to use the validation layers we specify
            bool _is_device_suitable(VkPhysicalDevice device); // check if an available device is suitable for operations we need to perform
            int _rate_device_suitability(VkPhysicalDevice device); // rate the desirability of a GPU to choose from
            QueueFamilyIndices _find_queue_families(VkPhysicalDevice device); // find the queue families we can put command and other queues into
            
            // Private memeber fields
            VkInstance _instance;                                  // Vulkan instance
            VkPhysicalDevice _physical_device;                     // the physical GPU
            VkQueue _graphics_queue;                               // queue to interface with the device
            VkDevice _logical_device;                              // logical device to interface with
            VkDebugUtilsMessengerEXT _debug_messenger;             // debug messenger
            const std::vector <const char*> _validation_layers = { // validation layer checks that we want
                "VK_LAYER_KHRONOS_validation"
            };
    };
}

#endif /* AGE_DEVICE */
