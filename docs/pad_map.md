# Pad Map - Dual GPIO Controller

## Overview
This document defines the pad assignments for the dual 8-bit GPIO controller in the Caravel user project. The design uses 16 bidirectional I/O pads from the `mprj_io[37:0]` array to connect GPIO0 and GPIO1 to the outside world.

## Caravel I/O Pad Structure
The Caravel harness provides 38 configurable multipurpose I/O pads (`mprj_io[37:0]`). Each pad has three signals:
- **mprj_io_in[N]**: Input data from pad to user project
- **mprj_io_out[N]**: Output data from user project to pad
- **mprj_io_oeb[N]**: Output enable bar (Active Low) - 0=Output enabled, 1=Input/Hi-Z

## Reserved Pads
**WARNING**: Pads `mprj_io[4:0]` are reserved for Caravel housekeeping and MUST NOT be used by user projects.

## GPIO Pad Assignments

### GPIO0 - 8-bit Port (mprj_io[15:8])
| mprj_io Pin | GPIO Pin | Signal Name | Direction    | Description                |
|------------|----------|-------------|--------------|----------------------------|
| mprj_io[8] | GPIO0[0] | gpio0_io[0] | Bidirectional| GPIO0 Pin 0                |
| mprj_io[9] | GPIO0[1] | gpio0_io[1] | Bidirectional| GPIO0 Pin 1                |
| mprj_io[10]| GPIO0[2] | gpio0_io[2] | Bidirectional| GPIO0 Pin 2                |
| mprj_io[11]| GPIO0[3] | gpio0_io[3] | Bidirectional| GPIO0 Pin 3                |
| mprj_io[12]| GPIO0[4] | gpio0_io[4] | Bidirectional| GPIO0 Pin 4                |
| mprj_io[13]| GPIO0[5] | gpio0_io[5] | Bidirectional| GPIO0 Pin 5                |
| mprj_io[14]| GPIO0[6] | gpio0_io[6] | Bidirectional| GPIO0 Pin 6                |
| mprj_io[15]| GPIO0[7] | gpio0_io[7] | Bidirectional| GPIO0 Pin 7                |

### GPIO1 - 8-bit Port (mprj_io[23:16])
| mprj_io Pin | GPIO Pin | Signal Name | Direction    | Description                |
|------------|----------|-------------|--------------|----------------------------|
| mprj_io[16]| GPIO1[0] | gpio1_io[0] | Bidirectional| GPIO1 Pin 0                |
| mprj_io[17]| GPIO1[1] | gpio1_io[1] | Bidirectional| GPIO1 Pin 1                |
| mprj_io[18]| GPIO1[2] | gpio1_io[2] | Bidirectional| GPIO1 Pin 2                |
| mprj_io[19]| GPIO1[3] | gpio1_io[3] | Bidirectional| GPIO1 Pin 3                |
| mprj_io[20]| GPIO1[4] | gpio1_io[4] | Bidirectional| GPIO1 Pin 4                |
| mprj_io[21]| GPIO1[5] | gpio1_io[5] | Bidirectional| GPIO1 Pin 5                |
| mprj_io[22]| GPIO1[6] | gpio1_io[6] | Bidirectional| GPIO1 Pin 6                |
| mprj_io[23]| GPIO1[7] | gpio1_io[7] | Bidirectional| GPIO1 Pin 7                |

## Complete Pad Allocation Table
| mprj_io Range | Usage        | Peripheral | Notes                              |
|--------------|--------------|------------|------------------------------------|
| [4:0]        | Reserved     | Caravel    | Housekeeping SPI, UART, etc.       |
| [7:5]        | Unused       | -          | Available for future expansion     |
| [15:8]       | GPIO Port    | GPIO0      | 8-bit bidirectional GPIO           |
| [23:16]      | GPIO Port    | GPIO1      | 8-bit bidirectional GPIO           |
| [37:24]      | Unused       | -          | Available for future expansion     |

## Pad Configuration Details

### Bidirectional I/O Operation
Each GPIO pin is connected as a bidirectional pad using the following wiring pattern in `user_project_wrapper.v`:

```verilog
// For GPIO0 pin N (where N = 0 to 7):
assign gpio0_io_in[N] = mprj_io_in[8+N];      // Input path
assign mprj_io_out[8+N] = gpio0_io_out[N];     // Output path
assign mprj_io_oeb[8+N] = ~gpio0_io_oe[N];     // Output enable (inverted)

// For GPIO1 pin N (where N = 0 to 7):
assign gpio1_io_in[N] = mprj_io_in[16+N];     // Input path
assign mprj_io_out[16+N] = gpio1_io_out[N];    // Output path
assign mprj_io_oeb[16+N] = ~gpio1_io_oe[N];    // Output enable (inverted)
```

### Direction Control Logic
- **When DIR bit = 0 (Input mode)**:
  - `io_oe` = 0 → `mprj_io_oeb` = 1 (Output driver disabled)
  - Pin is in high-impedance state
  - External signal can drive the pin
  - Value read from `io_in` reflects pad state

- **When DIR bit = 1 (Output mode)**:
  - `io_oe` = 1 → `mprj_io_oeb` = 0 (Output driver enabled)
  - Pin drives value from `io_out` (DATAO register)
  - External inputs are ignored

## Physical Pin Mapping (Example - Caravel Chip)
The following shows approximate physical locations on the Caravel die (refer to Caravel documentation for exact coordinates):

```
                    TOP
        ┌──────────────────────────┐
        │   mprj_io[23:16] (GPIO1) │
   LEFT │                          │ RIGHT
        │   mprj_io[15:8]  (GPIO0) │
        │                          │
        └──────────────────────────┘
                   BOTTOM
```

## Firmware Configuration
No firmware configuration is required for pad assignment - the connections are hardwired in `user_project_wrapper.v`. However, the GPIO direction must be configured via firmware:

```c
// Configure GPIO0 pins [3:0] as outputs, [7:4] as inputs
#define GPIO0_BASE 0x30000000
#define GPIO_DIR_OFFSET 0x08
*(volatile uint32_t*)(GPIO0_BASE + GPIO_DIR_OFFSET) = 0x0F;  // 0b00001111
```

## Expansion Options
To modify pad assignments:

1. Edit `user_project_wrapper.v` to change the `mprj_io` index assignments
2. Avoid pads [4:0] (reserved)
3. Update this documentation to reflect changes
4. Re-run verification tests to ensure proper operation

## Example Alternative Pad Assignment
If different pad locations are needed (e.g., for PCB routing optimization):

```verilog
// Alternative: Use mprj_io[29:22] for GPIO0
assign gpio0_io_in = mprj_io_in[29:22];
assign mprj_io_out[29:22] = gpio0_io_out;
assign mprj_io_oeb[29:22] = ~gpio0_io_oe;
```

## Testing and Verification
During verification:
1. GPIO0 uses `mprj_io[15:8]` in simulation
2. GPIO1 uses `mprj_io[23:16]` in simulation
3. Testbenches monitor/drive these specific pad indices
4. Ensure no conflicts with other user project signals

## Notes
1. All GPIO pads support bidirectional operation
2. Default state after reset: All pins configured as inputs (high-Z)
3. Pad drive strength is determined by Caravel pad cell configuration
4. Pull-up/pull-down resistors are not available on these pads
5. Maximum operating frequency depends on pad loading and PCB design
