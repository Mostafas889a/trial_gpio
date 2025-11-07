# Integration Notes - Dual GPIO Controller

## Overview
This document provides detailed technical information about integrating the dual 8-bit GPIO controller into the Caravel user project, including clocking, reset architecture, bus timing, interrupt mapping, and verification procedures.

## Clock and Reset Architecture

### Clock Domain
- **Single Clock Domain**: The entire design operates on a single clock domain
- **Clock Source**: `wb_clk_i` from Caravel management SoC
- **Clock Frequency**: Designed for 25 MHz operation (40ns period)
- **No Clock Gating**: Clock gating is explicitly avoided for simplicity

### Reset Strategy
- **Reset Signal**: `wb_rst_i` (Synchronous, Active-High)
- **Reset Type**: Synchronous reset for all sequential elements
- **Reset Path**: Management SoC → user_project_wrapper → user_project → GPIO peripherals
- **Reset Behavior**:
  - All GPIO pins default to input mode (DIR = 0x00)
  - All output registers cleared (DATAO = 0x00)
  - All interrupt masks disabled (IM = 0x00000000)
  - All interrupt flags cleared

## Wishbone Bus Integration

### Bus Protocol
- **Protocol**: Wishbone B4 Classic
- **Data Width**: 32 bits
- **Address Width**: 32 bits
- **Byte Addressing**: Supported via `wbs_sel_i[3:0]`
- **Transfer Mode**: Single cycle read/write

### Signal Descriptions
| Signal       | Direction | Width | Description                              |
|-------------|-----------|-------|------------------------------------------|
| wb_clk_i    | Input     | 1     | System clock                             |
| wb_rst_i    | Input     | 1     | System reset (sync, active-high)         |
| wbs_cyc_i   | Input     | 1     | Bus cycle indicator                      |
| wbs_stb_i   | Input     | 1     | Strobe - address/data valid              |
| wbs_we_i    | Input     | 1     | Write enable (1=write, 0=read)           |
| wbs_sel_i   | Input     | 4     | Byte lane select                         |
| wbs_adr_i   | Input     | 32    | Address bus                              |
| wbs_dat_i   | Input     | 32    | Data input (write data)                  |
| wbs_dat_o   | Output    | 32    | Data output (read data)                  |
| wbs_ack_o   | Output    | 1     | Transfer acknowledge                     |

### Bus Timing

#### Read Transaction
```
Clock:    ___/‾‾‾\_/‾‾‾\_/‾‾‾\_/‾‾‾\_
cyc_i:    _______/‾‾‾‾‾‾‾‾‾‾‾‾\_____
stb_i:    _______/‾‾‾‾‾‾‾‾‾‾‾‾\_____
we_i:     ___________________________
adr_i:    _______<VALID_ADDR>_______
dat_o:    _____________<DATA>_______
ack_o:    _____________/‾‾‾\_________
```

#### Write Transaction
```
Clock:    ___/‾‾‾\_/‾‾‾\_/‾‾‾\_/‾‾‾\_
cyc_i:    _______/‾‾‾‾‾‾‾‾‾‾‾‾\_____
stb_i:    _______/‾‾‾‾‾‾‾‾‾‾‾‾\_____
we_i:     _______/‾‾‾‾‾‾‾‾‾‾‾‾\_____
adr_i:    _______<VALID_ADDR>_______
dat_i:    _______<VALID_DATA>_______
ack_o:    _____________/‾‾‾\_________
```

### Bus Latency
- **Read Latency**: 1 clock cycle (data available on cycle after request)
- **Write Latency**: 1 clock cycle (acknowledge on cycle after request)
- **Pipeline**: Not pipelined - single outstanding transaction

### Address Decoding
The design uses a 2-peripheral Wishbone bus splitter with the following decoding:

```verilog
// Address decode based on bit 16
wire [0:0] peripheral_sel = wbs_adr_i[16];

// GPIO0: wbs_adr_i[16] == 0 (0x30000000 - 0x3000FFFF)
// GPIO1: wbs_adr_i[16] == 1 (0x30010000 - 0x3001FFFF)

// Strobe generation (CRITICAL: Never gate cyc_i!)
assign gpio0_stb = wbs_stb_i & (peripheral_sel == 1'b0);
assign gpio1_stb = wbs_stb_i & (peripheral_sel == 1'b1);

// cyc_i routed unmodified to both peripherals
assign gpio0_cyc = wbs_cyc_i;
assign gpio1_cyc = wbs_cyc_i;
```

### Invalid Address Handling
- **Invalid Reads**: Return `0xDEADBEEF`
- **Invalid Writes**: Acknowledge but discard data
- **No Hang**: Always acknowledge to prevent bus lockup

