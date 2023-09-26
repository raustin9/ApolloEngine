#include "age_device.hh"

#include <cstring>
#include <map>
#include <stdexcept>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace age {

    /**********************************************
     *                Public
     *********************************************/
    // Constructor //
    age_device::age_device() {
        this->_create_instance();
        this->_setup_debug_messenger();
        this->_pick_physical_device();
    }

    // Destructor //
    age_device::~age_device() {
        if (this->enable_validation_layers) {
            age_device::destroy_debug_messenger(this->_instance, this->_debug_messenger, nullptr);
        }

        vkDestroyInstance(this->_instance, nullptr);
    }

    /**********************************************
     *                 Private
     *********************************************/

    // Static member to perform a callback to display debug error information
    VKAPI_ATTR VkBool32 VKAPI_CALL 
    age_device::debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,     // severity of the warning/error
            VkDebugUtilsMessageTypeFlagsEXT message_type,                // the type of warning/error that occured
            const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data, // contains the details of error message
            void* p_user_data                                            // pointer specified during setup of callback that
                                                                         // allows you to pass your own data to it
        ) {
        if (message_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            std::cerr << "Validation layer: " << p_callback_data->pMessage << std::endl;
        }
        return VK_FALSE;
    }

    // Initialize the vulkan library and instance
    void
    age_device::_init_vulkan() {
        this->_create_instance();
        this->_setup_debug_messenger();
    }

    // Populate the debug info struct
    void
    age_device::_populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT &create_info) {
        create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT 
                                     | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                     | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                                 | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                                 | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        create_info.pfnUserCallback = age_device::debugCallback;
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
        std::vector <const char*> extensions = this->_get_required_extensions();
        instance_create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        instance_create_info.ppEnabledExtensionNames = extensions.data();

        // Set validation layer information
        VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};
        if (this->enable_validation_layers) {
            instance_create_info.enabledLayerCount = static_cast<uint32_t>(this->_validation_layers.size());
            instance_create_info.ppEnabledLayerNames = this->_validation_layers.data();

            this->_populate_debug_messenger_create_info(debug_create_info);
            debug_create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debug_create_info;
        } else {
            instance_create_info.enabledLayerCount = 0;
            debug_create_info.pNext = nullptr;
        }

        // Create the instance
        VkResult result = vkCreateInstance(&instance_create_info, nullptr, &this->_instance);
        if (result != VK_SUCCESS) {
            throw new std::runtime_error("Error: unable to create vulkan instance");
        }
    }

    // Pick the physical device (GPU)
    // that we are going to be using
    void
    age_device::_pick_physical_device() {
        this->_physical_device = VK_NULL_HANDLE;
        uint32_t device_count = 0;
        
        vkEnumeratePhysicalDevices(this->_instance, &device_count, nullptr);

        // Cannot find a GPU 
        if (device_count == 0) {
            throw std::runtime_error("Error: failed to find GPU with vulkan support");
        }

        std::multimap<int, VkPhysicalDevice> candidates;
        std::vector<VkPhysicalDevice> devices(device_count);
        vkEnumeratePhysicalDevices(this->_instance, &device_count, devices.data());

        for (VkPhysicalDevice &device : devices) {
            int score = this->_rate_device_suitability(device);
            candidates.insert(std::make_pair(score, device));
//            if (this->_is_device_suitable(device)) {
//                this->_physical_device = device;
//                break;
//            }
        }

        if (candidates.rbegin()->first > 0) {
            this->_physical_device = candidates.rbegin()->second;
        } else {
            throw std::runtime_error("Error: unable to find suitable GPU");
        }

//        if (this->_physical_device == VK_NULL_HANDLE) {
//            throw std::runtime_error("Error: failed to find suitable GPU");
//        } 
    }

    // Rate the suitability of devices that we can choose from
    int
    age_device::_rate_device_suitability(VkPhysicalDevice device) {
        VkPhysicalDeviceProperties device_properties;
        VkPhysicalDeviceFeatures device_features;

        vkGetPhysicalDeviceProperties(device, &device_properties);
        vkGetPhysicalDeviceFeatures(device, &device_features);

        int score = 0;
        if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            score += 1000;
        }

        score += device_properties.limits.maxImageDimension2D;

        if (!device_features.geometryShader)
            return 0;

        return score;
    }

    // Check if a device is suitable for the 
    // operations that we are going to perform
    // with it
    bool
    age_device::_is_device_suitable(VkPhysicalDevice device) {
        VkPhysicalDeviceProperties device_properties;
        VkPhysicalDeviceFeatures device_features;

        vkGetPhysicalDeviceProperties(device, &device_properties);
        vkGetPhysicalDeviceFeatures(device, &device_features);

        return device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU
               && device_features.geometryShader;
    }

    // Setup the validation debug messenger 
    // using our callback function
    void
    age_device::_setup_debug_messenger() {
        if (!this->enable_validation_layers) {
            return;
        }

        // Create and fill struct with information about
        // debug messenger and its callback
        VkDebugUtilsMessengerCreateInfoEXT debug_info;
        this->_populate_debug_messenger_create_info(debug_info);
        debug_info.pUserData = nullptr; // you can set this pointer to be used in the callback

        if (this->_create_debug_utils_messenger(this->_instance, &debug_info, nullptr, &this->_debug_messenger) != VK_SUCCESS) {
            throw std::runtime_error("Error: failed to setup the debug messenger");
        }
    }
 
    // Setup proxy to create the debug messenger
    VkResult 
    age_device::_create_debug_utils_messenger(
            VkInstance instance,
            const VkDebugUtilsMessengerCreateInfoEXT *debug_info,
            const VkAllocationCallbacks *p_allocator,
            VkDebugUtilsMessengerEXT *p_debug_messenger
        ) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, debug_info, p_allocator, p_debug_messenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    // Static member to destroy the debug messenger on cleanup
    void
    age_device::destroy_debug_messenger(
            VkInstance instance,
            VkDebugUtilsMessengerEXT debug_messenger,
            const VkAllocationCallbacks *p_allocator
        ) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debug_messenger, p_allocator);
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

    // Get a list of all the extensions that are required for 
    // vulkan to run properly
    std::vector <const char*> 
    age_device::_get_required_extensions() {
        uint32_t glfw_extension_count = 0;
        const char** glfw_extensions;
        
        glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
        std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

        if (this->enable_validation_layers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }
}
