-- sdram_controller.vhd (Top-Level Wrapper)
-- NOTE: This file instantiates all submodules: avalon_interface, sdram_fsm, sdram_fifo

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity sdram_controller is
    port (
        clk       : in  std_logic;
        rst       : in  std_logic;

        -- Avalon-MM slave
        av_address       : in  std_logic_vector(7 downto 0);
        av_write         : in  std_logic;
        av_read          : in  std_logic;
        av_writedata     : in  std_logic_vector(15 downto 0);
        av_readdata      : out std_logic_vector(15 downto 0);
        av_waitrequest   : out std_logic;
        av_readdatavalid : out std_logic;

        -- SDRAM physical interface
        sdram_clk   : out std_logic;
        sdram_cke   : out std_logic;
        sdram_cs_n  : out std_logic;
        sdram_ras_n : out std_logic;
        sdram_cas_n : out std_logic;
        sdram_we_n  : out std_logic;
        sdram_ba    : out std_logic_vector(1 downto 0);
        sdram_addr  : out std_logic_vector(12 downto 0);
        sdram_dq    : inout std_logic_vector(15 downto 0)
    );
end entity;

architecture rtl of sdram_controller is

    -- Config registers
    type config_array_t is array(0 to 7) of std_logic_vector(15 downto 0);
    signal config_regs : config_array_t := (others => (others => '0'));

    -- Avalon data signals
    signal wr_pipe_data  : std_logic_vector(15 downto 0);
    signal wr_pipe_valid : std_logic;
    signal rd_pipe_data  : std_logic_vector(15 downto 0);
    signal rd_pipe_valid : std_logic;

    -- FIFO control
    signal write_fifo_wr_en  : std_logic;
    signal write_fifo_data   : std_logic_vector(15 downto 0);
    signal read_fifo_rd_en   : std_logic;
    signal read_fifo_data    : std_logic_vector(15 downto 0);

    -- FSM control
    signal do_read, do_write, refresh_req : std_logic;

begin

    -- Avalon interface submodule
    avalon_inst : entity work.avalon_interface
        port map (
            clk               => clk,
            rst               => rst,
            av_address        => av_address,
            av_write          => av_write,
            av_read           => av_read,
            av_writedata      => av_writedata,
            av_readdata       => av_readdata,
            av_waitrequest    => av_waitrequest,
            av_readdatavalid  => av_readdatavalid,
            wr_pipe_data      => wr_pipe_data,
            wr_pipe_valid     => wr_pipe_valid,
            rd_pipe_data      => rd_pipe_data,
            rd_pipe_valid     => rd_pipe_valid,
            config_regs       => config_regs,
            read_fifo_data    => read_fifo_data,
            read_fifo_rd_en   => read_fifo_rd_en,
            write_fifo_data   => write_fifo_data,
            write_fifo_wr_en  => write_fifo_wr_en
        );

    -- Write FIFO
    write_fifo_inst : entity work.sdram_fifo
        generic map (WIDTH => 16, DEPTH => 16)
        port map (
            clk   => clk,
            rst   => rst,
            wr_en => write_fifo_wr_en,
            wr_data => write_fifo_data,
            rd_en => do_write,
            rd_data => open
        );

    -- Read FIFO
    read_fifo_inst : entity work.sdram_fifo
        generic map (WIDTH => 16, DEPTH => 16)
        port map (
            clk   => clk,
            rst   => rst,
            wr_en => do_read,
            wr_data => open,
            rd_en => read_fifo_rd_en,
            rd_data => read_fifo_data
        );

    -- SDRAM FSM
    fsm_inst : entity work.sdram_fsm
        port map (
            clk         => clk,
            rst         => rst,
            config_regs => config_regs,
            do_read     => do_read,
            do_write    => do_write,
            refresh_req => refresh_req,
            sdram_clk   => sdram_clk,
            sdram_cke   => sdram_cke,
            sdram_cs_n  => sdram_cs_n,
            sdram_ras_n => sdram_ras_n,
            sdram_cas_n => sdram_cas_n,
            sdram_we_n  => sdram_we_n,
            sdram_ba    => sdram_ba,
            sdram_addr  => sdram_addr,
            sdram_dq    => sdram_dq
        );

end architecture;
