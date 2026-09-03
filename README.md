# DispatchIQ

DispatchIQ is a C++20 emergency-response dispatch simulator built to demonstrate practical
data-structure selection. The project begins with an in-memory incident registry and will grow
into a dispatch workflow with intake queues, priority-based triage, responder lookup, route
management, reporting, and undo support.

## Current status

Milestones **DIQ-001: Incident Registry MVP**, **DIQ-002: Emergency Call Intake**,
**DIQ-003: Incident Triage**, and **DIQ-004: Fast Incident Index** are implemented. The project
can:

- represent incident coordinates with `std::pair<double, double>`;
- preserve incident arrival order in `std::vector<Incident>`;
- accept incidents whose severity is between 1 and 5;
- reject duplicate incident IDs;
- locate an incident with a linear ID search;
- print accepted incidents and handle an empty registry;
- accept validated emergency calls into a `std::queue`;
- inspect the oldest pending call without removing it;
- process calls in first-in, first-out order while handling an empty queue safely;
- prioritize incidents by severity with a `std::priority_queue`;
- preserve arrival order when incidents have equal severity;
- safely peek, dispatch, and count prioritized incidents;
- index active incidents by ID with `std::unordered_map<int, Incident>`;
- reject duplicate index entries without overwriting the original incident; and
- find and remove indexed incidents with average constant-time operations.

The command-line program is currently a deterministic DIQ-004 acceptance harness. Interactive
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
    src/incident_index.cpp \
    -o build/dispatchiq
./build/dispatchiq
```

## Example output

```text
=======FAST INCIDENT INDEX=======

Initial count test passed.
Empty-index lookup test passed.
Incident 4001 accepted.
Incident 4002 accepted.
Incident 4003 accepted.
Duplicate incident 4001 was rejected as expected.
Invalid incident 4004 was rejected as expected.
Insertion count test passed.
Existing incident lookup test passed.
Missing incident lookup test passed.
Duplicate protection test passed.
Existing incident removal test passed.
Post-removal count test passed.
Post-removal lookup test passed.
Missing incident removal test passed.
Missing-removal count test passed.
Reinsertion test passed.
Final count test passed.

DIQ-004 fast incident index tests passed.
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
│   ├── incident_index.hpp       # Hash index model and operation contracts
│   ├── incident.hpp             # Incident model and public operations
│   └── incident_triage.hpp      # Priority-triage model and operation contracts
├── src/
│   ├── emergency_call.cpp       # FIFO intake implementation
│   ├── incident_index.cpp       # Hash-based incident lookup and removal
│   ├── incident.cpp             # Incident registry implementation
│   ├── incident_triage.cpp      # Severity and arrival-order triage
│   └── main.cpp                 # Current DIQ-004 acceptance harness
└── tests/                      # Future automated tests
```

## Data-structure roadmap

| Structure | Planned responsibility | Status |
|---|---|---|
| `std::pair` | Latitude and longitude | Implemented |
| `std::vector` | Incident history in arrival order | Implemented |
| `std::queue` | FIFO emergency-call intake | Implemented |
| `std::priority_queue` | Severity-based triage | Implemented |
| `std::unordered_map` | Fast lookup by incident or responder ID | Implemented |
| `std::unordered_set` | Duplicate-event detection | Next |
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
- [DIQ-004 milestone record](docs/milestones/DIQ-004-fast-incident-index.md)

## Current limitations

- Records exist only for the lifetime of the process.
- The original vector registry still uses linear duplicate detection and lookup; the active
  incident index provides average constant-time alternatives.
- Duplicate emergency-call IDs are not yet detected.
- Duplicate IDs are not yet detected when incidents enter triage.
- Indexed incidents are independent copies, so synchronization with the registry and triage is
  not yet coordinated by a single workflow.
- Hash-table iteration order is intentionally unspecified and must not be used for reporting.
- Latitude and longitude ranges are not yet validated.
- Validation operations report success or failure but not the precise rejection reason.
- Verification is currently manual; automated tests have not yet been added.
