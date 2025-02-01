import os

import pytest
from obc.tools.python.bin_formatter import create_bin


# Expected header is the header in bytes
@pytest.mark.parametrize(
    "input_name, metadata_input_name, bin_version, bin_data, cmake_data, expected_name, expected_header, expect_data",
    [
        (
            "OBC-firmware.bin",
            "OBC-metadata.bin",
            0,
            b"0123456789",
            # Simulate what the contents of the metadata bin file would be as generated from top level CMake file
            b"1\n",
            "OBC-firmware_formatted.bin",
            b"\x00\x00\x00\x00\n\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
            b"0123456789",
        ),  # \n has ASCII value of 10 so struct.pack uses \n
        (
            "OBC-firmware.bin",
            "OBC-metadata.bin",
            2,
            b"10987",
            b"2\n",
            "OBC-firmware_formatted.bin",
            b"\x02\x00\x00\x00\x05\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
            b"10987",
        ),
        (
            "OBC-firmware.bin",
            "OBC-metadata.bin",
            3,
            b"1",
            b"0\n",
            "OBC-firmware_formatted.bin",
            b"\x03\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
            b"1",
        ),
    ],
)
def test_create_bin(
    input_name, metadata_input_name, bin_version, bin_data, cmake_data, expected_name, expected_header, expect_data
):
    with open(input_name, "wb") as f:
        f.write(bin_data)

    with open(metadata_input_name, "wb") as f:
        f.write(cmake_data)

    output_name = create_bin(input_name, bin_version)

    with open(output_name, "rb") as f:
        output_data = f.read()

    os.remove(input_name)
    os.remove(output_name)
    os.remove(metadata_input_name)

    assert output_name == expected_name
    assert output_data == expected_header + expect_data
