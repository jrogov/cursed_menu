#include <unistd.h>
#include <stdlib.h>


#include "menu.h"
#include "common.h"


struct menu {
    WINDOW* window;     // Window used for menus

    int stack_cnt;      // points to last submenu in stack
    submenu_t* stack[MAX_MENU_NESTING];
};

//-------------------------------TASK.FUNCTION--------------------------------

void* menu_task_exit(void* code){
    endwin();
    exit((int)(long)code);
    return NULL;
}

//-------------------------------MENU FUNCTIONS---------------------------------

menu_t* menu_create(submenu_t* root, WINDOW* window){
    menu_t* m = (menu_t*) malloc(sizeof(menu_t));
    m->window = window;
    m->stack_cnt = 0;
    m->stack[0] = root;
    // m->root = root;
    return m;
}

int menu_add_attr(NCURSES_ATTR_T* attr, NCURSES_ATTR_T newattr){
    *attr = *attr | newattr;
    return 0;
}

int menu_redraw(menu_t* menu){
    submenu_t* submenu;
    struct menu_item* item;
    int select;
    int i = 0;

    submenu = menu->stack[menu->stack_cnt];

    wclear(menu->window);

    // TODO WHAT? SEGFAULT HERE?
    // for (int i = 0; menu->items[i]; i++)
    //     mvaddstr(i, 0, menu->items[i]->text);

    while( (item = submenu->items[i++]) ){
        NCURSES_ATTR_T attrs = (item->params & I_LOCKED)? submenu->lock_attrs : submenu->item_attrs;
        wattrset(menu->window, attrs);
        mvwaddstr( menu->window,
                  item->c.y,
                  item->c.x,
                  item->text
      );
    }

    wattrset(menu->window, submenu->sel_attrs);
    // mvaddstr(menu->selected, 0, menu->items[menu->selected]->text);
    select = submenu->selected;
    mvwaddstr( menu->window,
              submenu->items[select]->c.y,
              submenu->items[select]->c.x,
              submenu->items[select]->text
    );

    wrefresh(menu->window);
    return 0;
}


int menu_select_relative(menu_t* menu, int select){
    return menu_update(menu, select, SELECT_RELATIVE);
}

int menu_select_absolute(menu_t* menu, int select){
    return menu_update(menu, select, SELECT_ABSOLUTE);
}

/*
    Update menu with selection
    menu   : menu to update
    select : absolute/relative position of menu item to select
    flag   : meaning of @select ( 0 - absolute, relative otherwise)
*/
int menu_update(menu_t* menu, int select, char flag){

    submenu_t* submenu;
    struct menu_item* prev_sel;
    struct menu_item* sel;

    int direction;
    int init_select;

    submenu = menu->stack[menu->stack_cnt];
    if( flag )
        select += submenu->selected;
    init_select = select;
    direction = (select < submenu->selected)?-1:+1; // 0 - Up, 1 - down

    if( submenu->params & M_LOOPED ){
        int items_num = submenu->items_num;

        // At least ONE submenu item must NOT be locked
        for(;;){

            while(select >= items_num)
                select -= items_num;
            while(select < 0)
                select += items_num;

            if( !(submenu->items[select]->params & I_UNSEL) )
                break;

            select += direction;
        }
    }
    else{

        // TODO: I_UNSEL SUPPORT
        for(;;){
            if( select < 0 ) select = 0;
            else if (select >= submenu->items_num) select = submenu->items_num-1;

            if( !(submenu->items[select]->params & I_UNSEL))
                break;

            select += direction;

            if( select < 0 || select >= submenu->items_num ){
                direction = -direction;
                select = init_select;
            }


        }

        // POSSIBLE BUG (version above) Uncomment for no I_UNSEL support stable version
        // if( select < 0 ) select = 0;
        // else if (select >= menu->items_num) select = menu->items_num-1;
    }


    // Reset previous item
    prev_sel = submenu->items[submenu->selected];
    wattrset( menu->window, submenu->item_attrs);
    // mvaddstr(menu->selected, 0, menu->items[menu->selected]->text);
    mvwaddstr( menu->window,
               prev_sel->c.y,
               prev_sel->c.x,
               prev_sel->text);

    // Select new item
    sel = submenu->items[select];
    wattrset( menu->window, submenu->sel_attrs);
    mvwaddstr( menu->window,
               sel->c.y,
               sel->c.x,
               sel->text);
    // mvaddstr(select, 0, menu->items[select]->text);
    wrefresh(menu->window);

    submenu->selected = select;

    return MERR_OK;
}

int menu_init(){
    return MERR_OK;
}

// limited by MAX_MENU_NESTING
// Will not follow to submenu when limit reached
int menu_open(menu_t* m){
    int* cnt_p;
    submenu_t* submenu;
    menuitem_t* item;
    submenu_t* selected_submenu;


    cnt_p = &m->stack_cnt;
    if(*cnt_p >= MAX_MENU_NESTING)
        return MERR_STACK_BOUNDS;


    submenu = m->stack[*cnt_p];
    item = submenu->items[submenu->selected];

    if(item->params & I_LOCKED)
        return MERR_ITEM_LOCKED;

    selected_submenu = item->task.submenu;

    if( NULL == selected_submenu )
        return MERR_NULL_SUBMENU;

    // stack_cur = next_cur;
    m->stack[++*cnt_p] = selected_submenu;

    // Check for wrong ->selected, e.g. Locked item
    // NOTE May be redundant. Consider to remove.
    // menu_select_relative(m, 0);

    return MERR_OK;
}

int menu_back(menu_t* menu){

    int* cnt_p = &menu->stack_cnt;
    if(*cnt_p > 0)
        *cnt_p -=1;

    return 0;
}