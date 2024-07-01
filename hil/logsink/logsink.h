#include <semaphore>
#include <queue>
#include <thread>
#include <string>
#include <fstream>
#include <wiringPi.h>
#include <mutex>

class LogSink {

    private:
    int m_serialFd;
    std::ofstream m_outputFile;
    std::queue <std::string> m_logQueue;
    std::mutex m_queueLock;
    bool m_isRunning;
    std::counting_semaphore<255> m_queueSemaphore{0};
    std::thread m_readThread;
    std::thread m_writeThread;
    
    void uartReadThread();
    void writeFileThread();
    

    public:
    LogSink(std::string uartPort, int baudRate, std::string outputFile);
    ~LogSink();
    int start();
    void stop();

};