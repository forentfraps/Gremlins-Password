
#include "resource.h"
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

HINSTANCE g_hInstance; // Global or static variable to hold the instance handle
//
BOOL CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam,
                         LPARAM lParam) {
  switch (uMsg) {
  case WM_INITDIALOG:
    RECT rect;
    GetWindowRect(GetParent(hwndDlg), &rect);
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int x = (screenWidth - (rect.right - rect.left)) / 2 - 100;
    int y = (screenHeight - (rect.bottom - rect.top)) / 2 - 50;

    SetWindowPos(hwndDlg, HWND_TOPMOST, x, y, 0, 0, SWP_NOSIZE);
    return (INT_PTR)TRUE;
  case WM_COMMAND:
    switch (LOWORD(wParam)) {
    case ID_OK: {
      char buffer[256];
      GetDlgItemText(hwndDlg, ID_EDIT, buffer, sizeof(buffer));
      EndDialog(hwndDlg, strtoul(buffer, NULL, 10));
      return TRUE;
    }
    case ID_CANCEL:
      EndDialog(hwndDlg, 0);
      return TRUE;
    }
    break;
  }
  return FALSE;
}

__declspec(dllexport) unsigned long long ShowInputDialog() {
  return (unsigned long long)DialogBoxParam(g_hInstance,
                                            MAKEINTRESOURCE(ID_INPUTDIALOG),
                                            NULL, (DLGPROC)DialogProc, 0);
}

__declspec(dllexport) void ELog(char *data) {
  DWORD written;
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  WriteConsole(hConsole, data, lstrlenA(data), &written, NULL);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call,
                      LPVOID lpReserved) {
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH:
    g_hInstance = hModule;
    // AllocConsole();

    ELog("DEBUG CONSOLE\n");

    break;
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
  case DLL_PROCESS_DETACH:
    break;
  }
  return TRUE;
}
