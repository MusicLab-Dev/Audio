/**
 * @ Author: Pierre Veysseyre
 * @ Description: DummyPlugin.hpp
 */

#pragma once

#include <Audio/IPlugin.hpp>
#include <Audio/Plugins/Managers/NoteManager.hpp>


static std::atomic<int> GlobalCounter { 0 };

struct TestDataBase
{
    enum class Dir {
        In, Out
    };

    int frame { 0 };
    Dir dir;
};

struct NoteTestData : public TestDataBase
{
    NoteEvents noteData {};
};

struct AudioTestData : public TestDataBase
{
    BufferViews audioData {};
};

struct DummyPluginBase
{
    std::vector<NoteTestData> noteData;
    std::vector<AudioTestData> audioData;
};

static void FeedNoteTestData(std::vector<NoteTestData> &testData, const NoteEvents &noteEvents, const TestDataBase::Dir dir)
{
    std::cout << (dir == TestDataBase::Dir::Out ? "receive: " : "send: ") << noteEvents.size() << " notes" << std::endl;
    NoteTestData data;
    data.dir = dir;
    data.frame = GlobalCounter;
    for (const auto &evt : noteEvents)
        data.noteData.push(evt);
    testData.push_back(data);
    std::cout << testData.size() << std::endl;
}

static void FeedAudioTestData(std::vector<AudioTestData> &testData, const BufferViews &audioBuffers, const TestDataBase::Dir dir)
{
    AudioTestData data;
    data.dir = dir;
    data.frame = GlobalCounter;
    for (const auto &buf : audioBuffers)
        data.audioData.push(buf);
    testData.push_back(data);
}


struct DummyCompleteIO : public IPlugin, public DummyPluginBase
{
    NoteManager noteManager;
    Buffer buffer;

    virtual ~DummyCompleteIO(void) override = default;

    IPlugin::Flags getFlags(void) const noexcept
    {
        return static_cast<Flags>(
            static_cast<std::size_t>(Flags::AudioInput) |
            static_cast<std::size_t>(Flags::AudioOutput) |
            static_cast<std::size_t>(Flags::NoteInput) |
            static_cast<std::size_t>(Flags::NoteOutput) |
            static_cast<std::size_t>(Flags::ControlInput)
        );
    }

    virtual void sendAudio(const BufferViews &inputs) noexcept
    {
        FeedAudioTestData(audioData, inputs, TestDataBase::Dir::In);
    }

    virtual void receiveAudio(BufferView output) noexcept
    {
        FeedAudioTestData(audioData, BufferViews({ output }), TestDataBase::Dir::Out);
        noteManager.resetBlockCache();
    }

    virtual void sendNotes(const NoteEvents &notes) noexcept
    {
        FeedNoteTestData(noteData, notes, TestDataBase::Dir::In);
    }

    virtual void receiveNotes(NoteEvents &notes) noexcept
    {
        FeedNoteTestData(noteData, notes, TestDataBase::Dir::Out);
    }

    virtual void sendControls(const ControlEvents &controls) noexcept {}

    virtual void onAudioGenerationStarted(const BeatRange &range) noexcept {}
};

struct DummyNoteInAudioOut : public DummyPluginBase, public IPlugin
{
    NoteManager noteManager;
    Buffer buffer;

    virtual ~DummyNoteInAudioOut(void) override = default;

    IPlugin::Flags getFlags(void) const noexcept
    {
        return static_cast<Flags>(
            static_cast<std::size_t>(Flags::AudioOutput) |
            static_cast<std::size_t>(Flags::NoteInput) |
            static_cast<std::size_t>(Flags::ControlInput)
        );
    }
    virtual void sendAudio(const BufferViews &inputs) noexcept {}
    virtual void receiveNotes(NoteEvents &notes) noexcept {}

    virtual void receiveAudio(BufferView output) noexcept
    {
        FeedAudioTestData(audioData, BufferViews({ output }), TestDataBase::Dir::Out);
        noteManager.resetBlockCache();
    }

    virtual void sendNotes(const NoteEvents &notes) noexcept
    {
        FeedNoteTestData(noteData, notes, TestDataBase::Dir::In);
    }

    virtual void sendControls(const ControlEvents &controls) noexcept {}

    virtual void onAudioGenerationStarted(const BeatRange &range) noexcept {}
};

struct DummyAudioIO : public IPlugin, public DummyPluginBase
{
    Buffer buffer;

    virtual ~DummyAudioIO(void) override = default;

    IPlugin::Flags getFlags(void) const noexcept
    {
        return static_cast<Flags>(
            static_cast<std::size_t>(Flags::AudioInput) |
            static_cast<std::size_t>(Flags::AudioOutput) |
            static_cast<std::size_t>(Flags::ControlInput)
        );
    }
    virtual void sendNotes(const NoteEvents &notes) noexcept {}
    virtual void receiveNotes(NoteEvents &notes) noexcept {}

    virtual void sendAudio(const BufferViews &inputs) noexcept
    {
        FeedAudioTestData(audioData, inputs, TestDataBase::Dir::In);
    }

    virtual void receiveAudio(BufferView output) noexcept
    {
        FeedAudioTestData(audioData, BufferViews({ output }), TestDataBase::Dir::Out);
    }

    virtual void sendControls(const ControlEvents &controls) noexcept {}

    virtual void onAudioGenerationStarted(const BeatRange &range) noexcept {}
};

struct DummyNoteIO : public IPlugin, public DummyPluginBase
{
    NoteManager noteManager;

    virtual ~DummyNoteIO(void) override = default;

    IPlugin::Flags getFlags(void) const noexcept
    {
        return static_cast<Flags>(
            static_cast<std::size_t>(Flags::NoteInput) |
            static_cast<std::size_t>(Flags::NoteOutput) |
            static_cast<std::size_t>(Flags::ControlInput)
        );
    }
    virtual void sendAudio(const BufferViews &inputs) noexcept {}
    virtual void receiveAudio(BufferView output) noexcept {}


    virtual void sendNotes(const NoteEvents &notes) noexcept
    {
        FeedNoteTestData(noteData, notes, TestDataBase::Dir::In);
    }

    virtual void receiveNotes(NoteEvents &notes) noexcept
    {
        FeedNoteTestData(noteData, notes, TestDataBase::Dir::Out);
    }

    virtual void sendControls(const ControlEvents &controls) noexcept {}

    virtual void onAudioGenerationStarted(const BeatRange &range) noexcept {}
};
