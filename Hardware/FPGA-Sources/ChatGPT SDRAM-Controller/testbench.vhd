-- testbench.vhd
-- Basic testbench for SDRAM controller system

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity testbench is
end entity;

architecture tb of testbench is

    -- Component declarations
    component avalon_interface
        port (
            clk               : in  std_logic;
            rst               : in  std_logic;

            av_address        : in  std_logic_vector(7 downto 0);
            av_write          : in  std_logic;
            av_read           : in  std_logic;
            av_writedata      : in  std_logic_vector(15 downto 0);
            av_readdata       : out std_logic_vector(15 downto 0);
            av_waitrequest    : out std_logic;
            av_readdatavalid  : out std_logic,

            wr_pipe_data      : out std_logic_vector(15 downto 0);
            wr_pipe_valid     : out std_logic;
            rd_pipe_data      : in  std_logic_vector(15 downto 0);
            rd_pipe_valid     : in  std_logic;

            config_regs       : out std_logic_vector(15 downto 0);

            read_fifo_data    : in  std_logic_vector(15 downto 0);
            read_fifo_rd_en   : out std_logic;
            write_fifo_data   : out std_logic_vector(15 downto 0);
            write_fifo_wr_en  : out std_logic
        );
    end component;

    -- Testbench signals
    signal clk             : std_logic := '0';
    signal rst             : std_logic := '1';
    signal av_address      : std_logic_vector(7 downto 0);
    signal av_write        : std_logic;
    signal av_read         : std_logic;
    signal av_writedata    : std_logic_vector(15 downto 0);
    signal av_readdata     : std_logic_vector(15 downto 0);
    signal av_waitrequest  : std_logic;
    signal av_readdatavalid: std_logic;

    signal wr_pipe_data    : std_logic_vector(15 downto 0);
    signal wr_pipe_valid   : std_logic;
    signal rd_pipe_data    : std_logic_vector(15 downto 0) := (others => '0');
    signal rd_pipe_valid   : std_logic := '0';
    signal config_regs     : std_logic_vector(15 downto 0);

    signal read_fifo_data  : std_logic_vector(15 downto 0) := x"BEEF";
    signal read_fifo_rd_en : std_logic;
    signal write_fifo_data : std_logic_vector(15 downto 0);
    signal write_fifo_wr_en: std_logic;

    constant clk_period : time := 10 ns;

begin
    -- Clock generation
    clk_process : process
    begin
        clk <= '0';
        wait for clk_period / 2;
        clk <= '1';
        wait for clk_period / 2;
    end process;

    -- Reset
    rst_process : process
    begin
        rst <= '1';
        wait for 50 ns;
        rst <= '0';
        wait;
    end process;

    -- DUT instance
    uut: avalon_interface
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

    -- Simple stimulus
    stim_proc: process
    begin
        wait until rst = '0';

        -- Write config register 0
        av_address   <= x"00";
        av_writedata <= x"0001";
        av_write     <= '1';
        wait for clk_period;
        av_write     <= '0';

        -- Write to SDRAM
        av_address   <= x"10";
        av_writedata <= x"1234";
        av_write     <= '1';
        wait for clk_period;
        av_write     <= '0';

        -- Simulate valid read
        rd_pipe_data  <= x"ABCD";
        rd_pipe_valid <= '1';
        av_address    <= x"10";
        av_read       <= '1';
        wait for clk_period;
        av_read       <= '0';
        rd_pipe_valid <= '0';

        wait;
    end process;

end architecture;
