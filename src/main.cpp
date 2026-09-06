#include "dispatchiq/event_deduplicator.hpp"

#include <array>
#include <cstddef>
#include <iostream>

int main() {
    std::cout << "=======EVENT DEDUPLICATION=======\n\n";

    bool allTestsPassed{true};
    dispatchiq::EventDeduplicator eventDeduplicator{};

    const std::size_t initialCount =
        dispatchiq::processedEventCount(eventDeduplicator);

    if (initialCount == 0) {
        std::cout << "Initial count test passed.\n";
    } else {
        std::cout << "Initial count test failed: expected 0, received "
                  << initialCount << ".\n";
        allTestsPassed = false;
    }

    if (!dispatchiq::hasProcessedEvent(eventDeduplicator, 5001)) {
        std::cout << "Unseen event test passed.\n";
    } else {
        std::cout << "Unseen event test failed: event 5001 was unexpectedly present.\n";
        allTestsPassed = false;
    }

    constexpr std::array<int, 3> eventIds{5001, 5002, 5003};

    for (const int eventId : eventIds) {
        if (dispatchiq::recordEvent(eventDeduplicator, eventId)) {
            std::cout << "Event " << eventId << " accepted.\n";
        } else {
            std::cout << "Event " << eventId << " was unexpectedly rejected.\n";
            allTestsPassed = false;
        }
    }

    const std::size_t afterRecordingCount =
        dispatchiq::processedEventCount(eventDeduplicator);

    if (afterRecordingCount == eventIds.size()) {
        std::cout << "Unique-event count test passed.\n";
    } else {
        std::cout << "Unique-event count test failed: expected "
                  << eventIds.size() << ", received "
                  << afterRecordingCount << ".\n";
        allTestsPassed = false;
    }

    bool allRecordedEventsPresent{true};

    for (const int eventId : eventIds) {
        if (!dispatchiq::hasProcessedEvent(eventDeduplicator, eventId)) {
            std::cout << "Recorded-event membership test failed: event "
                      << eventId << " is missing.\n";
            allRecordedEventsPresent = false;
            allTestsPassed = false;
        }
    }

    if (allRecordedEventsPresent) {
        std::cout << "Recorded-event membership test passed.\n";
    }

    if (!dispatchiq::recordEvent(eventDeduplicator, 5002)) {
        std::cout << "Duplicate event rejection test passed.\n";
    } else {
        std::cout << "Duplicate event rejection test failed: event 5002 was accepted.\n";
        allTestsPassed = false;
    }

    const std::size_t afterDuplicateCount =
        dispatchiq::processedEventCount(eventDeduplicator);

    if (afterDuplicateCount == 3) {
        std::cout << "Duplicate-attempt count test passed.\n";
    } else {
        std::cout << "Duplicate-attempt count test failed: expected 3, received "
                  << afterDuplicateCount << ".\n";
        allTestsPassed = false;
    }

    if (dispatchiq::hasProcessedEvent(eventDeduplicator, 5002)) {
        std::cout << "Duplicate preservation test passed.\n";
    } else {
        std::cout << "Duplicate preservation test failed: event 5002 is missing.\n";
        allTestsPassed = false;
    }

    const bool zeroAccepted =
        dispatchiq::recordEvent(eventDeduplicator, 0);
    const bool negativeAccepted =
        dispatchiq::recordEvent(eventDeduplicator, -5);

    if (!zeroAccepted && !negativeAccepted) {
        std::cout << "Invalid event rejection test passed.\n";
    } else {
        std::cout << "Invalid event rejection test failed:";

        if (zeroAccepted) {
            std::cout << " event 0 was accepted;";
        }

        if (negativeAccepted) {
            std::cout << " event -5 was accepted;";
        }

        std::cout << "\n";
        allTestsPassed = false;
    }

    const std::size_t afterInvalidCount =
        dispatchiq::processedEventCount(eventDeduplicator);

    if (afterInvalidCount == 3) {
        std::cout << "Invalid-attempt count test passed.\n";
    } else {
        std::cout << "Invalid-attempt count test failed: expected 3, received "
                  << afterInvalidCount << ".\n";
        allTestsPassed = false;
    }

    const bool zeroPresent =
        dispatchiq::hasProcessedEvent(eventDeduplicator, 0);
    const bool negativePresent =
        dispatchiq::hasProcessedEvent(eventDeduplicator, -5);

    if (!zeroPresent && !negativePresent) {
        std::cout << "Invalid-event membership test passed.\n";
    } else {
        std::cout << "Invalid-event membership test failed.\n";
        allTestsPassed = false;
    }

    if (dispatchiq::forgetEvent(eventDeduplicator, 5002)) {
        std::cout << "Existing event removal test passed.\n";
    } else {
        std::cout << "Existing event removal test failed: event 5002 was not removed.\n";
        allTestsPassed = false;
    }

    const std::size_t afterRemovalCount =
        dispatchiq::processedEventCount(eventDeduplicator);

    if (afterRemovalCount == 2) {
        std::cout << "Post-removal count test passed.\n";
    } else {
        std::cout << "Post-removal count test failed: expected 2, received "
                  << afterRemovalCount << ".\n";
        allTestsPassed = false;
    }

    if (!dispatchiq::hasProcessedEvent(eventDeduplicator, 5002)) {
        std::cout << "Forgotten-event lookup test passed.\n";
    } else {
        std::cout << "Forgotten-event lookup test failed: event 5002 is still present.\n";
        allTestsPassed = false;
    }

    const bool event5001Present =
        dispatchiq::hasProcessedEvent(eventDeduplicator, 5001);
    const bool event5003Present =
        dispatchiq::hasProcessedEvent(eventDeduplicator, 5003);

    if (event5001Present && event5003Present) {
        std::cout << "Remaining-event preservation test passed.\n";
    } else {
        std::cout << "Remaining-event preservation test failed.\n";
        allTestsPassed = false;
    }

    if (!dispatchiq::forgetEvent(eventDeduplicator, 5002)) {
        std::cout << "Missing event removal test passed.\n";
    } else {
        std::cout << "Missing event removal test failed: removal returned true.\n";
        allTestsPassed = false;
    }

    const std::size_t afterMissingRemovalCount =
        dispatchiq::processedEventCount(eventDeduplicator);

    if (afterMissingRemovalCount == 2) {
        std::cout << "Missing-removal count test passed.\n";
    } else {
        std::cout << "Missing-removal count test failed: expected 2, received "
                  << afterMissingRemovalCount << ".\n";
        allTestsPassed = false;
    }

    if (!dispatchiq::forgetEvent(eventDeduplicator, 0)) {
        std::cout << "Invalid event removal test passed.\n";
    } else {
        std::cout << "Invalid event removal test failed: removal returned true.\n";
        allTestsPassed = false;
    }

    const std::size_t afterInvalidRemovalCount =
        dispatchiq::processedEventCount(eventDeduplicator);

    if (afterInvalidRemovalCount == 2) {
        std::cout << "Invalid-removal count test passed.\n";
    } else {
        std::cout << "Invalid-removal count test failed: expected 2, received "
                  << afterInvalidRemovalCount << ".\n";
        allTestsPassed = false;
    }

    if (dispatchiq::recordEvent(eventDeduplicator, 5002)) {
        std::cout << "Event reinsertion test passed.\n";
    } else {
        std::cout << "Event reinsertion test failed: event 5002 was rejected.\n";
        allTestsPassed = false;
    }

    const std::size_t finalCount =
        dispatchiq::processedEventCount(eventDeduplicator);

    if (finalCount == 3) {
        std::cout << "Final count test passed.\n";
    } else {
        std::cout << "Final count test failed: expected 3, received "
                  << finalCount << ".\n";
        allTestsPassed = false;
    }

    if (dispatchiq::hasProcessedEvent(eventDeduplicator, 5002)) {
        std::cout << "Re-recorded event lookup test passed.\n";
    } else {
        std::cout << "Re-recorded event lookup test failed: event 5002 is missing.\n";
        allTestsPassed = false;
    }

    if (allTestsPassed) {
        std::cout << "\nDIQ-005 event deduplication tests passed.\n";
    } else {
        std::cout << "\nDIQ-005 event deduplication tests failed.\n";
    }

    return allTestsPassed ? 0 : 1;
}
