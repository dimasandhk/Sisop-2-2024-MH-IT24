#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>

void donlot() {
    pid_t pid = fork();
    if (pid == 0) {
        FILE *out = fopen("/home/dim/uni/sisop/Sisop-2-2024-MH-IT24/soal_2/logfile.txt", "w");
        if (out == NULL) {
            perror("Couldn't open logfile");
            exit(EXIT_FAILURE);
        }
        dup2(fileno(out), 1); // redirect stdout to out
        dup2(fileno(out), 2); // redirect stderr to out
        fclose(out);

        char *donlot[] = {"wget", "--no-check-certificate", "https://drive.google.com/uc?export=download&id=1rUIZmp10lXLtCIH3LAZJzRPeRks3Crup", "-O", "/home/dim/uni/sisop/Sisop-2-2024-MH-IT24/soal_2/library.zip", NULL};
        execv("/bin/wget", donlot);
    } else {
        int stat;
        waitpid(pid, &stat, 0);  
    }
}

int main() {
    pid_t pid, sid;

    pid = fork();

    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    umask(0);

    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    if ((chdir("/")) < 0) {
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    sleep(30);

    exit(EXIT_SUCCESS);
}