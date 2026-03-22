# synthesis_constraints.sdc
# TimeQuest SDC file for Altera MAX 10 SDRAM controller project

# Define clock
create_clock -name clk -period 10.0 [get_ports clk]

# Set input and output delays relative to external clock
set_input_delay  2.0 -clock clk [get_ports {av_* wr_* rd_* rst}]
set_output_delay 2.0 -clock clk [get_ports {av_* wr_* rd_*}]

# Set false paths between asynchronous domains (if any)
# Example: if using SDRAM_clk and clk separately, you would include:
# set_false_path -from [get_clocks clk] -to [get_clocks sdram_clk]

# Set max delay on config_reg changes (slow control)
set_max_delay 20.0 -from [get_ports config_regs[*]] -to [get_ports wr_pipe_data]

# Prevent timing analysis warnings for unused ports
set_false_path -from [get_ports read_fifo_data]
set_false_path -from [get_ports write_fifo_data]

# Hold timing requirements on reset
set_max_delay 5.0 -from [get_ports rst] -to [all_registers]

# SDRAM-specific constraints (example pin naming)
# set_location_assignment PIN_A1 -to sdram_clk
# set_location_assignment PIN_B2 -to sdram_cke
# set_location_assignment PIN_C3 -to sdram_cs
# ... (add actual SDRAM pin mapping as per your design)

# Recommended: constrain internal PLLs or clock crossing logic if used
