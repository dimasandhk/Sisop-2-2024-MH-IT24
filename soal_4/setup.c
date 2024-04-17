#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define MAX_APPS 10
#define MAX_APP_NAME 50
#define FILENAME "running_apps.txt"

// Variabel global untuk menyimpan PID aplikasi yang sedang berjalan
pid_t running_apps[MAX_APPS];
int num_running_apps = 0;

// Fungsi untuk menambahkan PID aplikasi yang sedang berjalan
void addRunningApp(pid_t pid) {
    if (num_running_apps < MAX_APPS) {
        running_apps[num_running_apps++] = pid;
        printf("Num run apps: %d\n", num_running_apps);
    }
}

// Fungsi untuk membaca data dari file teks saat program dimulai
void loadRunningApps() {
    FILE *file = fopen(FILENAME, "r");
    if (file != NULL) {
        fscanf(file, "%d", &num_running_apps);
        for (int i = 0; i < num_running_apps; i++) {
            fscanf(file, "%d", &running_apps[i]);
        }
        fclose(file);
    }
}

// Fungsi untuk menyimpan data ke file teks saat program dihentikan
void saveRunningApps() {
    FILE *file = fopen(FILENAME, "w");
    if (file != NULL) {
        fprintf(file, "%d\n", num_running_apps);
        for (int i = 0; i < num_running_apps; i++) {
            fprintf(file, "%d\n", running_apps[i]);
        }
        fclose(file);
    }
}

// Fungsi untuk membuka aplikasi
void openApps(char *apps[], int num_apps[]) {
    int i, j;
    pid_t pid;

    for (i = 0; i < MAX_APPS && apps[i] != NULL; i++) {
        for (j = 0; j < num_apps[i]; j++) {
            pid = fork();

            if (pid == 0) {
                execlp(apps[i], apps[i], NULL);
                exit(0);
            } else {
                addRunningApp(pid); // Tambahkan PID aplikasi yang baru dimulai
            }
        }
    }
}

void kill_app(char *app) {
    if (fork() == 0) {
        execlp("pkill", "pkill", app, NULL);
        exit(0);
    }
    sleep(1); // add a small delay
}

void killAppsFromFile(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (file != NULL) {
        char app[256];
        int num;
        while (fscanf(file, "%s %d", app, &num) == 2) {
            kill_app(app);
        }
        fclose(file);
    }
}

// Fungsi untuk membaca file konfigurasi
void readConfigFile(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Cannot open file %s\n", filename);
        exit(1);
    }

    char line[MAX_APP_NAME];
    char *app;
    int num;

    // Parsing setiap baris dalam file konfigurasi
    while (fgets(line, sizeof(line), file)) {
        // Mengambil nama aplikasi dan jumlahnya
        app = strtok(line, " ");
        num = atoi(strtok(NULL, " \n"));

        // Menjalankan aplikasi sesuai dengan konfigurasi
        for (int i = 0; i < num; i++) {
            pid_t pid = fork();
            if (pid == 0) {
                execlp(app, app, NULL);
                exit(0);
            } else {
                addRunningApp(pid);
            }
        }
    }

    fclose(file);
}

// Fungsi untuk mematikan aplikasi sesuai dengan PID yang diberikan
void killApps() {
    FILE *file = fopen(FILENAME, "r");
    if (file != NULL) {
        int num_apps;
        fscanf(file, "%d", &num_apps);
        printf("%d", num_apps);
        for (int i = 0; i < num_apps; i++) {
            int pid;
            fscanf(file, "%d", &pid);
            printf(" = %d, ", pid);
            if (kill(pid, SIGTERM) == -1) {
                perror("Error killing process");
            }
        }
        fclose(file);
    }
}

int main(int argc, char *argv[]) {
    loadRunningApps(); // Muat data dari file teks saat program dimulai

    if (argc < 2) {
        printf("Usage: %s -o <app1> <num1> <app2> <num2> ... <appN> <numN> or %s -f <filename> or %s -k\n", argv[0], argv[0], argv[0]);
        exit(1);
    }

    // Memeriksa perintah yang diminta
    if (strcmp(argv[1], "-o") == 0) {
        // Menginisialisasi variabel untuk menyimpan aplikasi dan jumlahnya
        char *apps[MAX_APPS];
        int num_apps[MAX_APPS];
        int numArgs = argc - 3; // Mengabaikan argumen pertama (-o) dan dua argumen terakhir

        // Parsing argumen dari command line
        int i, j, appIndex = 0;
        for (i = 2; i < argc; i += 2) {
            apps[appIndex] = argv[i];
            num_apps[appIndex] = atoi(argv[i + 1]);
            appIndex++;
        }
        apps[appIndex] = NULL;

        // Membuka aplikasi sesuai dengan argumen yang diberikan
        openApps(apps, num_apps);

        saveRunningApps(); // Simpan data ke file teks saat program dihentikan
    } else if (strcmp(argv[1], "-f") == 0) {
        // Membaca file konfigurasi
        if (argc != 3) {
            printf("Usage: %s -f <filename>\n", argv[0]);
            exit(1);
        }
        readConfigFile(argv[2]);

        saveRunningApps(); // Simpan data ke file teks saat program dihentikan
    } else if (strcmp(argv[1], "-k") == 0) {
        if (argc == 3) {
            killAppsFromFile(argv[2]);
        } else {
            killApps();
            remove(FILENAME);
        }
    } else {
        printf("Invalid option\n");
        exit(1);
    }

    return 0;
}