#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>
#include <time.h>
#include <stdbool.h>

#define MAX_PATH_LEN 2048
char *path;


void print_stat(const char *entry_name) {
    char full_path[MAX_PATH_LEN];
    snprintf(full_path, sizeof(full_path), "%s/%s", path, entry_name);

    struct stat st;
    if (stat(full_path, &st) == 0) {
        printf("Stat info for %s/%s:\n", path, entry_name);
        printf("  Size: %lld bytes\n", (long long) st.st_size);
        printf("  Inode: %llu\n", (unsigned long long) st.st_ino);
        printf("  Number of Hard Links: %hu\n", st.st_nlink);
        printf("  Last Access Time: %s", ctime(&st.st_atime));
        printf("  Last Modification Time: %s", ctime(&st.st_mtime));
        printf("  Last Status Change Time: %s", ctime(&st.st_ctime));
    }
    printf("\n");
}

void find_all_hlinks(const char *source) {
    struct stat st;
    if (lstat(source, &st) == -1) {
        fprintf(stderr, "ERROR: lstat failed");
        exit(EXIT_FAILURE);
    }

    printf("Hard links to %s \t Inode: %llu:\n", source, (unsigned long long) st.st_ino);

    DIR *dir;
    struct dirent *entry;
    if ((dir = opendir(path)) == NULL) {
        fprintf(stderr, "ERROR: opening of directory failed\n");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] != '.') {
            char entry_path[PATH_MAX];
            snprintf(entry_path, sizeof(entry_path), "%s/%s", path, entry->d_name);
            struct stat entry_stat;
            if (lstat(entry_path, &entry_stat) == -1) {
                fprintf(stderr, "ERROR: lstat failed\n");
                exit(EXIT_FAILURE);
            }

            if (entry_stat.st_ino == st.st_ino && strcmp(entry_path, source) != 0) {
                printf("Hard link founded:\n\tPath: %s Inode: %llu \n", entry_path,
                       (unsigned long long) entry_stat.st_ino);
            }
        }
    }
    printf("\n");
    closedir(dir);
}


void unlink_all(const char *source) {
    struct stat st;
    char last_link_entry[PATH_MAX];
    bool name_found = false;
    if (lstat(source, &st) == -1) {
        fprintf(stderr, "ERROR:  lstat failed\n");
        exit(EXIT_FAILURE);
    }

    printf("Remove all duplicates of a hard link for %s\n", source);
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(path)) == NULL) {
        fprintf(stderr, "ERROR: opening of directory failed\n");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] != '.') {
            char entry_path[PATH_MAX];
            snprintf(entry_path, sizeof(entry_path), "%s/%s", path, entry->d_name);

            struct stat entry_stat;
            if (lstat(entry_path, &entry_stat) == -1) {
                fprintf(stderr, "ERROR: lstat failed\n");
                exit(EXIT_FAILURE);
            }
            if (entry_stat.st_ino == st.st_ino && strcmp(entry_path, source) != 0) {
                if (unlink(entry_path) == -1) {
                    fprintf(stderr, "ERROR: unlinking failed\n");
                    exit(EXIT_FAILURE);
                }
                printf("Successfully unlinked: %s\n", entry_path);
            }
            if (entry_stat.st_ino == st.st_ino && strcmp(entry_path, source) == 0) {
                strcpy(last_link_entry, entry->d_name);
                name_found = true;
            }
        }
    }
    if (name_found == true) {
        printf("Last hard link path: %s/%s\n", path, last_link_entry);
        printf("Statistics for the last hard link:\n");
        print_stat(last_link_entry);
    }
    else{
        printf("Source file is not found\n");
    }
    printf("\n");
    closedir(dir);
}


void create_sym_link(const char *source, const char *link) {
    char link_path[PATH_MAX];
    snprintf(link_path, sizeof(link_path), "%s/%s", path, link);

    if (symlink(source, link_path) == -1) {
        fprintf(stderr, "ERROR: creating of symbolic link failed\n");
        exit(EXIT_FAILURE);
    }
    printf("Symbolic link %s created for source %s\n\n", link_path, source);
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "ERROR: provide directory path to watch\n");
        exit(EXIT_FAILURE);
    }
    path = argv[1];

    char myfile1_path[PATH_MAX];
    snprintf(myfile1_path, sizeof(myfile1_path), "%s/myfile1.txt", path);
    FILE *myfile1 = fopen(myfile1_path, "w");
    if (myfile1 == NULL) {
        fprintf(stderr, "ERROR: opening of file failed\n");
        exit(EXIT_FAILURE);
    }
    usleep(100);
    fprintf(myfile1, "Hello world.");
    usleep(100);
    fclose(myfile1);

    char myfile11_path[PATH_MAX];
    char myfile12_path[PATH_MAX];
    snprintf(myfile11_path, sizeof(myfile11_path),
             "%s/myfile11.txt", path);
    snprintf(myfile12_path, sizeof(myfile12_path),
             "%s/myfile12.txt", path);
    if (link(myfile1_path, myfile11_path) == -1 || link(myfile1_path, myfile12_path) == -1) {
        fprintf(stderr, "ERROR: link failed\n");
        exit(EXIT_FAILURE);
    }
    usleep(100);
    find_all_hlinks(myfile1_path);
    usleep(100);

    if (rename(myfile1_path, "/tmp/myfile1.txt") == -1) {
        fprintf(stderr, "ERROR: renaming failed\n");
        exit(EXIT_FAILURE);
    }
    usleep(100);
    FILE *myfile11 = fopen(myfile11_path, "a");
    if (myfile11 == NULL) {
        fprintf(stderr, "ERROR: opening of file failed\n");
        exit(EXIT_FAILURE);
    }
    usleep(100);

    fprintf(myfile11, "Modification.");
    usleep(100);
    fclose(myfile11);
    create_sym_link("/tmp/myfile1.txt", "myfile13.txt");
    usleep(100);

    FILE *tmpfile1 = fopen("/tmp/myfile1.txt", "a");
    if (tmpfile1 == NULL) {
        fprintf(stderr, "ERROR: opening of file failed\n");
        exit(EXIT_FAILURE);
    }
    usleep(100);

    fprintf(tmpfile1, "Modification");
    usleep(100);
    fclose(tmpfile1);

    unlink_all(myfile11_path);
    usleep(100);

    return 0;
}

