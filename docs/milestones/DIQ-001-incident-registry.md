# DIQ-001: Incident Registry MVP

**Status:** Implemented; quality follow-up remains  
**Primary containers:** `std::pair`, `std::vector`

## Business requirement

Dispatchers need an in-memory registry that accepts valid incidents, preserves arrival order,
rejects duplicate IDs, and retrieves an incident by ID before dispatch automation is introduced.

## Delivered behavior

- Defines incidents with an ID, category, severity, and geographic coordinates.
- Stores accepted incidents in insertion order.
- Accepts severity values from 1 through 5.
- Rejects duplicate IDs and invalid severity values without modifying the registry.
- Finds incidents with a linear search and safely reports missing IDs.
- Displays all accepted incidents and handles an empty registry.

## Files

| File | Responsibility |
|---|---|
| `include/dispatchiq/incident.hpp` | Incident model and operation declarations |
| `src/incident.cpp` | Validation, insertion, lookup, and printing |
| `src/main.cpp` | Manual acceptance scenario |
| `CMakeLists.txt` | C++20 executable configuration |

## Acceptance results

| Scenario | Expected result | Result |
|---|---|---|
| Add incident 1001 with severity 5 | Accepted | Pass |
| Add incident 1002 with severity 4 | Accepted | Pass |
| Add another incident with ID 1001 | Rejected | Pass |
| Add incident 1003 with severity 8 | Rejected | Pass |
| Print registry | Only 1001 and 1002 shown, in order | Pass |
| Find ID 1002 | Matching incident returned | Pass |
| Find ID 9999 | `nullptr` returned and handled safely | Pass |

## Verified output

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

## Complexity baseline

- Duplicate-aware insertion: O(n)
- ID lookup: best O(1), worst O(n)
- Full registry display: O(n)
- End insertion after validation: amortized O(1)

This baseline will later be compared with an `std::unordered_map` index.

## Quality follow-up

- Remove the four current `-Wunused-but-set-variable` warnings in the demonstration code.
- Add automated tests for boundary severities, duplicate insertion, empty lookup, and insertion order.
- Add coordinate-range validation.
- Consider replacing the Boolean add result with an enum that identifies the rejection reason.

## Concepts demonstrated

- Aggregate initialization
- Const references and read-only traversal
- Range-based loops
- Linear search
- Pointer null checks and non-owning pointer lifetime
- Vector insertion order and reallocation risk
- Separation of declarations, definitions, and program orchestration
