/**
 * @ Author: Pierre Veysseyre
 * @ Description: Unit tests of Envelope Generator class
 */

#include <gtest/gtest.h>

#include <Audio/DSP/EnvelopeGenerator.hpp>

using namespace Audio;

static constexpr BlockSize Size = 1024u;
static constexpr SampleRate SR = 48000u;

using EnvAD =   DSP::EnvelopeBase<DSP::EnvelopeType::AD>;
using EnvAR =   DSP::EnvelopeBase<DSP::EnvelopeType::AR>;
using EnvADSR = DSP::EnvelopeBase<DSP::EnvelopeType::ADSR>;


TEST(EnvelopeGenerator, Simple_AttackDecay)
{
    EnvAD ad;
    UNUSED(ad);
}

TEST(EnvelopeGenerator, Simple_AttackRelease)
{
    EnvAR ar;
    UNUSED(ar);
}

TEST(EnvelopeGenerator, Simple_AttackDecayReleaseSustain)
{
    DSP::EnvelopeBase<DSP::EnvelopeType::ADSR> env;

    Key key { 69u };
    const float atk { 0.1f };
    const float dec { 0.1f };
    const float sus { 0.5f };
    const float rel { 0.1f };
    bool trigger { true };

    auto i = 0u;
    for (; i < 30u; ++i) {
        auto gain = env.adsr(key, i, trigger, atk, dec, sus, rel, 100u);
        UNUSED(gain);
        // std::cout << "i: " << i << " -> " << gain << std::endl;
    }
    trigger = false;
    env.setTriggerIndex(key, i);
    for (; i < 40u; ++i) {
        auto gain = env.adsr(key, i, trigger, atk, dec, sus, rel, 100u);
        // std::cout << "i: " << i << " -> " << gain << std::endl;
        UNUSED(gain);
    }
    UNUSED(env);
}
