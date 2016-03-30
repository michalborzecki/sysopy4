#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include "main_lib.h"

int read_args(int argc, char *argv[], int *signal_num);
void sigusr_handler(int signum, siginfo_t *siginfo, void *context);
void send_signals();

int count_on = 0;
int signal_num;
int signal_counter = 0;
int sigusr1_received = 0;
pid_t catcher_pid;

int main(int argc, char *argv[]) {
    char *args_help = "Enter number of signals.\n";
    if (read_args(argc, argv, &signal_num) != 0) {
        printf(args_help);
        return 1;
    }

    struct sigaction act_sigusr;
    act_sigusr.sa_sigaction = sigusr_handler;
    act_sigusr.sa_flags = SA_SIGINFO;
    sigset_t mask_set;	/* used to set a signal masking set. */
    sigfillset(&mask_set);
    act_sigusr.sa_mask = mask_set;
    sigaction(SIGUSR1, &act_sigusr, NULL);
    sigaction(SIGUSR2, &act_sigusr, NULL);


    FILE * catcher_ps = popen("pidof catcher", "r");
    if (catcher_ps == NULL) {
        printf("Error while searching for catcher process occurred.\n");
        return 1;
    }
    if (fscanf(catcher_ps, "%d", &catcher_pid) == EOF) {
        printf("Catcher process not found.\n");
        return 1;
    }
    pclose(catcher_ps);

    send_signals();

    while (1) {
        pause();
    }
}

int read_args(int argc, char *argv[], int *signal_num) {
    if (argc != 2) {
        printf("Incorrect number of arguments.\n");
        return 1;
    }
    int n = atoi(argv[1]);
    if (n <= 0) {
        printf("Incorrect number of signals. It should be > 0.\n");
        return 1;
    }
    *signal_num = n;

    return 0;
}

void send_signals() {
    printf("Sending %d SIGUSR1 signals to catcher...\n", signal_num);
     for (int i = 0; i < signal_num; i++) {
        send_signal(catcher_pid, SIGUSR1);
        while (!sigusr1_received);
        sigusr1_received = 0;
    }
    printf("Sending SIGUSR2 to catcher...\n");
    count_on = 1;
    send_signal(catcher_pid, SIGUSR2);
}

void sigusr_handler(int signum, siginfo_t *siginfo, void *context) {
    if (signum == SIGUSR1) {
        sigusr1_received = 1;
        if (count_on) {
            signal_counter++;
            send_signal(siginfo->si_pid, SIGUSR1);
        }
    }
    else if (signum == SIGUSR2) {
        printf("SIGUSR2 received.\n%d SIGUSR1 received of %d SIGUSR1 signals sent.\n", signal_counter, signal_num);
        exit(0);
    }
}