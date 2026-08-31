# DispatchIQ

DispatchIQ is a C++20 emergency-response dispatch simulator built to demonstrate practical
data-structure selection. The project begins with an in-memory incident registry and will grow
into a dispatch workflow with intake queues, priority-based triage, responder lookup, route
management, reporting, and undo support.

## Current status

Milestones **DIQ-001: Incident Registry MVP**, **DIQ-002: Emergency Call Intake**, and
**DIQ-003: Incident Triage** are implemented. The project can:

- represent incident coordinates with `std::pair<double, double>`;
- preserve incident arrival order in `std::vector<Incident>`;
- accept incidents whose severity is between 1 and 5;
- reject duplicate incident IDs;
- locate an incident with a linear ID search;
- print accepted incidents and handle an empty registry;
- accept validated emergency calls into a `std::queue`;
- inspect the oldest pending call without removing it; and
- process calls in first-in, first-out order while handling an empty queue safely;
- prioritize incidents by severity with a `std::priority_queue`;
- preserve arrival order when incidents have equal severity; and
- safely peek, dispatch, and count prioritized incidents.

The command-line program is currently a deterministic DIQ-003 acceptance harness. Interactive
input, persistence, and automated tests are planned for later milestones.

## Requirements

- A C++20-compatible compiler such as Apple Clang, Clang, GCC, or MSVC
- CMake 3.20 or newer for the recommended build workflow

## Build and run

### CMake

From the project root:

```bash
cmake -S . -B build
cmake --build build
./build/dispatchiq
```

Run tests after test targets are introduced:

```bash
ctest --test-dir build --output-on-failure
```

### Compiler-only fallback

If CMake is not available on the command line:

```bash
mkdir -p build
clang++ -std=c++20 -Wall -Wextra -Wpedantic \
    -Iinclude \
    src/main.cpp src/incident.cpp src/emergency_call.cpp src/incident_triage.cpp \
    -o build/dispatchiq
./build/dispatchiq
```

## Example output

```text
=======INCIDENT TRIAGE=======

Initial count test passed.
Highest-priority peek test passed.
Incident 3001 was accepted.
Incident 3002 was accepted.
Incident 3003 was accepted.
Incident 3004 was accepted.
Incident 3005 was rejected.
Insertion count test passed.
Arrival-sequence test passed.
Highest-priority incident test passed.
Peek count test passed.
Dispatch priority test passed.
Final empty-state test passed.
Final peek test passed.
Final dispatch test passed.

DIQ-003 incident triage tests passed.
```

## Project layout

```text
DispatchIQ/
├── CMakeLists.txt
├── README.md
├── data/                       # Small sample datasets
├── docs/
│   ├── architecture.md         # Design and container decisions
│   ├── learning-roadmap.md     # Milestone sequence
│   └── milestones/             # Completed work records
├── include/dispatchiq/
│   ├── emergency_call.hpp       # Emergency-call model and queue operations
│   ├── incident.hpp             # Incident model and public operations
│   └── incident_triage.hpp      # Priority-triage model and operation contracts
├── src/
│   ├── emergency_call.cpp       # FIFO intake implementation
│   ├── incident.cpp             # Incident registry implementation
│   ├── incident_triage.cpp      # Severity and arrival-order triage
│   └── main.cpp                 # Current DIQ-003 acceptance harness
└── tests/                      # Future automated tests
```

## Data-structure roadmap

| Structure | Planned responsibility | Status |
|---|---|---|
| `std::pair` | Latitude and longitude | Implemented |
| `std::vector` | Incident history in arrival order | Implemented |
| `std::queue` | FIFO emergency-call intake | Implemented |
| `std::priority_queue` | Severity-based triage | Implemented |
| `std::unordered_map` | Fast lookup by incident or responder ID | Next |
| `std::unordered_set` | Duplicate-event detection | Planned |
| `std::map` | Ordered operational reports | Planned |
| `std::set` | Sorted unique skills and service regions | Planned |
| `std::list` | Frequently edited route stops | Planned |
| `std::stack` | Undo history for dispatch actions | Planned |

## Documentation

- [Architecture and design decisions](docs/architecture.md)
- [Learning roadmap](docs/learning-roadmap.md)
- [DIQ-001 milestone record](docs/milestones/DIQ-001-incident-registry.md)
- [DIQ-002 milestone record](docs/milestones/DIQ-002-emergency-call-intake.md)
- [DIQ-003 milestone record](docs/milestones/DIQ-003-incident-triage.md)

## Current limitations

- Records exist only for the lifetime of the process.
- Duplicate detection and ID lookup are linear-time operations.
- Duplicate emergency-call IDs are not yet detected.
- Duplicate IDs are not yet detected when incidents enter triage.
- Latitude and longitude ranges are not yet validated.
- Validation operations report success or failure but not the precise rejection reason.
- Verification is currently manual; automated tests have not yet been added.
