#include "logsink.h"
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <gtest/gtest.h>


int main(int argc, char **argv){
    ::testing::InitGoogleTest(&argc, argv);
    if(wiringPiSetup() == -1){
        std::cout << "Could not complete wiringPiSetup. Errno :: "<< errno << std::endl;
        exit(-1);
   }

    std::cout<<"Setup Passed"<<std::endl;
    LogSink testLogSink("/dev/ttyS0", 115200, "logsink.txt");
   

    testLogSink.start();
    sleep(10);
    testLogSink.stop();
    printf("Ran Successfully\n");

    int result = RUN_ALL_TESTS();

    return result;
}