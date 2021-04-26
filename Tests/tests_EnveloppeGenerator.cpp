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
    EnvADSR adsr;
    UNUSED(adsr);
}
