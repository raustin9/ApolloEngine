#pragma once
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
            void _init_vulkan();
            void _create_instance();
            
            bool _check_validation_layer_support();

            VkInstance _instance;

            const std::vector <const char*> _validation_layers = {
                "VK_LAYER_KHRONOS_validation"
            };
    };
}

#endif /* AGE_DEVICE */
