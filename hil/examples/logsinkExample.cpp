#include "logsink.h"
#include <iostream>
#include <unistd.h>
#include <gtest/gtest.h>
#include <fstream>
#include <string>

#define minLogsAquired 7
#define specificLog "Firmware/OBC-firmware/obc/app/drivers/sdcard/sdc_bdev.c:416 - 108"
#define logFile "logsink.txt"
#define timeperiod 10

LogSink logger("/dev/ttyS0", 115200, "logsink.txt");

TEST(LogIntegrationTest, Generated_Logs) {
    // Did it Read?
    std::ifstream read;
    read.open(logFile);
    ASSERT_TRUE(read.is_open());
    char temp = '_';
    read>>temp;
    ASSERT_TRUE(temp != ' ' && temp != '\n');
    read.close();
}

TEST(LogIntegrationTest, Number_Logs) {
    // Did it read expected times?
    std::ifstream read;
    read.open(logFile);
    char buff[256];
    int count;
    while(read.getline(buff,256)){
        count++;
    }
    ASSERT_TRUE(count>minLogsAquired) << "Required: "<<minLogsAquired<<" Iterated: "<<count;
    read.close();
}

TEST(LogIntegrationTest, Speific_Logs) {
    // Did it read a specific log?

    std::ifstream read;
    read.open(logFile);
    char buffarr[256];
    std::string buffstr;
    int count=0;
    bool found = false;
    while(read.getline(buffarr,256)){
        buffstr = buffarr;
        
        if(buffstr.find(specificLog) != std::string::npos){
            found = true;
            break;
        }
        count++;
    }
    ASSERT_TRUE(found) <<"Iterated "<<count<<" times!";
    read.close();
}

TEST(LogIntegrationTest, TimeFrame_Logs){
    //Did it read all logs within a specific time frame of eachother?

    std::ifstream read;
    read.open(logFile);
    char buffarr[256];
    std::string buffstr, buffcurrent;
    bool ok = true;
    int index = 0, prevtime = 100;
    while(read.getline(buffarr, 256)){
        buffstr = buffarr;
        buffcurrent = buffstr.substr(7, 2);
        ASSERT_TRUE(std::stoi(buffcurrent) - prevtime < timeperiod) << "buffcurrent is "<<buffcurrent<<" and "<< std::stoi(buffcurrent)<<" prevtime is "<<prevtime<<" "<<std::stoi(buffcurrent) - prevtime<<" is >= than 10";
        prevtime = stoi(buffcurrent);
    }
    read.close();
}

int main(int argc, char **argv){
    LogSink logger("/dev/ttyS0", 115200, "logsink.txt");
    logger.runFor(15);
   
    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    return result;
}