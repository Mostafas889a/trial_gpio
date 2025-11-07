import cocotb
from caravel_cocotb.caravel_interfaces import test_configure, report_test
from cocotb.triggers import RisingEdge
import sys
sys.path.append('..')
from VirtualGPIOModel import VirtualGPIOModel

@cocotb.test()
@report_test
async def gpio1_test(dut):
    caravelEnv = await test_configure(dut, timeout_cycles=1000000)
    cocotb.log.info("[TEST] start gpio1_test")
    await caravelEnv.release_csb()

    vgpio = VirtualGPIOModel(caravelEnv)
    vgpio.start()

    cocotb.log.info("[TEST] Waiting for firmware signal sequence via Virtual GPIO...")

    await vgpio.wait_output(1)
    cocotb.log.info("[TEST] Firmware ready - GPIO pads configured")

    await vgpio.wait_output(2)
    cocotb.log.info("[TEST] GPIO1 direction set to input (0x00)")

    gpio_in_val = 0x5A
    caravelEnv.drive_gpio_in((23, 16), gpio_in_val)
    cocotb.log.info(f"[TEST] Driving GPIO1 inputs with pattern 0x{gpio_in_val:02X}")

    await vgpio.wait_output(3)
    cocotb.log.info("[TEST] Firmware read GPIO1 input data")

    await vgpio.wait_output(4)
    cocotb.log.info("[TEST] GPIO1 direction set to output (0xFF)")

    await vgpio.wait_output(5)
    cocotb.log.info("[TEST] GPIO1 output set to 0xCC")
    
    await RisingEdge(caravelEnv.clk)
    await RisingEdge(caravelEnv.clk)
    await RisingEdge(caravelEnv.clk)
    await RisingEdge(caravelEnv.clk)
    
    gpio_out = caravelEnv.monitor_gpio(23, 16).binstr
    cocotb.log.info(f"[TEST] Observed GPIO1 output: {gpio_out}")
    assert gpio_out[-1] == '0', f"GPIO[16] expected 0 (LSB of 0xCC), got {gpio_out[-1]}"
    assert gpio_out[-2] == '0', f"GPIO[17] expected 0, got {gpio_out[-2]}"
    assert gpio_out[-3] == '1', f"GPIO[18] expected 1, got {gpio_out[-3]}"

    await vgpio.wait_output(6)
    cocotb.log.info("[TEST] GPIO1 output set to 0x33")
    
    await RisingEdge(caravelEnv.clk)
    await RisingEdge(caravelEnv.clk)
    await RisingEdge(caravelEnv.clk)
    await RisingEdge(caravelEnv.clk)
    
    gpio_out = caravelEnv.monitor_gpio(23, 16).binstr
    cocotb.log.info(f"[TEST] Observed GPIO1 output: {gpio_out}")
    assert gpio_out[-1] == '1', f"GPIO[16] expected 1 (LSB of 0x33), got {gpio_out[-1]}"
    assert gpio_out[-2] == '1', f"GPIO[17] expected 1, got {gpio_out[-2]}"
    assert gpio_out[-3] == '0', f"GPIO[18] expected 0, got {gpio_out[-3]}"

    await vgpio.wait_output(7)
    cocotb.log.info("[TEST] GPIO1 output set to 0x00")
    
    await RisingEdge(caravelEnv.clk)
    await RisingEdge(caravelEnv.clk)
    await RisingEdge(caravelEnv.clk)
    await RisingEdge(caravelEnv.clk)
    
    gpio_out = caravelEnv.monitor_gpio(23, 16).binstr
    cocotb.log.info(f"[TEST] Observed GPIO1 output: {gpio_out}")
    
    cocotb.log.info("[TEST] GPIO1 test completed successfully")
