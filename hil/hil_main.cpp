#include "logsink.h"
#include <iostream>
#include <unistd.h>
#include <ctime>


int main(){

    if(wiringPiSetup() == -1){
        std::cout << "Could not complete wiringPiSetup. Errno :: "<< errno << std::endl;
        exit(-1);
   } else{
        std::cout<<"Setup Passed"<<std::endl;
        LogSink testLogSink("/dev/ttyS0", 115200, "logsink.txt");
   }

    testLogSink.start();
    sleep(60);
    testLogSink.stop();
    printf("Ran Successfully\n");
    return 0;
}