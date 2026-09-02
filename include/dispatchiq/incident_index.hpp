#pragma once

#include "dispatchiq/incident.hpp"

#include <cstddef>
#include <unordered_map>

namespace dispatchiq {

// Owns active incidents keyed by their unique incident ID.
struct IncidentIndex {
    std::unordered_map<int, Incident> incidentsById;
};

bool addIncidentToIndex(
    IncidentIndex& index,
    const Incident& incident
);

const Incident* findIncidentInIndexById(
    const IncidentIndex& index,
    int incidentId
);

bool removeIncidentFromIndex(
    IncidentIndex& index,
    int incidentId
);

std::size_t indexedIncidentCount(
    const IncidentIndex& index
);

} // namespace dispatchiq
