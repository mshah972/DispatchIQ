#pragma once

#include "dispatchiq/incident.hpp"

#include <cstddef>
#include <queue>
#include <vector>

namespace dispatchiq {

// Represents one incident inside the priority queue.
struct TriageEntry {
    Incident incident;
    std::size_t arrivalSequence{};
};

// Determines which TriageEntry has higher priority.
struct IncidentPriority {
    bool operator()(
        const TriageEntry& left,
        const TriageEntry& right
    ) const;
};

// The priority queue type used by the triage system.
using TriageQueue = std::priority_queue<
    TriageEntry,
    std::vector<TriageEntry>,
    IncidentPriority
>;

// Stores the queue and generates arrival sequence numbers.
struct IncidentTriage {
    TriageQueue pendingIncidents;
    std::size_t nextArrivalSequence{};
};

bool addIncidentToTriage(
    IncidentTriage& triage,
    const Incident& incident
);

const Incident* peekHighestPriorityIncident(
    const IncidentTriage& triage
);

bool dispatchHighestPriorityIncident(
    IncidentTriage& triage,
    Incident& dispatchedIncident
);

std::size_t pendingTriageCount(
    const IncidentTriage& triage
);

} // namespace dispatchiq
