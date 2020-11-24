/**
 * @ Author: Pierre Veysseyre
 * @ Description: AScheduler
 */

#include "AScheduler.hpp"

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

}