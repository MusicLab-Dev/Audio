/**
 * @ Author: Pierre Veysseyre
 * @ Description: Unit tests of Envelope Generator class
 */

#include <gtest/gtest.h>

#include <Audio/DSP/EnvelopeGenerator.hpp>

using namespace Audio;

static constexpr BlockSize Size = 1024u;
static constexpr SampleRate SR = 48000u;


TEST(EnvelopeGenerator, Simple_AttackDecayReleaseSustain)
{
    DSP::EnvelopeClipLinear<DSP::EnvelopeType::ADSR, 1u> env;

    Key key { 69u };
    DSP::EnvelopeSpecs specs {
        0.0f, // delay
        0.1f, // attack
        1.0f, // peak
        0.0f, // hold
        0.1f, // decay
        0.5f, // sustain
        0.1f // release
    };
    env.setSpecs<0u>(specs);
    env.setSampleRate(100u);

    auto i = 0u;
    for (; i < 30u; ++i) {
        auto gain = env.adsr(key, i);
        UNUSED(gain);
        // std::cout << "i: " << i << " -> " << gain << std::endl;
    }
    env.setTriggerIndex(key, i);
    for (; i < 40u; ++i) {
        auto gain = env.adsr(key, i);
        // std::cout << "i: " << i << " -> " << gain << std::endl;
        UNUSED(gain);
    }
    UNUSED(env);
}


TEST(EnvelopeGenerator, NoCliping)
{
    DSP::EnvelopeDefaultLinear<DSP::EnvelopeType::ADSR, 1u> env;

    Key key { 69u };
    const SampleRate sampleRate { 44100u };

    DSP::EnvelopeSpecs specs {
        0.0f, // delay
        0.0f, // attack
        1.0f, // peak
        0.0f, // hold
        1.0f, // decay
        0.2f, // sustain
        1.0f // release
    };

    env.setSpecs<0u>(specs);
    env.setSampleRate(sampleRate);

    auto i = 0u;
    for (; i < 30u; ++i) {
        auto gain = env.getGain(key, i);
        UNUSED(gain);
        // std::cout << "i: " << i << " -> " << gain << std::endl;
    }
    env.setTriggerIndex(key, i);
    for (; i < 40u; ++i) {
        auto gain = env.getGain(key, i);
        UNUSED(gain);
        // std::cout << "i: " << i << " -> " << gain << std::endl;
    }
    UNUSED(env);
}
