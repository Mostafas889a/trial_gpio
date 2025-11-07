/*
 * SPDX-FileCopyrightText: 2025 NativeChips
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _GPIO_H_
#define _GPIO_H_

#include <stdint.h>

#define GPIO0_BASE 0x30000000
#define GPIO1_BASE 0x30010000

#define GPIO_DATAI_OFFSET  0x0000
#define GPIO_DATAO_OFFSET  0x0004
#define GPIO_DIR_OFFSET    0x0008
#define GPIO_IM_OFFSET     0xFF00
#define GPIO_MIS_OFFSET    0xFF04
#define GPIO_RIS_OFFSET    0xFF08
#define GPIO_IC_OFFSET     0xFF0C

#define GPIO_DATAI_REG(base)  (*(volatile uint32_t*)((base) + GPIO_DATAI_OFFSET))
#define GPIO_DATAO_REG(base)  (*(volatile uint32_t*)((base) + GPIO_DATAO_OFFSET))
#define GPIO_DIR_REG(base)    (*(volatile uint32_t*)((base) + GPIO_DIR_OFFSET))
#define GPIO_IM_REG(base)     (*(volatile uint32_t*)((base) + GPIO_IM_OFFSET))
#define GPIO_MIS_REG(base)    (*(volatile uint32_t*)((base) + GPIO_MIS_OFFSET))
#define GPIO_RIS_REG(base)    (*(volatile uint32_t*)((base) + GPIO_RIS_OFFSET))
#define GPIO_IC_REG(base)     (*(volatile uint32_t*)((base) + GPIO_IC_OFFSET))

#define GPIO_DIR_INPUT   0
#define GPIO_DIR_OUTPUT  1

static inline void gpio_set_direction(uint32_t base, uint8_t pin_mask) {
    GPIO_DIR_REG(base) = pin_mask;
}

static inline void gpio_write(uint32_t base, uint8_t value) {
    GPIO_DATAO_REG(base) = value;
}

static inline uint8_t gpio_read(uint32_t base) {
    return (uint8_t)(GPIO_DATAI_REG(base) & 0xFF);
}

static inline void gpio_set_pin(uint32_t base, uint8_t pin) {
    uint8_t current = gpio_read(base);
    gpio_write(base, current | (1 << pin));
}

static inline void gpio_clear_pin(uint32_t base, uint8_t pin) {
    uint8_t current = gpio_read(base);
    gpio_write(base, current & ~(1 << pin));
}

static inline void gpio_toggle_pin(uint32_t base, uint8_t pin) {
    uint8_t current = gpio_read(base);
    gpio_write(base, current ^ (1 << pin));
}

static inline void gpio_enable_irq(uint32_t base, uint32_t irq_mask) {
    GPIO_IM_REG(base) = irq_mask;
}

static inline uint32_t gpio_get_irq_status(uint32_t base) {
    return GPIO_MIS_REG(base);
}

static inline void gpio_clear_irq(uint32_t base, uint32_t irq_mask) {
    GPIO_IC_REG(base) = irq_mask;
}

#define GPIO_IRQ_P0HI  (1 << 0)
#define GPIO_IRQ_P1HI  (1 << 1)
#define GPIO_IRQ_P2HI  (1 << 2)
#define GPIO_IRQ_P3HI  (1 << 3)
#define GPIO_IRQ_P4HI  (1 << 4)
#define GPIO_IRQ_P5HI  (1 << 5)
#define GPIO_IRQ_P6HI  (1 << 6)
#define GPIO_IRQ_P7HI  (1 << 7)

#define GPIO_IRQ_P0LO  (1 << 8)
#define GPIO_IRQ_P1LO  (1 << 9)
#define GPIO_IRQ_P2LO  (1 << 10)
#define GPIO_IRQ_P3LO  (1 << 11)
#define GPIO_IRQ_P4LO  (1 << 12)
#define GPIO_IRQ_P5LO  (1 << 13)
#define GPIO_IRQ_P6LO  (1 << 14)
#define GPIO_IRQ_P7LO  (1 << 15)

#define GPIO_IRQ_P0PE  (1 << 16)
#define GPIO_IRQ_P1PE  (1 << 17)
#define GPIO_IRQ_P2PE  (1 << 18)
#define GPIO_IRQ_P3PE  (1 << 19)
#define GPIO_IRQ_P4PE  (1 << 20)
#define GPIO_IRQ_P5PE  (1 << 21)
#define GPIO_IRQ_P6PE  (1 << 22)
#define GPIO_IRQ_P7PE  (1 << 23)

#define GPIO_IRQ_P0NE  (1 << 24)
#define GPIO_IRQ_P1NE  (1 << 25)
#define GPIO_IRQ_P2NE  (1 << 26)
#define GPIO_IRQ_P3NE  (1 << 27)
#define GPIO_IRQ_P4NE  (1 << 28)
#define GPIO_IRQ_P5NE  (1 << 29)
#define GPIO_IRQ_P6NE  (1 << 30)
#define GPIO_IRQ_P7NE  (1 << 31)

#endif /* _GPIO_H_ */
