/**
 * @ Author: Pierre Veysseyre
 * @ Description: Unit tests of Enveloppe Generator class
 */

#include <gtest/gtest.h>

#include <Audio/DSP/EnveloppeGenerator.hpp>

using namespace Audio;

static constexpr BlockSize Size = 1024u;
static constexpr SampleRate SR = 48000u;

using EnvAD =   DSP::EnveloppeBase<DSP::EnveloppeType::AD>;
using EnvAR =   DSP::EnveloppeBase<DSP::EnveloppeType::AR>;
using EnvADSR = DSP::EnveloppeBase<DSP::EnveloppeType::ADSR>;


TEST(EnveloppeGenerator, Simple_AttackDecay)
{
    EnvAD ad;
    UNUSED(ad);
}

TEST(EnveloppeGenerator, Simple_AttackRelease)
{
    EnvAR ar;
    UNUSED(ar);
}

TEST(EnveloppeGenerator, Simple_AttackDecayReleaseSustain)
{
    DSP::EnveloppeBase<DSP::EnveloppeType::ADSR> env;

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
