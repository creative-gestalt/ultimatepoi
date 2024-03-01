#ifndef REMOTE_H
#define REMOTE_H

// Adafruit IR Remote Codes:
//   Button       Code         Button  Code
//   -----------  ------       ------  -----
//   VOL-:        0x0       0/10+:  0xC
//   Play/Pause:  0x1       1:      0x10
//   VOL+:        0x2       2:      0x11
//   SETUP:       0x4       3:      0x12
//   STOP/MODE:   0x6       4:      0x14
//   UP:          0x5       5:      0x15
//   DOWN:        0xD       6:      0x16
//   LEFT:        0x8       7:      0x18
//   RIGHT:       0xA       8:      0x19
//   ENTER/SAVE:  0x9       9:      0x1A
//   Back:        0xE

class Remote {

public:
    Remote();

    static constexpr unsigned int BTN_BRIGHT_UP = 0x2;
    static constexpr unsigned int BTN_BRIGHT_DOWN = 0x0;
    static constexpr unsigned int BTN_RESTART = 0x1;
    static constexpr unsigned int BTN_BATTERY = 0x4;
    static constexpr unsigned int BTN_FASTER = 0x5;
    static constexpr unsigned int BTN_SLOWER = 0xD;
    static constexpr unsigned int BTN_OFF = 0x6;
    static constexpr unsigned int BTN_PATTERN_PREV = 0x8;
    static constexpr unsigned int BTN_PATTERN_NEXT = 0xA;
    static constexpr unsigned int BTN_NONE = 0xFFFF;
    static constexpr unsigned int BTN_AUTOPLAY = 0x9;
    static constexpr unsigned int BTN_MODE = 0xC;
    static constexpr unsigned int BTN_ONE = 0x10;
    static constexpr unsigned int BTN_TWO = 0x11;
    static constexpr unsigned int BTN_THREE = 0x12;
    static constexpr unsigned int BTN_FOUR = 0x14;
    static constexpr unsigned int BTN_FIVE = 0x15;
};

#endif  // REMOTE_H