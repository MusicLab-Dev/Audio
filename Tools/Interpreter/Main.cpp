/**
 * @ Author: Pierre Veysseyre
 * @ Description: Interpreter
 */

#include <iostream>

#include "Interpreter.hpp"

using namespace Audio;

int main(void)
{
    try {
        Device::DriverInstance driverInstance;
        PluginTable::Instance pluginTableInstance;
        Interpreter interpreter;

        interpreter.run();
        return 0;
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
}