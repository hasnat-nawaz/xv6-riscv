#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

void memdump(char *fmt, char *data, int len);

int
main(int argc, char *argv[])
{
  if (argc == 1) {
    // Built-in demos when run with no args: $ memdump
    printf("Example 1:\n");
    int a[2] = {61810, 2026};
    memdump("ii", (char *)a, sizeof(a));

    printf("Example 2:\n");
    memdump("S", "a string", sizeof("a string"));

    printf("Example 3:\n");
    char *s = "another";
    // Pass &s so format 's' reads the pointer value itself
    memdump("s", (char *)&s, sizeof(s));

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;

    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");

    printf("Example 4:\n");
    memdump("pihcS", (char *)&example, sizeof(example));

    printf("Example 5:\n");
    // 's' prints "hello"; next 'c's read bytes of num1 (look like "world")
    memdump("sccccc", (char *)&example, sizeof(example));
  } else if (argc == 2) {
    // Format from argv[1]; payload from stdin until EOF
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while (n < sizeof(data)) {
      int nn = read(0, data + n, sizeof(data) - n);
      if (nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data, n);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

// Print len bytes at data according to fmt:
// i=4B int, p=8B hex, h=2B short, c=1B char,
// s=8B pointer to string, S=string in remaining data
void
memdump(char *fmt, char *data, int len)
{
  int left = len;

  while (*fmt) {
    switch (*fmt) {
    case 'i':
      if (left < 4) {
        printf("memdump: not enough data for 'i'\n");
        return;
      }
      printf("%d\n", *(int *)data);
      data += 4;
      left -= 4;
      break;

    case 'p':
      if (left < 8) {
        printf("memdump: not enough data for 'p'\n");
        return;
      }
      printf("%lx\n", *(uint64 *)data);
      data += 8;
      left -= 8;
      break;

    case 'h':
      if (left < 2) {
        printf("memdump: not enough data for 'h'\n");
        return;
      }
      printf("%d\n", *(short *)data);
      data += 2;
      left -= 2;
      break;

    case 'c':
      if (left < 1) {
        printf("memdump: not enough data for 'c'\n");
        return;
      }
      printf("%c\n", *data);
      data += 1;
      left -= 1;
      break;

    case 's':
      if (left < 8) {
        printf("memdump: not enough data for 's'\n");
        return;
      }
      printf("%s\n", *(char **)data);
      data += 8;
      left -= 8;
      break;

    case 'S':
      // Print until '\0' or end of valid data
      while (left > 0 && *data != '\0') {
        printf("%c", *data);
        data += 1;
        left -= 1;
      }
      printf("\n");
      // Skip the null terminator if it is still inside the buffer
      if (left > 0) {
        data += 1;
        left -= 1;
      }
      break;
    }
    fmt++;
  }
}
