#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int f(int x) {
    return x * x;
}

int g(int x) {
    return x + 1;
}


int main() {
    int x;
    printf("Enter a value for x: ");
    scanf("%d", &x);

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid_f = fork();

    if (pid_f == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid_f == 0) { 
        close(pipefd[0]);

        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        int result_f = f(x);
        printf("%d", result_f);
        exit(EXIT_SUCCESS);
    } else { 
        pid_t pid_g = fork();

        if (pid_g == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid_g == 0) { 
            close(pipefd[1]);

            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);

            int result_g;
            scanf("%d", &result_g);

            int result = result_g * g(x);
            printf("%d", result);
            exit(EXIT_SUCCESS);
        } else { 
            close(pipefd[0]);
            close(pipefd[1]);

            // wait(NULL);
            // wait(NULL);
        }
    }

    return 0;
}
