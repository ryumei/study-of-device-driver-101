/*
 *
 */
#include <stdio.h>

int main()
{
    /* https://www.raspberrypi.org/documentation/hardware/raspberrypi/peripheral_addresses.md */
    extern unsigned bcm_host_get_sdram_address(void);
    printf("%08X\n", bcm_host_get_sdram_address());
    extern unsigned bcm_host_get_peripheral_address(void);;
    printf("%08X\n", bcm_host_get_peripheral_address());

    return 0;
}
