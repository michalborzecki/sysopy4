#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include "main_lib.h"

void sigusr_handler(int signum, siginfo_t *siginfo, void *context);

int signal_counter = 0;

int main(int argc, char *argv[]) {
    struct sigaction act_sigusr;
    act_sigusr.sa_sigaction = sigusr_handler;
    act_sigusr.sa_flags = SA_SIGINFO;
    sigset_t mask_set;
    sigfillset(&mask_set);
    act_sigusr.sa_mask = mask_set;
    sigaction(SIGRTMIN, &act_sigusr, NULL);
    sigaction(SIGRTMIN + 1, &act_sigusr, NULL);

    while (1) {
        pause();
    }
}

void sigusr_handler(int signum, siginfo_t *siginfo, void *context) {
    if (signum == SIGRTMIN)
            signal_counter++;
    else if (signum == SIGRTMIN + 1) {
        for (int i = 0; i < signal_counter; i++)
            send_signal(siginfo->si_pid, SIGRTMIN);
        send_signal(siginfo->si_pid, SIGRTMIN + 1);
        exit(0);
    }
}
