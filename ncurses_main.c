//
//  ncurses_main.c
//  
//
//  Created by Gurden Angi on 2020-11-11.
//

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
    
    char* choices[10] =
    {
        "port: ",
        "sin family: ",
        "addr: ",
        "maximum concurrent connections: ",
        "maximum open connections: ",
        "read buffer size: ",
        "log connections: ",
        "concurrency model: ",
        "content root directory: ",
        "error 404 page: ",
        
    };
    int choice;
    int highlight = 0;
    
    while(1){
        for(int i = 0; i < 10; i++){
            if(i==highlight)
                wattron(menuwin, A_REVERSE);
            mvwprintw(menuwin, i+1, 1, choices[i]);
            wattroff(menuwin, A_REVERSE);
        }
        choice = wgetch(menuwin);
        
        switch (choice) {
            case KEY_UP:
                highlight--;
                if (highlight == -1) {
                    highlight = 0;
                }
                break;
            case KEY_DOWN:
                highlight++;
                if (highlight == 10) {
                    highlight = 9;
                }
                break;
            default:
                break;
        }
        if(choice == 10) //user pressed enter
            break;
    }
    
    printw("Your choice was: %s", choices[highlight]);
    
//    int c = wgetch(inputwin);
//    if(c == KEY_UP){
//        mvwprintw(inputwin, 1, 1, "You pressed up!");
//        wrefresh(inputwin);
//    }
    
    getch();
    endwin();
    
    
    return 0;
}

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
