#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// Print numbers in fd that are multiples of 5 or 6.
// A number is a run of digits bounded by separators in " -\r\t\n./,".
// Start and end of the stream also count as separators.
void
sixfive(int fd)
{
  char c;
  int in_num;   // 1 while a valid digit run is being built
  int poisoned; // 1 if a letter broke the current run (e.g. "xv6")
  int value;
  int n;

  in_num = 0;
  poisoned = 0;
  value = 0;

  while ((n = read(fd, &c, 1)) > 0) {
    if (strchr(" -\r\t\n./,", c)) {
      // A separator ends the current number.
      if (in_num && !poisoned && (value % 5 == 0 || value % 6 == 0))
        printf("%d\n", value);
      in_num = 0;
      poisoned = 0;
      value = 0;
    } else if (c >= '0' && c <= '9') {
      // Digits after a letter are not a number.
      if (poisoned)
        continue;
      value = value * 10 + (c - '0');
      in_num = 1;
    } else {
      // Non-digit, non-separator: this run is not a number.
      poisoned = 1;
      in_num = 0;
      value = 0;
    }
  }
  if (n < 0) {
    fprintf(2, "sixfive: read error\n");
    exit(1);
  }

  // EOF is an implicit separator.
  if (in_num && !poisoned && (value % 5 == 0 || value % 6 == 0))
    printf("%d\n", value);
}

int
main(int argc, char *argv[])
{
  int fd, i;

  // No file names: read standard input.
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
