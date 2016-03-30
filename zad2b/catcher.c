#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include "main_lib.h"

void sigusr_handler(int signum, siginfo_t *siginfo, void *context);
void send_signals(int signal_num, pid_t sender_pid);

int signal_counter = 0;
int sigusr1_received = 0;
int sigusr2_received = 0;
int sender_pid;
int count_on = 1;

int main(int argc, char *argv[]) {
    struct sigaction act_sigusr;
    act_sigusr.sa_sigaction = sigusr_handler;
    act_sigusr.sa_flags = SA_SIGINFO;
    sigset_t mask_set;	/* used to set a signal masking set. */
    sigfillset(&mask_set);
    act_sigusr.sa_mask = mask_set;
    sigaction(SIGUSR1, &act_sigusr, NULL);
    sigaction(SIGUSR2, &act_sigusr, NULL);

    while (!sigusr2_received);
    count_on = 0;
    send_signals(signal_counter, sender_pid);

    return 0;
}

void send_signals(int signal_num, pid_t sender_pid) {
    sigusr1_received = 0;
    for (int i = 0; i < signal_num; i++) {
        send_signal(sender_pid, SIGUSR1);
        while (!sigusr1_received);
        sigusr1_received = 0;
    }
    send_signal(sender_pid, SIGUSR2);
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
        sigusr2_received = 1;
    }
    sender_pid = siginfo->si_pid;
}
