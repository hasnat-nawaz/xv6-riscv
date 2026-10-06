#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// Scan one input stream (file or stdin) for numbers that are multiples of 5 or 6.
// A number is digits bounded by separators in " -\r\t\n./," (also SOF/EOF).
void
sixfive(int fd)
{
  char n;
  int isNum = 0;    // 1 if current digit run started after a separator
  int isEmpty = 1;  // 1 if we are not holding a number
  int value = 0;

  while (read(fd, &n, 1) > 0) {
    if (strchr(" -\r\t\n./,", n)) {
      // Separator ends a number — print if it is a multiple of 5 or 6
      if (!isEmpty && isNum && (value % 5 == 0 || value % 6 == 0)) {
        printf("%d\n", value);
        isNum = 0;
      } else {
        isNum = 1;  // next digits may start a new number
      }
      isEmpty = 1;
      value = 0;
    } else if (n >= '0' && n <= '9') {
      // Digits after a letter (e.g. xv6) are not a valid number
      if (!isEmpty && !isNum) {
        continue;
      } else {
        value = value * 10 + (n - '0');
        isNum = 1;
        isEmpty = 0;
      }
    } else {
      // Non-separator, non-digit: poison the current run
      isEmpty = 0;
      isNum = 0;
      value = 0;
    }
  }

  // EOF is an implicit separator — flush a trailing number
  if (!isEmpty && isNum && (value % 5 == 0 || value % 6 == 0))
    printf("%d\n", value);
}

int
main(int argc, char *argv[])
{
  int fd, i;

  // No filenames → read stdin (fd 0)
  if (argc <= 1) {
    sixfive(0);
    exit(0);
  }

  // Otherwise open each file and process it
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
