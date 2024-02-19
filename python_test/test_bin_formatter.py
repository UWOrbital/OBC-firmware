import os

from pytest import mark

from obc.tools.python.bin_formatter import create_bin


# Expected header is the header in bytes
@mark.parametrize(
    "input_name, bin_version, bin_data, expected_name, expected_header, expect_data",
    [
        (
            "test1.bin",
            0,
            b"0123456789",
            "test1_formatted.bin",
            b"\x00\x00\x00\x00\n\x00\x00\x00",
            b"0123456789",
        ),  # \n has ASCII value of 10 so struct.pack uses \n
        (
            "test2.bin",
            2,
            b"10987",
            "test2_formatted.bin",
            b"\x02\x00\x00\x00\x05\x00\x00\x00",
            b"10987",
        ),
        (
            "test3.bin",
            3,
            b"1",
            "test3_formatted.bin",
            b"\x03\x00\x00\x00\x01\x00\x00\x00",
            b"1",
        ),
    ],
)
def test_create_bin(
    input_name, bin_version, bin_data, expected_name, expected_header, expect_data
):
    with open(input_name, "wb") as f:
        f.write(bin_data)

    output_name = create_bin(input_name, bin_version)

    with open(output_name, "rb") as f:
        output_data = f.read()

    os.remove(input_name)
    os.remove(output_name)

    assert output_name == expected_name
    assert output_data == expected_header + expect_data
