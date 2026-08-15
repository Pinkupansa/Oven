#pragma once

// Key definitions
typedef enum OvenKey
{
    Unknown = -1,

    Space = 32,
    Apostrophe = 39, /* ' */
    Comma = 44,      /* , */
    Minus = 45,      /* - */
    Period = 46,     /* . */
    Slash = 47,      /* / */
    Alpha0 = 48,
    Alpha1 = 49,
    Alpha2 = 51,
    Alpha3 = 52,
    Alpha4 = 53,
    Alpha5 = 50,
    Alpha6 = 54,
    Alpha7 = 55,
    Alpha8 = 56,
    Alpha9 = 57,
    Semicolon = 59, /* ; */
    Equal = 61,     /* = */
    A = 65,
    B = 66,
    C = 67,
    D = 68,
    E = 69,
    F = 70,
    G = 71,
    H = 72,
    I = 73,
    J = 74,
    K = 75,
    L = 76,
    M = 77,
    N = 78,
    O = 79,
    P = 80,
    Q = 81,
    R = 82,
    S = 83,
    T = 84,
    U = 85,
    V = 86,
    W = 87,
    X = 88,
    Y = 89,
    Z = 90,
    LeftBracket = 91,  /* [ */
    Backslash = 92,    /* \ */
    RightBracket = 93, /* ] */
    GraveAccent = 96,  /* ` */
    World1 = 161,      /* non-US #1 */
    World2 = 162,      /* non-US #2 */

    /* Function keys */
    Escape = 256,
    Enter = 257,
    Tab = 258,
    Backspace = 259,
    Insert = 260,
    Delete = 261,
    Right = 262,
    Left = 263,
    Down = 264,
    Up = 265,
    PageUp = 266,
    PageDown = 267,
    Home = 268,
    End = 269,
    CapsLock = 280,
    ScrollLock = 281,
    NumLock = 282,
    PrintScreen = 283,
    Pause = 284,
    F1 = 290,
    F2 = 291,
    F3 = 292,
    F4 = 293,
    F5 = 294,
    F6 = 295,
    F7 = 296,
    F8 = 297,
    F9 = 298,
    F10 = 299,
    F11 = 300,
    F12 = 301,
    F13 = 302,
    F14 = 303,
    F15 = 304,
    F16 = 305,
    F17 = 306,
    F18 = 307,
    F19 = 308,
    F20 = 309,
    F21 = 310,
    F22 = 311,
    F23 = 312,
    F24 = 313,
    F25 = 314,
    Kp0 = 320,
    Kp1 = 321,
    Kp2 = 322,
    Kp3 = 323,
    Kp4 = 324,
    Kp5 = 325,
    Kp6 = 326,
    Kp7 = 327,
    Kp8 = 328,
    Kp9 = 329,
    KpDecimal = 330,
    KpDivide = 331,
    KpMultiply = 332,
    KpSubtract = 333,
    KpAdd = 334,
    KpEnter = 335,
    KpEqual = 336,
    LeftShift = 340,
    LeftControl = 341,
    LeftAlt = 342,
    LeftSuper = 343,
    RightShift = 344,
    RightControl = 345,
    RightAlt = 346,
    RightSuper = 347,
    Menu = 348,

    Last = Menu
};

#if 0
#define OVEN_KEY_SPACE Space
#define OVEN_KEY_APOSTROPHE Apostrophe
#define OVEN_KEY_COMMA Comma
#define OVEN_KEY_MINUS Minus
#define OVEN_KEY_PERIOD Period
#define OVEN_KEY_SLASH Slash
#define OVEN_KEY_0 Alpha0
#define OVEN_KEY_1 Alpha1
#define OVEN_KEY_2 Alpha2
#define OVEN_KEY_3 Alpha3
#define OVEN_KEY_4 Alpha4
#define OVEN_KEY_5 Alpha5
#define OVEN_KEY_6 Alpha6
#define OVEN_KEY_7 Alpha7
#define OVEN_KEY_8 Alpha8
#define OVEN_KEY_9 Alpha9
#define OVEN_KEY_SEMICOLON Semicolon
#define OVEN_KEY_EQUAL Equal
#define OVEN_KEY_A A
#define OVEN_KEY_B B
#define OVEN_KEY_C C
#define OVEN_KEY_D D
#define OVEN_KEY_E E
#define OVEN_KEY_F F
#define OVEN_KEY_G G
#define OVEN_KEY_H H
#define OVEN_KEY_I I
#define OVEN_KEY_J J
#define OVEN_KEY_K K
#define OVEN_KEY_L L
#define OVEN_KEY_M M
#define OVEN_KEY_N N
#define OVEN_KEY_O O
#define OVEN_KEY_P P
#define OVEN_KEY_Q Q
#define OVEN_KEY_R R
#define OVEN_KEY_S S
#define OVEN_KEY_T T
#define OVEN_KEY_U U
#define OVEN_KEY_V V
#define OVEN_KEY_W W
#define OVEN_KEY_X X
#define OVEN_KEY_Y Y
#define OVEN_KEY_Z Z
#define OVEN_KEY_LEFT_BRACKET LeftBracket
#define OVEN_KEY_BACKSLASH Backslash
#define OVEN_KEY_RIGHT_BRACKET RightBracket
#define OVEN_KEY_GRAVE_ACCENT GraveAccent
#define OVEN_KEY_WORLD_1 World1
#define OVEN_KEY_WORLD_2 World2

