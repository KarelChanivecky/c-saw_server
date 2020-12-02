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
#include <libconfig.h>
#include "server_config.h"

#define NUM_CONFIGS 13

void get_positions(int positions[], char* choices[]);
char** prepare_output(char* output_options[], int width);
void print_choices(WINDOW* menuwin, char* choices[]);
void print_instructions (int height, int width, int xMax);
void store_configs(WINDOW* menuwin, char** output, int positions[], int empty_configs_flags[]);
void write_output(char *filename, char** output, int empty_configs_flags[]);
void print_final_msg(int height, int width);
void print_previous_values(WINDOW* menuwin, char** previous_values, int positions[]);
char** read_confg_file(config_t* cfg, server_config_t * server_cfgs, int width);

int main(){
    initscr();
    cbreak();
//    raw();
    noecho();
    start_color();
    
//    get screen size
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);
    int height = yMax-2;
    int width = xMax-2;
    
    print_instructions(height, width, xMax);
    
//    create a window for our input
    WINDOW* menuwin = newwin(height-4, width, 3, 1);
    box(menuwin, 0, 0);
    refresh();
    wrefresh(menuwin);
    
    keypad(menuwin, true);
    
    char* choices[NUM_CONFIGS] =
    {
        "port: ",
        "sin family: ",
        "addr: ",
        "maximum concurrent connections: ",
        "maximum open connections: ",
        "read buffer size: ",
        "write buffer size: ",
        "page expiration time [minutes]: ",
        "log connections [true/false]: ",
        "concurrency model [true/false]: ",
        "pooled [true/false]: ",
        "content root directory: ",
        "error 404 page: "
    };
    
    char* output_options[NUM_CONFIGS] =
    {
        "port = ",
        "sin_family = ",
        "addr = ",
        "max_concurrent_conn = ",
        "max_open_conn = ",
        "read_buffer_size = ",
        "write_buffer_size = ",
        "page_expiration_time_mins = ",
        "log_connections = ",
        "concurrency_model = ",
        "pooled = ",
        "content_root_dir_path = ",
        "page_404_path = "
    };
    
    int positions[NUM_CONFIGS];
    get_positions(positions, choices);
    
    char** output = prepare_output(output_options, width);
    
    print_choices(menuwin, choices);
    
    struct server_config_t* server_cfgs = malloc(sizeof(server_config_t));
    config_t cfg;
    
    char** previous_values = read_confg_file(&cfg, server_cfgs, width);
    print_previous_values(menuwin, previous_values, positions);
    
    
    int pos = 0;
    int moved = 0;
    int exit = 0;
    wmove(menuwin, pos + 1, positions[pos]);

    while (1) {
        int move_pos = positions[pos];
        while (1) {
            int ch = wgetch(menuwin);
            // handle KEY DOWN
            if (ch == KEY_UP) {
                pos--;
                moved = 1;
                if (pos == -1) {
                    pos = 0;
                }
                move_pos = positions[pos];
            }
            // handle KEY DOWN
            if (ch == KEY_DOWN) {
                pos++;
                moved = 1;
                if (pos == NUM_CONFIGS) {
                    pos = NUM_CONFIGS-1;
                }
                move_pos = positions[pos];
            }
            // move
            wmove(menuwin, pos + 1, move_pos++);
            if (moved) {
                moved = 0;
                break;
            }
            // handle backspace
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
            // handle KEY LEFT
            int moved_horiz = 0;
            if (ch == KEY_LEFT) {
                move_pos--;
                moved_horiz = 1;
                if (move_pos <= positions[pos]) {
                    move_pos++;
                }
            }
            // handle KEY RIGHT
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
    
    int empty_configs_flags[NUM_CONFIGS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    
    store_configs(menuwin, output, positions, empty_configs_flags);

    write_output("example.cfg", output, empty_configs_flags);
    
    print_final_msg(height, width);
    
    for (int i = 0; i < 11; i++) {
        free(output[i]);
        free(previous_values[i]);
    }
    free(output);
    free(previous_values);
    free(server_cfgs);
    config_destroy(&cfg);
    
    getch();
    endwin();
    
    
    return 0;
}

void get_positions(int positions[], char* choices[]){
    for (int index = 0; index < NUM_CONFIGS; index++) {
        positions[index] = strlen(choices[index]) + 1;
    }
}

char** prepare_output(char* output_options[], int width){
    char** output = malloc(NUM_CONFIGS*sizeof(char*));
    for (int i = 0; i < NUM_CONFIGS; i++) {
        output[i] = malloc((width-5)*sizeof(char));
        strcpy(output[i], output_options[i]);
    }
    
    return output;
}

void print_choices(WINDOW* menuwin, char* choices[]){
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    wattron(menuwin, COLOR_PAIR(2));
    for(int i = 0; i < NUM_CONFIGS; i++){
        mvwprintw(menuwin, i+1, 1, choices[i]);
    }
    wattroff(menuwin, COLOR_PAIR(2));
}

void print_instructions (int height, int width, int xMax){
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    mvprintw(1, xMax/2 - 10, "Server Configuration");
        attroff(COLOR_PAIR(1));
    
    char * save_msg = "Press ENTER to save configurations";
    int offset = strlen(save_msg)/2;
    mvprintw(height, width/2 - offset, save_msg);
}

void store_configs(WINDOW* menuwin, char** output, int positions[], int empty_configs_flags[]){
    for (int i = 0; i < NUM_CONFIGS; i++) {
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
        if (pos == 0) empty_configs_flags[i] = 1;
        if (i > 10) {
            char* str_quotes = "\"";
            strcat(output[i], str_quotes);
            strcat(output[i], str);
            strcat(output[i], str_quotes);
        }
        else strcat(output[i], str);
    }
}

void write_output(char *filename, char** output, int empty_configs_flags[]) {
    int out = open(filename, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
    for (int i = 0; i < NUM_CONFIGS; i++) {
        if (empty_configs_flags[i]) write(out, "//", 2);
        write(out, output[i], strlen(output[i]));
        write(out, ";\n", 2);
    }
    close(out);
}

void print_final_msg(int height, int width) {
    char * final_msg = "Configurations are saved. Click any key to exit";
    int offset = strlen(final_msg)/2;
    mvprintw(height, width/2 - offset, final_msg);
}

char** read_confg_file(config_t* cfg, server_config_t * server_cfgs, int width){
    config_init(cfg);
    
    char** previous_values = calloc(NUM_CONFIGS, sizeof(char*));
    for (int i = 0; i < NUM_CONFIGS; i++) {
        previous_values[i] = malloc((width-5) * sizeof(char));
    }

    /* Read the file. If there is an error, report it and exit. */
    if(!config_read_file(cfg, "example.cfg"))
    {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(cfg),
            config_error_line(cfg), config_error_text(cfg));
        config_destroy(cfg);
        return(EXIT_FAILURE);
    }

    int port;
    if(config_lookup_int(cfg, "port", &port) == CONFIG_TRUE)
        sprintf(previous_values[0], "%d", port);
    
    int sin_family;
    if(config_lookup_int(cfg, "sin_family", &sin_family) == CONFIG_TRUE)
        sprintf(previous_values[1], "%d", sin_family);
    
    int addr;
    if(config_lookup_int(cfg, "addr", &addr) == CONFIG_TRUE)
        sprintf(previous_values[2], "%d", addr);
    
    int max_concurrent_conn;
    if(config_lookup_int(cfg, "max_concurrent_conn", &max_concurrent_conn) == CONFIG_TRUE)
        sprintf(previous_values[3], "%d", max_concurrent_conn);
    
    int max_open_conn;
    if(config_lookup_int(cfg, "max_open_conn", &max_open_conn) == CONFIG_TRUE)
        sprintf(previous_values[4], "%d", max_open_conn);
    
    int read_buffer_size;
    if(config_lookup_int(cfg, "read_buffer_size", &read_buffer_size) == CONFIG_TRUE)
        sprintf(previous_values[5], "%d", read_buffer_size);
    
    int write_buffer_size;
    if(config_lookup_int(cfg, "write_buffer_size", &write_buffer_size) == CONFIG_TRUE)
        sprintf(previous_values[6], "%d", write_buffer_size);
    
    int page_expiration_time_mins;
    if(config_lookup_int(cfg, "page_expiration_time_mins", &page_expiration_time_mins) == CONFIG_TRUE)
        sprintf(previous_values[7], "%d", page_expiration_time_mins);
    
    bool log_connections;
    if(config_lookup_bool(cfg, "log_connections", &log_connections) == CONFIG_TRUE){
        if (log_connections) previous_values[8] = "true";
        else previous_values[8] = "false";
    }
    
    
    bool concurrency_model;
    if(config_lookup_bool(cfg, "concurrency_model", &concurrency_model) == CONFIG_TRUE){
        if (concurrency_model) previous_values[9] = "true";
        else previous_values[9] = "false";
    }
    
    bool pooled;
    if(config_lookup_bool(cfg, "pooled", &pooled) == CONFIG_TRUE){
        if (pooled) previous_values[10] = "true";
        else previous_values[10] = "false";
    }
    
    config_lookup_string(cfg, "content_root_dir_path", &previous_values[11]);
    config_lookup_string(cfg, "page_404_path", &previous_values[12]);
    

    
    return previous_values;
}

void print_previous_values(WINDOW* menuwin, char** previous_values, int positions[]){
    for(int i = 0; i < NUM_CONFIGS; i++){
        mvwprintw(menuwin, i+1, positions[i], previous_values[i]);
    }
}
