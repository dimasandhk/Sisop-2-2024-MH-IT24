#include <stdio.h>
#include <string.h> // strlen(), strcat(), strcmp(), strcpy()
#include <dirent.h> // closedir(), DIR *, opendir(), readdir(), struct dirent *
#include <unistd.h> // fork(), close(), sleep()
#include <fcntl.h> // O_RDONLY, O_WRONLY
#include <time.h>
#include <locale.h>

void replace(char *old, char *new, char *tmp, FILE *fp, int fsize, const char *path);
int freplace(const char *path, char *trg, char *new);
void mark(const char *path, char *old, char *new);

int main(int argc, char *argv[]) {
	// input check
	if (argc != 2) {
		printf("Error: invalid input\n\nUsage: ./virus ./path/to/input/\nPUT '/' AT THE END OF TARGET PATH LIKE THE EXAMPLE ABOVE\n");
		return 1;
	}
	
	// redirect STDIN, STDOUT, STDERR to /dev/null in C
	for(int fd=0; fd<3; fd++) close(fd);
	open("/dev/null", O_RDONLY);
	open("/dev/null", O_WRONLY);
	open("/dev/null", O_WRONLY);
	
	
	// create a new child process
	pid_t pid;
	pid = fork();
	if (pid == 0) {
		DIR *dir;
		struct dirent *ent;
		char path[strlen(argv[1])+1]; 
		strcpy(path, argv[1]);	// dont do 'char *path = argv[1]' cuz it copies the ptr adress of source (can change 'argv[1]' if 'path' is changed). Add 1 for '\0' character.
		
		if ((dir = opendir (argv[1])) != NULL) {
			while ((ent = readdir (dir)) != NULL) {	// access all the files and directories within directory
				const char *fname = ent->d_name;
				if (strcmp(fname, ".") == 0 || strcmp(fname, "..") == 0) continue; // 'continue' will skip loop that opens "." and ".." directory
				strcat(path, fname);
				freplace(path, "m4LwAr3", "[MALWARE]");	// do find and replace operation
				freplace(path, "5pYw4R3", "[SPYWARE]");
				freplace(path, "R4nS0mWaR3", "[RANSOMWARE]");
				strcpy(path, argv[1]); // re-copy 'path' original value because strcat() change its value to strcat()'s result.
			}
			closedir (dir);
		} else {	// could not open directory
			printf("Error: Cant open dir\n");
			return 1;
		}
	} else if (pid != 0) { // stopping parent process
		printf("Ping: Wont open parent processes (%i)\n", pid);
		return 1;
	}
	return 0;
}

int freplace(const char *path, char *trg, char *new) {
  FILE *fp = fopen(path, "r");
  int fsize;
  fseek(fp, 0L, SEEK_END);
	fsize = ftell(fp);
	
	char tmp[fsize]; 
	rewind(fp); // dont erase, will segfault. Caused by prev 'fgetc()' moving file pos indicator to end. 
	
	// read file to a tmp array
	fread(tmp, sizeof tmp[0], fsize, fp);
	fclose(fp);
	
	// find 'n replace file
	fp = fopen(path,"w");
	replace(trg, new, tmp, fp, fsize, path); 
	fclose(fp);
	
	return 0;
}

void replace(char *trg, char *new, char *tmp, FILE *fp, int fsize, const char *path) {
	int trg_len = strlen(trg);
	for(int i = 0; i < fsize ; i++) {
		// find target string
		if(tmp[i] == trg[0] && tmp[i+trg_len-1] == trg[trg_len-1] && tmp[i+trg_len/2] == trg[trg_len/2]) {
			fputs(new, fp);
			i = i + trg_len - 1;
			mark(path, trg, new);
		} else {
			fputc(tmp[i], fp);
		}
	}
}

void mark(const char *path, char *old, char *new) {
	FILE *freport = fopen("virus.log", "a");
	char ping[100];
	time_t t = time(NULL);
	struct tm loctm;
	localtime_r(&t, &loctm);
	
	// print report to char array
	strftime(ping, sizeof ping, "[%d-%m-%Y][%X]  Suspicious string at ", &loctm);
	fprintf(freport, "%s%s successfully replaced (%s -> %s)\n", ping, path, old, new);
	fclose(freport);	// missed this part and whole thing just doesnt work :VV
}
