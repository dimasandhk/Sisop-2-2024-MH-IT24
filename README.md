# Sisop Praktikum Modul 2-2024-MH-IT 24

Anggota Kelompok:

- Dimas Andhika Diputra 5027231074
- Mochamad Fadhil Saifullah 5027231068
- Thio Billy Amansyah 5027231007

# Soal 1 - virus.c

Pada soal berikut, diminta sebuah _executable_ bernamakan `virus` yang berisi beberapa function yang dapat melakukan kerja **find and replace** dan **report** terhadap beberapa flag yang berada pada file-file text dalam suatu folder.

```

	-- Do --
	gcc -g virus.c -o virus && ./virus /path/to/folder/

	-- Don't --
	gcc -g virus.c -o virus && ./virus /path/to/folder

```

Program akan berjalan setelah mengcompile source code lalu memasukkan file executable beserta jalur menuju folder yang berisikan file text yang akan dicek.

> **Perlu diperhatikan bahwa path menuju folder perlu diakhiri dengan `/` agar bisa bekerja dikarenakan salah satu bagian dari design source code yang mendapatkan path menuju file text dengan mengabungkan path menuju folder dengan nama dari file text.**

# Files

Beberapa nama berikut adalah bagian-bagian yang diperlukan untuk menjalakan program beserta deskripsi detailnya.

### Executable File

**File executable** yang didapatkan dari mengcompile source code yang berasal dari `virus.c`. Fungsinya adalah mendeteksi dan mengubah flag-flag yang telah ditentukan dalam file text. Berisikan binary yang bisa langsung dijalankan dalam sebuah terminal. Source code dibentuk dalam mesin Linux dan menggunakan library `<unistd.h>` sehingga akan lebih baik apabila file `exe` dijalankan dalam mesin yang sama untuk kelancaran.

### Text Files

File text hanya berisikan string yang akan menjadi input utama dari **virus**. Flag-flag yang berada dalam file ini adalah **`"m4LwAr3"`**, **`"5pYw4R3"`**, dan **`"R4nS0mWaR3"`**.

### /path/to/folder/

Path menuju folder ini menjadi input utama kedua yang diperlukan untuk mendapatkan file-file text di dalamnya. Input hanya dapat dimasukkan dengan cara direct, seperti `/path/to/folder/` dan `./path-to-folder/`. Ingat untuk memasukkan `/` pada akhir path dikarenakan oleh alasan yang telah dijelaskan pada pembukaan.

# Source Code

Terdapat beberapa function buatan yang penting dalam menjalankan fungsi utama dari program ini. Function tersebut dapat dilihat di bawah ini dengan judul berupa nama atau fungsi yang dijalankannya.

## Input Validation

```

	if (argc != 2) {
		printf("Error: invalid input\n\nUsage: ./virus ./path/to/input/\nPUT '/' AT THE END OF TARGET PATH LIKE THE EXAMPLE ABOVE\n");
		return 1;
	}

	--- Output ---
	Error: invalid input

	Usage: ./virus ./path/to/input/
	PUT '/' AT THE END OF TARGET PATH LIKE THE EXAMPLE ABOVE

```

Memastikan input `path` dapat diproses oleh program. Pengecekan dilakukan dengan menghitung jumlah argumen yang masuk dari terminal. Argumen yang kurang/lebih dari dua akan dinilai tidak valid. Argumen yang kurang tanda `/` pada akhir argumen kedua juga termasuk tidak valid.

## Redirect STDIN, STDOUT, STDERR to /dev/null in C

```

	for(int fd=0; fd<3; fd++) close(fd);
	open("/dev/null", O_RDONLY);
	open("/dev/null", O_WRONLY);
	open("/dev/null", O_WRONLY);

```

Berfungsi untuk menyembunyikan output apapun dari terminal dengan mengarahkan segala output (STDIN, STDOUT, STDERR) ke `/dev/null`.

## Fork()

```

	pid_t pid;
	pid = fork();
	if (pid == 0) {
		...
		...
		...
	} else if (pid != 0) { // stopping parent process
		printf("Ping: Wont open parent processes (%i)\n", pid);
		return 1;
	}

```

This bit is to make the program run in background by running it on child process and killing the parent process. Result is two `pid` where one with value of `0` is a child process while the other is the parent. Also that ping won't show during runtime, its just remnant of debugging process.

## Accessing All Entries

```

		...
		if (pid == 0) {
			DIR *dir;
			struct dirent *ent;
			char path[strlen(argv[1])+1]; //  add 1 for '\0' character.
			strcpy(path, argv[1]); // dont do 'char *path = argv[1]' cuz it copies the ptr adress of source (can change 'argv[1]' if 'path' is changed)

			if ((dir = opendir (argv[1])) != NULL) {
				...
				...
				...
			} else {	// could not open directory
				printf("Error: Cant open dir\n");
				return 1;
			}
		return 0;
	}

```

Using `<dirent.h>` library, this program can look over a folder and its contents. Specifically using `ent->d_name` from `struct dirent *ent` in the next section. Any important notes during debugging is in the source code as comments (aint typing allat back). Any non-null directory will be further processed, as ones that are, program won't do anything (prints error during debugging if that counts).

## Further Processing

```

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
		} else...

```

Will first check if file is null/not. Non-null can be searched through using self-made function `freplace()`. But before that, the program will make a proper direct path to a file because it will be an important input for the function.

The `ent->d_name` will have a string value of the file name. This is the part where the 'why' of putting `/` at end of path is answered. As you can see, there is a `strcat()` function to assemble a direct link to the file. If there isn't `/` at end, the path would look like `/path/tofile` instead of `/path/to/file`.

Single mention of `continue` function can skip one loop from a currently working loop statement.

## freplace()

```
	int freplace(const char *path, char *trg, char *new) {
	  FILE *fp = fopen(path, "r");
	  int fsize;
	  fseek(fp, 0L, SEEK_END); // incrementing fsize while fgetc() is on a loop will result in stack overflow. Only stops at +/- 32000 byte or so.
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

```

Function does its thing by opening file in `r` mode (read) to measure file size. Then it read its content to a `temp[]` char array. Finally it uses all previously gained argument to do `find and replace` operation on the file, such as target string, replacement string, buffer text string, file pointer, file size, and direct path to file.

## replace()

```

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

```

Records length of target string before doing printing and target check one character at a time to appointed text file. It checks the starting, middle, and end character between temp array and target array.

A match would print replacement string and skip writing from `tmp[]` array by how big/long the replacement string is.

## mark()

```

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

```

Reports the findings in separate text file on single log file. Much of the past bugs is written as comments if you are interested in reading it.
