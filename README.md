# Desktop Quick-Lock & Shutdown Hotkeys

A lightweight, background Windows utility that allows you to instantly **Lock** or **Shut Down** your computer by typing a PIN whenever your desktop background is in focus.

- **Zero UI / Silent**: Runs completely invisible in the background.
- **Ultra-lightweight**: Written in native Win32 C++, consumes ~2 MB RAM and 0% CPU.
- **Accidental-press prevention**: Keystrokes are strictly evaluated **only when your Desktop wallpaper is clicked/focused**. If you are typing inside Chrome, Word, games, or any app, the keys are completely ignored.

---

## Default PINs (Customizable)

| Action | Desktop PIN |
|---|---|
| **Lock Workstation** | `9696` |
| **Instant Shutdown** | `2580` |

---

## 🚀 Quick Setup (1-Click)

1. Download or clone this repository.
2. Double-click **`install.bat`** *(prompts for Administrator permission)*.

That's it! 
- The script automatically compiles `lock_shut.cpp` (if `g++` is installed) or uses the precompiled `lock_shut.exe`.
- It registers a Windows Scheduled Task (`LockShutKey`) to start with highest privileges automatically on Windows startup.
- It starts the background process immediately so you can use it right away.

---

## 🎯 How to Use

1. Click any empty spot on your **Desktop wallpaper**.
2. Type **`9696`** &rarr; Your screen locks immediately.
3. Type **`2580`** &rarr; Your PC initiates shutdown.

---

## 🔑 Changing Your PINs (Interactive 1-Click)

Want to use your own PINs instead of `9696` and `2580`?

1. Double-click **`change_codes.bat`**.
2. Type your new Lock PIN and press `Enter`.
3. Type your new Shutdown PIN and press `Enter`.

The script will automatically update `lock_shut.cpp`, recompile it, and restart the background service with your new PINs immediately—no manual editing required!

---

## 🗑️ How to Uninstall

Double-click **`uninstall.bat`**. It will stop the process and cleanly remove the scheduled task from Windows.

---

## 🛠️ Manual Compilation (Optional)

If you have MinGW / GCC:
```cmd
g++ -O2 lock_shut.cpp -o lock_shut.exe -mwindows
```
*(The `-mwindows` flag ensures no console window pops up).*