## Interrupt Integration

### Interrupt Signals
- **GPIO0 IRQ**: Connected to `user_irq[0]`
- **GPIO1 IRQ**: Connected to `user_irq[1]`
- **IRQ Type**: Level-sensitive, active-high
- **IRQ Logic**: OR of all masked interrupt sources

### Interrupt Sources per GPIO
Each GPIO peripheral provides multiple interrupt sources:
- **Pin High (HI)**: Level-sensitive, pin is logic 1
- **Pin Low (LO)**: Level-sensitive, pin is logic 0
- **Positive Edge (PE)**: Edge-sensitive, rising edge detected
- **Negative Edge (NE)**: Edge-sensitive, falling edge detected

### Interrupt Handling Flow
1. **Event Detection**: Hardware detects pin state change
2. **RIS Update**: Raw Interrupt Status (RIS) register is set
3. **Masking**: Masked Interrupt Status (MIS) = RIS & IM
4. **IRQ Generation**: IRQ = |MIS (OR of all MIS bits)
5. **Software Response**:
   - Read MIS to identify interrupt source
   - Service the interrupt
   - Write IC (Interrupt Clear) to clear the flag
   - Clear propagates: IC → RIS → MIS → IRQ

### Interrupt Service Example
```c
void gpio0_isr(void) {
    uint32_t mis = *(volatile uint32_t*)(GPIO0_BASE + MIS_OFFSET);
    
    if (mis & EF_GPIO8_P0PE_FLAG) {
        // Handle pin 0 positive edge
        *(volatile uint32_t*)(GPIO0_BASE + IC_OFFSET) = EF_GPIO8_P0PE_FLAG;
    }
    // Handle other sources...
}
```

## Design Hierarchy

### Module Structure
```
user_project_wrapper
├── user_project
│   ├── wishbone_bus_splitter (NUM_PERIPHERALS=2)
│   ├── EF_GPIO8_WB (GPIO0 instance)
│   │   └── EF_GPIO8 (GPIO0 core)
│   └── EF_GPIO8_WB (GPIO1 instance)
│       └── EF_GPIO8 (GPIO1 core)
```

### Signal Flow
```
Caravel Management SoC
    ↓ (Wishbone Bus)
user_project_wrapper (pass-through)
    ↓ (Wishbone Bus)
user_project
    ↓ (wishbone_bus_splitter)
    ├─→ EF_GPIO8_WB (GPIO0) ←→ mprj_io[15:8]
    └─→ EF_GPIO8_WB (GPIO1) ←→ mprj_io[23:16]
```

## IP Core Integration

### EF_GPIO8 IP
- **Source**: NativeChips Verified IP Library
- **Version**: v1.1.0
- **Location**: `/nc/ip/EF_GPIO8/v1.1.0`
- **Wrapper**: `EF_GPIO8_WB.v` (Wishbone interface)
- **Core**: `EF_GPIO8.v` (GPIO logic)

### IP Linking
IPs are linked using the `ipm_linker` tool:

```bash
python /nc/agent_tools/ipm_linker/ipm_linker.py \
    --file /workspace/trial_gpio/ip/link_IPs.json \
    --project-root /workspace/trial_gpio
```

### IP Files Required
- `EF_GPIO8.v` - GPIO core logic
- `EF_GPIO8_WB.v` - Wishbone wrapper
- `EF_GPIO8_regs.h` - Firmware header

## Synthesis and Timing

### Synthesis Strategy
- **Tool**: Yosys (open-source)
- **Target**: Sky130 PDK
- **Optimization**: Area-optimized with timing constraints

### Critical Paths
1. **Wishbone to Register**: Address decode → register write
2. **Register to Pad**: Output register → pad driver
3. **Pad to Register**: Pad input → input register
4. **Interrupt Logic**: Pin change detect → IRQ output

### Timing Constraints
```tcl
# Clock definition
create_clock -name wb_clk_i -period 40.0 [get_ports wb_clk_i]

# Input delays (relative to clock)
set_input_delay -clock wb_clk_i -max 10.0 [get_ports wbs_*]
set_input_delay -clock wb_clk_i -max 10.0 [get_ports io_in]

# Output delays (relative to clock)
set_output_delay -clock wb_clk_i -max 10.0 [get_ports wbs_*_o]
set_output_delay -clock wb_clk_i -max 10.0 [get_ports io_out]
set_output_delay -clock wb_clk_i -max 10.0 [get_ports io_oe]
```

## Verification Strategy

