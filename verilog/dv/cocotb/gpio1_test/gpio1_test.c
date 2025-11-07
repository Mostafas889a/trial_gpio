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

uint16_t vgpio_read_input(void) {
    volatile uint32_t *vgpio_reg = (volatile uint32_t *)VGPIO_REG_ADDR;
    uint32_t reg_val = *vgpio_reg;
    return (uint16_t)((reg_val >> 16) & 0xFFFF);
}

void main(void) {
    enableHkSpi(false);
    
    GPIOs_configure(16, GPIO_MODE_USER_STD_BIDIRECTIONAL);
    GPIOs_configure(17, GPIO_MODE_USER_STD_BIDIRECTIONAL);
    GPIOs_configure(18, GPIO_MODE_USER_STD_BIDIRECTIONAL);
    GPIOs_configure(19, GPIO_MODE_USER_STD_BIDIRECTIONAL);
    GPIOs_configure(20, GPIO_MODE_USER_STD_BIDIRECTIONAL);
    GPIOs_configure(21, GPIO_MODE_USER_STD_BIDIRECTIONAL);
    GPIOs_configure(22, GPIO_MODE_USER_STD_BIDIRECTIONAL);
    GPIOs_configure(23, GPIO_MODE_USER_STD_BIDIRECTIONAL);
    GPIOs_loadConfigs();
    
    User_enableIF();
    
    vgpio_write_output(1);
    
    USER_writeWord(0x00, GPIO1_BASE + GPIO_DIR_OFFSET);
    vgpio_write_output(2);
    
    uint32_t read_val = USER_readWord(GPIO1_BASE + GPIO_DATAI_OFFSET);
    vgpio_write_output(3);
    
    USER_writeWord(0xFF, GPIO1_BASE + GPIO_DIR_OFFSET);
    vgpio_write_output(4);
    
    USER_writeWord(0xCC, GPIO1_BASE + GPIO_DATAO_OFFSET);
    vgpio_write_output(5);
    
    USER_writeWord(0x33, GPIO1_BASE + GPIO_DATAO_OFFSET);
    vgpio_write_output(6);
    
    USER_writeWord(0x00, GPIO1_BASE + GPIO_DATAO_OFFSET);
    vgpio_write_output(7);
}
