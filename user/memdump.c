#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

void memdump(char *, char *, int);

int
main(int argc, char *argv[])
{
  if (argc == 1) {
    // Built-in examples used when memdump is run with no arguments.
    printf("Example 1:\n");
    int a[2] = {61810, 2026};
    memdump("ii", (char *)a, sizeof(a));

    printf("Example 2:\n");
    memdump("S", "a string", sizeof("a string"));

    printf("Example 3:\n");
    char *s = "another";
    // &s: format 's' reads the pointer value, not the string bytes.
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
    memdump("sccccc", (char *)&example, sizeof(example));
  } else if (argc == 2) {
    // Format string is argv[1]; bytes come from standard input.
    char data[512];
    int n, nn;

    n = 0;
    memset(data, 0, sizeof(data));
    while (n < sizeof(data)) {
      nn = read(0, data + n, sizeof(data) - n);
      if (nn < 0) {
        fprintf(2, "memdump: read error\n");
        exit(1);
      }
      if (nn == 0)
        break;
      n += nn;
    }
    memdump(argv[1], data, n);
  } else {
    fprintf(2, "Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

// Dump len bytes at data.
// i: 4-byte decimal, p: 8-byte hex, h: 2-byte decimal, c: 1 byte,
// s: 8-byte pointer to a string, S: string in the remaining bytes.
void
memdump(char *fmt, char *data, int len)
{
  int left;

  left = len;
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
      // Stop at the null byte or at the end of the valid data.
      while (left > 0 && *data != '\0') {
        printf("%c", *data);
        data += 1;
        left -= 1;
      }
      printf("\n");
      if (left > 0) {
        data += 1;
        left -= 1;
      }
      break;
    }
    fmt++;
  }
}
