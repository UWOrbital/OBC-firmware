cmake_minimum_required(VERSION 3.15)

set(INCLUDE_DIRS
    include
    core/include
    os/freertos/include
    projects/freertos_rm46/host
)

set(SOURCES 
    bdev/bdev.c
    core/driver/blockio.c
    core/driver/buffer.c
    core/driver/buffercmn.c
    core/driver/core.c
    core/driver/dir.c
    core/driver/format.c
    core/driver/imap.c
    core/driver/imapextern.c
    core/driver/imapinline.c
    core/driver/inode.c
    core/driver/inodedata.c
    core/driver/redbufferpriv.h
    core/driver/volume.c
    fse/fse.c
    os/freertos/services/osassert.c
    os/freertos/services/osbdev.c
    os/freertos/services/osbdev_fatfs.h
    os/freertos/services/osclock.c
    os/freertos/services/osmutex.c
    os/freertos/services/osoutput.c
    os/freertos/services/ostask.c
    os/freertos/services/ostimestamp.c
    os/freertos/services/osuidgid.c
    posix/path.c
    posix/posix.c
    util/bitmap.c
    util/crc.c
    util/endian.c
    util/ftype.c
    util/heap.c
    util/memory.c
    util/namelen.c
    util/perm.c
    util/sign.c
    util/string.c
    projects/freertos_rm46/host/redconf.c
)

target_include_directories(OBC-firmware.out PUBLIC ${INCLUDE_DIRS})
target_sources(OBC-firmware.out PUBLIC ${SOURCES})
