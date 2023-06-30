cmake_minimum_required(VERSION 3.15)

target_include_directories(OBC-firmware.out PUBLIC
    include
)

target_sources(OBC-firmware.out PUBLIC
    source/aes128.c
    source/ax25.c
    source/cc1120_recv_task.c
    source/comms_manager.c
    source/decode_telemetry.c
    source/encode_telemetry.c
    source/fec.c
    source/send_telemetry.c
)
