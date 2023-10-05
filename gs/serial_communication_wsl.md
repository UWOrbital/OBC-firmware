# Using serial communication in WSL2

1. Setup  
    1.1 Install usbipd from https://github.com/dorssel/usbipd-win/releases in powershell  
    1.2 On the Ubuntu side, run the following:  
    ```
    sudo apt install linux-tools-generic hwdata  
    sudo update-alternatives --install /usr/local/bin/usbip usbip /usr/lib/linux-tools/*-generic/usbip 20
    ```
3. Forwarding a USB device  
    2.1 To see what devices can be forwarded:
   ```
   usbipd wsl list
   ```  
    2.2 Attach the bus ID (should be on the far left):
   ```
   usbipd wsl attach --busid <busid>
   ```  
    2.3 Now in Ubuntu run the following command to see if it was forwarded correctly:
   ```
   lsusb
   ```
5. Detaching a USB device  
    3.1 ```usbipd wsl detach --busid <busid>```
