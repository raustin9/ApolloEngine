#pragma once
#ifndef AGE_ENGINE
#define AGE_ENGINE

#include "age_window.hh"
#include "age_device.hh"
#include "age_swapchain.hh"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <string>
#include <memory>

namespace age {
    
    class age_engine {
        public:
            age_engine(uint32_t width, uint32_t height, std::string name);
            age_engine(const age_engine&) = delete;
            age_engine& operator= (const age_engine&) = delete;
            ~age_engine();

            void run();

        private:
            void _main_loop();

            // Member fields
            age_window _window;
            age_device _device;
            age_swapchain _swapchain;
 
            // Utils
            void _create_instance();
    };

}

#endif /* AGE_ENGINE */
