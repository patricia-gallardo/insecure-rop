#include <string.h>

int main(int argc, char **argv) {
  char buff[32];

  if (argc >= 2)
    strcpy(buff, argv[1]);

  return (0);
}