#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <positive_int>\n", argv[0]);
        fprintf(stderr, "Invalid arg count: Expected %d got %d\n", 1, argc-1);
        return 1;
    }

    long start = strtol(argv[1], NULL, 10);
    if (start < 1) {
        fprintf(stderr, "Usage: %s <positive_int>\n", argv[0]);
        fprintf(stderr, "%s is not a positive integer\n", argv[1]);
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        fprintf(stderr, "Fork failed\n");
        return 1;
    }

    if (pid == 0) { // CHILD -- print collatz sequence
        printf("%ld", start);
        while (start != 1) {
            start = (start & 1) ? start*3+1 : start/2;
            printf(", %ld", start);
        }
        printf("\n");
    }
    else { // PARENT
        if (wait(NULL) == -1) {
            fprintf(stderr, "Wait failed\n");
            return 1;
        }
    }

    return 0;
}
