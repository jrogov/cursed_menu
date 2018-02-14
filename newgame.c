#include <locale.h>
#include <stdlib.h>

#include "menu.h"
#include "common.h"

//-------------------------------SAMPLE MENU--------------------------------


/*
game_menu

    main_submenu:
    "New Game" -> newgame_submenu
    "Settings" -> settings_submenu
        "_Go To New Game menu" -> newgame_menu
    "Exit"

*/


static submenu_t newgame_submenu = {
    .selected = 0,
    .params = M_NOPARAM,
    .item_attrs = COLOR_PAIR(1),
    .sel_attrs = A_BLINK | COLOR_PAIR(2),
    .lock_attrs = A_BOLD | A_REVERSE,

    .items_num = 4,
    .items =
    MENU_START
        MENU_ITEM
                .params = I_FLOCKED,
                .text = "NIGHTMARE",
                .c = { .y = 3, .x = 5 },
                .task.submenu = NULL
        MENU_ITEM_END,

        MENU_ITEM
                .params = I_FLOCKED,
                .text = "Hard",
                .c = { .y = 6, .x = 5 },
                .task.submenu = NULL
        MENU_ITEM_END,

        MENU_ITEM
                .params = I_NOPARAM,
                .text = "Normal",
                .c = { .y = 9, .x = 5 },
                .task.submenu = NULL
        MENU_ITEM_END,

        MENU_ITEM
                .params = I_FLOCKED,
                .text = "Easy-peasy",
                .c = { .y = 12, .x = 5 },
                .task.submenu = NULL
        MENU_ITEM_END,
    MENU_END,
};

static submenu_t settings_submenu = {
    .selected = 0,
    .params = M_NOPARAM,
    .item_attrs = COLOR_PAIR(1),
    .sel_attrs = A_BLINK | COLOR_PAIR(2),
    .lock_attrs = A_BOLD | A_REVERSE,

    .items_num = 6,
    .items =
    MENU_START
        MENU_ITEM
                .params = I_NOPARAM,
                .text = "Controls",
                .c = { .y = 3, .x = 5 },
                // .task.submenu = NULL
        MENU_ITEM_END,

        MENU_ITEM
                .params = I_NOPARAM,
                .text = "Sound Effects",
                .c = { .y = 6, .x = 5 },
        MENU_ITEM_END,

        MENU_ITEM
                .params = I_NOPARAM,
                .text = "Music",
                .c = { .y = 9, .x = 5 },
        MENU_ITEM_END,

        MENU_ITEM
                .params = I_FLOCKED,
                .text = "Locked item",
                .c = { .y = 12, .x = 5 }
        MENU_ITEM_END,

        MENU_ITEM
                .params = I_FLOCKED,
                .text = "Erase saves",
                .c = { .y = 15, .x = 5 },
        MENU_ITEM_END,

        MENU_ITEM
                .params = I_NOPARAM,
                .text = "_Go To New Game menu",
                .c = { .y = 18, .x = 5 },
                .task.submenu = &newgame_submenu
        MENU_ITEM_END,

    MENU_END,
};

static submenu_t main_submenu = {
    .selected = 0,
    .params = M_LOOPED,
    .item_attrs = COLOR_PAIR(1),
    .sel_attrs = A_BLINK | COLOR_PAIR(2),
    .lock_attrs = A_BOLD | A_REVERSE,

    .items_num = 3,
    .items =
    MENU_START
        MENU_ITEM
                .params = I_NOPARAM,
                .text = "New Game",
                .c = { .y = 3, .x = 5 },
                .task.submenu = &newgame_submenu
        MENU_ITEM_END,

        MENU_ITEM
                .params = I_NOPARAM,
                .text = "Settings",
                .c = { .y = 6, .x = 5 },
                .task.submenu = &settings_submenu
        MENU_ITEM_END,

        MENU_ITEM
                .params = I_NOPARAM,
                .text = "Exit",
                .c = { .y = 9, .x = 5 },
                // .task.function = menu_task_exit
        MENU_ITEM_END,
    MENU_END
};


NCURSES_COLOR_T i_fcolor = COLOR_BLACK;
NCURSES_ATTR_T i_bcolor= COLOR_WHITE;

NCURSES_COLOR_T sel_fcolor = COLOR_BLACK;
NCURSES_COLOR_T sel_bcolor = COLOR_CYAN;



int routine(WINDOW* w){

    int ch;
    menu_t* m;

    // m = test_menu();

    init_pair(1, i_fcolor, i_bcolor);
    init_pair(2, sel_fcolor, sel_bcolor);

    menu_init();

    m = menu_create(&main_submenu, w);
    menu_redraw(m);

    // DEBUG_WMSG(DEBUG_WINDOW, 0, 0, "Debug window!");

    #define KEY_ESC 27
    #define CASE_INS(a) ((a) & ~0x0020)

    loop:
    // while( CASE_INS((ch = wgetch(w))) != 'Q' ){
    while( CASE_INS((ch = wgetch(w))) != 'Q' ){

        // DEBUG_PRINTF(0,0,"Input:\n\tChar:%c\n\tCode:%d\n", ch, ch);
        DEBUG_PRINTF(0,0,"Input:\n\tCode:%d\n", ch, ch);

        // printw("%c\n", ch);
        // printw("%d\n", ch);

        switch(ch){
            case KEY_UP:
                menu_select_relative(m, -1);
                goto loop;

            case KEY_DOWN:
                menu_select_relative(m, 1);
                goto loop;

            case KEY_ENTER:
            case 10:
            case 13:
            case KEY_RIGHT:
                // printw("ENTER");
                menu_open(m);
                menu_redraw(m);
                // clear();

                goto loop;

            case KEY_LEFT:
            case KEY_ESC:
                // printw("ESCAPE");
                menu_back(m);
                menu_redraw(m);
                goto loop;

        }
        // refresh()472;
    }

    return 0;
}

int main(){

    WINDOW * mainwin;
    WINDOW * menu_window;
    // WINDOW* debug_window;

    setlocale(LC_ALL, "");
    /*  Initialize ncurses  */
    if ( (mainwin = initscr()) == NULL ) {
        fprintf(stderr, "Error initialising ncurses.\n");
        exit(EXIT_FAILURE);
    }

    menu_window = newwin(30, 30, 0, 0);
    DEBUG_WINDOW = newwin(30, 30, 30, 30);

    cbreak();
    noecho();
    start_color();
    raw();
    set_escdelay(0);
    keypad(menu_window, TRUE);
    curs_set(0);

    // TODO REMOVE THIS
    init_pair(3, COLOR_RED, COLOR_BLUE);
    // wbkgd(menu_window, COLOR_PAIR(3));
    wrefresh(menu_window);

    // refresh();

    routine(menu_window);


    delwin(menu_window);
    // delwin(mainwin);
    endwin();


    return EXIT_SUCCESS;
}