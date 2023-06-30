cmake_minimum_required(VERSION 3.15)

target_include_directories(OBC-firmware.out PUBLIC
    include
)

target_sources(OBC-firmware.out PUBLIC
    source/obc_fs_utils.c
    source/obc_heap.c
    source/obc_logging.c
    source/obc_pack_utils.c
    source/obc_persistent_store.c
    source/obc_states.c
    source/obc_time.c
    source/obc_unpack_utils.c
)
