import sys
import zlib
from pathlib import Path


def append_crc(input_path: str | Path, output_path: str | Path) -> None:
    """
    Read a file, calculate its CRC32 checksum, and create a new file with the checksum appended.

    Args:
        input_path: Path to the input file to read
        output_path: Path where the output file (original data + CRC32) will be written

    Returns:
        None
    """
    input_path = Path(input_path)
    output_path = Path(output_path)

    with input_path.open("rb") as f:
        data = f.read()

    crc = zlib.crc32(data) & 0xFFFFFFFF
    crc_bytes = crc.to_bytes(4, byteorder="little")

    with output_path.open("wb") as f:
        f.write(data)
        f.write(crc_bytes)

    print(f"CRC32: 0x{crc:08X} appended to {output_path.name}")


if __name__ == "__main__":
    """
    Main function

    Args:
        None

    """
    if len(sys.argv) != 3:
        print("python append_crc.py")
        print("invalid input")
        sys.exit(1)
    append_crc(sys.argv[1], sys.argv[2])
