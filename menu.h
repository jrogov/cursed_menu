#ifndef _MENU_H_
#define _MENU_H_

#include <ncurses.h>

//-------------------------------CONFIG-----------------------------------------


#define MAX_MENU_NESTING 10


//-------------------------------ENUMS----------------------------------------

enum menu_params {
    M_NOPARAM = 0,
    M_LOOPED = 1
};

enum item_params {
    I_NOPARAM  = 0,
    I_UNSEL    = 0x1,  // Unselectable item
    I_LOCKED   = 0x2,  // Locked item: can't be opened/executed
    I_FLOCKED  = 0x1|0x2, // Unselectable and locked: can't be selected and opened

    I_SUBMENU  = 1<<10, // task = submenu
    I_FUNCTION = 1<<11  // task = function
};

// These are warning rather than errors:
//      it won't fail if any code but MERR_OK returned,
//      but will apply implemented error-proof behaviour
// So use these only as a warning
enum menu_errors {
    MERR_OK = 0,        // OK
    MERR_ERROR,         // generic error
    MERR_STACK_BOUNDS,  // out of bounds submenu navigation (either lower or upper)
    MERR_NULL_SUBMENU,  // submenu's item has no submenu to follow
    MERR_ITEM_LOCKED,   // Tried to open I_LOCKED item
};


//-------------------------------STRUCTS AND TYPES------------------------------

struct menu;
struct submenu;
struct menu_item;

typedef struct menu menu_t;
typedef struct submenu submenu_t;
typedef struct menu_item menuitem_t;

struct submenu {
    // TODO: smth with label

    int selected;               // index of selected item

    // MENU STYLE
    enum menu_params params;    // MENU STYLe

    // TODO Should menu style be unified for all submenus of all menu?

    // ITEMS STYLE
    NCURSES_ATTR_T item_attrs;
    NCURSES_ATTR_T sel_attrs;
    NCURSES_ATTR_T lock_attrs;

    // ITEMS DATA
    int items_num;
    struct menu_item** items;
};

struct menu_item {

    enum item_params params;
    char* text;
    struct win_coord {
        unsigned short y;
        unsigned short x;
    } c;

    union {
        submenu_t* submenu;
        // void* (*function)(void*);
    } task;
};


#define MENU_START ((struct menu_item*[]){
#define MENU_ITEM &((struct menu_item){
#define MENU_ITEM_END })
#define MENU_END NULL})


//-------------------------------FUNCTIONS-------------------------------------

// Handy macroses for #menu_update
#define SELECT_ABSOLUTE 0
#define SELECT_RELATIVE 1

void* menu_task_exit(void* code);
menu_t* menu_create(submenu_t* root, WINDOW* window);
int menu_add_attr(NCURSES_ATTR_T* attr, NCURSES_ATTR_T newattr);
int menu_redraw(menu_t* menu);
int menu_select_relative(menu_t* menu, int select);
int menu_select_absolute(menu_t* menu, int select);
int menu_update(menu_t* menu, int select, char flag);
int menu_init();
int menu_open(menu_t* m);
int menu_back(menu_t* menu);

//-------------------------------TASK.FUNCTION--------------------------------

void* menu_task_exit(void*);


#endif /* _MENU_H_ */


