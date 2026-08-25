# DispatchIQ

DispatchIQ is a C++20 emergency-response dispatch simulator built to demonstrate practical
data-structure selection. The project begins with an in-memory incident registry and will grow
into a dispatch workflow with intake queues, priority-based triage, responder lookup, route
management, reporting, and undo support.

## Current status

Milestone **DIQ-001: Incident Registry MVP** is implemented. The current command-line demo can:

- represent incident coordinates with `std::pair<double, double>`;
- preserve incident arrival order in `std::vector<Incident>`;
- accept incidents whose severity is between 1 and 5;
- reject duplicate incident IDs;
- locate an incident with a linear ID search; and
- print accepted incidents and handle an empty registry.

The program is currently a deterministic demonstration harness. Interactive input, persistence,
and automated tests are planned for later milestones.

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
    src/main.cpp src/incident.cpp \
    -o build/dispatchiq
./build/dispatchiq
```

## Example output

```text
No incidents have been recorded.
Incident 1001 accepted.
Incident 1002 accepted.
Incident 1001 rejected.
Incident 1003 rejected.
1001. Fire - 5 - (29.7604, -95.3698)
1002. Medical - 4 - (32.7767, -96.797)
Incident 1002 found.
Incident 9999 not found.
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
│   └── incident.hpp            # Incident model and public operations
├── src/
│   ├── incident.cpp            # Incident registry implementation
│   └── main.cpp                # Current demonstration program
└── tests/                      # Future automated tests
```

## Data-structure roadmap

| Structure | Planned responsibility | Status |
|---|---|---|
| `std::pair` | Latitude and longitude | Implemented |
| `std::vector` | Incident history in arrival order | Implemented |
| `std::queue` | FIFO emergency-call intake | Next |
| `std::priority_queue` | Severity-based triage | Planned |
| `std::unordered_map` | Fast lookup by incident or responder ID | Planned |
| `std::unordered_set` | Duplicate-event detection | Planned |
| `std::map` | Ordered operational reports | Planned |
| `std::set` | Sorted unique skills and service regions | Planned |
| `std::list` | Frequently edited route stops | Planned |
| `std::stack` | Undo history for dispatch actions | Planned |

## Documentation

- [Architecture and design decisions](docs/architecture.md)
- [Learning roadmap](docs/learning-roadmap.md)
- [DIQ-001 milestone record](docs/milestones/DIQ-001-incident-registry.md)

## Current limitations

- Records exist only for the lifetime of the process.
- Duplicate detection and ID lookup are linear-time operations.
- Latitude and longitude ranges are not yet validated.
- `addIncident` reports success or failure but not the precise rejection reason.
- Verification is currently manual; automated tests have not yet been added.
