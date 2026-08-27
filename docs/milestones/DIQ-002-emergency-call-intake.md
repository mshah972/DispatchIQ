# DIQ-002: Emergency Call Intake

**Status:** Implemented
**Primary container:** `std::queue`

## Business requirement

Emergency calls must enter an intake queue and be processed in arrival order before a later triage
stage assigns operational priority.

## Delivered behavior

- Defines emergency calls with an ID, category, reported severity, and location.
- Accepts reported severity values from 1 through 5.
- Rejects invalid severity without changing the queue.
- Adds accepted calls to the back of the queue.
- Peeks at the oldest call without removing it.
- Copies and removes the oldest call during processing.
- Reports pending-call count and handles empty-queue operations safely.

## Files

| File | Responsibility |
|---|---|
| `include/dispatchiq/emergency_call.hpp` | Emergency-call model and public queue operations |
| `src/emergency_call.cpp` | Validation, enqueue, peek, process, and count behavior |
| `src/main.cpp` | Manual DIQ-002 acceptance scenario |
| `CMakeLists.txt` | Builds the emergency-call implementation into the executable |

## Acceptance results

| Scenario | Expected result | Result |
|---|---|---|
| Inspect an empty queue | Count 0 and `nullptr` peek | Pass |
| Enqueue calls 2001, 2002, and 2003 | Accepted | Pass |
| Enqueue call 2004 with severity 7 | Rejected | Pass |
| Peek after valid enqueue operations | Call 2001 returned; count remains 3 | Pass |
| Process all valid calls | IDs returned as 2001, 2002, 2003 | Pass |
| Observe counts while processing | Counts transition from 3 to 2 to 1 to 0 | Pass |
| Process after the queue is empty | Returns `false`; count remains 0 | Pass |

## Complexity

- Severity validation: O(1)
- Queue insertion: O(1) container operation
- Front inspection: O(1)
- Front removal: O(1) container operation
- Pending-call count: O(1)

Copying an `EmergencyCall` may additionally depend on the length of its string fields.

## Pointer lifetime

The pointer returned by `peekNextCall` is non-owning. It must not be dereferenced after the front
call is processed and removed, or after the queue itself is destroyed.

## Deferred work

- Detect duplicate call IDs with `std::unordered_set`.
- Replace manual checks in `main.cpp` with automated unit tests.
- Validate latitude and longitude ranges.
- Replace Boolean validation results with a type that reports the rejection reason.

## Concepts demonstrated

- FIFO behavior
- Queue adapters and restricted access
- `push`, `front`, `pop`, `empty`, and `size`
- Safe null-pointer checks
- Output parameters and copy-before-pop ordering
- Constant-time queue operations
