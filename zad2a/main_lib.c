#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include "main_lib.h"

int send_signal(pid_t pid, int signal_no) {
    int result = sigqueue(pid, signal_no, (union sigval)0);
    if (result != 0)
        printf("Error while sending signal occurred.\n");
    return result;
}
