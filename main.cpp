#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include "res.h"

#include <windows.h>
#include <tchar.h>
#include <cstdio>

HICON icon;
NOTIFYICONDATA nid;
LRESULT KbHookProc(int nCode, WPARAM wParam, LPARAM lParam);
void handleIME();
void createNotifyIcon(HWND hwnd);
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
  icon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAIN));
  HHOOK hk = SetWindowsHookEx(WH_KEYBOARD_LL, KbHookProc, 0, 0);

  HWND hWnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG), NULL, DlgProc);
  ShowWindow(hWnd, SW_HIDE);

  createNotifyIcon(hWnd);

  MSG msg = {};
  while (GetMessage(&msg, NULL, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  UnhookWindowsHookEx(hk);

  return 0;
}

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
  case WM_USER:
    if (lParam == WM_LBUTTONDOWN)
    {
      DestroyWindow(hDlg);
      PostQuitMessage(0);
    }
    break;
  case WM_CLOSE:
    break;
  case WM_INITDIALOG:
    return (INT_PTR)TRUE;
  }
  return false;
}

void createNotifyIcon(HWND hwnd)
{
  nid.cbSize = sizeof nid;
  nid.hWnd = hwnd;
  nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
  nid.hIcon = icon;
  nid.uCallbackMessage = WM_USER;
  _tcscpy(nid.szInfo, L"RAlt key to switch keyboard layout");
  Shell_NotifyIcon(NIM_ADD, &nid);
}

LRESULT KbHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  if (nCode == HC_ACTION)
  {
    if (wParam == WM_SYSKEYDOWN || wParam == WM_KEYDOWN)
    {
      KBDLLHOOKSTRUCT kbdStruct = *((KBDLLHOOKSTRUCT *)lParam);
      if (kbdStruct.vkCode == VK_RMENU)
      {
        handleIME();
        return 1;
      }
    }
  }
  return CallNextHookEx(0, nCode, wParam, lParam);
}

HKL next(HKL current)
{
  HKL list[256];
  DWORD number;
  int i = 0;
  number = GetKeyboardLayoutList(256, list);
  for (; i < number; i++)
  {
    if (list[i] == current)
      break;
  }
  return list[(i + 1) % number];
}

void handleIME()
{
  INPUT ip_win = {}, ip_space = {};
  ip_win.type = INPUT_KEYBOARD;
  ip_space.type = INPUT_KEYBOARD;
  ip_win.ki.wVk = VK_LWIN;
  ip_space.ki.wVk = VK_SPACE;
  SendInput(1, &ip_win, sizeof ip_win);
  SendInput(1, &ip_space, sizeof ip_space);
  ip_win.ki.dwFlags = KEYEVENTF_KEYUP;
  ip_space.ki.dwFlags = KEYEVENTF_KEYUP;
  SendInput(1, &ip_space, sizeof ip_space);
  SendInput(1, &ip_win, sizeof ip_win);
}