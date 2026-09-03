# DispatchIQ learning roadmap

The roadmap introduces one container at a time, beginning with a measurable baseline and then
refactoring when a new requirement makes another data structure appropriate.

## Foundation

- [x] Create a C++20 and CMake project scaffold.
- [x] Separate public headers from implementation files.
- [x] Enable common compiler warnings.
- [ ] Add an automated test framework and continuous integration.

## Feature milestones

| Ticket | Feature | Primary concepts | Status |
|---|---|---|---|
| DIQ-001 | Incident Registry MVP | `std::pair`, `std::vector`, linear search, pointers | Implemented |
| DIQ-002 | Emergency Call Intake | `std::queue`, FIFO behavior | Implemented |
| DIQ-003 | Incident Triage | `std::priority_queue`, custom comparator | Implemented |
| DIQ-004 | Fast Incident Index | `std::unordered_map`, hash lookup | Implemented |
| DIQ-005 | Event Deduplication | `std::unordered_set`, uniqueness | Next |
| DIQ-006 | Ordered Operations Report | `std::map`, `std::set`, ordering | Planned |
| DIQ-007 | Responder Route Editing | `std::list`, iterators, insertion and erasure | Planned |
| DIQ-008 | Dispatch Undo History | `std::stack`, LIFO behavior | Planned |

## Project-hardening milestones

- [ ] Replace demonstration-only input with a command-line workflow.
- [ ] Validate latitude and longitude ranges.
- [ ] Persist and restore incident data.
- [ ] Add unit and integration tests.
- [ ] Benchmark linear, ordered, and hash-based lookup strategies.
- [ ] Add representative sample datasets.
- [ ] Document failure modes and operational assumptions.
- [ ] Produce a reproducible release build and demonstration script.

## Learning outcomes

By the end of the roadmap, the project should demonstrate:

- selecting containers based on ordering, lookup, mutation, and memory requirements;
- explaining average and worst-case complexity;
- managing iterator, reference, and pointer validity;
- separating domain logic from input/output code;
- testing data-structure behavior and edge cases; and
- communicating engineering decisions in project documentation.
