#include "age_engine.hh"
#include "age_window.hh"

#include <GLFW/glfw3.h>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace age {

    /**********************************************
     *                Public
     *********************************************/

    // Constructor
    age_engine::age_engine(uint32_t width, uint32_t height, std::string name)
    : _window(width, height, name) {

    }

    // Destructor
    age_engine::~age_engine() {
        this->_cleanup();
    }

    // Runs the event loop
    void
    age_engine::run() {
        this->_init_vulkan();
        this->_window.open();
        while (!this->_window.should_close()) {
            // Event loop
        }
    }


    /**********************************************
     *                 Private
     *********************************************/

    // Initialize vulkan library
    void
    age_engine::_init_vulkan() {
        this->_create_instance();
    }

    void
    age_engine::_main_loop() {

    }

    void
    age_engine::_cleanup() {
        vkDestroyInstance(this->_instance, nullptr);
    }


    /**********************************************
     *                  Utils
     *********************************************/

    void
    age_engine::_create_instance() {
        uint32_t glfw_extension_count = 0;
        const char** glfw_extensions;

        // Create the application info struct to later create the vulkan instance
        VkApplicationInfo app_info{};
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName = "Apollo Engine";
        app_info.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
        app_info.pEngineName = "No engine";
        app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.apiVersion = VK_API_VERSION_1_0;
        app_info.pNext = nullptr; // This can be used to point to extension information in the future
 
        // Create the vulkan instance
        VkInstanceCreateInfo instance_create_info{};
        instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_create_info.pApplicationInfo = &app_info;

        glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

        instance_create_info.enabledExtensionCount = glfw_extension_count;
        instance_create_info.ppEnabledExtensionNames = glfw_extensions;
        instance_create_info.enabledLayerCount = 0;

        // Create the instance
        VkResult result = vkCreateInstance(&instance_create_info, nullptr, &this->_instance);
        if (result != VK_SUCCESS) {
            throw new std::runtime_error("Error: unable to create vulkan instance");
        }
    }
}
