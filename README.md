# Caravel User Project - Dual 8-bit GPIO Controller

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

## Initial User Prompt
**"Create caravel user project that have 2 8 bit gpios and they can configurable using firmware"**

## Project Overview
This project implements a Caravel user project with two independent 8-bit GPIO controllers that can be configured and controlled via firmware through the Wishbone bus interface. Each GPIO port provides full bidirectional capability with individual pin control.

### Key Features
- ✅ Two independent 8-bit GPIO ports (GPIO0 and GPIO1)
- ✅ Firmware-configurable via Wishbone bus interface
- ✅ Individual pin direction control (input/output)
- ✅ Bidirectional I/O capability
- ✅ Memory-mapped register interface
- ✅ Integration with Caravel SoC harness

### Requirements
1. **Two 8-bit GPIO Peripherals**: Each GPIO must support 8 independent I/O pins
2. **Firmware Configuration**: All GPIO settings must be configurable through firmware
3. **Wishbone Interface**: Standard Wishbone B4 classic slave interface for bus communication
4. **Caravel Integration**: Proper integration with Caravel user_project_wrapper

## Technology Stack
- **RTL Language**: Verilog-2005
- **IP Cores**: EF_GPIO8 (NativeChips verified IP)
- **Bus Protocol**: Wishbone B4 Classic
- **SoC Harness**: Efabless Caravel
- **PDK**: Skywater 130nm
- **EDA Tools**: OpenLane 2, Yosys, Verilator
- **Verification**: Cocotb/PyUVM with caravel-cocotb framework

## Project Status
🔄 **In Progress** - Project Setup Phase

### Current Milestone
- Setting up project structure
- Creating documentation framework
- Planning IP integration strategy

## Architecture Overview

### System Block Diagram
```
┌─────────────────────────────────────────────────────────────┐
│                    Caravel SoC                              │
│  ┌────────────────────────────────────────────────────┐     │
│  │        Management SoC Core                         │     │
│  │                                                     │     │
│  └─────────────────┬──────────────────────────────────┘     │
│                    │ Wishbone Bus                           │
│                    │                                         │
│  ┌─────────────────▼──────────────────────────────────┐     │
│  │         user_project_wrapper                       │     │
│  │  ┌──────────────────────────────────────────────┐  │     │
│  │  │           user_project                       │  │     │
│  │  │                                              │  │     │
│  │  │  ┌──────────────────────────────────────┐   │  │     │
│  │  │  │  Wishbone Bus Splitter              │   │  │     │
│  │  │  │  (Address Decoder & Mux)            │   │  │     │
│  │  │  └─────┬────────────────┬───────────────┘   │  │     │
│  │  │        │                │                    │  │     │
│  │  │  ┌─────▼──────┐   ┌────▼────────┐          │  │     │
│  │  │  │  EF_GPIO8  │   │  EF_GPIO8   │          │  │     │
│  │  │  │   (GPIO0)  │   │   (GPIO1)   │          │  │     │
│  │  │  │  0x30000000│   │  0x30010000 │          │  │     │
│  │  │  └─────┬──────┘   └────┬────────┘          │  │     │
│  │  │        │[7:0]           │[7:0]              │  │     │
│  │  └────────┼────────────────┼───────────────────┘  │     │
│  │           │                │                      │     │
│  └───────────┼────────────────┼──────────────────────┘     │
│              │                │                            │
│  mprj_io[15:8]               mprj_io[23:16]               │
└──────────────┼────────────────┼────────────────────────────┘
               │                │
          GPIO0 Pins        GPIO1 Pins
         (8 bidirectional) (8 bidirectional)
```

### Address Map
| Peripheral | Base Address | Size   | Description                    |
|-----------|--------------|--------|--------------------------------|
| GPIO0     | 0x30000000   | 64KB   | 8-bit GPIO Port 0              |
| GPIO1     | 0x30010000   | 64KB   | 8-bit GPIO Port 1              |

## Directory Structure
```
trial_gpio/
├── docs/
│   ├── README.md -> ../README.md
│   ├── register_map.md          # Register definitions
│   ├── pad_map.md                # Pin assignments
│   ├── integration_notes.md      # Integration details
│   └── dv/
│       └── verification_handoff.md
├── verilog/
│   ├── rtl/
│   │   ├── user_project.v             # Main integration module
│   │   ├── user_project_wrapper.v     # Caravel wrapper
│   │   └── wishbone_bus_splitter.v    # Bus decoder/mux
│   ├── dv/
│   │   └── cocotb/
│   │       ├── gpio0_test/
│   │       ├── gpio1_test/
│   │       └── system_integration_test/
│   └── includes/
├── fw/
│   ├── gpio.h                    # GPIO register definitions
│   └── gpio_test.c               # Firmware test
├── ip/
│   └── link_IPs.json             # IP integration manifest
└── openlane/
    ├── user_project/
    └── user_project_wrapper/
```

## Implementation Plan

### Phase 1: Project Setup ✅
- [x] Copy Caravel template structure
- [x] Create documentation framework
- [x] Initialize README with requirements
- [ ] Create register_map.md
- [ ] Create pad_map.md
- [ ] Create integration_notes.md

### Phase 2: RTL Development
- [ ] Link EF_GPIO8 IP cores using ipm_linker
- [ ] Create user_project with Wishbone bus splitter
- [ ] Integrate GPIO0 and GPIO1 peripherals
- [ ] Create user_project_wrapper
- [ ] Verify against Caravel RTL Acceptance Checklist

### Phase 3: Verification
- [ ] Develop cocotb testbenches for each GPIO
- [ ] Create system integration tests
- [ ] Run caravel-cocotb verification
- [ ] Generate test reports and waveforms

### Phase 4: Documentation
- [ ] Complete all technical documentation
- [ ] Generate final reports
- [ ] Create retrospective analysis

## Next Steps
1. Create detailed register map documentation
2. Define pad assignments for GPIO pins
3. Link EF_GPIO8 IP cores
4. Implement user_project integration module

## References
- [Caravel Documentation](https://caravel-harness.readthedocs.io/)
- [Wishbone B4 Specification](https://opencores.org/howto/wishbone)
- [EF_GPIO8 IP Documentation](See ip/ directory after linking)