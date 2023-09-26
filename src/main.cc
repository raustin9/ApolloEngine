#include "age_engine.hh"

#include <iostream>
#include <stdexcept>
#include <string>
#include <exception>
#include <GLFW/glfw3.h>

int
main (void) {
    age::age_engine engine = age::age_engine(800, 600, "Apollo Engine");

    try {
        engine.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
