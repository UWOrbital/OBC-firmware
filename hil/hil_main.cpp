#include <iostream>
#include <unistd.h>
#include <ctime>
#include <gtest/gtest.h>
#include "logsink.h"

int main(int argc, char **argv) {
  LogSink logger("/dev/ttyS0", 115200, "logsink.txt");
  logger.start();
  sleep(15);
  logger.stop();

  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  return result;
}
