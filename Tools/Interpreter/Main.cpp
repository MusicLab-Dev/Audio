/**
 * @ Author: Pierre Veysseyre
 * @ Description: Interpreter
 */

#include <iostream>

#include "Interpreter.hpp"


int main(void)
{
    try {
        Audio::Device::DriverInstance driverInstance;
        Audio::PluginTable::Instance pluginTableInstance;
        Interpreter interpreter;

        interpreter.run();
        return 0;
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
}