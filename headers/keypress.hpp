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
#endif

// Forward declarations
#ifdef PLATFORM_WINDOWS
int32_t getKeyboardPress_Windows();
int32_t VirtualKeyToStandard(int vkey, bool shift = false);
#endif

#ifdef PLATFORM_LINUX
int32_t getKeyboardPress_Linux();
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
            DWORD controlState = irBuffer.Event.KeyEvent.dwControlKeyState;
            bool shiftHeld = (controlState & SHIFT_PRESSED) != 0;
            keyCode = VirtualKeyToStandard(irBuffer.Event.KeyEvent.wVirtualKeyCode, shiftHeld);
            break;
        }
    }
    
    // Restore console mode
    SetConsoleMode(hStdin, mode);
    return keyCode;
}

int32_t VirtualKeyToStandard(int vkey, bool shift) {
    // Map Windows virtual keys to standard keycodes
    // Handle Shift+Arrow combinations first
    if (shift) {
        switch (vkey) {
            case VK_UP:     return 1010;  // Shift+Up
            case VK_DOWN:   return 1011;  // Shift+Down
            case VK_LEFT:   return 1012;  // Shift+Left
            case VK_RIGHT:  return 1013;  // Shift+Right
        }
    }
    
    // Regular keys
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

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

static struct termios old_tio, new_tio;
static int initialized = 0;

void initTerminal() {
    if (initialized) return;
    tcgetattr(STDIN_FILENO, &old_tio);
    new_tio = old_tio;
    new_tio.c_lflag &= (~ICANON & ~ECHO);
    new_tio.c_iflag &= ~(IXON | IXOFF);  // Disable flow control (Ctrl+S and Ctrl+Q)
    new_tio.c_cc[VMIN] = 1;    // Blocking reads - wait for at least 1 byte
    new_tio.c_cc[VTIME] = 0;   // No timeout between characters
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
    initialized = 1;
}

void resetTerminal() {
    if (!initialized) return;
    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
    initialized = 0;
}

int32_t getKeyboardPress_Linux() {
    initTerminal();
    
    unsigned char c;
    ssize_t nread = read(STDIN_FILENO, &c, 1);
    if (nread != 1) return -1;
    
    // Handle regular ASCII
    if (c != 27) { // Not escape character
        return (int32_t)c;
    }
    
    // Handle escape sequences
    unsigned char seq[8] = {0};
    seq[0] = c; // Store the ESC
    
    // Use select to see if more bytes are available within a timeout
    struct timeval tv;
    fd_set readfds;
    int seqIdx = 1;
    
    // Try to read up to 7 more bytes (for longer sequences)
    for (int i = 0; i < 7; i++) {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        tv.tv_sec = 0;
        tv.tv_usec = 100000; // 100ms timeout - longer to catch all bytes
        
        int ret = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);
        if (ret > 0 && FD_ISSET(STDIN_FILENO, &readfds)) {
            nread = read(STDIN_FILENO, &seq[seqIdx], 1);
            if (nread == 1) {
                seqIdx++;
                // Check if sequence is complete (ends with letter or ~)
                if ((seq[seqIdx-1] >= 'A' && seq[seqIdx-1] <= 'Z') ||
                    (seq[seqIdx-1] >= 'a' && seq[seqIdx-1] <= 'z') ||
                    seq[seqIdx-1] == '~') {
                    break; // Sequence is complete
                }
            } else {
                break;
            }
        } else {
            break; // Timeout - no more bytes available
        }
    }
    
    // Parse escape sequences
    if (seqIdx >= 2 && seq[1] == '[') {
        if (seqIdx == 3) {
            // Single character sequences: ^[[A, ^[[B, etc
            switch (seq[2]) {
                case 'A': return 1000; // Up
                case 'B': return 1001; // Down
                case 'C': return 1003; // Right
                case 'D': return 1002; // Left
                case 'H': return 1004; // Home
                case 'F': return 1005; // End
                default: break;
            }
        } else if (seqIdx >= 4 && seq[2] == '1' && seq[3] == ';' && seq[4] == '2') {

            char lastChar = seq[seqIdx-1];
            switch (lastChar) {
                case 'A': return 1010; // Shift+Up
                case 'B': return 1011; // Shift+Down
                case 'C': return 1013; // Shift+Right
                case 'D': return 1012; // Shift+Left
                default: break;
            }
        } else if (seqIdx >= 4 && seq[seqIdx-1] == '~') {
            // Extended sequences: ^[[5~, ^[[6~, etc
            switch (seq[2]) {
                case '3': return 1006; // Delete
                case '5': return 1008; // Page Up
                case '6': return 1009; // Page Down
                default: break;
            }
        }
    }
    
    // If we read sequence bytes but didn't recognize it, consume and retry
    if (seqIdx > 1) {
        return getKeyboardPress_Linux();
    }
    
    // Pure escape key
    return 27;
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

// ============================================================================
// Key Constants
// ============================================================================
#define KEY_SHIFT_UP    1010
#define KEY_SHIFT_DOWN  1011
#define KEY_SHIFT_LEFT  1012
#define KEY_SHIFT_RIGHT 1013

#endif