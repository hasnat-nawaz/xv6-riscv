#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  // Need a tick count: sleep <ticks>
  if (argc < 2) {
    fprintf(2, "Usage: sleep ticks\n");
    exit(1);
  }

  // argv[1] is a string; atoi turns it into an int
  // pause() sleeps for that many kernel timer ticks
  pause(atoi(argv[1]));
  exit(0);
}
