#ifndef HW1_BONUSLIB_H
#define HW1_BONUSLIB_H

#include <windows.h>

#define DEFAULT_SLEEP_TIME 1000 //milliseconds

typedef enum _eColor
{
    COLOR_DEFAULT_WHITE = 7,
    COLOR_GREEN = 10,
    COLOR_RED = 12,
    COLOR_YELLOW = 14,
}eColor;

void setTextColor(eColor color);

void gotoxy(int x, int y);

#endif //HW1_BONUSLIB_H