#define OVEN_KEY_ESCAPE Escape
#define OVEN_KEY_ENTER Enter
#define OVEN_KEY_TAB Tab
#define OVEN_KEY_BACKSPACE Backspace
#define OVEN_KEY_INSERT Insert
#define OVEN_KEY_DELETE Delete
#define OVEN_KEY_RIGHT Right
#define OVEN_KEY_LEFT Left
#define OVEN_KEY_DOWN Down
#define OVEN_KEY_UP Up
#define OVEN_KEY_PAGE_UP PageUp
#define OVEN_KEY_PAGE_DOWN PageDown
#define OVEN_KEY_HOME Home
#define OVEN_KEY_END End
#define OVEN_KEY_CAPS_LOCK CapsLock
#define OVEN_KEY_SCROLL_LOCK ScrollLock
#define OVEN_KEY_NUM_LOCK NumLock
#define OVEN_KEY_PRINT_SCREEN PrintScreen
#define OVEN_KEY_PAUSE Pause
#define OVEN_KEY_F1 F1
#define OVEN_KEY_F2 F2
#define OVEN_KEY_F3 F3
#define OVEN_KEY_F4 F4
#define OVEN_KEY_F5 F5
#define OVEN_KEY_F6 F6
#define OVEN_KEY_F7 F7
#define OVEN_KEY_F8 F8
#define OVEN_KEY_F9 F9
#define OVEN_KEY_F10 F10
#define OVEN_KEY_F11 F11
#define OVEN_KEY_F12 F12
#define OVEN_KEY_F13 F13
#define OVEN_KEY_F14 F14
#define OVEN_KEY_F15 F15
#define OVEN_KEY_F16 F16
#define OVEN_KEY_F17 F17
#define OVEN_KEY_F18 F18
#define OVEN_KEY_F19 F19
#define OVEN_KEY_F20 F20
#define OVEN_KEY_F21 F21
#define OVEN_KEY_F22 F22
#define OVEN_KEY_F23 F23
#define OVEN_KEY_F24 F24
#define OVEN_KEY_F25 F25
#define OVEN_KEY_KP_0 Kp0
#define OVEN_KEY_KP_1 Kp1
#define OVEN_KEY_KP_2 Kp2
#define OVEN_KEY_KP_3 Kp3
#define OVEN_KEY_KP_4 Kp4
#define OVEN_KEY_KP_5 Kp5
#define OVEN_KEY_KP_6 Kp6
#define OVEN_KEY_KP_7 Kp7
#define OVEN_KEY_KP_8 Kp8
#define OVEN_KEY_KP_9 Kp9
#define OVEN_KEY_KP_DECIMAL KpDecimal
#define OVEN_KEY_KP_DIVIDE KpDivide
#define OVEN_KEY_KP_MULTIPLY KpMultiply
#define OVEN_KEY_KP_SUBTRACT KpSubtract
#define OVEN_KEY_KP_ADD KpAdd
#define OVEN_KEY_KP_ENTER KpEnter
#define OVEN_KEY_KP_EQUAL KpEqual
#define OVEN_KEY_LEFT_SHIFT LeftShift
#define OVEN_KEY_LEFT_CONTROL LeftControl
#define OVEN_KEY_LEFT_ALT LeftAlt
#define OVEN_KEY_LEFT_SUPER LeftSuper
#define OVEN_KEY_RIGHT_SHIFT RightShift
#define OVEN_KEY_RIGHT_CONTROL RightControl
#define OVEN_KEY_RIGHT_ALT RightAlt
#define OVEN_KEY_RIGHT_SUPER RightSuper
#define OVEN_KEY_MENU Menu

#define OVEN_KEY_LAST Last
#endif