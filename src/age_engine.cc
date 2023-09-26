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
    : _window{width, height, name}, _device() {
    }

    // Destructor
    age_engine::~age_engine() {
    }

    // Runs the event loop
    void
    age_engine::run() {
        this->_window.open();
        while (!this->_window.should_close()) {
            // Event loop
        }
    }


    /**********************************************
     *                 Private
     *********************************************/

    void
    age_engine::_main_loop() {

    }
}
