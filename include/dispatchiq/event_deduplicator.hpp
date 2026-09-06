#pragma once

#include <cstddef>
#include <unordered_set>

namespace dispatchiq {

// Tracks processed event IDs so retries can be detected without storing event data.
struct EventDeduplicator {
    std::unordered_set<int> processedEventIds;
};

bool recordEvent(
    EventDeduplicator& eventDeduplicator,
    int eventId
);

bool hasProcessedEvent(
    const EventDeduplicator& eventDeduplicator,
    int eventId
);

bool forgetEvent(
    EventDeduplicator& eventDeduplicator,
    int eventId
);

std::size_t processedEventCount(
    const EventDeduplicator& eventDeduplicator
);

} // namespace dispatchiq
