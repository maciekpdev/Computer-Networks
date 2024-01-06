// Maciej Pietrewicz 325022

#include "receiver.hpp"

#include <algorithm>

#define ICMP_COUNT 3

void Receiver::convertBinaryToIp(struct in_addr *recipients, char *dest)
{
    inet_ntop(AF_INET, recipients, dest, 100);
}

void Receiver::printRouter(int ttl, struct in_addr *recipients, int receivedMesssages, float waitTime)
{
    char ips[100];
    std::cout << ttl << ". ";
    if (receivedMesssages == 0)
    {
        std::cout << "*\n";
        return;
    }
    else if (receivedMesssages < ICMP_COUNT)
    {
        convertBinaryToIp(recipients, ips);

        if(ICMP_COUNT == 2)
        {
            if(recipients[0].s_addr != recipients[1].s_addr)
                convertBinaryToIp(recipients + 1, ips);  

        }
        std::cout << ips;

        std::cout <<" ???\n";
        return;
    }

    if (recipients[0].s_addr == recipients[1].s_addr)
    {
        convertBinaryToIp(recipients, ips);
    }
    else
    {
        convertBinaryToIp(recipients, ips);
        convertBinaryToIp(recipients + 1, ips);
    }

    if (recipients[2].s_addr != recipients[0].s_addr && recipients[2].s_addr != recipients[0].s_addr)
    {
        convertBinaryToIp(recipients + 2, ips);
    }

    std::cout << ips;
    printf(" %.2fms\n", waitTime);
}

void Receiver::getDataFromBuffer(uint8_t *buffer, uint16_t *id, uint16_t *seq)
{
    struct ip *ipHeader = (struct ip *)buffer;
    ssize_t offset = sizeof(int32_t) * ipHeader->ip_hl;
    struct icmp* icmpInfo = (struct icmp *)(buffer + offset);

    switch (icmpInfo->icmp_type)
    {
        case ICMP_ECHOREPLY:
            *seq = icmpInfo->icmp_seq;
            *id = icmpInfo->icmp_id;
            break;

        case ICMP_TIMXCEED:
            offset += 8;
            ipHeader = (struct ip *)(buffer + offset);
            offset += sizeof(int32_t) * ipHeader->ip_hl;
            icmpInfo = (struct icmp *)(buffer + offset);
            *seq = icmpInfo->icmp_seq;
            *id = icmpInfo->icmp_id;
            break;

        default:
            return;
    }
}

int Receiver::selectWrapper(int sockfd, struct timeval *tv)
{
    fd_set descriptors;
    FD_ZERO(&descriptors);
    FD_SET(sockfd, &descriptors);
    int retval = select(sockfd + 1, &descriptors, NULL, NULL, tv);

    if (retval < 0)
    {
        std::cerr << "Select error";
        exit(EXIT_FAILURE);
    }
    return retval;
}

bool Receiver::receiveResponse(
    int sockfd,
    int ttl,
    struct sockaddr_in destination,
    std::vector<int> sentPacketsSeqs
)
{
    struct sockaddr_in source;
    float receiveTime = 0;
    struct timeval tv = {1, 0};
    uint8_t buffer[IP_MAXPACKET];
    int receivedMessages = 0, ready, destCount = 0;
    struct in_addr answeredIps[ICMP_COUNT];
    uint16_t my_pid = getpid(), seq = 0, id = 0;

    while ((ready = selectWrapper(sockfd, &tv)))
    {
        socklen_t sender_len = sizeof(source);
        ssize_t packet_len = recvfrom(
            sockfd,
            buffer,
            IP_MAXPACKET,
            0,
            (struct sockaddr *)&source,
            &sender_len
        );

        if (packet_len == -1)
        {
            std::cerr << "Error while recieving a packet\n";
            exit(EXIT_FAILURE);
        }

        getDataFromBuffer(buffer, &id, &seq);

        bool correctSeq = false;
        int i = 0;
        for(const auto& seq2: sentPacketsSeqs)
        {
            if(seq == seq2)
            {
                correctSeq = true;
                sentPacketsSeqs.erase(
                    std::remove(sentPacketsSeqs.begin(), sentPacketsSeqs.end(), seq),
                    sentPacketsSeqs.end()
                );
                sentPacketsSeqs.shrink_to_fit();
            }
            i++;
        
        }

        if (correctSeq && my_pid == id)
        {
            answeredIps[receivedMessages++] = source.sin_addr;
            if (source.sin_addr.s_addr == destination.sin_addr.s_addr)
            {
                destCount++;
            }
            receiveTime += (1000000 - tv.tv_usec) / 1000.0;

            if (sentPacketsSeqs.size() == 0)
                break;
        }
    }
    printRouter(ttl, answeredIps, receivedMessages, receiveTime / 3);
    if(destCount)
        return true;
    return false;
}