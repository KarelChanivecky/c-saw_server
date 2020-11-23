//
//  ncurses_main.c
//  
//
//  Created by Gurden Angi on 2020-11-11.
//

#include <stdio.h>
#include <curses.h>
#include <string.h>

int main(){
    initscr();
    cbreak();
//    raw();
    noecho();
    
//    get screen size
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);
    int height = yMax-2;
    int width = xMax-2;
    
        start_color();
        init_pair(1, COLOR_CYAN, COLOR_BLACK);
        attron(COLOR_PAIR(1));
    mvprintw(0, xMax/2 - 10, "Server Configuration");
        attroff(COLOR_PAIR(1));
    
//    create a window for our input
    WINDOW* menuwin = newwin(height, width, 1, 1);
//    WINDOW* menuwin = newwin(6, xMax-12, yMax-8, 5);
    box(menuwin, 0, 0);
    refresh();
    wrefresh(menuwin);
    
    keypad(menuwin, true);
    
    char* choices[11] =
    {
        "port: ",
        "sin family: ",
        "addr: ",
        "maximum concurrent connections: ",
        "maximum open connections: ",
        "read buffer size: ",
        "write buffer size: ",
        "log connections: ",
        "concurrency model: ",
        "content root directory: ",
        "error 404 page: "
    };
    
    int positions[11];
    for (int index = 0; index < 11; index++) {
        positions[index] = strlen(choices[index]) + 1;
    }
    
    int choice;
    int highlight = 0;
    
    for(int i = 0; i < 11; i++){
        mvwprintw(menuwin, i+1, 1, choices[i]);
    }
    
    int pos = 0;
    wmove(menuwin, pos + 1, positions[pos]);
    
    while(1){

        choice = wgetch(menuwin);
        int moved = 0;
        switch (choice) {
            case KEY_UP:
                pos--;
                moved = 1;
                if (pos == -1) {
                    pos = 0;
                }
                break;
            case KEY_DOWN:
                pos++;
                moved = 1;
                if (pos == 11) {
                    pos = 10;
                }
                break;
            default:
                break;
        }
        
        wmove(menuwin, pos + 1, positions[pos]);
        if (moved) {
            continue;
        }
        int move_pos = positions[pos];
        while (1) {
            int ch = wgetch(menuwin);
            
            if (ch == KEY_UP) {
                pos--;
                moved = 1;
                move_pos = positions[pos];
                if (pos == -1) {
                    pos = 0;
                }
            }
            if (ch == KEY_DOWN) {
                pos++;
                moved = 1;
                move_pos = positions[pos];
                if (pos == 11) {
                    pos = 10;
                }
            }
            
            wmove(menuwin, pos + 1, move_pos++);
            if (moved) {
                break;
            }
            if (ch == 127 || ch == 8) {
                if (move_pos == positions[pos]+1) {
                    continue;
                }
                --move_pos;
                wmove(menuwin, pos + 1, --move_pos);
                waddch(menuwin, ' ');
                wmove(menuwin, pos + 1, move_pos);
                wrefresh(menuwin);
                continue;
            }
            int moved_horiz = 0;
            if (ch == KEY_LEFT) {
                move_pos--;
                moved_horiz = 1;
                if (move_pos == positions[pos]+1) {
                    move_pos++;
                }
            }
            if (ch == KEY_RIGHT) {
                move_pos++;
                moved_horiz = 1;
                if (move_pos == width-1) {
                    move_pos--;
                }
            }
            
            
            waddch(menuwin, ch);
        }
        if(choice == 10) //user pressed enter
            break;
    }
    
    printw("Your choice was: %s", choices[highlight]);
    

    
    getch();
    endwin();
    
    
    return 0;
}

//        for(int i = 0; i < 10; i++){
//            if(i==highlight)
//                wattron(menuwin, A_REVERSE);
//            mvwprintw(menuwin, i+1, 1, choices[i]);
//            wattroff(menuwin, A_REVERSE);
//        }


//                highlight--;
//                if (highlight == -1) {
//                    highlight = 0;
//                }


//                highlight++;
//                if (highlight == 11) {
//                    highlight = 10;
//                }


//    int c = wgetch(inputwin);
//    if(c == KEY_UP){
//        mvwprintw(inputwin, 1, 1, "You pressed up!");
//        wrefresh(inputwin);
//    }



//    int y, x, yBeg, xBeg, yMax, xMax;
//
//    getyx(stdscr, y, x); // window, y, x;
//    getbegyx(stdscr, yBeg, xBeg); // window, yBeg, xBeg;
//    getmaxyx(stdscr, yMax, xMax); // window, yMax, xMax;
//    printw("%d %d %d %d %d %d", y, x, yBeg, xBeg, yMax, xMax);



//    WINDOW* win = newwin(10, 20, 10, 10); // height, width, start_y, start_x
//    refresh();
//
//    box(win, 0, 0); // window, left-right border, top-bottom border
//    mvwprintw(win, 1, 1, "this is my box");
//    wrefresh(win);



    
//    start_color();
//    init_pair(1, COLOR_RED, COLOR_BLUE);
//    attron(COLOR_PAIR(1));
//    attron(A_STANDOUT | A_UNDERLINE);
//    int derp = 4;
//    printw("Hello, World! = %d", derp);
//    attroff(COLOR_PAIR(1));
//
//    addch('a');
//    move(12, 13); //(y,x)
//
//    mvprintw(15, 20, "Movement");
//    attroff(A_STANDOUT | A_UNDERLINE);
//    mvaddch(12, 50, '@');
