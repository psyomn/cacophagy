#include <stdio.h>

int main(int argc, char *argv[]) {
  const char* str = "hello how are you today";
  int strfd = fmemopen(str, sizeof(str), "r");
  int switcheroo = dup2(stdin, strfd);

  char c = 0;
  while ((c = getchar())) {
    if (c == EOF) break;
    if (c == 0) break;
    printf("%c.", c);
  }

  printf("\n");

  return 0;
}
