# setup.tcl
# Quartus project setup script for SDRAM controller on Altera MAX 10

# Set project name and top-level entity
set PROJECT_NAME sdram_controller
set TOP_LEVEL_ENTITY sdram_controller
set_family "MAX 10"
set_part "10M50DAF484C6GES"   ;# Change this to match your specific MAX 10 device

project_new $PROJECT_NAME -overwrite
set_global_assignment -name FAMILY "$family"
set_global_assignment -name DEVICE $part
set_global_assignment -name TOP_LEVEL_ENTITY $TOP_LEVEL_ENTITY

# Add source files
set_global_assignment -name VHDL_FILE avalon_interface.vhd
set_global_assignment -name VHDL_FILE sdram_fsm.vhd
set_global_assignment -name VHDL_FILE sdram_fifo.vhd
set_global_assignment -name VHDL_FILE sdram_controller.vhd

# Add constraint file
set_global_assignment -name SDC_FILE synthesis_constraints.sdc

# Compilation options
set_global_assignment -name OPTIMIZATION_MODE "AGGRESSIVE PERFORMANCE"
set_global_assignment -name TIMEQUEST_MULTICORNER_ANALYSIS ON
set_global_assignment -name ENABLE_TIMING_ANALYZER_MULTICORNER ON
set_global_assignment -name NUM_PARALLEL_PROCESSORS ALL

# Set simulation files (optional)
set_global_assignment -name VHDL_FILE testbench.vhd -library work
set_global_assignment -name SIMULATION_TOP_LEVEL testbench

# Save project
project_close
