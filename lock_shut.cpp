
#include <windows.h>
#include <shellapi.h>
#include <iostream>
#include <string>
using namespace std;
#define LOCK_CODE "9696"
#define SHUTDOWN_CODE "2580"
HHOOK keyboardHook;
string input = "";
bool IsDesktopActive()
{
    HWND hwnd = GetForegroundWindow();

    char className[256];
    GetClassNameA(hwnd, className, sizeof(className));

    return string(className) == "Progman" ||
           string(className) == "WorkerW";
}
void ExecuteCode()
{
    if (input == LOCK_CODE)
    {
        LockWorkStation();
    }
    else if (input == SHUTDOWN_CODE)
    {
        ShellExecuteA(
            NULL,
            "open",
            "shutdown.exe",
            "/s /t 0",
            NULL,
            SW_HIDE
        );
    }
    input = "";
}
LRESULT CALLBACK KeyboardProc(
    int nCode,
    WPARAM wParam,
    LPARAM lParam
)
{
    if (nCode == HC_ACTION && wParam == WM_KEYDOWN)
    {
        if (IsDesktopActive())
        {
            KBDLLHOOKSTRUCT* key =
                (KBDLLHOOKSTRUCT*)lParam;
            int digit = -1;
            if (key->vkCode >= '0' && key->vkCode <= '9')
            {
                digit = key->vkCode - '0';
            }
            else if (key->vkCode >= VK_NUMPAD0 &&
                     key->vkCode <= VK_NUMPAD9)
            {
                digit = key->vkCode - VK_NUMPAD0;
            }

            if (digit >= 0)
            {
                input += char('0' + digit);

                if (input.length() == 4)
                {
                    ExecuteCode();
                }
            }
            else
            {
                input = "";
            }
        }
        else
        {
            input = "";
        }
    }
    return CallNextHookEx(
        keyboardHook,
        nCode,
        wParam,
        lParam
    );
}
int main()
{
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    keyboardHook = SetWindowsHookExA(
        WH_KEYBOARD_LL,
        KeyboardProc,
        GetModuleHandle(NULL),
        0
    );
    if (!keyboardHook)
    {
        return 1;
    }
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnhookWindowsHookEx(keyboardHook);
    return 0;
}

