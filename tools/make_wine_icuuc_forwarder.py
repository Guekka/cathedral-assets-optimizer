#!/usr/bin/env python3
"""Generate a code-free PE32+ icuuc.dll forwarder for Wine and Proton."""

import argparse
import struct
from pathlib import Path

EXPORTS = (
    "UCNV_TO_U_CALLBACK_SUBSTITUTE",
    "UCNV_FROM_U_CALLBACK_SUBSTITUTE",
    "ucnv_open",
    "ucnv_close",
    "ucnv_reset",
    "ucnv_getMaxCharSize",
    "ucnv_getName",
    "ucnv_getToUCallBack",
    "ucnv_getFromUCallBack",
    "ucnv_setToUCallBack",
    "ucnv_setFromUCallBack",
    "ucnv_fromUnicode",
    "ucnv_toUnicode",
    "ucnv_countAvailable",
    "ucnv_getAvailableName",
    "ucnv_cbToUWriteUChars",
    "ucnv_cbFromUWriteUChars",
    "ucnv_toUCountPending",
    "ucnv_fromUCountPending",
    "ucnv_getStandardName",
)

SECTION_RVA = 0x1000
FILE_ALIGNMENT = 0x200
SECTION_ALIGNMENT = 0x1000
HEADER_SIZE = 0x200


def write_forwarder(output: Path) -> None:
    names = sorted(set(EXPORTS), key=lambda name: name.encode())
    dll_name = output.name.encode() + b"\0"
    eat = 40
    name_pointers = eat + 4 * len(names)
    ordinals = name_pointers + 4 * len(names)
    strings = ordinals + 2 * len(names)
    blob = bytearray(strings)
    pool = bytearray()

    def add_string(value: bytes) -> int:
        offset = strings + len(pool)
        pool.extend(value)
        return SECTION_RVA + offset

    name_rva = add_string(dll_name)
    forwarder_rvas = [
        add_string(f"icu.{name}".encode() + b"\0") for name in names
    ]
    name_rvas = [add_string(name.encode() + b"\0") for name in names]
    section = bytearray(bytes(blob) + bytes(pool))
    section_size = len(section)

    struct.pack_into(
        "<IIHHIIIIIII",
        section,
        0,
        0,
        0,
        0,
        0,
        name_rva,
        1,
        len(names),
        len(names),
        SECTION_RVA + eat,
        SECTION_RVA + name_pointers,
        SECTION_RVA + ordinals,
    )
    for index in range(len(names)):
        struct.pack_into("<I", section, eat + 4 * index, forwarder_rvas[index])
        struct.pack_into("<I", section, name_pointers + 4 * index, name_rvas[index])
        struct.pack_into("<H", section, ordinals + 2 * index, index)

    raw_section = bytes(section) + b"\0" * (-len(section) % FILE_ALIGNMENT)
    image_size = SECTION_RVA + (
        (section_size + SECTION_ALIGNMENT - 1) // SECTION_ALIGNMENT
    ) * SECTION_ALIGNMENT
    dos_header = bytearray(0x40)
    dos_header[0:2] = b"MZ"
    struct.pack_into("<I", dos_header, 0x3C, 0x40)
    coff_header = struct.pack("<4sHHIIIHH", b"PE\0\0", 0x8664, 1, 0, 0, 0, 240, 0x2022)
    optional_header = struct.pack(
        "<HBBIIIIIQIIHHHHHHIIIIHHQQQQII",
        0x20B,
        14,
        0,
        0,
        len(raw_section),
        0,
        0,
        SECTION_RVA,
        0x180000000,
        SECTION_ALIGNMENT,
        FILE_ALIGNMENT,
        6,
        0,
        0,
        0,
        6,
        0,
        0,
        image_size,
        HEADER_SIZE,
        0,
        2,
        0x0100,
        0x100000,
        0x1000,
        0x100000,
        0x1000,
        0,
        16,
    )
    directories = struct.pack("<II", SECTION_RVA, section_size) + b"\0" * (8 * 15)
    section_header = struct.pack(
        "<8sIIIIIIHHI",
        b".edata",
        section_size,
        SECTION_RVA,
        len(raw_section),
        HEADER_SIZE,
        0,
        0,
        0,
        0,
        0x40000040,
    )
    headers = bytes(dos_header) + coff_header + optional_header + directories + section_header
    if len(optional_header) + len(directories) != 240 or len(headers) > HEADER_SIZE:
        raise ValueError("Invalid PE header layout")

    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_bytes(headers + b"\0" * (HEADER_SIZE - len(headers)) + raw_section)
    print(f"{output}: {len(names)} forwarders to icu.dll, {HEADER_SIZE + len(raw_section)} bytes")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("output", type=Path)
    arguments = parser.parse_args()
    write_forwarder(arguments.output)


if __name__ == "__main__":
    main()
