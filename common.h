#ifndef _COMMON_H_
#define _COMMON_H_

#include <unistd.h>
#include <curses.h>

#define msleep(a) usleep((a)*1000)
#define ARRAY_LEN(a) (sizeof(a) / sizeof(*a))

WINDOW* DEBUG_WINDOW;

#define DEBUG_PRINTF(y, x, s, ...) { attrset(A_NORMAL); mvwprintw(DEBUG_WINDOW, y, x, s, __VA_ARGS__); wrefresh(DEBUG_WINDOW);}
#define DEBUG_PRINT(y, x, s) { attrset(A_NORMAL); mvwaddstr(DEBUG_WINDOW, y, x, s); wrefresh(DEBUG_WINDOW);}

#endif /*  _COMMON_H_  */