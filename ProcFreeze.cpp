#include <windows.h>
#include <stdio.h>

typedef long NTSTATUS;
typedef NTSTATUS(NTAPI* pNtProc)(HANDLE);

pNtProc NtSuspendProcess, NtResumeProcess;

BOOL EnableDebugPriv() {
    HANDLE hToken; TOKEN_PRIVILEGES tp;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) return FALSE;
    if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid)) { CloseHandle(hToken); return FALSE; }
    tp.PrivilegeCount = 1; tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    BOOL res = AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
    CloseHandle(hToken);
    return res;
}

int main(int argc, char* argv[]) {
    if (argc != 2) { printf("Usage: %s <pid>\n", argv[0]); return 1; }
    DWORD pid = atoi(argv[1]); if (!pid) { printf("Invalid PID\n"); return 1; }
    if (!EnableDebugPriv()) { printf("Failed to enable debug privilege\n"); return 1; }
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    NtSuspendProcess = (pNtProc)GetProcAddress(ntdll, "NtSuspendProcess");
    NtResumeProcess = (pNtProc)GetProcAddress(ntdll, "NtResumeProcess");
    if (!NtSuspendProcess || !NtResumeProcess) { printf("Failed to load functions\n"); return 1; }
    HANDLE hProc = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, pid);
    if (!hProc) { printf("Failed to open process %d. Error: %u\n", pid, GetLastError()); return 1; }
    if (NtSuspendProcess(hProc) < 0) { printf("Suspend failed\n"); CloseHandle(hProc); return 1; }
    printf("Process suspended.\nPress Enter to resume...");
    getchar();
    if (NtResumeProcess(hProc) < 0) printf("Resume failed\n");
    else printf("Process resumed.\n");
    CloseHandle(hProc);
    return 0;
}
