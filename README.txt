Intel Xeon PHI kernel driver with new fast PCI copies interface

This driver adds new DMA functionality to Intel's driver.
The new set of ioctls will allow for transferring data between the host and a MIC device with little overhead, and irrespective of software running on the device side.

New ioctls are found in include/mic/micmem_io.h


Special thanks to our hardware sponsor aeoncomputing.com for providing us access to hardware during our development.


Current features
================

- Memory pinning
- Memory mapping
- DMA transfers (single or double channel)


Future features
===============

- Improve transfer speed
- Make the code thread-friendly by adding fine-grained locking

Building
========

Building has been tested on Linux kernel 2.6.32 on Red Hat systems nominally supported by MPSS 2.1. Build requirements are kernel header files and gcc. 

To build:

    $ cd kmod
    $ MIC_CARD_ARCH=k1om make

The module will compile with MICARCH=l1om, but without any micmem features.

Loading
========

The module has only been tested on k1om devices.
    rmmod mic
    insmod mic.ko

Running
=======

IMPORTANT: Not following these instructions might crash your system or render PHI unusable until reboot.

To use micmem functions, an uOS must be loaded to device, e.g.:
    $ echo "boot:elf:/path/to/uOS" > /sys/class/mic0/state # load binary
    Write failed: I/Oerror
    $ cat /sys/class/mic0/post_code # check if binary actually loaded (FF is desired)
    FF

If Intel-provided uOS image is used, then host system might hang up randomly. To use a custom uOS image, DMA must be prepared (set SBOX_DCR to 0x00001555). This operation must be performed once after every uOS load.

A system with uOS running and DMA enabled may be used with micmem.

User-mode interface
===================

The patch is entirely based on ioctls, piggybacking on /dev/mic/ctrl.
ioctls are described in detail in include/mic/micmem_io.h and constants in include/mic/micmem_const.h . These two files should be included from the program that's going to use micmem.

A single instance of /dev/mic/ctrl needs to be open regardless of the number of devices to use. To enable/disable a device, use IOCTL_MICMEM_OPENDEV and IOCTL_MICMEM_CLOSEDEV.

The only two calls that don't depend on a device are IOCTL_MICMEM_PINMEM and IOCTL_MICMEM_UNPINMEM. They lock/unlock the memory in order use it with a device later.
Pinned memory needs to be mapped to a particular device using IOCTL_MICMEM_MAPRANGE and IOCTL_MICMEM_UNMAPRANGE. These calls will *only* map memory pinned with IOCTL_MICMEM_PINMEM.

The commands IOCTL_MICMEM_HOST2DEV and IOCTL_MICMEM_DEV2HOST perform the actual transfer. They accept a flags argument, where the programmer can specify the number of channels to use simultaneously (MICMEM_AUTO, MICMEM_SINGLE, MICMEM_DUAL). The calls operate *only* on host memory ranges previously mapped using IOCTL_MICMEM_MAPRANGE to the appropriate device.

All ioctls return 0 on success. Most commands dealing with memory assume that memory is aligned to page size (4096B).
However, micmem detects if host memory is using huge pages and can improve transfer speed by reducing number of dma transfers, therefore it's recommended to align all memory to 2MiB boundaries (e.g. using posix_memalign()).

All operations are synchronous.


Based on Intel kmod version 3.1-0.1.build0
