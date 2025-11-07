/*
 * SPDX-FileCopyrightText: 2025 NativeChips
 * SPDX-License-Identifier: Apache-2.0
 */

#include <firmware_apis.h>

#define GPIO0_BASE 0x30000000
#define GPIO1_BASE 0x30010000

#define GPIO_DATAI_OFFSET  0x0000
#define GPIO_DATAO_OFFSET  0x0004
#define GPIO_DIR_OFFSET    0x0008

#define VGPIO_REG_ADDR 0x30FFFFFC

void vgpio_write_output(uint16_t value) {
    volatile uint32_t *vgpio_reg = (volatile uint32_t *)VGPIO_REG_ADDR;
    uint32_t reg_val = *vgpio_reg;
    reg_val = (reg_val & 0xFFFF0000) | (value & 0xFFFF);
    *vgpio_reg = reg_val;
}

void main(void) {
    enableHkSpi(false);
    
    for (int i = 8; i <= 23; i++) {
        GPIOs_configure(i, GPIO_MODE_USER_STD_BIDIRECTIONAL);
    }
    GPIOs_loadConfigs();
    
    User_enableIF();
    
    vgpio_write_output(1);
    
    USER_writeWord(0xFF, GPIO0_BASE + GPIO_DIR_OFFSET);
    USER_writeWord(0xFF, GPIO1_BASE + GPIO_DIR_OFFSET);
    vgpio_write_output(2);
    
    USER_writeWord(0x12, GPIO0_BASE + GPIO_DATAO_OFFSET);
    USER_writeWord(0x34, GPIO1_BASE + GPIO_DATAO_OFFSET);
    vgpio_write_output(3);
    
    USER_writeWord(0x00, GPIO0_BASE + GPIO_DIR_OFFSET);
    USER_writeWord(0x00, GPIO1_BASE + GPIO_DIR_OFFSET);
    vgpio_write_output(4);
    
    uint32_t gpio0_read = USER_readWord(GPIO0_BASE + GPIO_DATAI_OFFSET);
    uint32_t gpio1_read = USER_readWord(GPIO1_BASE + GPIO_DATAI_OFFSET);
    vgpio_write_output(5);
    
    USER_writeWord(0x0F, GPIO0_BASE + GPIO_DIR_OFFSET);
    USER_writeWord(0xF0, GPIO1_BASE + GPIO_DIR_OFFSET);
    vgpio_write_output(6);
    
    USER_writeWord(0xAA, GPIO0_BASE + GPIO_DATAO_OFFSET);
    USER_writeWord(0x55, GPIO1_BASE + GPIO_DATAO_OFFSET);
    vgpio_write_output(7);
    
    gpio0_read = USER_readWord(GPIO0_BASE + GPIO_DATAI_OFFSET);
    gpio1_read = USER_readWord(GPIO1_BASE + GPIO_DATAI_OFFSET);
    vgpio_write_output(8);
}
