#include "kernel/types.h"
#include "user/user.h"

// Pause for a user-specified number of timer ticks.
// Usage: sleep <ticks>
int
main(int argc, char *argv[])
{
  char *p;
  int ticks;

  if (argc < 2) {
    fprintf(2, "Usage: sleep ticks\n");
    exit(1);
  }

  // Command-line args arrive as strings; atoi would silently
  // turn a non-numeric one into 0, so reject it here.
  for (p = argv[1]; *p; p++) {
    if (*p < '0' || *p > '9') {
      fprintf(2, "sleep: %s is not a number of ticks\n", argv[1]);
      exit(1);
    }
  }

  ticks = atoi(argv[1]);
  if (pause(ticks) < 0) {
    fprintf(2, "sleep: pause failed\n");
    exit(1);
  }
  exit(0);
}
