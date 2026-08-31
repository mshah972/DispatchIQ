#include "dispatchiq/incident_triage.hpp"

namespace dispatchiq {

bool IncidentPriority::operator()(
    const TriageEntry& left,
    const TriageEntry& right
) const {
    if (left.incident.severity != right.incident.severity) {
        return left.incident.severity < right.incident.severity;
    }

    return left.arrivalSequence > right.arrivalSequence;
}

bool addIncidentToTriage(
    IncidentTriage& triage,
    const Incident& incident
) {
    if (incident.severity < 1 || incident.severity > 5) {
        return false;
    }

    const TriageEntry entry {
        .incident = incident,
        .arrivalSequence = triage.nextArrivalSequence
    };

    triage.pendingIncidents.push(entry);
    ++triage.nextArrivalSequence;
    return true;
}

const Incident* peekHighestPriorityIncident(
    const IncidentTriage& triage
) {
    if (triage.pendingIncidents.empty()) {
        return nullptr;
    }

    const TriageEntry& highestPriorityEntry = triage.pendingIncidents.top();
    return &highestPriorityEntry.incident;
}

bool dispatchHighestPriorityIncident(
    IncidentTriage& triage,
    Incident& dispatchedIncident
) {
    if (triage.pendingIncidents.empty()) {
        return false;
    }

    dispatchedIncident = triage.pendingIncidents.top().incident;
    triage.pendingIncidents.pop();
    return true;
}

std::size_t pendingTriageCount(
    const IncidentTriage& triage
) {
    return triage.pendingIncidents.size();
}

} // namespace dispatchiq
