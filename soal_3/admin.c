#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define LOG_FILE_EXTENSION ".log"

void write_log(const char *user, const char *activity, bool success) {
    time_t current_time;
    struct tm *time_info;
    char time_str[20];
    char log_filename[50];
    FILE *log_file;

    time(&current_time);
    time_info = localtime(&current_time);
    strftime(time_str, sizeof(time_str), "%d-%m-%Y_%H-%M-%S", time_info);

    snprintf(log_filename, sizeof(log_filename), "%s%s", user, LOG_FILE_EXTENSION);

    log_file = fopen(log_filename, "a");
    if (log_file == NULL) {
        perror("Error opening log file");
        exit(EXIT_FAILURE);
    }

    if (success) {
        fprintf(log_file, "[%s]-pid_kegiatan-%s_RUNNING\n", time_str, activity);
    } else {
        fprintf(log_file, "[%s]-pid_kegiatan-%s_FAILED\n", time_str, activity);
    }

    fclose(log_file);
}

void monitor_user_activity(const char *user) {
    char activity[100];

    while (1) {
        strcpy(activity, "Sample activity");

        write_log(user, activity, true);

        sleep(1);
    }
}

void stop_monitoring_user(const char *user) {
    printf("Stop monitoring user: %s\n", user);
}

void cancel_user_activities(const char *user) {
    printf("Canceling activities for user: %s\n", user);
    write_log(user, "User is currently active", false);
}

void allow_user_activities(const char *user) {
    printf("Allowing activities for user: %s\n", user);
    write_log(user, "User is currently active", true);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <command> <user>\n", argv[0]);
        printf("Commands:\n");
        printf("  -m : Monitor user activity\n");
        printf("  -s : Stop monitoring user activity\n");
        printf("  -c : Cancel user activities\n");
        printf("  -a : Allow user activities\n");
        return EXIT_FAILURE;
    }

    char *command = argv[1];
    char *user = argv[2];

    if (strcmp(command, "-m") == 0) {
        printf("Monitoring user: %s\n", user);
        monitor_user_activity(user);
    } else if (strcmp(command, "-s") == 0) {
        stop_monitoring_user(user);
    } else if (strcmp(command, "-c") == 0) {
        cancel_user_activities(user);
    } else if (strcmp(command, "-a") == 0) {
        allow_user_activities(user);
    } else {
        printf("Unknown command: %s\n", command);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}