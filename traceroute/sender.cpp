// Maciej Pietrewicz 325022

#include "sender.hpp"

uint16_t Sender::computeIcmpChecksum(const uint16_t *buff, int length)
{
    uint32_t sum;
    const uint16_t *ptr = buff;
    assert(length % 2 == 0);
    for (sum = 0; length > 0; length -= 2)
        sum += *ptr++;
    sum = (sum >> 16) + (sum & 0xffff);
    return (uint16_t)(~(sum + (sum >> 16)));
}

int Sender::sendIcmpPacket(int sockfd, struct sockaddr_in destination, int ttl, int seq)
{
    struct icmp header;

    header.icmp_type = ICMP_ECHO;
    header.icmp_code = 0;
    header.icmp_id = (uint16_t)getpid();
    header.icmp_seq = seq;
    header.icmp_cksum = 0;
    header.icmp_cksum = computeIcmpChecksum(
    (uint16_t *)&header, sizeof(header));

    setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));

    ssize_t bytes = sendto(
        sockfd,
        &header,
        sizeof(header),
        0,
        (struct sockaddr *)&destination,
        sizeof(destination));
    if (bytes == -1)
    {
        fprintf(stderr, "Failing while sending ICMP");
        exit(EXIT_FAILURE);
    }

    return seq;
}