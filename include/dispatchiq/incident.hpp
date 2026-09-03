#pragma once

#include <string>
#include <utility>
#include <vector>

namespace dispatchiq {

struct Incident {
    int id;
    std::string category;
    int severity;
    std::pair<double, double> location;
};

bool addIncident(
    std::vector<Incident>& incidents,
    const Incident& incident
);

const Incident* findIncidentById(
    const std::vector<Incident>& incidents,
    int id
);

void printIncidents(
    const std::vector<Incident>& incidents
);

} // namespace dispatchiq
