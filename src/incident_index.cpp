#include "dispatchiq/incident_index.hpp"

namespace dispatchiq {

bool addIncidentToIndex(
    IncidentIndex& index,
    const Incident& incident
) {
    if (incident.severity < 1 || incident.severity > 5) {
        return false;
    }

    const auto insertionResult =
        index.incidentsById.emplace(incident.id, incident);

    return insertionResult.second;
}

const Incident* findIncidentInIndexById(
    const IncidentIndex& index,
    int incidentId
) {
    const auto findResult = index.incidentsById.find(incidentId);

    if (findResult == index.incidentsById.end()) {
        return nullptr;
    }

    return &findResult->second;
}

bool removeIncidentFromIndex(
    IncidentIndex& index,
    int incidentId
) {
    const std::size_t erasedCount =
        index.incidentsById.erase(incidentId);

    return erasedCount == 1;
}

std::size_t indexedIncidentCount(
    const IncidentIndex& index
) {
    return index.incidentsById.size();
}

} // namespace dispatchiq
