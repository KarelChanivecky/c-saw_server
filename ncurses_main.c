//
//  ncurses_main.c
//  
//
//  Created by Gurden Angi on 2020-11-11.
//

#include <stdio.h>
#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

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
    mvprintw(1, xMax/2 - 10, "Server Configuration");
        attroff(COLOR_PAIR(1));
    
    char * save_msg = "Press ENTER to save configurations";
    int offset = strlen(save_msg)/2;
    mvprintw(height, width/2 - offset, save_msg);
    
//    create a window for our input
    WINDOW* menuwin = newwin(height-4, width, 3, 1);
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
    
    char* output[11];
    for (int i = 0; i < 11; i++) {
        output[i] = malloc((width-5)*sizeof(char));
        strcpy(output[i], choices[i]);
    }
    
    int choice;
    int highlight = 0;
    
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    wattron(menuwin, COLOR_PAIR(2));
    for(int i = 0; i < 11; i++){
        mvwprintw(menuwin, i+1, 1, choices[i]);
    }
    wattroff(menuwin, COLOR_PAIR(2));
    
    int pos = 0;
    wmove(menuwin, pos + 1, positions[pos]);
    int n = 0;

    int moved = 0;
    int exit = 0;
    while (1) {
        int move_pos = positions[pos];
        while (1) {
            int ch = wgetch(menuwin);
            
            if (ch == KEY_UP) {
                pos--;
                moved = 1;
                if (pos == -1) {
                    pos = 0;
                }
                move_pos = positions[pos];
            }
            if (ch == KEY_DOWN) {
                pos++;
                moved = 1;
                if (pos == 11) {
                    pos = 10;
                }
                move_pos = positions[pos];
            }
            
            wmove(menuwin, pos + 1, move_pos++);
            if (moved) {
                moved = 0;
                break;
            }
            
            if (ch == 127 || ch == 8) {
                if (move_pos <= positions[pos]+1) {
                    --move_pos;
                    continue;
                }
                int outer = move_pos;
                char c;
                while (winch(menuwin) != 32) {
                    c = winch(menuwin);
                    int inner = outer-1;
                    wmove(menuwin, pos + 1, --inner);
                    waddch(menuwin, c);
                    wmove(menuwin, pos + 1, outer++);
                    wrefresh(menuwin);
                }
                c = winch(menuwin);
                int inner = outer-1;
                wmove(menuwin, pos + 1, --inner);
                waddch(menuwin, c);
                wmove(menuwin, pos + 1, outer++);
                wrefresh(menuwin);
                
                --move_pos;
                wmove(menuwin, pos + 1, --move_pos);
                wrefresh(menuwin);
                continue;
            }
            
            int moved_horiz = 0;
            if (ch == KEY_LEFT) {
                move_pos--;
                moved_horiz = 1;
                if (move_pos <= positions[pos]) {
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
            
            if (moved_horiz) {
                wmove(menuwin, pos + 1, --move_pos);
                continue;
            }
            
            if(ch == 10){ //user pressed enter
                exit = 1;
                break;
            }
            waddch(menuwin, ch);
        }
        if (exit) break;
    }
    
    for (int i = 0; i < 11; i++) {
        int x_pos = positions[i];
        wmove(menuwin, i+1, x_pos++);
        char c;
        char str[100];
        int pos = 0;
        while ((c = winch(menuwin)) != 32) {
            str[pos++] = c;
            wmove(menuwin, i+1, x_pos++);
        }
        str[pos] = '\0';
        strcat(output[i], str);
    }

    int out = open("configs.txt", O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
    for (int i = 0; i < 11; i++) {
        write(out, output[i], strlen(output[i]));
        write(out, ";\n", 2);
    }
    close(out);
//    wmove(menuwin, 1, 3);
//    int l = mvwinch(menuwin, 1, 18);
    char * final_msg = "Configurations are saved. Click any key to exit";
    offset = strlen(final_msg)/2;
    mvprintw(height, width/2 - offset, final_msg);
    for (int i = 0; i < 11; i++) {
        free(output[i]);
    }
    

    
    getch();
    endwin();
    
    
    return 0;
}
