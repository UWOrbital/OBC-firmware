/*
@brief  Simple LogSink unit test flow for validating integrated software on hardware ie. LM75BD log validation on the Rev1 board.
        Generated Logs simply checks if any logs have been outputed, Number Logs validates the number of logs after running for a duration,
        Specific Logs validates if the LogSink fetched a certain log, and Timeframe Logs validates the file output speed of the logs.
*/
#include "logsink.h"
#include <iostream>
#include <unistd.h>
#include <gtest/gtest.h>
#include <fstream>
#include <string>

#define MIN_LOGS_AQUIRED 7
#define SPECIFIC_LOG "Firmware/OBC-firmware/obc/app/drivers/sdcard/sdc_bdev.c:416 - 108"
#define LOG_FILE "logsink.txt"
#define TIME_PERIOD 10

TEST(LogIntegrationTest, Generated_Logs) {
  // Did it Read?
  std::ifstream read;
  read.open(LOG_FILE);
  ASSERT_TRUE(read.is_open());
  if (read.is_open()) {
    char temp = '_';
    read >> temp;
    EXPECT_TRUE(temp != ' ' && temp != '\n');
    read.close();
  }
}

TEST(LogIntegrationTest, Number_Logs) {
  // Did it read expected times?
  std::ifstream read;
  read.open(LOG_FILE);
  ASSERT_TRUE(read.is_open());
  if (read.is_open()) {
    char buff[256];
    int count = 0;
    while (read.getline(buff, 256)) {
      count++;
    }
    EXPECT_TRUE(count > MIN_LOGS_AQUIRED) << "Required: " << MIN_LOGS_AQUIRED << " Iterated: " << count;
    read.close();
  }
}

TEST(LogIntegrationTest, Speific_Logs) {
  // Did it read a specific log?

  std::ifstream read;
  read.open(LOG_FILE);
  ASSERT_TRUE(read.is_open());
  if (read.is_open()) {
    char buffarr[256];
    std::string buffstr;
    int count = 0;
    bool found = false;
    while (read.getline(buffarr, 256)) {
      buffstr = buffarr;

      if (buffstr.find(SPECIFIC_LOG) != std::string::npos) {
        found = true;
        break;
      }
      count++;
    }
    EXPECT_TRUE(found) << "Iterated " << count << " times!";
    read.close();
  }
}

TEST(LogIntegrationTest, TimeFrame_Logs) {
  // Did it read all logs within a specific time frame of eachother?

  std::ifstream read;
  read.open(LOG_FILE);
  ASSERT_TRUE(read.is_open());
  if (read.is_open()) {
    char buffarr[256];
    std::string buffstr, buffcurrent;
    int prevtime = 100;
    while (read.getline(buffarr, 256)) {
      buffstr = buffarr;
      buffcurrent = buffstr.substr(7, 2);
      EXPECT_TRUE(std::stoi(buffcurrent) - prevtime < TIME_PERIOD)
          << "buffcurrent is " << buffcurrent << " and " << std::stoi(buffcurrent) << " prevtime is " << prevtime << " "
          << std::stoi(buffcurrent) - prevtime << " is >= than 10";
      prevtime = stoi(buffcurrent);
    }
    read.close();
  }
}
