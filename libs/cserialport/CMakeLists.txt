cmake_minimum_required(VERSION 3.15)

set(CSERIALPORT_LIB CSerialPort)

if(UNIX)
add_library(${CSERIALPORT_LIB} STATIC
    source/SerialPort.cpp
    source/SerialPortBase.cpp
    source/SerialPortInfo.cpp
    source/SerialPortInfoBase.cpp
    source/SerialPortInfoUnixBase.cpp    
    source/SerialPortUnixBase.cpp
    bindings/c/cserialport.cpp
)

elseif(WIN32)
add_library(${CSERIALPORT_LIB} STATIC
    source/SerialPort.cpp
    source/SerialPortBase.cpp
    source/SerialPortInfo.cpp
    source/SerialPortInfoBase.cpp
    source/SerialPortInfoWinBase.cpp
    source/SerialPortWinBase.cpp
    bindings/c/cserialport.cpp
)
endif()

target_include_directories(${CSERIALPORT_LIB} PUBLIC
    include
)
