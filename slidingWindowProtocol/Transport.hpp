#include "TimeoutManager.hpp"
#include "Window.hpp"

#include <iostream>

class Transport {
    public:
        Transport(const char *fileName);
        ~Transport();
        void run(struct sockaddr_in serverAddress, size_t lenOfFile);
        
    private:
        void askServerForData(struct sockaddr_in serverAddress, size_t sumOfbytesReceived, size_t bytesToSend);
        void sendMessage(struct sockaddr_in serverAddress, size_t startPos, size_t len);
        void writeToFile(size_t &sumOfbytesReceived, size_t &remainingBytes, size_t lenOfFile);
        size_t receiveMessage(char *buffer, struct sockaddr_in serverAddress);

        int sockfd;
        FILE *filePtr;
        Window window;
};