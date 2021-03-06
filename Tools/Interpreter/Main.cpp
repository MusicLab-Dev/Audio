/**
 * @ Author: Pierre Veysseyre
 * @ Description: Interpreter
 */

#include <iostream>

#include "Interpreter.hpp"

using namespace Audio;

static void CB(void *, std::uint8_t *stream, const int length)
{

}


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