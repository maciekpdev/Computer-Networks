// Maciej Pietrewicz 325022

#include "TimeoutManager.hpp"

#include <poll.h>
#include <iostream>

void TimeoutManager::startTimer(int timeout_){
  clock_gettime(CLOCK_REALTIME, &start);
  timeout = timeout_;
}

int TimeoutManager::waitOrTimeout(int sockfd)
{
  clock_gettime(CLOCK_REALTIME, &finish);

  timeout -= (finish.tv_sec - start.tv_sec) * 1000;
  timeout -= (finish.tv_nsec - start.tv_nsec) / 1000000;

  if (timeout < 0)
  {
    timeout = 0;
    return 0;
  }

  struct pollfd fds = { sockfd, POLLIN, 0 };

  int result = poll(&fds, 1, timeout);

  if (result == -1)
  {
    std::cerr <<  "poll error\n";
    exit(EXIT_FAILURE);
  }
  else if (result == 0)
  {
    timeout = 0;
    return result;
  }

  return result;
  
}