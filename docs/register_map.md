# Register Map - Dual GPIO Controller

## Overview
This document defines the register map for the dual 8-bit GPIO controller implementation in the Caravel user project. The design integrates two EF_GPIO8 IP cores with dedicated address spaces.

## Address Space Allocation

### Base Addresses
| Peripheral | Base Address | End Address  | Size  | Description                    |
|-----------|--------------|--------------|-------|--------------------------------|
| GPIO0     | 0x3000_0000  | 0x3000_FFFF  | 64KB  | 8-bit GPIO Port 0              |
| GPIO1     | 0x3001_0000  | 0x3001_FFFF  | 64KB  | 8-bit GPIO Port 1              |

### Address Decoding
- **Decoder bits**: `wbs_adr_i[16]` (bit 16 selects between GPIO0 and GPIO1)
- **GPIO0**: `wbs_adr_i[16] == 0`
- **GPIO1**: `wbs_adr_i[16] == 1`

## GPIO Register Map (EF_GPIO8)

Each GPIO peripheral has the following register layout:

### Data Registers

#### DATAI - Data Input Register (Offset: 0x0000)
| Bits  | Name   | Type | Reset | Description                              |
|-------|--------|------|-------|------------------------------------------|
| [7:0] | DATAI  | RO   | 0x00  | Current state of GPIO input pins         |
| [31:8]| -      | -    | 0x00  | Reserved                                 |

**Description**: Read-only register reflecting the current logic level on each GPIO pin configured as input.

#### DATAO - Data Output Register (Offset: 0x0004)
| Bits  | Name   | Type | Reset | Description                              |
|-------|--------|------|-------|------------------------------------------|
| [7:0] | DATAO  | WO   | 0x00  | Data to drive on GPIO output pins        |
| [31:8]| -      | -    | 0x00  | Reserved                                 |

**Description**: Write-only register to set output values for pins configured as outputs.

#### DIR - Direction Control Register (Offset: 0x0008)
| Bits  | Name   | Type | Reset | Description                              |
|-------|--------|------|-------|------------------------------------------|
| [7:0] | DIR    | WO   | 0x00  | Pin direction control (0=Input, 1=Output)|
| [31:8]| -      | -    | 0x00  | Reserved                                 |

**Description**: Write-only register to configure each pin as input (0) or output (1).
- Bit 0: Pin 0 direction
- Bit 1: Pin 1 direction
- ...
- Bit 7: Pin 7 direction

### Interrupt Registers

#### IM - Interrupt Mask Register (Offset: 0xFF00)
| Bits    | Name     | Type | Reset      | Description                           |
|---------|----------|------|------------|---------------------------------------|
| [7:0]   | P[7:0]HI | RW   | 0x00       | Pin high-level interrupt mask         |
| [15:8]  | P[7:0]LO | RW   | 0x00       | Pin low-level interrupt mask          |
| [23:16] | P[7:0]PE | RW   | 0x00       | Pin positive-edge interrupt mask      |
| [31:24] | P[7:0]NE | RW   | 0x00       | Pin negative-edge interrupt mask      |

**Description**: Enable/disable interrupt sources. Set bit to 1 to enable corresponding interrupt.

#### MIS - Masked Interrupt Status Register (Offset: 0xFF04)
| Bits    | Name     | Type | Reset      | Description                           |
|---------|----------|------|------------|---------------------------------------|
| [7:0]   | P[7:0]HI | RO   | 0x00       | Masked pin high-level interrupt       |
| [15:8]  | P[7:0]LO | RO   | 0x00       | Masked pin low-level interrupt        |
| [23:16] | P[7:0]PE | RO   | 0x00       | Masked pin positive-edge interrupt    |
| [31:24] | P[7:0]NE | RO   | 0x00       | Masked pin negative-edge interrupt    |

**Description**: Shows active interrupts after masking (IRQ = MIS & IM).

