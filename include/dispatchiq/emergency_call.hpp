# pragma once

#include <cstddef>
#include <queue>
#include <string>
#include <utility>

namespace dispatchiq {
struct EmergencyCall {
    int callId{};
    std::string category;
    int reportedSeverity{};
    std::pair<double, double> location{};
};

bool enqueueCall(
    std::queue<EmergencyCall>& pendingCalls,
    const EmergencyCall& call
);

const EmergencyCall* peekNextCall(
    const std::queue<EmergencyCall>& pendingCalls
);

bool processNextCall(
    std::queue<EmergencyCall>& pendingCalls,
    EmergencyCall& processedCall
);

std::size_t pendingCallCount(
    const std::queue<EmergencyCall>& pendingCalls
);
} // namespace dispatchiq