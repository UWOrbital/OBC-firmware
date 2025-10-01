void i2cSetSlaveAdd(void* i2c, unsigned int slaveAdd) {}
void i2cSetDirection(void* i2c, unsigned int direction) {}
void i2cSetCount(void* i2c, unsigned int count) {}
void i2cSetMode(void* i2c, unsigned int mode) {}
void i2cSetStop(void* i2c) {}
void i2cSetStart(void* i2c) {}
void i2cSend(void* i2c, unsigned char* data, unsigned int length) {}
void i2cReceive(void* i2c, unsigned char* data, unsigned int length) {}
void i2cClearSCD(void* i2c) {}