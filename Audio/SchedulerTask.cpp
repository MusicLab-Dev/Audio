/**
 * @file SchedulerTask.cpp
 * @brief StereoArrangementTask implementation
 *
 * @author Pierre V
 * @date 2021-11-29
 */

#include <cassert>

#include "SchedulerTask.hpp"

void Audio::StereoArrangementTask::operator()(void) noexcept
{
    // return;
    const auto channels = static_cast<std::size_t>(_node->cache().channelArrangement());
    if (channels == 1u) {
        // std::cout << "[AudioScheduler] - StereoArrangementTask: Mono" << std::endl;
        return;
    }

    _cache.copy(_node->cache());
    const auto size = _cache.channelSampleCount();
    const auto *in = _cache.data<float>();
    auto *out = _node->cache().data<float>();

    assert(channels <= 2u);
    // "Audio::StereoArrangementTask: more than 2 channels not supported !"
    auto idx { 0ul };
    for (auto i = 0ul; i < size; ++i, idx += channels) {
        out[idx] = in[i];
        out[idx + 1u] = in[i + size];
    }
}
