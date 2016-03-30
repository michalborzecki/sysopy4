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
    sigset_t mask_set;	/* used to set a signal masking set. */
    sigfillset(&mask_set);
    act_sigusr.sa_mask = mask_set;
    sigaction(SIGUSR1, &act_sigusr, NULL);
    sigaction(SIGUSR2, &act_sigusr, NULL);

    while (1) {
        pause();
    }
}

void sigusr_handler(int signum, siginfo_t *siginfo, void *context) {
    if (signum == SIGUSR1) {
        signal_counter++;
    }
    else if (signum == SIGUSR2) {
        printf("SIGUSR2 received in catcher.\n");
        for (int i = 0; i < signal_counter; i++)
            send_signal(siginfo->si_pid, SIGUSR1);
        send_signal(siginfo->si_pid, SIGUSR2);
        exit(0);
    }
}
