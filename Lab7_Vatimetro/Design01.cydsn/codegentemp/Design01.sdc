# THIS FILE IS AUTOMATICALLY GENERATED
# Project: C:\Users\santa\Documents\PSoC Creator\Laboratorio_7_Vatimetro\Lab7_Vatimetro\Design01.cydsn\Design01.cyprj
# Date: Mon, 09 Mar 2020 19:31:25 GMT
#set_units -time ns
create_clock -name {CyILO} -period 1000000 -waveform {0 500000} [list [get_pins {ClockBlock/ilo}] [get_pins {ClockBlock/clk_100k}] [get_pins {ClockBlock/clk_1k}] [get_pins {ClockBlock/clk_32k}]]
create_clock -name {CyIMO} -period 333.33333333333331 -waveform {0 166.666666666667} [list [get_pins {ClockBlock/imo}]]
create_clock -name {CyPLL_OUT} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/pllout}]]
create_clock -name {CyMASTER_CLK} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/clk_sync}]]
create_generated_clock -name {CyBUS_CLK} -source [get_pins {ClockBlock/clk_sync}] -edges {1 2 3} [list [get_pins {ClockBlock/clk_bus_glb}]]
create_generated_clock -name {i2c_IntClock} -source [get_pins {ClockBlock/clk_sync}] -edges {1 15 31} [list [get_pins {ClockBlock/dclk_glb_0}]]
create_generated_clock -name {Clock_1} -source [get_pins {ClockBlock/clk_sync}] -edges {1 15 31} [list [get_pins {ClockBlock/dclk_glb_1}]]
create_generated_clock -name {UART_IntClock} -source [get_pins {ClockBlock/clk_sync}] -edges {1 313 627} [list [get_pins {ClockBlock/dclk_glb_2}]]


# Component constraints for C:\Users\santa\Documents\PSoC Creator\Laboratorio_7_Vatimetro\Lab7_Vatimetro\Design01.cydsn\TopDesign\TopDesign.cysch
# Project: C:\Users\santa\Documents\PSoC Creator\Laboratorio_7_Vatimetro\Lab7_Vatimetro\Design01.cydsn\Design01.cyprj
# Date: Mon, 09 Mar 2020 19:31:09 GMT
