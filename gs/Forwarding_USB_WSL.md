# Using serial communication in WSL2

1. Setup <br />
    1.1 Install usbipd from https://github.com/dorssel/usbipd-win/releases in powershell <br />
    1.2 On the Ubuntu side, run the following: <br />
    ```
    sudo apt install linux-tools-generic hwdata
    sudo update-alternatives --install /usr/local/bin/usbip usbip /usr/lib/linux-tools/*-generic/usbip 20
    ```
3. Forwarding a USB device <br />
    2.1 To see what devices can be forwarded: <br />
   ```
   usbipd wsl list <br />
   ```
    2.2 Attach the bus ID (should be on the far left): <br />
   ```
   usbipd wsl attach --busid <busid>
   ```
    2.3 Now in Ubuntu run the following command to see if it was forwarded correctly: <br />
   ```
   lsusb
   ```
    2.4 The path should be dev/ttyACM0 or dev/ttyACM1, you can check by navigating to /home/${USER}/bin/dev and typing ls into the console.
5. Detaching a USB device <br />
    3.1 ```usbipd wsl detach --busid <busid>```
