#include <cstdint>
#include <ctime>
#include <iostream>

#include <stddef.h>
#include <unistd.h>

#include "psycal.h"

int main(int argc, char *argv[]) {
  int opt = 0;

  struct session {
    bool foreground;
    std::int64_t timestamp;
  } session = {0};

  while ((opt = getopt(argc, argv, "df")) != -1) {
    switch (opt) {
    case 'd':
      session.foreground = false;
      break;
    case 'f':
      session.foreground = true;
      break;
    case 't':
      {
        std::tm tm = {0};
        char* try1 = ::strptime(optarg, "%Hh%m", &tm);
        // try to find the current time + the above

        char* try2 = ::strptime(optarg, "%Y-%m-%dT%Hh%m", &tm);
        if (try2 == NULL) {
          std::cerr << "bad time format" << std::endl;
        }
      }
      break;
    default: /* '?' */
      std::cout << "usage" << std::endl;
      return (EXIT_FAILURE);
    }
  }

  return 0;
}
