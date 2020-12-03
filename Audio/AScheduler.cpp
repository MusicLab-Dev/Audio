/**
 * @ Author: Pierre Veysseyre
 * @ Description: AScheduler
 */

#include "SchedulerTask.hpp"

using namespace Audio;

void AScheduler::setState(const State state) noexcept
{
    switch (state) {
    case State::Pause:
        _state = state;
        break;
    case State::Play:
    {
        for (State expected = State::Pause; !_state.compare_exchange_strong(expected, State::Play);) {
            if (expected == State::Play)
                return;
        }
        scheduleProjectGraph();
        break;
    }
    }
}

void AScheduler::buildProjectGraph(void)
{
    // auto *parent = _project->master().get();

    // TaskSchedulerAudioWrite task(parent, this);
    // task();

    // auto flowNode = _flow->emplace(task);

    // Master -> controls / partitions
    // Mixer 1 -> controls / partitions
    // Arpeggiator 1 -> controls / partitions
    // Sampler 1 -> controls / partitions
    // Sampler 1 -> audio
    // Sampler 2 -> controls / partitions
    // Sampler 2 -> audio
    // Arpegiator 1 -> audio:
    /*  -> Check if node has audio ? -> No
          -> Check if node has at least one children with audio ? -> Yes
            -> Merge children
    */
    // Effect 1 -> controls / partitions
    // Sampler 3 -> controls / partitions
    // Sampler 3 -> audio
}

/*

    // Static task
    _flow->emplace([](void) -> void {
    });


    // Switch task
    auto switchNode = _flow->emplace([](void) -> int {
        return 2; // Only case2 will follow
    });
    auto [case0, case1, case2] = _flow->emplace(
        [] {},
        [] {},
        [] {}
    );
    switchNode.precede(case0, case1, case2, switchNode);
*/