#include "dispatchiq/event_deduplicator.hpp"

namespace dispatchiq {

bool recordEvent(
    EventDeduplicator& eventDeduplicator,
    int eventId
) {
    if (eventId <= 0) {
        return false;
    }

    const auto insertionResult =
        eventDeduplicator.processedEventIds.insert(eventId);

    return insertionResult.second;
}

bool hasProcessedEvent(
    const EventDeduplicator& eventDeduplicator,
    int eventId
) {
    if (eventId <= 0) {
        return false;
    }

    return eventDeduplicator.processedEventIds.contains(eventId);
}

bool forgetEvent(
    EventDeduplicator& eventDeduplicator,
    int eventId
) {
    if (eventId <= 0) {
        return false;
    }

    const std::size_t erasedCount =
        eventDeduplicator.processedEventIds.erase(eventId);

    return erasedCount == 1;
}

std::size_t processedEventCount(
    const EventDeduplicator& eventDeduplicator
) {
    return eventDeduplicator.processedEventIds.size();
}

} // namespace dispatchiq
