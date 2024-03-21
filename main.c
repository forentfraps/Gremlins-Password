#include <Windows.h>
#include <stdio.h>
#include <winternl.h>

#include <tlhelp32.h>

#define TARGET_EXE_PATH                                                        \
  "C:\\Program Files "                                                         \
  "(x86)\\Steam\\steamapps\\common\\Gremlins\\Gremlins_Inc.exe"
#define DLL_PATH "./ac.dll"
DWORD FindProcessId(const char *processName) {
  DWORD processId = 0;
  HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hSnapshot != INVALID_HANDLE_VALUE) {
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hSnapshot, &pe)) {
      do {
        if (strcmp(pe.szExeFile, processName) == 0) {
          processId = pe.th32ProcessID;
          break;
        }
      } while (Process32Next(hSnapshot, &pe));
    }
    CloseHandle(hSnapshot);
  }
  return processId;
}
BOOL InjectDLL(DWORD processId, const char *dllPath) {
  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
  if (hProcess == NULL) {
    printf("Failed to open target process.\n");
    return FALSE;
  }

  // Allocate memory in the target process for the DLL path
  LPVOID pDllPath = VirtualAllocEx(hProcess, NULL, strlen(dllPath) + 1,
                                   MEM_COMMIT, PAGE_READWRITE);

  // Write the DLL path to the allocated memory
  WriteProcessMemory(hProcess, pDllPath, (LPVOID)dllPath, strlen(dllPath) + 1,
                     NULL);

  // Get the address of LoadLibrary in our own process
  LPVOID pLoadLibrary =
      (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");

  // Create a remote thread to run LoadLibrary in the target process
  HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0,
                                      (LPTHREAD_START_ROUTINE)pLoadLibrary,
                                      pDllPath, 0, NULL);
  if (hThread == NULL) {
    printf("Failed to create remote thread.\n");
    VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
    CloseHandle(hProcess);
    return FALSE;
  }

  // Wait for the remote thread to finish
  WaitForSingleObject(hThread, INFINITE);

  // Clean up
  VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
  CloseHandle(hThread);
  CloseHandle(hProcess);

  return TRUE;
}
BOOL SuspendResumeProcess(DWORD processID, BOOL suspend) {
  HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
  if (hThreadSnapshot == INVALID_HANDLE_VALUE)
    return FALSE;

  THREADENTRY32 te;
  te.dwSize = sizeof(THREADENTRY32);

  BOOL result = FALSE;

  if (Thread32First(hThreadSnapshot, &te)) {
    do {
      if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) +
                           sizeof(te.th32OwnerProcessID)) {
        if (te.th32OwnerProcessID == processID) {
          HANDLE hThread =
              OpenThread(THREAD_SUSPEND_RESUME, FALSE, te.th32ThreadID);
          if (hThread != NULL) {
            if (suspend) {
              SuspendThread(hThread);
            } else {
              ResumeThread(hThread);
            }
            CloseHandle(hThread);
            result = TRUE;
          }
        }
      }
      te.dwSize = sizeof(THREADENTRY32);
    } while (Thread32Next(hThreadSnapshot, &te));
  }

  CloseHandle(hThreadSnapshot);
  return result;
}
int main() {
  while (1) {
    DWORD pid = 0;
    printf("Searching for Gremlins!\n");
    while ((pid = FindProcessId("Gremlins_Inc.exe")) == 0) {
      Sleep(100);
    }
    printf("Found\n");
    printf("Injected %d\n", InjectDLL(pid, DLL_PATH));
    while (pid == FindProcessId("Gremlins_Inc.exe")) {
      Sleep(100);
    }
  }
}
