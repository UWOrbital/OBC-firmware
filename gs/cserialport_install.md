# How to install and build the CSerialPort Library

1. Clone the repository (outside of OBC-firmware):
```
git clone https://github.com/itas109/CSerialPort.git
```

2. Follow these steps:
```
cd CSerialPort
mkdir bin && cd bin
cmake .. -DCMAKE_INSTALL_PREFIX=install -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=ON
cmake --build . --config Debug
cmake --install . --config Debug
```

3. Then simply use the library with the bindings found in common/cserialport.h
