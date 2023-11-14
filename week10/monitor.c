#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <sys/inotify.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>

#define EVENT_SIZE  (sizeof(struct inotify_event))
#define EVENT_BUF_LEN  (1024 * (EVENT_SIZE + 16))
#define MAX_PATH_LEN 2048

char buffer[EVENT_BUF_LEN];
char *path;
int fd, wd;

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

void print_all_stat() {
    int files_counter = 0;
    printf("Statistics for all files:\n");
    DIR *dir = opendir(path);
    if (dir == NULL) {
        fprintf(stderr, "ERROR: Unable to open the directory\n");
        closedir(dir);
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] != '.') {
            if (entry->d_type == DT_REG || entry->d_type == DT_DIR) {
                print_stat(entry->d_name);
                files_counter++;
            }
        }
    }
    if (files_counter == 0) {
        printf("There are no files at the moment");
    }
    closedir(dir);
}


void signal_handler(int sig) {
    if (sig == SIGINT) {
        printf("Printing statistics for all files of watching directory...\n");
        if (fd) {
            inotify_rm_watch(fd, wd);
            close(fd);
        }
        print_all_stat();
        printf("Terminating...\n");
        exit(EXIT_SUCCESS);
    }
}


int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "ERROR: provide directory path to watch\n");
        exit(EXIT_FAILURE);
    }

    path = argv[1];
    signal(SIGINT, signal_handler);

    fd = inotify_init();
    if (fd < 0) {
        fprintf(stderr, "ERROR: Unable to initialize inotify\n");
        exit(EXIT_FAILURE);
    }

    wd = inotify_add_watch(fd, path, IN_ACCESS | IN_CREATE | IN_DELETE | IN_MODIFY | IN_OPEN | IN_ATTRIB);
    if (wd == -1) {
        fprintf(stderr, "ERROR: impossible to watch inotify %s\n", path);
        close(fd);
        exit(EXIT_FAILURE);
    }

    print_all_stat();

    printf("\nMonitoring %s...\n", path);
    while (1) {
        int i = 0;
        int length = read(fd, buffer, EVENT_BUF_LEN);

        if (length < 0) {
            fprintf(stderr, "ERROR: read error");
            exit(EXIT_FAILURE);
        }

        while (i < length) {
            struct inotify_event *event = (struct inotify_event *) &buffer[i];
            if (event->len) {
                if (event->mask & IN_CREATE) {
                    if (event->mask & IN_ISDIR) {
                        printf("New directory %s created.\n", event->name);
                    } else {
                        printf("New file %s created.\n", event->name);
                    }
                } else if (event->mask & IN_DELETE) {
                    if (event->mask & IN_ISDIR) {
                        printf("Directory %s deleted.\n", event->name);
                    } else {
                        printf("File %s deleted.\n", event->name);
                    }
                } else if (event->mask & IN_MODIFY) {
                    if (event->mask & IN_ISDIR) {
                        printf("Directory %s modified.\n", event->name);
                    } else {
                        printf("File %s modified.\n", event->name);
                    }
                } else if (event->mask & IN_ACCESS) {
                    if (event->mask & IN_ISDIR) {
                        printf("Directory %s accessed.\n", event->name);
                    } else {
                        printf("File %s accessed.\n", event->name);
                    }
                } else if (event->mask & IN_OPEN) {
                    if (event->mask & IN_ISDIR) {
                        printf("Directory %s was opened.\n", event->name);
                    } else {
                        printf("File %s was opened.\n", event->name);
                    }
                } else if (event->mask & IN_ATTRIB) {
                    if (event->mask & IN_ISDIR) {
                        printf("Metadata of directory %s changed.\n", event->name);
                    } else {
                        printf("Metadata of file %s changed.\n", event->name);
                    }
                }
                print_stat(event->name);
            }
            i += EVENT_SIZE + event->len;
        }
    }
}
