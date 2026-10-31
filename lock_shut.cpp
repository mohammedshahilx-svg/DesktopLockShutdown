#include <windows.h>
#include <shellapi.h>
#include <string>

#define LOCK_CODE "9696"
#define SHUTDOWN_CODE "2580"

HHOOK keyboardHook = NULL;
std::string input = "";

// Helper to determine if a window is an actual open user application
bool IsUserApplicationWindow(HWND hwnd)
{
    if (!IsWindowVisible(hwnd))
        return false;

    // Ignore iconic/minimized windows (they are minimized to taskbar, user is looking at desktop)
    if (IsIconic(hwnd))
        return false;

    // Check title length
    int len = GetWindowTextLengthA(hwnd);
    if (len == 0)
        return false;

    // Filter out common system/shell background overlays
    char className[256] = {0};
    GetClassNameA(hwnd, className, sizeof(className));
    std::string cls(className);

    if (cls == "Progman" || cls == "WorkerW" || cls == "Shell_TrayWnd" ||
        cls == "Shell_SecondaryTrayWnd" || cls == "Windows.UI.Core.CoreWindow" ||
        cls == "ApplicationFrameWindow")
    {
        // For ApplicationFrameWindow, verify if it actually has visible child windows
        if (cls == "ApplicationFrameWindow")
        {
            char title[256] = {0};
            GetWindowTextA(hwnd, title, sizeof(title));
            if (std::string(title).empty() || std::string(title) == "Settings")
                return false;
        }
        else
        {
            return false;
        }
    }

    // Must have standard window style or tool window check
    LONG exStyle = GetWindowLongA(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW)
        return false;

    // Verify window has a visible non-zero area
    RECT rect;
    if (GetWindowRect(hwnd, &rect))
    {
        if ((rect.right - rect.left) <= 0 || (rect.bottom - rect.top) <= 0)
            return false;
    }

    return true;
}

struct EnumParam {
    bool hasApp;
};

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    EnumParam* param = (EnumParam*)lParam;
    if (IsUserApplicationWindow(hwnd))
    {
        param->hasApp = true;
        return FALSE; // Stop enumerating, an active user app is open on screen
    }
    return TRUE;
}

// Returns TRUE if Desktop is focused, Taskbar is focused, or NO app window is open
bool IsDesktopActive()
{
    HWND fg = GetForegroundWindow();
    if (!fg)
    {
        // No window has focus (common right after minimizing all apps with Win+D)
        return true;
    }

    char className[256] = {0};
    GetClassNameA(fg, className, sizeof(className));
    std::string cls(className);

    // 1. Desktop background / wallpaper directly focused
    if (cls == "Progman" || cls == "WorkerW")
        return true;

    // 2. Taskbar / Start button / System tray focused
    if (cls == "Shell_TrayWnd" || cls == "Shell_SecondaryTrayWnd")
        return true;

    // 3. Desktop icon list / shell elements
    HWND parent = GetParent(fg);
    if (parent)
    {
        char pClass[256] = {0};
        GetClassNameA(parent, pClass, sizeof(pClass));
        std::string pCls(pClass);
        if (pCls == "Progman" || pCls == "WorkerW")
            return true;
    }

    // 4. Check if all user programs are minimized / closed
    EnumParam param = { false };
    EnumWindows(EnumWindowsProc, (LPARAM)&param);

    // If no active program window is currently displayed on screen, the user is looking at the desktop!
    if (!param.hasApp)
    {
        return true;
    }

    return false;
}

void ExecuteCode()
{
    if (input == LOCK_CODE)
    {
        LockWorkStation();
    }
    else if (input == SHUTDOWN_CODE)
    {
        ShellExecuteA(NULL, "open", "shutdown.exe", "/s /t 0", NULL, SW_HIDE);
    }
    input = "";
}

// Safely convert key press to digit (0-9) supporting top row and Numpad with or without NumLock
int GetDigitFromKey(DWORD vkCode)
{
    // 1. Top number row ('0' to '9')
    if (vkCode >= '0' && vkCode <= '9')
    {
        return (int)(vkCode - '0');
    }

    // 2. Standard Numeric Keypad with NumLock ON (VK_NUMPAD0 to VK_NUMPAD9)
    if (vkCode >= VK_NUMPAD0 && vkCode <= VK_NUMPAD9)
    {
        return (int)(vkCode - VK_NUMPAD0);
    }

    // 3. Numeric Keypad with NumLock OFF (Windows maps them to navigation virtual keys)
    switch (vkCode)
    {
        case VK_INSERT: return 0; // Numpad 0
        case VK_END:    return 1; // Numpad 1
        case VK_DOWN:   return 2; // Numpad 2
        case VK_NEXT:   return 3; // Numpad 3 (PageDown)
        case VK_LEFT:   return 4; // Numpad 4
        case VK_CLEAR:  return 5; // Numpad 5
        case VK_RIGHT:  return 6; // Numpad 6
        case VK_HOME:   return 7; // Numpad 7
        case VK_UP:     return 8; // Numpad 8
        case VK_PRIOR:  return 9; // Numpad 9 (PageUp)
        default:        break;
    }

    return -1;
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION && wParam == WM_KEYDOWN)
    {
        if (IsDesktopActive())
        {
            KBDLLHOOKSTRUCT* key = (KBDLLHOOKSTRUCT*)lParam;
            int digit = GetDigitFromKey(key->vkCode);

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
    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    keyboardHook = SetWindowsHookExA(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
    if (!keyboardHook) return 1;

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(keyboardHook);
    return 0;
}
