"""
TODO:
1.) Read from CSV [x]
2.) Extract SDL, SCL, and Time data [x]
3.) Extract i2c params
"""

import pandas as pd

df = pd.read_csv("./example_csv/digital.csv")

timeStr = "Time [s]"
SDLStr = "Channel 0"
SCLStr = "Channel 1"

i2cDf = df[[timeStr, SDLStr, SCLStr]]
print(i2cDf.iloc[0])
