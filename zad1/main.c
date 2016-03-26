#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

enum Direction {LTR, RTL};

int read_args(int argc, char *argv[], int *repeat_num, char ** text);
void sigtstp_handler(int signum);
void sigint_handler(int signum);

enum Direction dir = LTR;
int repeat_num = 1;
int dir_changed = 0; // only true just after LTR -> RTL change
int repeat_num_change_sign = 1; // is repeat_num increasing or decreasing
int max_repeat_num;

int main(int argc, char *argv[]) {
    char * args_help = "Enter: repeats number and text to print (between \" characters).";
    char * text = NULL;
    if (read_args(argc, argv, &max_repeat_num, &text) != 0) {
        printf("%s\n", args_help);
        return 1;
    }

    struct sigaction act;
    act.sa_handler = sigtstp_handler;
    sigaction(SIGTSTP, &act, NULL);
    signal(SIGINT, sigint_handler);

    while (1) {
        enum Direction actual_dir = dir;
        int actual_repeat_num = repeat_num;
        for (int i = 1; i <= actual_repeat_num; i++)
            if (actual_dir == LTR)
                printf("%s", text);
            else
                for (long j = strlen(text) - 1; j >= 0; j--)
                    printf("%c", text[j]);
        printf("\n");
        sleep(1);
    }
}

int read_args(int argc, char *argv[], int *repeat_num, char ** text) {
    if (argc != 3) {
        printf("Incorrect number of arguments.\n");
        return 1;
    }
    int n = atoi(argv[1]);
    if (n <= 0) {
        printf("Incorrect repeat number. It should be > 0.\n");
        return 1;
    }
    *repeat_num = n;
    if (strlen(argv[2]) == 0) {
        printf("Empty text is incorrect.\n");
        return 1;
    }
    *text = argv[2];

    return 0;
}

void sigtstp_handler(int signum) {
    if (dir_changed == 0 && dir == LTR)
        dir = RTL;
    else if (dir == RTL) {
        dir = LTR;
        dir_changed = 1;
    }
    else {
        repeat_num += repeat_num_change_sign;
        if (repeat_num_change_sign == 1 && repeat_num == max_repeat_num)
            repeat_num_change_sign = -1;
        else if (repeat_num == 1)
            repeat_num_change_sign = 1;
        dir_changed = 0;
    }
}

void sigint_handler(int signum) {
    printf("Odebrano sygnal SIGINT.\n");
    exit(EXIT_SUCCESS);
}
