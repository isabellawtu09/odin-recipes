# Expression Calculator with Deadlock Detection

## Overview
This is a multi-threaded expression calculator that can detect when no progress can be made due to improper input sequences.

## Implementation Details

### Threads
The program uses four threads:

1. **Adder**: Finds and evaluates addition operations (e.g., `2+3` → `5`)
2. **Multiplier**: Finds and evaluates multiplication operations (e.g., `2*3` → `6`)
3. **Degrouper**: Removes unnecessary parentheses (e.g., `(5)` → `5`)
4. **Sentinel**: Monitors progress and detects deadlocks

### Deadlock Detection (Step 6)

The sentinel thread detects deadlocks by monitoring whether the buffer is modified after all three worker threads (adder, multiplier, degrouper) have completed a cycle.

**Shared data structures:**
- `progress_t` structure tracks:
  - `adder_done`: Flag indicating adder has completed
  - `multiplier_done`: Flag indicating multiplier has completed
  - `degrouper_done`: Flag indicating degrouper has completed
  - `buffer_changed`: Flag indicating if buffer was modified
- `progress_mutex`: Protects access to the progress structure

**Detection logic:**
1. Each worker thread marks itself as done after completing its pass
2. When a worker thread modifies the buffer, it sets `buffer_changed = 1`
3. The sentinel checks if all three workers are done
4. If all are done and `buffer_changed == 0`, no progress was made
5. If the buffer is not a single number and no progress was made, the sentinel outputs "No progress can be made\n" and exits with `EXIT_FAILURE`
6. If progress was made, the sentinel resets all flags for the next cycle

## Building and Running

### Compile
```bash
make
```

### Run
```bash
./calc "expression"
```

### Examples

**Valid inputs:**
```bash
./calc "2+3"        # Outputs: 5
./calc "2*3+4"      # Outputs: 10
./calc "(2+3)*4"    # Outputs: 20
./calc "((5))"      # Outputs: 5
```

**Invalid inputs (deadlock detected):**
```bash
./calc "5++3"       # Outputs: No progress can be made
./calc "5**3"       # Outputs: No progress can be made
./calc "+5"         # Outputs: No progress can be made
./calc "5+"         # Outputs: No progress can be made
```

## Testing
```bash
make test
```

## Key Features
- Thread-safe buffer access using mutexes
- No false positives in deadlock detection
- Properly handles valid expressions with correct operator precedence
- Detects improper input sequences (consecutive operators, leading/trailing operators, etc.)
- Exits immediately upon deadlock detection with proper error message