### Verification Levels
1. **Unit Level**: Individual GPIO peripheral testing
2. **Integration Level**: Wishbone bus splitter and multi-peripheral
3. **System Level**: Full Caravel integration with firmware

### Verification Framework
- **Framework**: Cocotb with caravel-cocotb harness
- **Language**: Python testbenches
- **Firmware**: C code compiled for RISC-V management core

### Test Structure
```
verilog/dv/cocotb/
├── gpio0_test/
│   ├── gpio0_test.py          # Cocotb testbench
│   ├── gpio0_test.c           # Firmware test code
│   └── gpio0_test.hex         # Compiled firmware
├── gpio1_test/
│   ├── gpio1_test.py
│   ├── gpio1_test.c
│   └── gpio1_test.hex
├── system_integration_test/
│   ├── system_test.py
│   ├── system_test.c
│   └── system_test.hex
├── cocotb_tests.py            # Test runner configuration
└── design_info.yaml           # Design configuration
```

### Test Coverage
- ✅ Basic read/write operations
- ✅ Direction control (input/output)
- ✅ Bidirectional I/O operation
- ✅ Interrupt generation and clearing
- ✅ Address decoding (GPIO0 vs GPIO1)
- ✅ Invalid address handling
- ✅ Bus protocol compliance
- ✅ Concurrent multi-GPIO operation

### Running Verification

#### Individual Peripheral Test
```bash
cd /workspace/trial_gpio/verilog/dv/cocotb/gpio0_test
make test
```

#### All Tests
```bash
cd /workspace/trial_gpio/verilog/dv/cocotb
pytest cocotb_tests.py -v
```

#### Waveform Analysis
```bash
gtkwave gpio0_test.vcd &
```

## Power Considerations

### Power Domains
- **Core Power**: vccd1 (1.8V digital core)
- **User Power**: vccd2 (1.8V user project area)
- **Ground**: vssd1/vssd2 (digital ground)

### Power Connections in user_project_wrapper
```verilog
`ifdef USE_POWER_PINS
    .VPWR(vccd2),
    .VGND(vssd2),
`endif
```

### Low Power Features
- Clock gating available in EF_GPIO8_WB (via GCLK register)
- Unused peripherals can be clock-gated if needed
- GPIO outputs only consume power when driving loads

## Design for Test (DFT)

### Scan Chain
- Not implemented in this design (optional for future)
- EF_GPIO8 has DFT-enabled wrappers available if needed

### Testability Features
- All registers accessible via Wishbone
- Interrupt status visible in registers
- Loopback testing possible (output → external wire → input)

## Known Limitations and Notes

### Limitations
1. **No Pull-Up/Pull-Down**: External resistors required for floating inputs
2. **No Analog Support**: Digital I/O only
3. **Fixed Pad Assignment**: Hardwired in wrapper (requires RTL change to modify)
4. **Interrupt Priority**: No hardware priority - firmware must handle
5. **No DMA**: All transfers via CPU/Wishbone

### Design Decisions
1. **Single Clock Domain**: Simplifies design, avoids CDC complexity
2. **Non-Power-of-2 Peripherals**: Using 2 peripherals (not 4, 8, etc.) enables automatic error detection for invalid addresses
3. **Synchronous Reset**: Easier timing closure, recommended for ASIC
4. **64KB Windows**: Large address space allows future register additions

### Future Expansion
- Additional GPIO ports can be added by:
  1. Adding more EF_GPIO8 instances
  2. Increasing `NUM_PERIPHERALS` in bus splitter
  3. Updating address decoder
  4. Assigning different `mprj_io` pads

## Troubleshooting

### Common Issues

#### Bus Hangs
- **Symptom**: System stops responding
- **Cause**: Missing ACK on Wishbone transaction
- **Solution**: Verify all address ranges generate ACK

#### IRQ Not Triggering
- **Symptom**: No interrupt service routine called
- **Cause**: Interrupt mask not enabled
- **Solution**: Write appropriate mask to IM register

#### Wrong Data Read
- **Symptom**: Unexpected values from registers
- **Cause**: Address decoding error
- **Solution**: Verify address calculation in firmware

#### Pad Not Responding
- **Symptom**: GPIO output doesn't change pad
- **Cause**: Pin not configured as output
- **Solution**: Write DIR register with appropriate value

## References
- [Caravel Documentation](https://caravel-harness.readthedocs.io/)
- [Wishbone B4 Specification](https://opencores.org/howto/wishbone)
- [EF_GPIO8 Datasheet](../ip/EF_GPIO8/v1.1.0/EF_GPIO8.pdf)
- [Register Map](register_map.md)
- [Pad Map](pad_map.md)
