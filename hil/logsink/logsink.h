#include <semaphore>
#include <queue>
#include <thread>
#include <string>
#include <fstream>
#include <wiringPi.h>
#include <mutex>
#define XSTR(x) STR(x)
#define STR(x) #x

#pragma message "CPP STD " XSTR(__cplusplus)

class LogSink {
 private:
  int m_serialFd;
  std::ofstream m_outputFile;
  std::queue<std::string> m_logQueue;
  std::mutex m_queueLock;
  bool m_isRunning;
  std::counting_semaphore<255> m_queueSemaphore{0};
  std::thread m_readThread;
  std::thread m_writeThread;

  void uartReadThread();
  void writeFileThread();

 public:
  std::string m_fileName;
  LogSink(std::string uartPort, int baudRate, std::string outputFile);
  bool isRunning() const;
  ~LogSink();
  int start();
  void stop();
};
