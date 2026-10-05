#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

void
sixfive(int fd)
{
  char n;
  int isNum = 0;
  int isEmpty = 1;
  int value = 0;

  while (read(fd, &n, 1) > 0) {
    if (strchr(" -\r\t\n./,", n)) {
      if (!isEmpty && isNum && (value % 5 == 0 || value % 6 == 0)) {
        printf("%d\n", value);
        isNum = 0;
      } else {
        isNum = 1;
      }
      isEmpty = 1;
      value = 0;
    } else if (n >= '0' && n <= '9') {
      if (!isEmpty && !isNum) {
        continue;
      } else {
        value = value * 10 + (n - '0');
        isNum = 1;
        isEmpty = 0;
      }
    } else {
      isEmpty = 0;
      isNum = 0;
      value = 0;
    }
  }

  // EOF is an implicit separator
  if (!isEmpty && isNum && (value % 5 == 0 || value % 6 == 0))
    printf("%d\n", value);
}

int
main(int argc, char *argv[])
{
  int fd, i;

  if (argc <= 1) {
    sixfive(0);
    exit(0);
  }

  for (i = 1; i < argc; i++) {
    if ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "sixfive: cannot open %s\n", argv[i]);
      exit(1);
    }
    sixfive(fd);
    close(fd);
  }
  exit(0);
}