#### RIS - Raw Interrupt Status Register (Offset: 0xFF08)
| Bits    | Name     | Type | Reset      | Description                           |
|---------|----------|------|------------|---------------------------------------|
| [7:0]   | P[7:0]HI | RO   | 0x00       | Raw pin high-level interrupt          |
| [15:8]  | P[7:0]LO | RO   | 0x00       | Raw pin low-level interrupt           |
| [23:16] | P[7:0]PE | RO   | 0x00       | Raw pin positive-edge interrupt       |
| [31:24] | P[7:0]NE | RO   | 0x00       | Raw pin negative-edge interrupt       |

**Description**: Shows raw interrupt status before masking.

#### IC - Interrupt Clear Register (Offset: 0xFF0C)
| Bits    | Name     | Type | Reset      | Description                           |
|---------|----------|------|------------|---------------------------------------|
| [7:0]   | P[7:0]HI | WO   | 0x00       | Clear pin high-level interrupt        |
| [15:8]  | P[7:0]LO | WO   | 0x00       | Clear pin low-level interrupt         |
| [23:16] | P[7:0]PE | WO   | 0x00       | Clear pin positive-edge interrupt     |
| [31:24] | P[7:0]NE | WO   | 0x00       | Clear pin negative-edge interrupt     |

**Description**: Write 1 to clear corresponding interrupt flag (W1C - Write-1-to-Clear).

## Complete Address Map

### GPIO0 Registers (Base: 0x3000_0000)
| Address      | Register | Access | Description                    |
|-------------|----------|--------|--------------------------------|
| 0x3000_0000 | DATAI    | RO     | GPIO0 Data Input               |
| 0x3000_0004 | DATAO    | WO     | GPIO0 Data Output              |
| 0x3000_0008 | DIR      | WO     | GPIO0 Direction Control        |
| 0x3000_FF00 | IM       | RW     | GPIO0 Interrupt Mask           |
| 0x3000_FF04 | MIS      | RO     | GPIO0 Masked Interrupt Status  |
| 0x3000_FF08 | RIS      | RO     | GPIO0 Raw Interrupt Status     |
| 0x3000_FF0C | IC       | WO     | GPIO0 Interrupt Clear          |

### GPIO1 Registers (Base: 0x3001_0000)
| Address      | Register | Access | Description                    |
|-------------|----------|--------|--------------------------------|
| 0x3001_0000 | DATAI    | RO     | GPIO1 Data Input               |
| 0x3001_0004 | DATAO    | WO     | GPIO1 Data Output              |
| 0x3001_0008 | DIR      | WO     | GPIO1 Direction Control        |
| 0x3001_FF00 | IM       | RW     | GPIO1 Interrupt Mask           |
| 0x3001_FF04 | MIS      | RO     | GPIO1 Masked Interrupt Status  |
| 0x3001_FF08 | RIS      | RO     | GPIO1 Raw Interrupt Status     |
| 0x3001_FF0C | IC       | WO     | GPIO1 Interrupt Clear          |

## Firmware Usage Examples

### Configure Pin as Output
```c
#define GPIO0_BASE 0x30000000
#define GPIO_DIR_OFFSET 0x08

// Set pin 0 as output
*(volatile uint32_t*)(GPIO0_BASE + GPIO_DIR_OFFSET) = 0x01;
```

### Write to Output Pin
```c
#define GPIO_DATAO_OFFSET 0x04

// Set pin 0 high
*(volatile uint32_t*)(GPIO0_BASE + GPIO_DATAO_OFFSET) = 0x01;
```

### Read from Input Pin
```c
#define GPIO_DATAI_OFFSET 0x00

// Read all GPIO0 pins
uint32_t input = *(volatile uint32_t*)(GPIO0_BASE + GPIO_DATAI_OFFSET);
```

### Enable Interrupts
```c
#define GPIO_IM_OFFSET 0xFF00

// Enable positive-edge interrupt on pin 0
*(volatile uint32_t*)(GPIO0_BASE + GPIO_IM_OFFSET) = 0x00010000;
```

## Notes
1. All registers are 32-bit aligned and must be accessed as full words
2. Unused bits return 0 on read and are ignored on write
3. Write-only registers return 0 on read
4. Invalid address accesses return 0xDEADBEEF on read
5. The Wishbone bus uses byte-lane selection via `wbs_sel_i`

