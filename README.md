# ProcFreeze

**ProcFreeze** a minimal Windows proof-of-concept to suspend and resume a process by calling undocumented NT APIs from `ntdll.dll` (`NtSuspendProcess` / `NtResumeProcess`). Intended for debugging, malware analysis, and authorized red-team research. **Use responsibly.**

## About

`ProcFreeze` demonstrates a simple technique to freeze (suspend) and thaw (resume) an entire Windows process by dynamically resolving undocumented functions exported by `ntdll.dll`:

* `NtSuspendProcess` — suspends all threads in the target process
* `NtResumeProcess`  — resumes all threads in the target process

The project is intentionally small and auditable (single-source-file C implementation) to make it easy to study, adapt, and extend for lab use.

## Features

* Single-file C PoC (easy to inspect and compile).
* Dynamically resolves NT API functions from `ntdll.dll` at runtime.
* Attempts to enable `SeDebugPrivilege` to increase the chance of opening protected processes.
* Simple CLI: suspend a process, wait for user input, resume it.

## Requirements
* Administrator privileges for some targets; enabling `SeDebugPrivilege` increases capability but still requires appropriate rights.

## Usage

```
procfreeze.exe <pid>
```

Example:

```
procfreeze.exe 1234
# Output:
# Process suspended.
# Press Enter to resume...
```

Behavior:

1. Enables `SeDebugPrivilege` where possible.
2. Loads `NtSuspendProcess` and `NtResumeProcess` from `ntdll.dll`.
3. Opens the target process using `PROCESS_SUSPEND_RESUME` access.
4. Calls `NtSuspendProcess` to suspend the process and waits for user input.
5. On Enter, calls `NtResumeProcess` and exits.
