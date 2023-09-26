#include "age_device.hh"

#include <cstring>
#include <stdexcept>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace age {

    /**********************************************
     *                Public
     *********************************************/
    age_device::age_device() {
        this->_init_vulkan();
    }

    age_device::~age_device() {
        vkDestroyInstance(this->_instance, nullptr);
    }

    /**********************************************
     *                 Private
     *********************************************/

    // Initialize the vulkan library and instance
    void
    age_device::_init_vulkan() {
        this->_create_instance();
    }

    // Create the vulkan instance
    void
    age_device::_create_instance() {
        if (enable_validation_layers && !_check_validation_layer_support()) {
            throw new std::runtime_error("Error: validation layer requested but not available");
        }

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

        // Set the extension information
        instance_create_info.enabledExtensionCount = glfw_extension_count;
        instance_create_info.ppEnabledExtensionNames = glfw_extensions;

        // Set validation layer information
        if (this->enable_validation_layers) {
            instance_create_info.enabledLayerCount = static_cast<uint32_t>(this->_validation_layers.size());
            instance_create_info.ppEnabledLayerNames = this->_validation_layers.data();
        } else {
            instance_create_info.enabledLayerCount = 0;
        }

        // Create the instance
        VkResult result = vkCreateInstance(&instance_create_info, nullptr, &this->_instance);
        if (result != VK_SUCCESS) {
            throw new std::runtime_error("Error: unable to create vulkan instance");
        }
    }

    // Check if all of the requested layers are available 
    // and list them
    bool
    age_device::_check_validation_layer_support() {
        // Find the available layers
        uint32_t layer_count;
        vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

        std::vector <VkLayerProperties> available_layers(layer_count);
        vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

        // Iterate through the all layers in the validation layers field, and
        // see if they exist in the available layers list
        for (const char* layer_name: this->_validation_layers) {
            bool layer_found = false;

            for (const auto& layer_properties: available_layers) {
                if (strcmp(layer_name, layer_properties.layerName) == 0) {
                    layer_found = true;
                    break;
                }
            }

            if (!layer_found) {
                return false;
            }
        }

        return true;
    }
}
