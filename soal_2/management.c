#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <sys/wait.h>
#include <dirent.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>

int default_mode = 1;
int backup_mode = 1;
int restore_mode = 1; 
int exiting = 0; 

void histlog(char *name, char *action) {
    char result[1000];
    time_t curr_time;
    struct tm * timeinfo; 
    time(&curr_time);
    timeinfo = localtime(&curr_time);

    if (strstr(action, "rename") != NULL) {
        sprintf(result, "[dim][%d:%d:%d] - %s - Succesfully Renamed.", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, name);
    }

    FILE *file = fopen("/home/dim/uni/sisop/Sisop-2-2024-MH-IT24/soal_2/history.log", "a"); 
    if (file != NULL) {  
        fprintf(file, "%s\n", result); 
        fclose(file); 
    } else { 
        printf("Error opening the file\n"); 
    } 
}

int file_exist(const char *fname) {
    char fixname[100] = "/home/dim/uni/sisop/Sisop-2-2024-MH-IT24/soal_2";
    strcat(fixname, fname);
    if (access(fixname, F_OK) == 0) {
        return 1;
    } else {    
        return 0;
    }
}

int rename_stuff() {
    char *basePath = "/home/dim/uni/sisop/Sisop-2-2024-MH-IT24/soal_2/library";
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    while ((dp = readdir(dir)) != NULL) {
        if (default_mode == 1) {
            if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {

            char to_change[500] = "/home/dim/uni/sisop/Sisop-2-2024-MH-IT24/soal_2/library/";
            char original_name[500] = "/home/dim/uni/sisop/Sisop-2-2024-MH-IT24/soal_2/library/";
            char text[100], ch;
            int key = 19;
            strcpy(text, dp->d_name);
                if (text[0] >= '0' && text[0] <= '9') {
                    if (strstr(dp->d_name, "d3Let3") != NULL) {
                        strcat(to_change, dp->d_name);
                        remove(to_change);
                        histlog(text, "delete");
                    } else if (strstr(text, "r3N4mE") != NULL) {
                        strcat(to_change, dp->d_name);
                        if (strstr(text, ".ts") != NULL) {
                            rename(to_change, "/home/dim/uni/sisop/Sisop-2-2024-MH-IT24/soal_2/library/helper.ts");
                            histlog("helper.ts", "rename");
                        }
                        else if (strstr(text, ".py") != NULL) {
                            rename(to_change, "/home/dim/uni/sisop/Sisop-2-2024-MH-IT24/soal_2/library/calculator.py");
                            histlog("calculator.py", "rename");
                        }
                        else if (strstr(text, ".go") != NULL) {
                            rename(to_change, "/home/dim/uni/sisop/Sisop-2-2024-MH-IT24/soal_2/library/server.go");
                            histlog("server.go", "rename");
                        }
                        else {
                            rename(to_change, "/home/dim/uni/sisop/Sisop-2-2024-MH-IT24/soal_2/library/renamed.file");
                            histlog("renamed.file", "rename");
                        }
                    }
                } else {
                    for (int i = 0; text[i] != '\0'; ++i) {
                        ch = text[i];
                        if (isalnum(ch)) {
                            if (islower(ch)) {
                                ch = (ch - 'a' - key + 26) % 26 + 'a';
                            }
                            if (isupper(ch)) {
                                ch = (ch - 'A' - key + 26) % 26 + 'A';
                            }
                        }
                        text[i] = ch;
                    }
                    if (strstr(text, "d3Let3") != NULL) {
                        strcat(to_change, dp->d_name);
                        remove(to_change);
                        histlog(dp->d_name, "delete");
                    } else if (strstr(text, "r3N4mE") != NULL) {
                        strcat(to_change, dp->d_name);
                        if (strstr(text, ".ts") != NULL) {
                            rename(to_change, "/home/dim/uni/sisop/Sisop-2-2024-MH-IT24/soal_2/library/helper.ts");
                            histlog("helper.ts", "rename");
                        }
                        else if (strstr(text, ".py") != NULL) {
                            rename(to_change, "/home/dim/uni/sisop/Sisop-2-2024-MH-IT24/soal_2/library/calculator.py");
                            histlog("calculator.py", "rename");
                        }
                        else if (strstr(text, ".go") != NULL) {
                            rename(to_change, "/home/dim/uni/sisop/Sisop-2-2024-MH-IT24/soal_2/library/server.go");
                            histlog("server.go", "rename");
                        }
                        else {
                            rename(to_change, "/home/dim/uni/sisop/Sisop-2-2024-MH-IT24/soal_2/library/renamed.file");
                            histlog("renamed.file", "rename");
                        }
                    } else if (strstr(text, "m0V3") != NULL) {
                        strcat(original_name, dp->d_name);
                        strcat(to_change, text);
                        rename(original_name, to_change);
                        histlog(dp->d_name, "rename");
                    }
                }  
            }
        }
        sleep(1);
    }
    closedir(dir);
}

