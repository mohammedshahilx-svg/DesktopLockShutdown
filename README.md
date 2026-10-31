# Desktop Quick-Lock & Instant Shutdown

A lightweight, background Windows utility that allows you to instantly **Lock** or **Shut Down** your computer by typing a PIN whenever your desktop is in view—**no extra clicking required**.

- **Zero UI / 100% Silent**: Runs completely invisible in the background.
- **Ultra-lightweight**: Written in native Win32 C++, consumes ~2 MB RAM and 0% CPU.
- **Smart Desktop Detection**: Triggers whenever you are on the Desktop, when apps are minimized, or when the taskbar is active.
- **Accidental-press prevention**: Keystrokes are strictly ignored whenever you are actively typing inside an open application window (browser, editor, game, office, etc.).

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
- Registers a Windows Scheduled Task (`LockShutKey`) to start with highest privileges automatically on Windows startup.
- Starts the background process immediately so you can use it right away.

---

## 🎯 How to Use

Simply be on your Desktop (e.g. after starting Windows, closing your apps, or pressing `Win + D`):
- Type **`9696`** &rarr; Your screen locks immediately!
- Type **`2580`** &rarr; Your PC initiates shutdown!

*(No need to explicitly click the wallpaper first!)*

---

## 🔑 Changing Your PINs (Interactive 1-Click)

Want to use your own PINs instead of `9696` and `2580`?

1. Double-click **`change_codes.bat`**.
2. Type your new Lock PIN and press `Enter`.
3. Type your new Shutdown PIN and press `Enter`.

The script will automatically update `lock_shut.cpp`, recompile it, and restart the background service with your new PINs immediately—**no system reboot required!**

---

## 🗑️ How to Uninstall

Double-click **`uninstall.bat`**. It will stop the process and cleanly remove the scheduled task from Windows.
