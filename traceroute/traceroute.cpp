// Maciej Pietrewicz 325022

#include "sender.hpp"
#include "receiver.hpp"

#include <iostream>
#include <stdio.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/ip_icmp.h>
#include <assert.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>
#include <utility>
#include <vector>

#define MAX_TTL 30
#define ICMP_COUNT 3

struct sockaddr_in getSockAddrFromIp(const char *ip)
{
    struct sockaddr_in sock;
    bzero(&sock, sizeof(sock));
    sock.sin_family = AF_INET;
    if (!inet_pton(AF_INET, ip, &sock.sin_addr))
    {
        std::cerr << "Invalid ip\n";
        exit(EXIT_FAILURE);
    }
    return sock;
}

void traceroute(const char *destinationIp)
{
    struct sockaddr_in destination = getSockAddrFromIp(destinationIp);
    int sockfd = 0;

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0)
    {
        std::cerr << "Socket error\n";
        exit(EXIT_FAILURE);
    }

    Sender sender;
    Receiver receiver;
    std::vector<int> sentPacketsSeqs;

    for (int ttl = 1; ttl <= MAX_TTL; ++ttl)
    {
        for (int i = 0; i < ICMP_COUNT; i++)
        {
            sentPacketsSeqs.push_back(
                sender.sendIcmpPacket(sockfd, destination, ttl, (ttl*999) * (i+1))
            );
        }

        if (receiver.receiveResponse(sockfd, ttl, destination, sentPacketsSeqs))
            break;

        sentPacketsSeqs.clear();
    }
}

bool isArgValid(int numberOfArgs)
{
    if (numberOfArgs != 2)
    {
        std::cerr << "Wrong number of arguments \n";
        return false;
    }

    return true;
}

int main(int numberOfArgs, char *args[])
{
    if (!isArgValid(numberOfArgs))
        return EXIT_FAILURE;

    std::cout << "traceroute to " << args[1] << "," <<  MAX_TTL << " hops max\n";
    traceroute(args[1]);

    return EXIT_SUCCESS;
}