int default_act() {
    int status;
    if (file_exist("library.zip") == 0) {
        pid_t download_child = fork();
        
        if (download_child == 0) {
            backup_mode = 0;
            char *argv[6] = {"Download", "--content-disposition", "https://docs.google.com/uc?export=download&id=1rUIZmp10lXLtCIH3LAZJzRPeRks3Crup", "-P", "/home/dim/uni/sisop/Sisop-2-2024-MH-IT24/soal_2", NULL};
            execv("/bin/wget", argv);
        }
        
    } 
    
    wait(&status);
    if (WIFEXITED(status)) {
        if (file_exist("history.log") == 0) {
            pid_t history_child = fork();
            if (history_child == 0) {
                char *argv[3] = {"make a history", "/home/dim/uni/sisop/Sisop-2-2024-MH-IT24/soal_2/history.log", NULL};
                execv("/bin/touch", argv);
            }
        
        }
    }

    wait(&status);
    if (WIFEXITED(status)) {
        if (file_exist("library") == 0) {
            pid_t unzip_child = fork();
            if (unzip_child == 0) {
                char *argv[5] = {"unzip", "/home/dim/uni/sisop/Sisop-2-2024-MH-IT24/soal_2/library.zip", "-d", "/home/dim/uni/sisop/Sisop-2-2024-MH-IT24/soal_2", NULL};
                execv("/bin/unzip", argv);
            }
        } else {
            pid_t lib_delete_child = fork();
            if (lib_delete_child < 0) {
                printf("garpu gagal");
                exit(EXIT_FAILURE);
            }

            else if (lib_delete_child == 0) {
                backup_mode = 0;
                if (file_exist("library.zip") != 0) {
                    pid_t del_childchild = fork();
                    if (del_childchild == 0) {
                        char *argv[4] = {"remove", "-rf", "/home/dim/uni/sisop/Sisop-2-2024-MH-IT24/soal_2/library", NULL};
                        execv("/bin/rm", argv);
                    }
                    wait(&status);
                    if (WIFEXITED(status)) {
                        pid_t zip_child = fork();
                        if (zip_child == 0) {
                            char *argv[5] = {"unzip", "/home/dim/uni/sisop/Sisop-2-2024-MH-IT24/soal_2/library.zip", "-d", "/home/dim/uni/sisop/Sisop-2-2024-MH-IT24/soal_2", NULL};
                            execv("/bin/unzip", argv);
                        }
                        else {
                            wait(&status);
                            if (WIFEXITED(status)) {
                                rename_stuff();
                            }
                        }
                    }
                }
            }
        }
    }

    wait(&status);
    if (WIFEXITED(status)) {
        if (file_exist("library") != 0) {
            pid_t mkdir_child = fork();
            if (mkdir_child == 0) {
                char *argv[3] = {"making folder", "/home/dim/uni/sisop/Sisop-2-2024-MH-IT24/soal_2/library/backup", NULL};
                execv("/bin/mkdir", argv);
            }
        }
    }

    wait(&status);
    if (WIFEXITED(status)) {
        rename_stuff();
    }
    sleep(2024);
}

void signal_handler(int signal) {
    if (signal == SIGRTMIN) {
        default_mode = 1;
        restore_mode = 0;
        backup_mode = 0;
    } 
}

int main(int argc, char *argv[]) {
    signal(SIGRTMIN, signal_handler);
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

    printf("PID: %d\n", getpid());

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    while (1) {
        default_act();
        sleep(2024);
    }

    return 0;
}