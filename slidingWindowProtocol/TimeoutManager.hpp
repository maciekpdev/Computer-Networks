#include <stddef.h>
#include <time.h>

class TimeoutManager {
    public:
        int waitOrTimeout(int sockfd);
        void startTimer(int timeout_);
    private:
        struct timespec start, finish;
        int timeout;
};
