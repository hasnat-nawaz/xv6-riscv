#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

// Run "cmd ... path" for a match found by -exec
void
runcmd(char *path, int cmdc, char **cmdv)
{
  char *args[MAXARG];
  int i;
  int pid;

  // need room for command args + matched path + NULL
  if (cmdc + 2 > MAXARG) {
    fprintf(2, "find: too many arguments\n");
    return;
  }

  for (i = 0; i < cmdc; i++)
    args[i] = cmdv[i];
  args[cmdc] = path;
  args[cmdc + 1] = 0;

  pid = fork();
  if (pid < 0) {
    fprintf(2, "find: fork failed\n");
    return;
  }

  if (pid == 0) {
    exec(args[0], args);
    fprintf(2, "find: exec %s failed\n", args[0]);
    exit(1);
  }
  
  if (wait(0) < 0)
  fprintf(2, "find: wait failed\n");
}

// Recursively search under path for entries named `name`.
// If cmdc == 0, print matches; otherwise run cmdv via -exec.
void
find(char *path, char *name, int cmdc, char **cmdv)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if (st.type == T_DIR) {
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
      fprintf(2, "find: path too long\n");
      close(fd);
      return;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
      if (de.inum == 0)
        continue;

      // dirent names are fixed-width; force a C string
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;

      // avoid infinite recursion on . and ..
      if (strcmp(p, ".") == 0 || strcmp(p, "..") == 0)
        continue;

      if (stat(buf, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", buf);
        continue;
      }

      if (strcmp(p, name) == 0) {
        if (cmdc == 0)
          printf("%s\n", buf);
        else
          runcmd(buf, cmdc, cmdv);
      }

      // descend into subdirectories
      if (st.type == T_DIR)
        find(buf, name, cmdc, cmdv);
    }
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  // find <dir> <name>
  if (argc == 3) {
    find(argv[1], argv[2], 0, 0);
  } else if (argc >= 5 && strcmp(argv[3], "-exec") == 0) {
    // find <dir> <name> -exec <cmd> [args...]
    find(argv[1], argv[2], argc - 4, argv + 4);
  } else {
    fprintf(2, "usage: find directory name [-exec cmd ...]\n");
    exit(1);
  }
  exit(0);
}
