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
#include <vector>

class Receiver 
{
public:
    bool receiveResponse(
        int sockfd,
        int ttl,
        struct sockaddr_in destination,
        std::vector<int> sentPacketsSeqs
    );
    
private:
    void getDataFromBuffer(uint8_t *buffer, uint16_t *id, uint16_t *seq);
    int selectWrapper(int sockfd, struct timeval *tv);
    void convertBinaryToIp(struct in_addr *recipients, char *dest);
    void printRouter(int ttl, struct in_addr *recipients, int receivedMesssages, float waitTime);
};
