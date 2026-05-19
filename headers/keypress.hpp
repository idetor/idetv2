#ifndef KEYPRESS_HPP
#define KEYPRESS_HPP
#include <cstdint>

// Platform detection
#if defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS
    #include <Windows.h>
#elif defined(__APPLE__)
    #define PLATFORM_MACOS
    #include <Carbon/Carbon.h>
#elif defined(__linux__)
    #define PLATFORM_LINUX
    #include <X11/Xlib.h>
    #include <X11/keysym.h>
#endif

// Forward declarations
#ifdef PLATFORM_WINDOWS
int32_t getKeyboardPress_Windows();
int32_t VirtualKeyToStandard(int vkey);
#endif

#ifdef PLATFORM_LINUX
int32_t getKeyboardPress_Linux();
int32_t KeySymToStandard(KeySym keysym);
#endif

#ifdef PLATFORM_MACOS
int32_t getKeyboardPress_MacOS();
#endif

/**
 * Gets the next keyboard press as a platform-independent keycode.
 * Blocks until a key is pressed.
 * Returns a standardized keycode (see KEY_* constants below).
 */
int32_t getKeyboardPress() {
#ifdef PLATFORM_WINDOWS
    return getKeyboardPress_Windows();
#elif defined(PLATFORM_LINUX)
    return getKeyboardPress_Linux();
#elif defined(PLATFORM_MACOS)
    return getKeyboardPress_MacOS();
#else
    return -1; // Unsupported platform
#endif
}

// ============================================================================
// Windows Implementation
// ============================================================================
#ifdef PLATFORM_WINDOWS

int32_t getKeyboardPress_Windows() {
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hStdin, &mode);
    
    // Disable line buffering and echo
    SetConsoleMode(hStdin, mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
    
    INPUT_RECORD irBuffer;
    DWORD dwRead;
    int32_t keyCode = -1;
    
    while (true) {
        ReadConsoleInput(hStdin, &irBuffer, 1, &dwRead);
        
        if (irBuffer.EventType == KEY_EVENT && irBuffer.Event.KeyEvent.bKeyDown) {
            keyCode = VirtualKeyToStandard(irBuffer.Event.KeyEvent.wVirtualKeyCode);
            break;
        }
    }
    
    // Restore console mode
    SetConsoleMode(hStdin, mode);
    return keyCode;
}

int32_t VirtualKeyToStandard(int vkey) {
    // Map Windows virtual keys to standard keycodes
    switch (vkey) {
        case VK_ESCAPE:     return 27;
        case VK_RETURN:     return 13;
        case VK_TAB:        return 9;
        case VK_BACK:       return 8;
        case VK_SPACE:      return 32;
        case VK_UP:         return 1000;
        case VK_DOWN:       return 1001;
        case VK_LEFT:       return 1002;
        case VK_RIGHT:      return 1003;
        case VK_HOME:       return 1004;
        case VK_END:        return 1005;
        case VK_DELETE:     return 1006;
        case VK_INSERT:     return 1007;
        case VK_PRIOR:      return 1008; // Page Up
        case VK_NEXT:       return 1009; // Page Down
        case VK_F1:         return 2000;
        case VK_F2:         return 2001;
        case VK_F3:         return 2002;
        case VK_F4:         return 2003;
        case VK_F5:         return 2004;
        case VK_F6:         return 2005;
        case VK_F7:         return 2006;
        case VK_F8:         return 2007;
        case VK_F9:         return 2008;
        case VK_F10:        return 2009;
        case VK_F11:        return 2010;
        case VK_F12:        return 2011;
        case VK_CONTROL:    return 3000;
        case VK_SHIFT:      return 3001;
        case VK_MENU:       return 3002; // Alt
        case VK_LWIN:       return 3003;
        case VK_RWIN:       return 3004;
        default:
            // For printable ASCII characters
            if (vkey >= 0x30 && vkey <= 0x39) return vkey; // 0-9
            if (vkey >= 0x41 && vkey <= 0x5A) return vkey; // A-Z
            return vkey;
    }
}

#endif // PLATFORM_WINDOWS

// ============================================================================
// Linux Implementation
// ============================================================================
#ifdef PLATFORM_LINUX

int32_t getKeyboardPress_Linux() {
    Display *display = XOpenDisplay(nullptr);
    if (!display) return -1;
    
    Window root = DefaultRootWindow(display);
    XSelectInput(display, root, KeyPressMask);
    
    XEvent event;
    int32_t keyCode = -1;
    
    while (true) {
        XNextEvent(display, &event);
        
        if (event.type == KeyPress) {
            KeySym keysym = XLookupKeysym(&event.xkey, 0);
            keyCode = KeySymToStandard(keysym);
            break;
        }
    }
    
    XCloseDisplay(display);
    return keyCode;
}

int32_t KeySymToStandard(KeySym keysym) {
    // Map X11 KeySyms to standard keycodes
    switch (keysym) {
        case XK_Escape:     return 27;
        case XK_Return:     return 13;
        case XK_Tab:        return 9;
        case XK_BackSpace:  return 8;
        case XK_space:      return 32;
        case XK_Up:         return 1000;
        case XK_Down:       return 1001;
        case XK_Left:       return 1002;
        case XK_Right:      return 1003;
        case XK_Home:       return 1004;
        case XK_End:        return 1005;
        case XK_Delete:     return 1006;
        case XK_Insert:     return 1007;
        case XK_Page_Up:    return 1008;
        case XK_Page_Down:  return 1009;
        case XK_F1:         return 2000;
        case XK_F2:         return 2001;
        case XK_F3:         return 2002;
        case XK_F4:         return 2003;
        case XK_F5:         return 2004;
        case XK_F6:         return 2005;
        case XK_F7:         return 2006;
        case XK_F8:         return 2007;
        case XK_F9:         return 2008;
        case XK_F10:        return 2009;
        case XK_F11:        return 2010;
        case XK_F12:        return 2011;
        case XK_Control_L:  return 3000;
        case XK_Control_R:  return 3000;
        case XK_Shift_L:    return 3001;
        case XK_Shift_R:    return 3001;
        case XK_Alt_L:      return 3002;
        case XK_Alt_R:      return 3002;
        case XK_Super_L:    return 3003;
        case XK_Super_R:    return 3004;
        default:
            // For ASCII characters
            if (keysym >= 32 && keysym <= 126) return static_cast<int32_t>(keysym);
            return static_cast<int32_t>(keysym);
    }
}

#endif // PLATFORM_LINUX

// ============================================================================
// macOS Implementation
// ============================================================================
#ifdef PLATFORM_MACOS

int32_t getKeyboardPress_MacOS() {
    // For macOS console input, we need to use termios
    #include <termios.h>
    #include <unistd.h>
    
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    int32_t ch = getchar();
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

#endif // PLATFORM_MACOS


#endif