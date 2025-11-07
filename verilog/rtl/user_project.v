// SPDX-FileCopyrightText: 2025 NativeChips
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// SPDX-License-Identifier: Apache-2.0

`default_nettype none

module user_project #(
    parameter NUM_GPIO_PORTS = 2
)(
`ifdef USE_POWER_PINS
    inout vccd1,
    inout vssd1,
`endif

    input wb_clk_i,
    input wb_rst_i,
    input wbs_stb_i,
    input wbs_cyc_i,
    input wbs_we_i,
    input [3:0] wbs_sel_i,
    input [31:0] wbs_dat_i,
    input [31:0] wbs_adr_i,
    output wbs_ack_o,
    output [31:0] wbs_dat_o,

    input [127:0] la_data_in,
    output [127:0] la_data_out,
    input [127:0] la_oenb,

    input [15:0] io_in,
    output [15:0] io_out,
    output [15:0] io_oeb,

    output [2:0] irq
);

    wire [7:0] gpio0_io_in;
    wire [7:0] gpio0_io_out;
    wire [7:0] gpio0_io_oe;
    wire       gpio0_irq;

    wire [7:0] gpio1_io_in;
    wire [7:0] gpio1_io_out;
    wire [7:0] gpio1_io_oe;
    wire       gpio1_irq;

    assign gpio0_io_in = io_in[7:0];
    assign io_out[7:0] = gpio0_io_out;
    assign io_oeb[7:0] = ~gpio0_io_oe;

    assign gpio1_io_in = io_in[15:8];
    assign io_out[15:8] = gpio1_io_out;
    assign io_oeb[15:8] = ~gpio1_io_oe;

    assign irq[0] = gpio0_irq;
    assign irq[1] = gpio1_irq;
    assign irq[2] = 1'b0;

    assign la_data_out = 128'b0;

    wire [NUM_GPIO_PORTS-1:0] s_wb_cyc;
    wire [NUM_GPIO_PORTS-1:0] s_wb_stb;
    wire [NUM_GPIO_PORTS-1:0] s_wb_we;
    wire [NUM_GPIO_PORTS*4-1:0] s_wb_sel;
    wire [NUM_GPIO_PORTS*32-1:0] s_wb_adr;
    wire [NUM_GPIO_PORTS*32-1:0] s_wb_dat_m2s;
    wire [NUM_GPIO_PORTS*32-1:0] s_wb_dat_s2m;
    wire [NUM_GPIO_PORTS-1:0] s_wb_ack;
    wire [NUM_GPIO_PORTS-1:0] s_wb_err;

    wishbone_bus_splitter #(
        .NUM_PERIPHERALS(NUM_GPIO_PORTS),
        .ADDR_WIDTH(32),
        .DATA_WIDTH(32),
        .SEL_WIDTH(4),
        .ADDR_SEL_LOW_BIT(16)
    ) wb_splitter (
        .m_wb_adr_i(wbs_adr_i),
        .m_wb_dat_i(wbs_dat_i),
        .m_wb_dat_o(wbs_dat_o),
        .m_wb_we_i(wbs_we_i),
        .m_wb_sel_i(wbs_sel_i),
        .m_wb_cyc_i(wbs_cyc_i),
        .m_wb_stb_i(wbs_stb_i),
        .m_wb_ack_o(wbs_ack_o),
        .m_wb_err_o(),
        .s_wb_cyc_o(s_wb_cyc),
        .s_wb_stb_o(s_wb_stb),
        .s_wb_we_o(s_wb_we),
        .s_wb_sel_o(s_wb_sel),
        .s_wb_adr_o(s_wb_adr),
        .s_wb_dat_o(s_wb_dat_m2s),
        .s_wb_dat_i(s_wb_dat_s2m),
        .s_wb_ack_i(s_wb_ack),
        .s_wb_err_i(s_wb_err)
    );

    EF_GPIO8_WB gpio0 (
        .clk_i(wb_clk_i),
        .rst_i(wb_rst_i),
        .adr_i(s_wb_adr[31:0]),
        .dat_i(s_wb_dat_m2s[31:0]),
        .dat_o(s_wb_dat_s2m[31:0]),
        .sel_i(s_wb_sel[3:0]),
        .cyc_i(s_wb_cyc[0]),
        .stb_i(s_wb_stb[0]),
        .ack_o(s_wb_ack[0]),
        .we_i(s_wb_we[0]),
        .IRQ(gpio0_irq),
        .io_in(gpio0_io_in),
        .io_out(gpio0_io_out),
        .io_oe(gpio0_io_oe)
    );

    assign s_wb_err[0] = 1'b0;

    EF_GPIO8_WB gpio1 (
        .clk_i(wb_clk_i),
        .rst_i(wb_rst_i),
        .adr_i(s_wb_adr[63:32]),
        .dat_i(s_wb_dat_m2s[63:32]),
        .dat_o(s_wb_dat_s2m[63:32]),
        .sel_i(s_wb_sel[7:4]),
        .cyc_i(s_wb_cyc[1]),
        .stb_i(s_wb_stb[1]),
        .ack_o(s_wb_ack[1]),
        .we_i(s_wb_we[1]),
        .IRQ(gpio1_irq),
        .io_in(gpio1_io_in),
        .io_out(gpio1_io_out),
        .io_oe(gpio1_io_oe)
    );

    assign s_wb_err[1] = 1'b0;

endmodule

`default_nettype wire
