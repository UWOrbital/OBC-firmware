cmake_minimum_required(VERSION 3.15)

target_include_directories(OBC-firmware.out PUBLIC
    include
)

target_sources(OBC-firmware.out PUBLIC
    source/alarm_handler.c
    source/command_callbacks.c
    source/command_manager.c
    source/command_unpack.c
    source/health_collector.c
    source/supervisor.c
    source/telemetry_fs_utils.c
    source/telemetry_manager.c
    source/telemetry_pack.c
    source/timekeeper.c
)
