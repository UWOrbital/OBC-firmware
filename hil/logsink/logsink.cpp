#include "logsink.h"
#include <errno.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <poll.h>
#include <iostream>
#include <ctime>


LogSink::LogSink(std::string uartPort, int baudRate, std::string outputFile){
    m_outputFile.open(outputFile);
    if(!m_outputFile.is_open()){
        std::cerr<<"Could Not Open File. Errno "<<errno<<std::endl;
        return;
    }
    m_serialFd = serialOpen(uartPort.c_str(), baudRate);
    if(m_serialFd == -1){
        std::cerr<<"Could not open UART Port. Errno "<<errno<<std::endl;
        return;
    }
    m_isRunning = false;

}
LogSink::~LogSink(){
    if(m_isRunning){
        stop();
        m_readThread.join();
        m_writeThread.join();
    }
    serialClose(m_serialFd);
    m_outputFile.close();
}
void LogSink::uartReadThread(){

    std::string buffer = "";
    struct pollfd uartWait;
    uartWait.fd = m_serialFd;
    uartWait.events = POLLIN;

    time_t rawtime;
    struct tm * timeinfo;
    char timer [80];
    std::string times;


    while(m_isRunning){
        int ret = poll(&uartWait, 1, -1);
        if(ret == 1){

            int bytesAvailable = serialDataAvail(m_serialFd);
            if(bytesAvailable < 0){
                printf("Couldn't Read Data, Errno %d\n", errno);
                exit(-1);
            }
            else if(bytesAvailable>=0){
                for(int i = 0; i<bytesAvailable; i++){
                    char character= serialGetchar(m_serialFd);
                    buffer += character;
                    if(character == '\n'){
                        time(&rawtime);
                        timeinfo = localtime(&rawtime);
                        strftime (timer, 80, "[%r]", timeinfo);    
                        times = timer;
                        buffer = "["+times+"] "+buffer;

                        m_queueLock.lock();
                        m_logQueue.push(buffer);
                        m_queueLock.unlock();
                        buffer = "";
                        m_queueSemaphore.release();
                    }
                }
    
            }
        }
    }

   m_queueSemaphore.release(); 
}
void LogSink::writeFileThread(){

    while(m_isRunning){
        m_queueSemaphore.acquire();
        
        if(!m_logQueue.empty()){
            m_queueLock.lock();
            std::string temp = m_logQueue.front();
            m_logQueue.pop();
            m_queueLock.unlock();
            //std::cout<<temp;
            m_outputFile<<temp;
        }
    }

}
int LogSink::start(){
    m_isRunning = true;
    m_readThread = std::thread(&LogSink::uartReadThread, this);
    m_writeThread = std::thread(&LogSink::writeFileThread, this);

    return 0;
}
void LogSink::stop(){
    m_isRunning = false;
    m_readThread.join();
    m_writeThread.join();
}