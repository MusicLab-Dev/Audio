/**
 * @ Author: Pierre Veysseyre
 * @ Description: Interpreter
 */

#include <iostream>

#include "Interpreter.hpp"

#include <Audio/KissFFT.hpp>

using namespace Audio;

int main(void)
{
    try {
        KissFFT::Engine engine(8);
        KissFFT::TypeScalar time[1024];
        KissFFT::TypeScalar timeFilter[1024];
        KissFFT::TypeCpx freq[1024];
        engine.processForward(time, freq, 21, true);

        // 1200 48000
        KissFFT::Engine::Filter(KissFFT::FirFilterSpecs {
            KissFFT::FirFilterType::LowPass,
            DSP::WindowType::Hanning,
            48000,
            { 1200, 0 }
        }, time, timeFilter, 1023u);

        return 0;
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