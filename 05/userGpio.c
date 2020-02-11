/*
 * in user-space
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

/*
 * https://stackoverflow.com/questions/13124271/driving-beaglebone-gpio-through-dev-mem
 * https://home.wakatabe.com/ryo/wiki/index.php?BeagleBone#k53b696a
 */

/* Physical address of peripheral register */
#define REG_ADDR_GPIO_BASE   0x4804C000
#define REG_ADDR_GPIO1_END_ADDR 0x4804DFFF
#define REG_ADDR_GPIO_LENGTH (REG_ADDR_GPIO1_END_ADDR - REG_ADDR_GPIO_BASE)
#define REG_ADDR_GPIO_GPFSEL_0     0x0000
#define REG_ADDR_GPIO_OUTPUT_SET_0 0x001C
#define REG_ADDR_GPIO_OUTPUT_CLR_0 0x0028
#define REG_ADDR_GPIO_LEVEL_0      0x0034

#define REG(addr) (*((volatile unsigned int*)(addr)))
#define DUMP_REG(addr) printf("%08X\n", REG(addr));

int main()
{
    int address;    /* GPIOレジスタへの仮想アドレス(ユーザ空間) */
    int fd;

    /* Open device file to memory access */
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
        perror("open");
        return -1;
    }

    /* Map physical address to virtual address */
    address = (int)mmap(NULL, REG_ADDR_GPIO_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, REG_ADDR_GPIO_BASE);
    if (address == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return -1;
    }

    /* Set GPIO4 for output */
    REG(address + REG_ADDR_GPIO_GPFSEL_0) = 1 << 12;

    /* Output HIGH to GPIO4 */
    REG(address + REG_ADDR_GPIO_OUTPUT_SET_0) = 1 << 4;
    DUMP_REG(address + REG_ADDR_GPIO_LEVEL_0);

    /* GPIO4をLow出力 */
    REG(address + REG_ADDR_GPIO_OUTPUT_CLR_0) = 1 << 4;
    DUMP_REG(address + REG_ADDR_GPIO_LEVEL_0);

    /* 使い終わったリソースを解放する */
    munmap((void*)address, REG_ADDR_GPIO_LENGTH);
    close(fd);

    return 0;
}
