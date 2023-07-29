# import os
# import struct

# import ephemerisparser as ep
# import ephemeris


# def test_get_single_data_point_float():
#     filename = 'test_get_single_data_point_float.bin'

#     # Write a float to the file
#     with open(filename, 'wb') as f:
#         f.write(struct.pack(ephemeris.DATA_FLOAT, 2451545.0))

#     # Read the float from the file
#     with open(filename, 'rb') as f:
#         f.seek(0)
#         value = ep.get_single_data_point(f, True)

#     os.remove(filename)
#     assert value == 2451545.0
