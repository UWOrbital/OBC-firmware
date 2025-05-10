import ctypes

# find the library. for osx this is .dylib, for linux .so, for windows .dll
# path should be an absolute path to where the library was compiled and installed
libcorrect = ctypes.CDLL("../../../build/interfaces/libobc-gs-interface.a")

# tell ctypes about the functions in our library
create = libcorrect.correct_convolutional_create
create.argtypes = [ctypes.c_size_t, ctypes.c_size_t, ctypes.c_void_p]
create.restype = ctypes.c_void_p

destroy = libcorrect.correct_convolutional_destroy
destroy.argtypes = [ctypes.c_void_p]
destroy.restype = None

encode_len = libcorrect.correct_convolutional_encode_len
encode_len.argtypes = [ctypes.c_void_p, ctypes.c_size_t]
encode_len.restype = ctypes.c_size_t

encode = libcorrect.correct_convolutional_encode
encode.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_size_t, ctypes.c_void_p]
encode.restype = ctypes.c_size_t

decode = libcorrect.correct_convolutional_decode
decode.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_size_t, ctypes.c_void_p]
decode.restype = ctypes.c_ssize_t

# pick a polynomial to encode/decode with
# these are just taken from /usr/local/include/correct.h
# 0o represents octal
polynomials = (ctypes.c_uint16 * 2)(*[0o161, 0o127])

# create the conv encoder/decoder
conv = create(2, 7, polynomials)

# message will be a python string. we could also choose to make this
# an array of bytes instead
msg = "Hello, World!"

# turn our message into a ctypes array of uint8_ts
payload = (ctypes.c_uint8 * len(msg)).from_buffer_copy(msg)

# ask libcorrect how long the encoded message will be
encoded_len = encode_len(conv, len(payload))

print("Encoded message: %s" % msg)
print("Message length: %d bits" % (len(msg) * 8))
print("Encoded length: %d bits" % encoded_len)

# encoded_len is number of bits. we want to know how many bytes
encoded_len_bytes = encoded_len / 8
# in case encoded is not multiple of 8, add extra byte
encoded_len_bytes += 1

# allocate buffer to hold encoded message
encoded = (ctypes.c_uint8 * encoded_len_bytes)()

# do convolutional encode
encode(conv, payload, len(payload), encoded)

# mess with some bits
encoded[3] ^= 0x40
encoded[7] ^= 0x04

# allocate buffer to hold decoded message
decoded = (ctypes.c_uint8 * ((encoded_len / 2) / 8))()

decoded_len = decode(conv, encoded, encoded_len, decoded)

print("Decoded length: %d bits" % (decoded_len * 8))

print("Decoded message: %s" % bytearray(decoded))

# mess with too many bits
encoded[2] = 0

decoded_len = decode(conv, encoded, encoded_len, decoded)

print("Decoded length: %d bits" % (decoded_len * 8))

print("Decoded message: %s" % bytearray(decoded))

destroy(conv)
