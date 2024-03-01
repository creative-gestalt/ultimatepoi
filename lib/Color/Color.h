#ifndef COLOR_H
#define COLOR_H

class Color {

public:
    Color();

    static constexpr unsigned long RED    = 0xFF0000;
    static constexpr unsigned long ORANGE = 0xFFA500;
    static constexpr unsigned long YELLOW = 0xFFFF00;
    static constexpr unsigned int  GREEN  = 0x00FF00;
    static constexpr unsigned int  BLUE   = 0x0000FF;
    static constexpr unsigned long VIOLET = 0x8A2BE2;
    static constexpr unsigned long CYAN   = 0x00FFFF;
};

#endif  // COLOR_H