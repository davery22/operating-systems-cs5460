#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE 5

int nums[SIZE] = {0, 1, 2, 3, 4};

int main()
{
  int i;
  pid_t pid;
  int x = 3;

  pid = fork();

  if (pid == 0) {
    for (i = 0; i < SIZE; i++) {
      nums[i] *= -i;
      printf("CHILD: %d ", nums[i]); /* LINE X */
    }
    x += 1;
    printf("x: %d ", x);
  } else if (pid > 0) {
    wait(NULL);
    printf("x: %d ", x);
    for (i = 0; i < SIZE; i++)
      printf("PARENT: %d ", nums[i]); /* LINE Y */
  }

  return 0;
}
