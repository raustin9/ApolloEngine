#include "age_window.hh"
#include <stdexcept>
#include <string>
#include <cstdint>
#include <GLFW/glfw3.h>

namespace age {

    // Set the member fields and create the glfw window
    age_window::age_window(uint32_t width, uint32_t height, std::string name) {
        this->_width = width;
        this->_height = height;
        this->_name = name;

        this->init_window();
    }

    // Destroy the window and close GLFW
    age_window::~age_window() {
        glfwDestroyWindow(this->_window);
        printf("Window '%s' destroyed\n", this->_name.c_str());
        glfwTerminate();
    }

    // Initialize GLFW and create the window member field
    void
    age_window::init_window() {
        if (!glfwInit()) 
            throw new std::runtime_error("Error: could not initialize GLFW");
        
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        this->_window = glfwCreateWindow(this->_width, this->_height, this->_name.c_str(), nullptr, nullptr);
        if (!this->_window)
            throw new std::runtime_error("Error: unable to create GLFW window");
        return;
    }

    // Determine if the window should close
    bool
    age_window::should_close() {
        return glfwWindowShouldClose(this->_window);
    }

    // Open the window
    void
    age_window::open() {
        if (this->_window) {
            while (!glfwWindowShouldClose(this->_window)) {
                glfwPollEvents();
            }
        } else {
            throw new std::runtime_error("Error: cannot open window that does not exist");
        }
    }

}
