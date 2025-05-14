import os
import random
from ctypes import POINTER, Structure, c_uint, c_uint8, pointer
from pathlib import Path

import pytest
from ax25 import FrameType
from interfaces.obc_gs_interface.aes128 import aes128_gs as aes
from interfaces.obc_gs_interface.ax25 import ax25_gs as ax25
from interfaces.obc_gs_interface.fec import fec_gs as fec


# TEST: A simulated receive with the entire pipline
def test_receive():
    # Create file path and read from the file
    # NOTE: This file is written to by the C tests so if the tests do not pass try running the c tests first!
    path = (Path(__file__).parent / "../build/test/test_interfaces/unit/encode.bin").resolve()
    file = path.open("rb")
    bin = bytearray(file.read())

    # Instantiate the ax25 class to unstuff
    ax25_proto = ax25.AX25("ATLAS", "AKITO")
    bin = ax25_proto.unstuff(bytes(bin))

    # Instantiate FEC class to error correct
    fec_coder = fec.FEC()
    # NOTE: 17 to 272 is the range for info bytes that are needed for the decoding
    data_to_decode = fec_coder.decode(bin[17:272])
    # With the data decoded we need to add the rest of the data back to get a full frame
    decoded_data = bytes(bin[:17] + data_to_decode + bin[272:])

    # Now we can finally decode the frame and extract information
    rcv_frame = ax25_proto.decode_frame(decoded_data)
    assert str(rcv_frame.src) == "ATLAS"
    assert str(rcv_frame.dst) == "AKITO"
    assert str(rcv_frame.control.frame_type) == "FrameType.I"
    frame_data = rcv_frame.data

    # Instantiate the aes class with defaults from the c implementation and decrypt the data
    aes_cipher = aes.AES128(
        b"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f",
        b"\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01",
    )
    decrypted_data = aes_cipher.decrypt(frame_data)

    # What the data should be and see if it matches
    intended_str = "64 121 190 31 108 53 202 59 88 177 150 23 4 237 34 179 112 233 110 15 156 165 122 43 136 33 70 7 52 93 210 163 160 89 30 255 204 21 42 27 184 145 246 247 100 205 130 147 208 201 206 239 252 133 218 11 232 1 166 231 148 61 50 131 0 57 126 223 44 245 138 251 24 113 86 215 196 173 226 115 48 169 46 207 92 101 58 235 72 225 6 199 244 29 146 99 96 25 222 191 140 213 234 219 120 81 182 183 36 141 66 83 144 137 142 175 188 69 154 203 168 193 102 167 84 253 242 67 192 249 62 159 236 181 74 187 216 49 22 151 132 109 162 51 240 105 238 143 28 37 250 171 8 161 198 135 180 221 82 35 32 217 158 127 76 149 170 155 56 17 118 119 228 77 2 19 80 73 78 111 124 5 90 139 104 129 38 103 20 189 178 3 128 185 254 95 172 117 10 123 152 241 214 87 68 45 98 243 176 41 174 79 220 229 186 107 200 97 134 71 116 157 18 "
    actual_str = ""
    for i in range(223):
        actual_str += str(decrypted_data[i]) + " "

    assert intended_str == actual_str
    file.close


# TEST: A simulated send with the entire pipline (some bits are also flipped before sending to test fec)
def test_send():
    # Create a file and clear contents so that we can write to it
    # NOTE: These are used by the c tests to receive data
    path = (Path(__file__).parent / "../build/test/test_interfaces/unit/encode_more.bin").resolve()
    file = path.open("wb")
    file.truncate(0)

    # Instantiate our ax25 class to get ready to create frame
    ax25_proto = ax25.AX25("ATLAS", "AKITO")
    # Instantiate the fec class for forward error correction
    fec_coder = fec.FEC()

    # Generating pseudo-random data
    data = []
    random.seed(777)
    for i in range(223):
        data.append(random.randint(0, 255))

    # Instantaite the aes cipher with the same defaults from the c implementation
    aes_cipher = aes.AES128(
        b"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f",
        b"\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01",
    )
    # Encrypt data
    encrypted_data = aes_cipher.encrypt(data)
    # Encode data for error correction
    encode_data = fec_coder.encode(bytes(encrypted_data))
    # Create the frame
    send_frame = ax25_proto.encode_frame(encode_data, FrameType.I, 0)
    # Stuff the frame as per the standard
    send_frame_stuffed = ax25_proto.stuff(send_frame)
    # Convert to byte array so we can change data
    frame_array = bytearray(send_frame_stuffed)

    frame_array[18] += 1
    frame_array[200] += 1

    # Convert back to bytes and write to file
    send_frame = bytes(frame_array)
    file.write(send_frame)
    file.close

    # NOTE: This must be called after everything with fec is done running (thus, it is not in the first test)
    fec_coder.destroy()
    # If it gets here that means all went well!
    assert True
