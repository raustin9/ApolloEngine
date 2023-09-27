#pragma once
#include <vulkan/vulkan_core.h>
#ifndef ENGINE_WINDOW
#define ENGINE_WINDOW

#include <cstdint>
#include <string>
#include <GLFW/glfw3.h>

namespace age {
    class age_window {
        public:
            age_window(uint32_t width, uint32_t height, std::string name);
            age_window(const age_window&) = delete;
            age_window& operator= (const age_window&) = delete;
            ~age_window();

            void open();
            void init_window();
            bool should_close();
            void create_window_surface(VkInstance instance, VkSurfaceKHR *surface);

        private:
            // static void _resize_framebuffer_callback(GLFWwindow* window, int width, int height);
            uint32_t _width;
            uint32_t _height;
            std::string _name;
            GLFWwindow* _window;
    };
}

#endif /* ENGINE_WINDOW */
