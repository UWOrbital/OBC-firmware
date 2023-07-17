cmake_minimum_required(VERSION 3.15)

target_include_directories(OBC-firmware.out PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/alarm_mgr
    ${CMAKE_CURRENT_SOURCE_DIR}/altitude_ctrl_mgr
    ${CMAKE_CURRENT_SOURCE_DIR}/camera_mgr
    ${CMAKE_CURRENT_SOURCE_DIR}/command_mgr
    ${CMAKE_CURRENT_SOURCE_DIR}/comms_link_mgr
    ${CMAKE_CURRENT_SOURCE_DIR}/eps_mgr
    ${CMAKE_CURRENT_SOURCE_DIR}/health_collector
    ${CMAKE_CURRENT_SOURCE_DIR}/supervisor
    ${CMAKE_CURRENT_SOURCE_DIR}/telemetry_mgr
    ${CMAKE_CURRENT_SOURCE_DIR}/timekeeper
)

target_sources(OBC-firmware.out PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/alarm_mgr/alarm_handler.c

    ${CMAKE_CURRENT_SOURCE_DIR}/camera_mgr/payload_manager.c

    ${CMAKE_CURRENT_SOURCE_DIR}/command_mgr/command_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/command_mgr/command_callbacks.c

    ${CMAKE_CURRENT_SOURCE_DIR}/comms_link_mgr/comms_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/comms_link_mgr/comms_downlink_transmitter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/comms_link_mgr/comms_uplink_receiver.c
    ${CMAKE_CURRENT_SOURCE_DIR}/comms_link_mgr/downlink_encoder.c
    ${CMAKE_CURRENT_SOURCE_DIR}/comms_link_mgr/uplink_decoder.c

    ${CMAKE_CURRENT_SOURCE_DIR}/eps_mgr/eps_manager.c

    ${CMAKE_CURRENT_SOURCE_DIR}/health_collector/health_collector.c

    ${CMAKE_CURRENT_SOURCE_DIR}/supervisor/supervisor.c

    ${CMAKE_CURRENT_SOURCE_DIR}/telemetry_mgr/telemetry_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/telemetry_mgr/telemetry_fs_utils.c

    ${CMAKE_CURRENT_SOURCE_DIR}/timekeeper/timekeeper.c
)
