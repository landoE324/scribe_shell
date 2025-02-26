#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <dirent.h>
#define MAX_INPUT_SIZE 1024

void display_help() {
    printf("Available Shell Commands:\n");
    printf("help     - Display this help message\n");
    printf("exit     - Exit the shell\n");
    printf("cd       - Change the current directory\n");
    printf("ls       - List the files in the current directory\n");
    printf("pwd      - Print the working directory\n");
}

// Function to list the files in the current directory
void list_files() {
    DIR *dir;
    struct dirent *entry;
    dir = opendir(".");
    if (dir == NULL) {
        perror("opendir");
        return;
    }
    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }
    closedir(dir);
}


// Function to change the current directory
void change_directory(char *path) {
    if (path == NULL) {
        fprintf(stderr, "cdP missing argument\n");
        return;
    }
    if (chdir(path) != 0) {
        perror("cd");
    }
}


// Function to print the current directory
void print_working_directory() {
    char cwd[MAX_INPUT_SIZE];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("getcwd");
    }
}


int main() {
    char input[MAX_INPUT_SIZE];
    while (1) {
        printf("scribe> "); // Prompt for shell
        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
            break; // Exit on end of file
        }
    
        input[strcspn(input, "\n")] = '\0'; //Removes new line

        // Splitting input into command and arguments
        char *args[MAX_INPUT_SIZE];
        char *token = strtok(input, " ");
        int i = 0;
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL; // Null-terminate the argument list

        // Built-in command handlers
        if (args[0] == NULL) {
        } else if (strcmp(args[0], "help") == 0) {
            display_help();
        } else if (strcmp(args[0], "exit") == 0) {
            printf("Exiting shell...\n");
            break;
        } else if (strcmp(args[0], "ls") == 0) {
            list_files();
        } else if (strcmp(args[0], "cd") == 0) {
            change_directory(args[1]);
        } else if (strcmp(args[0], "pwd") == 0) {
            print_working_directory();
        } else {
            // Execute external commands
            pid_t pid = fork();
            if (pid == 0) {
                // Child process
                if (execvp(args[0], args) == -1) {
                    perror("execvp");
                    exit(EXIT_FAILURE);
                }
            } else if (pid > 0) {
                // Parent process
                wait(NULL); // Wait for child to finish
            } else {
                perror("fork");
            }
        }
    }
    return 0;
}