#include <iostream>
#include <stdio.h>
#include <errno.h>
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
#include <stdint.h>
#include <utility>

class Sender
{
public:
    int sendIcmpPacket(int sockfd, struct sockaddr_in destination, int ttl, int seq);
    
private:
    uint16_t computeIcmpChecksum(const uint16_t *buff, int length);

};