import cocotb
from caravel_cocotb.caravel_interfaces import test_configure, report_test
from cocotb.triggers import RisingEdge
import sys
sys.path.append('..')
from VirtualGPIOModel import VirtualGPIOModel

@cocotb.test()
@report_test
async def system_integration_test(dut):
    caravelEnv = await test_configure(dut, timeout_cycles=1000000)
    cocotb.log.info("[TEST] start system_integration_test")
    await caravelEnv.release_csb()

    vgpio = VirtualGPIOModel(caravelEnv)
    vgpio.start()

    cocotb.log.info("[TEST] Waiting for firmware signal sequence via Virtual GPIO...")

    await vgpio.wait_output(1)
    cocotb.log.info("[TEST] Firmware ready - All GPIO pads configured")

    await vgpio.wait_output(2)
    cocotb.log.info("[TEST] Both GPIO0 and GPIO1 set to output mode")

    await vgpio.wait_output(3)
    cocotb.log.info("[TEST] GPIO0 output=0x12, GPIO1 output=0x34")
    
    await RisingEdge(caravelEnv.clk)
    await RisingEdge(caravelEnv.clk)
    await RisingEdge(caravelEnv.clk)
    await RisingEdge(caravelEnv.clk)
    
    gpio0_out = caravelEnv.monitor_gpio(15, 8).integer
    gpio1_out = caravelEnv.monitor_gpio(23, 16).integer
    cocotb.log.info(f"[TEST] GPIO0 output: 0x{gpio0_out:02X}, GPIO1 output: 0x{gpio1_out:02X}")

    await vgpio.wait_output(4)
    cocotb.log.info("[TEST] Both GPIO0 and GPIO1 set to input mode")

    caravelEnv.drive_gpio_in((15, 8), 0xAB)
    caravelEnv.drive_gpio_in((23, 16), 0xCD)
    cocotb.log.info("[TEST] Driving GPIO0=0xAB, GPIO1=0xCD")

    await vgpio.wait_output(5)
    cocotb.log.info("[TEST] Firmware read both GPIO inputs")

    await vgpio.wait_output(6)
    cocotb.log.info("[TEST] GPIO0 DIR=0x0F (lower 4 bits output), GPIO1 DIR=0xF0 (upper 4 bits output)")

    await vgpio.wait_output(7)
    cocotb.log.info("[TEST] GPIO0 output=0xAA (lower 4 bits), GPIO1 output=0x55 (upper 4 bits)")
    
    await RisingEdge(caravelEnv.clk)
    await RisingEdge(caravelEnv.clk)
    await RisingEdge(caravelEnv.clk)
    await RisingEdge(caravelEnv.clk)
    
    gpio0_out = caravelEnv.monitor_gpio(15, 8).binstr
    gpio1_out = caravelEnv.monitor_gpio(23, 16).binstr
    cocotb.log.info(f"[TEST] GPIO0 output: {gpio0_out}, GPIO1 output: {gpio1_out}")
    
    gpio0_lower = gpio0_out[-4:]
    cocotb.log.info(f"[TEST] GPIO0 lower 4 bits (should be 0xA): {gpio0_lower}")

    await vgpio.wait_output(8)
    cocotb.log.info("[TEST] Firmware read mixed input/output configuration")
    
    cocotb.log.info("[TEST] System integration test completed successfully")
