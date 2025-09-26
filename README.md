# OBC-firmware

This repository holds all the code written by UW Orbital's software team. This includes OBC firmware/embedded software, ground station software, and all testing infrastructure.

## Table of Contents

- [UW Orbital Docs](#uw-orbital-docs)
- [Notion](#notion)
- [Firmware Dependencies](#firmware-dependencies)
- [Contributing](#contributing)
- [Authors](#authors)

## UW Orbital Docs
The UW Orbital Docs are a great source of codebase documentation. The site includes many detailed setup guides as well as code style guides. **We highly recommend following these guides if you are new to the team or software development in general.**

You will find it most helpful to check out the setup guides, build tutorials, and style guides.

Check out the site here: https://uworbital.github.io/OBC-firmware/

The documentation site now contains most of the information previously found in this README.

## Notion
The team Notion is another important source of information. Here you'll find other team documentation, onboarding info, and links to useful resources.

The software section of the Notion is available here: https://www.notion.so/uworbital/Software-a613c4aaa36449969010cabba2fc329d

Useful links:
- Firmware Onboarding: https://www.notion.so/uworbital/Firmware-Onboarding-48aeba74f55045f5929b40d029202842
- Ground Station Onboarding: https://www.notion.so/uworbital/Ground-Station-Onboarding-10f8a26d767780d7ae8de921d9782b77
- Software Development Life Cycle (SDLC): https://www.notion.so/uworbital/Software-Development-Life-Cycle-1218a26d767780619d42fa22f0785e73

## Firmware Dependencies

#### HALCoGen

Download HALCoGen here: https://www.ti.com/tool/HALCOGEN#downloads. This will be used for configuring the HAL. Unfortunately, the tool is only available on Windows. If you're on a non-Windows system, you may be able to set HALCoGen up in a Windows VM or someone else on the team can make HAL changes for you. We don't have to modify the HAL very often anyways.

#### Code Composer Studio (CCS)

Download Code Composer Studio (CCS): https://www.ti.com/tool/CCSTUDIO. This will be used for debugging.

#### Uniflash

Download UniFlash here: https://www.ti.com/tool/UNIFLASH#downloads. This will be used for flashing the RM46.

#### **Windows**

1. Download WSL2: https://learn.microsoft.com/en-us/windows/wsl/install

2. In WSL2, run the following:
   ```sh
   sudo apt-get update
   sudo apt-get install build-essential gcc-multilib g++-multilib curl
   ```
3. Choose the environment where you'll be running `git commit` (either WSL2 or the host) and install Python 3.11 and pip. (Only required for Backend devs)
   A. If using WSL, follow the instructions under the `Linux` section 2.

   B. If you are using Windows. Run the following commands in the OBC-firmware directory:

   Install Python 3.11.11: https://www.python.org/downloads/release/python-31111/

   ```sh
   C:\path\to\python\executable -m venv .venv
   .\Scripts\bin\activate
   pip install -r requirements.txt
   pip install -e .
   ```

4. Setup pre-commit.
   In the WSL, under the OBC-firmware directory, run the following commands:
   ```sh
   pip install -r requirements.txt # You may want to create a Python virtual env before this if you haven't already
   pre-commit install
   ```
   - You may receive a message in yellow saying where pre-commit.exe was installed and that you need to add it to PATH
     - To do this go to View advanced System settings -> Environment Variables -> Path -> Edit and click new to paste the path to where pre-commit.exe is installed into here. You may need to restart after doing this for the changes to take place.
   - Once your PATH is set up and pre-commit is installed you can use `pre-commit run --all-files` to format all of your files before committing
     **Note:** pre-commit is used to format your code whenever you make a commit.

You'll be using WSL2 for all development.

5. Setup the PostgreSQL database

This setup is only required for GS members. Please follow the instructions located in [POSTGRESQL_SETUP.md](gs/POSTGRESQL_SETUP.md)

#### **MacOS**

1. Install required build tools (CMake, Make, gcc)

```sh
brew install cmake
brew install make
brew install gcc
```

2. Install Python 3.11 and setup Python virtual environment

Run the following commands in the OBC-firmware directory:

```sh
brew install python@3.11
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
pip install -e .
```

3. Setup pre-commit

```sh
pip install -r requirements.txt # You may want to create a Python virtual env before this if you haven't already
pre-commit install
```

4. Setup the PostgreSQL database

This setup is only required for GS members. Please follow the instructions located in [POSTGRESQL_SETUP.md](gs/POSTGRESQL_SETUP.md)

#### **Linux**

1. Install required build tools (CMake, Make, gcc)

```sh
sudo apt-get update
sudo apt-get install build-essential gcc-multilib g++-multilib curl
```

2. Install Python 3.11 and setup Python virtual environment

This is only required for GS devs.
Run the following commands in the OBC-firmware directory:

```sh
sudo apt-get install python3.11
python3.11 -m venv .venv  # You might need to install python3.11-venv using `sudo apt install python3.11-venv` before running this setup
source .venv/bin/activate
pip install -r requirements.txt
pip install -e .
```

3. Setup pre-commit

```sh
pip install -r requirements.txt # You may want to create a Python virtual env before this if you haven't already
pre-commit install
```

4. Setup the PostgreSQL database

This setup is only required for GS members. Please follow the instructions located in [POSTGRESQL_SETUP.md](gs/POSTGRESQL_SETUP.md)

### Building

#### **OBC Firmware**

Before building, ensure that the Python virtual environment is activated or that the packages are installed globally.

From the top-level directory, run the following to build the OBC firmware.

```sh
mkdir -p build_arm && cd build_arm
cmake .. -DCMAKE_BUILD_TYPE=OBC
cmake --build .
```

OR you can just run `./scripts/obc-build.sh` from the top-level directory.

Take a look at `cmake/fw_build_options.cmake` to see the available build options.

#### **Ground Station**

From the top-level directory, run the following to build the ground station. Currently, the ground station has only been supported for Windows and may be subject to bugs on MacOS.

Run the following in the terminal from the top-level directory:

```sh
mkdir -p build_gs && cd build_gs
cmake .. -DCMAKE_BUILD_TYPE=GS
cmake --build .
```

OR you can just run `./scripts/gs-build.sh` from the top-level directory.

#### **Tests**

**Note**: GNU cross-compilation tools are required to build the tests. If you haven't already, run the following before building:

```sh
sudo apt-get update
sudo apt-get install gcc-multilib g++-multilib
```

From the top-level directory, run the following to build and run the tests.

```sh
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Test
cmake --build .
ctest --verbose
```

OR you can just run `./scripts/test.sh` from the top-level directory.

#### **Example files**

Before building, ensure that the Python virtual environment is activated or that the packages are installed globally.

From the top-level directory, run the following to build the example source file.

```sh
mkdir -p build_examples && cd build_examples
cmake .. -DCMAKE_BUILD_TYPE=Examples -DEXAMPLE_TYPE=[EXAMPLE_TO_BE_COMPILED]
cmake --build .
```

OR you can just run `./scripts/example-build.sh [EXAMPLE_TO_BE_COMPILED]` from the top-level directory.

Options for `EXAMPLE_TYPE` include:

- `DMA_SPI` - for `dma_spi_demo`
- `FRAM_PERSIST` - for `test_app_fram_persist`
- `FRAM_SPI` - for `test_app_fram_spi`
- `LM75BD` - for `test_app_lm75bd`
- `MPU6050` - for `test_app_mpu6050`
- `RE_SD` - for `test_app_reliance_sd`
- `RTC`- for `test_app_rtc`
- `UART_RX` - for `test_app_uart_rx`
- `UART_TX` - for `test_app_uart_tx`
- `VN100` - for `vn100_demo`
- `CC1120_SPI` - for `test_app_cc1120_spi`
- `ADC` - for `test_app_adc`

Instructions on how to add examples:

- Decide on a code for the example, the code must only contain uppercase letters, numbers and/or `_` referred to as `EXAMPLE_ID` from now on
- Add the code and destination above to the list of examples in the form to the `README.md`: `EXAMPLE_ID` - for `example_name`
- Add the following to the `OBC/CMakeLists.txt` above the comment that says `# ADD MORE EXAMPLES ABOVE THIS COMMENT`

```cmake
elseif(${EXAMPLE_TYPE} MATCHES EXAMPLE_ID)
	add_executable(${OUT_FILE_NAME} path_to_main_file_in_example)
```

Where `path_to_main_file_in_example` is relative to the project root, see `OBC/CMakeLists.txt` for examples

- Add the `EXAMPLE_ID` to the `.github/workflows/obc_examples.yml` above the comment that starts with `# ADD NEW EXAMPLES ABOVE THIS LINE`

### Flashing

To flash the RM46 (our microcontroller), we use Uniflash. Open Uniflash and select the appropriate device and connection.

#### **RM46 Launchpad:**

- Device = LAUNCHXL2-RM46
- Connection = Texas Instruments XDS110 USB Debug Probe

#### **OBC Revision 1/2:**

- Device = RM46L852
- Connection = Texas Instruments XDS110 USB Debug Probe

Then, click `Start` to begin a session. Select the `OBC-firmware.out` executable that you built (located in the `build_arm/` directory) and click `Load Image`. This will begin the flash process.

### Debugging

We use Code Composer Studio for debugging the firmware. **TODO**: Write a tutorial on how to use CCS.

### **Frontend Development**

To run the frontend, you will need nodejs installed.

<details>
<summary>If you don't have nodejs installed, run the following commands to install it:</summary>

#### **MacOS**

```sh
# Download and install nvm:
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.40.3/install.sh | bash

# in lieu of restarting the shell
\. "$HOME/.nvm/nvm.sh"

# Download and install Node.js:
nvm install 22

# Verify the Node.js version:
node -v # Should print "v22.19.0".

# Verify npm version:
npm -v # Should print "10.9.3".
```

#### **Windows**

```sh
# Download and install Chocolatey:
powershell -c "irm https://community.chocolatey.org/install.ps1|iex"

# Download and install Node.js:
choco install nodejs --version="22.19.0"

# Verify the Node.js version:
node -v # Should print "v22.19.0".

# Verify npm version:
npm -v # Should print "10.9.3".
```

#### **Linux**

```sh
# Download and install nvm:
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.40.3/install.sh | bash

# in lieu of restarting the shell
\. "$HOME/.nvm/nvm.sh"

# Download and install Node.js:
nvm install 22

# Verify the Node.js version:
node -v # Should print "v22.19.0".

# Verify npm version:
npm -v # Should print "10.9.3".

```

</details>

#### **Running the ARO Frontend**

If you have Docker installed, you can run the following command to start the ARO frontend:

_All instructions assume you are in the top-level directory_

```sh
docker compose up aro-frontend
```

Otherwise, you can run the following commands to start the ARO frontend:

```sh
cd gs/frontend/aro
npm install
npm run dev
```

#### **Running the MCC Frontend**

If you have Docker installed, you can run the following command to start the MCC frontend:

_All instructions assume you are in the top-level directory_

```sh
docker compose up mcc-frontend
```

Otherwise, you can run the following commands to start the MCC frontend:

```sh
cd gs/frontend/mcc
npm install
npm run dev
```

Note that after you install new dependencies, you need to rebuild the container with `docker-compose up --build aro-frontend` or `docker-compose up --build mcc-frontend`

## Contributing

1. Make sure you're added as a member to the UW Orbital organization on GitHub.
2. Create a feature branch for whatever task you're working on.
   - Our branch naming scheme is `<developer_name>/<feature_description>`.
     - Example: `danielg/implement-random-device-driver`
3. Make a PR.
   - For the PR description, make sure to fill in all the required details in the generated template.
   - Add at least three PR reviewers, including one software lead. When a PR is created, PR stats are added as a comment. You can use these stats to choose reviewers. Send a message in the #pr channel on Discord to notify the reviewers of your PR.
4. Make any requested changes and merge your branch onto main once the PR is approved.

## Authors

This codebase was developed by the members of UW Orbital, the University of Waterloo's CubeSat design team.
