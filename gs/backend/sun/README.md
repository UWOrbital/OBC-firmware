# Sun Position

Script for parsing data from the NASA Horizons API used by GNC for onboard algorithms

## Requirements:
Check the requirements.txt file for the latest version of the requirements (pip install -r requirements.txt)
- Python
- Requests (pip install requests)
- Skyfield (pip install skyfield)
- PyTest (pip install pytest)

## Usage:
usage: ephemeris.py [-h] [-s STEP_SIZE] [-t TARGET] [-o OUTPUT] [-d] [-p {0,1,2}] [-e {first,last,both,none}] [-l LOG] start_time stop_time

Position Ephemeris Retriever

positional arguments:
-  start_time            Start time in the format YYYY-MM-DD or JD#. Must be the same format as stop time.
-  stop_time             Stop time in the format YYYY-MM-DD or JD#. Must be the same format as start time.

options:
-  -h, --help           <br>
show this help message and exit
-  -s STEP_SIZE, --step-size STEP_SIZE <br>
                        Step size in the same format as the Horizons API (e.g. 1m, 1h, 1d, 1y, 100). Default: 5m
-  -t TARGET, --target TARGET <br>
                        Target object (e.g. sun, moon, mars). Default: sun
-  -o OUTPUT, --output OUTPUT <br>
                        Output file name. Default: output.bin
-  -p {0,1,2}, --print {0,1,2} <br>
                        Prints the output to the console used for debugging purposes.
                        0 = Always, 1 = Basic debugging, 2 = All output.  Default: 0
-  -e {first,last,both,none}, --exclude {first,last,both,none} <br>
                        Exclude the first, last, both or none of the values from the output file. Default: last
-  -l LOG, --log LOG      <br>Log file for debugging purposes. Default: None (Standard output)

## Contents of the output file:
### Header:
- 8 bytes: Start time in JD (min_jd) (type: double)
- 8 bytes: Step size of data points (step_size) (type: double)
- 4 bytes: Number of data points (n) (type: uint)

### Data: n data points of a total of 12 bytes per data point (all floats)
- 4 bytes: x
- 4 bytes: y
- 4 bytes: z

### JD calculation:
JD = min_jd + i * step_size <br>
Where i = 0, 1, 2, ..., n-1
