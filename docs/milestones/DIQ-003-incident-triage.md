# DIQ-003: Incident Triage

**Status:** Implemented

**Primary container:** `std::priority_queue`

## Business requirement

Dispatchers must handle the most severe incident first. Incidents with equal severity must retain
their arrival order so an equally urgent incident cannot jump ahead of an older one.

## Delivered behavior

- Wraps incidents with a monotonic arrival sequence.
- Accepts severity values from 1 through 5.
- Rejects invalid severity without changing queue state or consuming a sequence number.
- Keeps the highest-severity incident at the top of the triage queue.
- Uses first-arrived-first-out ordering when severities match.
- Peeks at the highest-priority incident without removing it.
- Copies and removes the highest-priority incident during dispatch.
- Reports the pending incident count and handles empty operations safely.

## Files

| File | Responsibility |
|---|---|
| `include/dispatchiq/incident_triage.hpp` | Triage entry, comparator, queue alias, state, and contracts |
| `src/incident_triage.cpp` | Ordering, validation, insertion, peeking, dispatch, and count behavior |
| `src/main.cpp` | Deterministic DIQ-003 acceptance scenario |
| `CMakeLists.txt` | Builds the triage implementation into the executable |

## Ordering contract

- Severity 5 outranks severity 4, continuing down through severity 1.
- For equal severity, a smaller arrival sequence outranks a larger sequence.
- The comparator returns `true` when the left entry has lower priority than the right entry.

For arrivals 3001/severity 2, 3002/severity 5, 3003/severity 3, and 3004/severity 5,
the expected dispatch order is 3002, 3004, 3003, and 3001.

## Acceptance results

| Scenario | Expected result | Result |
|---|---|---|
| Inspect empty triage | Count 0 and `nullptr` peek | Pass |
| Add incidents 3001 through 3004 | Accepted with sequences 0 through 3 | Pass |
| Add incident 3005 with severity 8 | Rejected; sequence remains 4 | Pass |
| Peek after insertion | Incident 3002; count remains 4 | Pass |
| Dispatch all valid incidents | IDs 3002, 3004, 3003, 3001 | Pass |
| Observe counts during dispatch | Counts transition from 4 to 3 to 2 to 1 to 0 | Pass |
| Operate after triage is empty | `nullptr` peek and `false` dispatch | Pass |

## Complexity

- Severity validation: O(1)
- Priority-queue insertion: O(log n)
- Highest-priority peek: O(1)
- Highest-priority dispatch: O(log n)
- Pending count: O(1)

Copying an `Incident` may additionally depend on the length of its string fields.

## Pointer lifetime

The pointer returned by `peekHighestPriorityIncident` is non-owning. It must not be dereferenced
after an insertion or dispatch mutates the priority queue or after the triage state is destroyed.

## Deferred work

- Detect duplicate triage IDs with a hash-based index.
- Replace the manual acceptance harness with automated unit tests.
- Validate latitude and longitude ranges.
- Replace Boolean validation results with a type that reports the rejection reason.
- Encapsulate triage state behind a class if external mutation becomes a maintenance risk.

## Concepts demonstrated

- Priority-queue adapters and heap ordering
- Stateful custom comparators
- Multi-key ordering by severity and arrival sequence
- Stable tie-breaking over a container that is not inherently stable
- `push`, `top`, `pop`, `empty`, and `size`
- Output parameters and copy-before-pop ordering
- Pointer lifetime after container mutation
