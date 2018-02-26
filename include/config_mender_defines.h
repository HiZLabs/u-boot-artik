/* Shell variables */
#define MENDER_BOOT_PART_NUMBER 0
#define MENDER_ROOTFS_PART_A_NUMBER 1
#define MENDER_ROOTFS_PART_B_NUMBER 2
#define MENDER_UBOOT_STORAGE_INTERFACE "mmc"
#define MENDER_UBOOT_STORAGE_DEVICE 0
#define MENDER_ROOTFS_PART_A_NAME "mmc 0:1"
#define MENDER_ROOTFS_PART_B_NAME "mmc 0:2"

/* BB variables. */
#define MENDER_STORAGE_DEVICE_BASE "/dev/mmcblk0p"

/* For sanity checks. */
#define MENDER_BOOTENV_SIZE 0x4000

#define MENDER_MTD_UBI_DEVICE_NAME ""
#define MENDER_BOOT_KERNEL_TYPE "Image"
#define MENDER_KERNEL_NAME "Image"
#define MENDER_DTB_NAME "s5p6818-artik710-raptor-rev00.dtb"

