// Maciej Pietrewicz 325022

#include "Transport.hpp"

#include <iostream>
#include <algorithm>
#include <netinet/ip.h>
#include <cstring>

#define MAX_LEN_OF_MSG 1000
#define LEN_OF_HEADER 40

void Transport::sendMessage(struct sockaddr_in serverAddress, size_t startPos, size_t len)
{
  char message[20];
  sprintf(message, "GET %ld %ld\n", startPos, len);
  if (!sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&serverAddress, sizeof(serverAddress))) {
    std::cerr <<  "sendto error\n";
    exit(EXIT_FAILURE);
  }
}


void Transport::askServerForData(struct sockaddr_in serverAddress, size_t sumOfbytesReceived, size_t bytesToSend)
{
  int wIdx = window.startIdx;
  for (int i = 0; i < window.size && i * MAX_LEN_OF_MSG < bytesToSend; i++)
  {
    if (!window.isSegmentReceived[wIdx])
    {
      size_t bytesOfRequest = std::min((size_t)MAX_LEN_OF_MSG, bytesToSend - i * MAX_LEN_OF_MSG);
      sendMessage(serverAddress, sumOfbytesReceived + i * MAX_LEN_OF_MSG, bytesOfRequest);
    }
    wIdx = (wIdx + 1) % window.size;
  }
}

void Transport::writeToFile(size_t &sumOfbytesReceived, size_t &remainingBytes, size_t lenOfFile)
{
  while (window.isSegmentReceived[window.startIdx] && remainingBytes > 0)
  {
    size_t writtenBytesLen = std::min((size_t)MAX_LEN_OF_MSG, remainingBytes);
    if(fwrite(window.segmentData[window.startIdx], sizeof(char), writtenBytesLen, filePtr) < writtenBytesLen)
    {
      std::cerr << "fwrite error\n";
    }
    sumOfbytesReceived += writtenBytesLen;
    remainingBytes -= writtenBytesLen;
    window.shift();
    const float progress = 100.0 * ((float)(sumOfbytesReceived) / (float)(lenOfFile));
    std::cout <<  progress << "%\n";
  }
}

size_t Transport::receiveMessage(char *buffer, struct sockaddr_in serverAddress)
{
  struct sockaddr_in sender;
  socklen_t senderLen = sizeof(sender);
  memset(buffer, 0, LEN_OF_HEADER + MAX_LEN_OF_MSG);
  memset(&sender, 0, senderLen);
  size_t receivedBytes = recvfrom(sockfd, buffer, IP_MAXPACKET, 0,
                                 (struct sockaddr *)&sender, &senderLen);
  if (receivedBytes < 0)
  {
    std::cerr << "recvfrom error\n";
    exit(EXIT_FAILURE);
  }

  if (
    sender.sin_addr.s_addr == serverAddress.sin_addr.s_addr && 
    sender.sin_port == serverAddress.sin_port
  )
    return receivedBytes;
   
  return 0;
}

FILE* getFilePtrToWrite(const char *fileName){
  FILE *filePtr = fopen(fileName, "w");
  if (!filePtr)
  {
    std::cerr << "fopen error\n";
    exit(EXIT_FAILURE);
  }

  return filePtr;
}

int getSocket()
{
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
  {
    std::cerr << "socket error\n";
    exit(EXIT_FAILURE);
  }
  return sockfd;
}

Transport::Transport(const char *fileName):
  window{Window(1000, MAX_LEN_OF_MSG)},
  sockfd{getSocket()},
  filePtr{getFilePtrToWrite(fileName)} 
{
}

Transport::~Transport()
{
  fclose(filePtr);
}

void Transport::run(struct sockaddr_in serverAddress, size_t lenOfFile)
{
  size_t recivedStartPos, recivedLen, sumOfbytesReceived = 0, remainingBytes = lenOfFile;
  char buffer[MAX_LEN_OF_MSG + LEN_OF_HEADER];
  int wIdx;
  TimeoutManager tm;

  while (remainingBytes)
  {
    askServerForData(serverAddress,sumOfbytesReceived, remainingBytes);
    tm.startTimer(500);
    while (tm.waitOrTimeout(sockfd))
    {
      size_t receivedBytes = receiveMessage(buffer, serverAddress);
      if (
        receivedBytes == 0 || 
        !sscanf(buffer, "DATA %ld %ld\n", &recivedStartPos, &recivedLen) || 
        recivedStartPos < sumOfbytesReceived ||
        recivedStartPos >= sumOfbytesReceived + window.size * MAX_LEN_OF_MSG
      )
        continue;
      
      wIdx = (((recivedStartPos - sumOfbytesReceived) / MAX_LEN_OF_MSG) + window.startIdx) % window.size;
      auto buffer_begin = buffer + receivedBytes - recivedLen;
      window.setData(buffer_begin, recivedLen, wIdx); 
    }
    writeToFile(sumOfbytesReceived, remainingBytes, lenOfFile);
    
  }
}
