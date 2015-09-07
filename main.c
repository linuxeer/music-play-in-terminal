#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>

#define SONGS_MAX 1000
#define PATH_LEN 100

void operate();
void play_song();
void read_songs();
void list_songs();
char * format_song_name();

char * songs[SONGS_MAX];
int song_cnt = 0;
int song_index = 0;
bool paused = false;
pid_t pid;

void operate() {
    mvprintw(1 , 0 , "you can select songs by up arrow or down arrow!");
    bool playing = true;
    while (playing) {
        int ch = getch();
        switch (ch) {
            case KEY_DOWN:
                song_index++;
                if (song_index >= song_cnt) {
                    song_index = 0;
                }
                list_songs(song_cnt , song_index);
                break;
            case KEY_UP:
                song_index--;
                if (song_index < 0) {
                    song_index = song_cnt - 1;
                }
                list_songs(song_cnt , song_index);
                break;
            case 'q':
                return;
            case 10:
                /*
                 * enter key
                 * */
                play_song();
                break;
            case 32:
                if (!paused) {
                    kill(pid , SIGTSTP);
                    paused = true;
                } else {
                    kill(pid , SIGCONT);
                    paused = false;
                }
                break;
        }
    }
}

void play_song() {
    if (pid) {
        kill(pid , SIGTERM);
    }
    char path[PATH_LEN] = "songs/";
    char * song_name = format_song_name();
    strcat(path , song_name);
    pid = fork();
    if (pid == 0) {
        execlp("play" , "play" , path , NULL);
        printf("the song is finished!");
        free(song_name);
    }
}

char * format_song_name() {
    char * song_name = songs[song_index];
    char * song_name_formated = malloc(sizeof(char) * 50);
    int len = strlen(song_name);
    int j = 0;
    for (int i = 0 ; i < len ; i++) {
        if (song_name[i] == ' ') {
            song_name_formated[j] = '\\';
            j++;
        }
        song_name_formated[j] = song_name[i];
        j++;
    }
    song_name_formated[j] = '\0';
    return song_name_formated;
}

void read_songs() {
    DIR * dp;
    struct dirent * filename;
    dp = opendir("songs");
    if (!dp) {
        printf("read songs error!");
        exit(1);
    }
    while (filename = readdir(dp)) {
        if (filename->d_name[0] == '.') {
            continue;
        }
        songs[song_cnt++] = filename->d_name;
    }
    closedir(dp);
    list_songs(song_cnt , song_index);
}

void list_songs() {
    for (int i = 0 ; i < song_cnt ; i++) {
        if (song_index == i) {
            wattron(stdscr , COLOR_PAIR(1));
            mvprintw(10 + i , 20 , "%s\n" , songs[i]);
            wattroff(stdscr , COLOR_PAIR(1));
        } else {
            mvprintw(10 + i , 20 , "%s\n" , songs[i]);
        }
    }
}

int main(int argc , char ** argv) {
    initscr();
    noecho();
    keypad(stdscr , TRUE);
    start_color();
    init_pair(1 , COLOR_WHITE , COLOR_RED);
    curs_set(0);
    printw("welcome to use this player!");
    read_songs();
    operate();
    return 0;
}
