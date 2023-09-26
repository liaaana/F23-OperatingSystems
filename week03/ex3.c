#include <string.h>
#include <stdio.h>

// + 1 because we need /0 at the end of the string
#define MAX_NAME 64
#define MAX_PATH 2049
#define MAX_DATA 1025
#define MAX_FILES_IN_DIR 255
#define MAX_DIRS_IN_DIR 255

struct Directory {
    char name[MAX_NAME];
    struct File *files[MAX_FILES_IN_DIR];
    struct Directory *directories[MAX_DIRS_IN_DIR];
    unsigned char nf;
    unsigned char nd;
    char path[MAX_PATH];
};

struct File {
    unsigned long id;
    char name[MAX_NAME];
    unsigned long size;
    char data[MAX_DATA];
    struct Directory *directory;
};

void show_file(struct File *pFile) {
    printf("%s ", pFile->name);
}

void show_dir(struct Directory *dir) {
    printf(" \nDIRECTORY\n");
    printf("path: %s\n", dir->path);
    printf("files: \n[");
    for (int i = 0; i < dir->nf; i++) {
        show_file(dir->files[i]);
    }
    printf("]\n");
    printf("directories: \n{ ");
    for (int i = 0; i < dir->nd; i++) {
        show_dir(dir->directories[i]);
    }
    printf(" } \n");
}


void add_dir(struct Directory *dir1, struct Directory *dir2) {
    if (dir2->nd < MAX_DIRS_IN_DIR) {
        if (dir1 && dir2) {
            dir2->directories[dir2->nd] = dir1;
            dir2->nd++;
            char temp_path[MAX_PATH];
            if (strcmp(dir2->path, "/")) {
                strcpy(temp_path, dir2->path);
                strcat(temp_path, "/");
                strcat(temp_path, dir1->name);
                strcpy(dir1->path, temp_path);
            } else {
                strcpy(temp_path, "/");
                strcat(temp_path, dir1->name);
                strcpy(dir1->path, temp_path);
            }
        }
    } else {
        printf("Impossible to add more directories to the directory.\n");
    }
}

void overwrite_to_file(struct File *file, const char *str) {
    strncpy(file->data, str, MAX_DATA - 1);
    file->data[MAX_DATA - 1] = '\0';
    file->size = strlen(file->data) + 1;
}

void append_to_file(struct File *file, const char *str) {
    strncat(file->data, str, MAX_DATA - strlen(file->data) - 1);
    file->size = strlen(file->data) + 1;
    file->data[file->size - 1] = '\0';
}


void printp_file(struct File *file) {
    printf("path: %s/", file->directory->path);
    show_file(file);
    printf("\n");
}

void add_file(struct File *file, struct Directory *dir) {
    if (dir->nf < MAX_FILES_IN_DIR) {
        dir->files[dir->nf] = file;
        dir->nf++;
        file->directory = dir;
    } else {
        printf("Impossible to add more files to the directory.\n");
    }
}


int main() {
    struct Directory root = {.name="root", .nf=0, .nd=0, .path="/"};
    struct Directory home = {.name = "home", .nf = 0, .nd = 0, .path = ""};
    struct Directory bin = {.name = "bin", .nf = 0, .nd = 0, .path = ""};
    add_dir(&bin, &root);
    add_dir(&home, &root);


    struct File bash = {.id = 1, .name = "bash", .size = 0, .data = ""};
    add_file(&bash, &bin);

    struct File ex3_1 = {.id = 2, .name = "ex3_1.c", .size = 0, .data = ""};
    overwrite_to_file(&ex3_1, "int printf(const char * format, ...);");
    struct File ex3_2 = {.id = 3, .name = "ex3_2.c", .size = 0, .data = ""};
    overwrite_to_file(&ex3_2, "//This is a comment in C language");
    add_file(&ex3_1, &home);
    add_file(&ex3_2, &home);

    overwrite_to_file(&bash, "Bourne Again Shell!!");
    append_to_file(&ex3_1, "int main(){printf(\"Hello World!\");}");

    printf("Paths:\n");
    printp_file(&bash);
    printp_file(&ex3_1);
    printp_file(&ex3_2);

    return 0;
}

