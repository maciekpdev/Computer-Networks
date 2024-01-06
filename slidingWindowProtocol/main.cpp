// Maciej Pietrewicz 325022

#include "Transport.hpp"

#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>

struct sockaddr_in getServerInfo(int numberOfArgs, char *args[])
{
  struct sockaddr_in serverAddress;

  serverAddress.sin_family = AF_INET;
  if (!inet_pton(AF_INET, args[1], &serverAddress.sin_addr))
  {
    std::cerr << "wrong ip address\n";
    exit(EXIT_FAILURE);
  }
  serverAddress.sin_port = htons(atoi(args[2]));
  if (serverAddress.sin_port == 0)
  {
    std::cerr << "invalid port\n";
    exit(EXIT_FAILURE);
  }

  return serverAddress;
}

size_t getFileLen(int numberOfArgs, char *args[])
{
  size_t lenOfFile = atoi(args[4]);
  if (lenOfFile == 0)
  {
    std::cout << "Length of file must be longer than 0\n";
    exit(EXIT_FAILURE);
  }

  return lenOfFile;
}

int main(int numberOfArgs, char *args[])
{
  if (numberOfArgs != 5)
  {
    std::cout << "Error parsing command line arguments. \n"
              << "Usage: ./transport-client-fast ip port output_file bytes\n";
    return EXIT_FAILURE;
  }

  struct sockaddr_in serverAddress = getServerInfo(numberOfArgs, args);
  size_t lenOfFile = getFileLen(numberOfArgs, args);
  Transport transport(args[3]);

  transport.run(serverAddress, lenOfFile);

  return EXIT_SUCCESS;
}