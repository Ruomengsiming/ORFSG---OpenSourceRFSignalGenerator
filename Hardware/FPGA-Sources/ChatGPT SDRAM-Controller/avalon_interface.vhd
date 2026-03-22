-- avalon_interface.vhd
-- Handles Avalon-MM transactions, config registers, and pipeline buffering

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity avalon_interface is
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

        config_regs       : out std_logic_vector(15 downto 0) := (others => '0');

        read_fifo_data    : in  std_logic_vector(15 downto 0);
        read_fifo_rd_en   : out std_logic;
        write_fifo_data   : out std_logic_vector(15 downto 0);
        write_fifo_wr_en  : out std_logic
    );
end entity;

architecture rtl of avalon_interface is
    type config_array_t is array(0 to 7) of std_logic_vector(15 downto 0);
    signal cfg_regs : config_array_t := (others => (others => '0'));
    signal rd_valid : std_logic := '0';
    signal rd_data  : std_logic_vector(15 downto 0) := (others => '0');

begin
    process(clk)
    begin
        if rising_edge(clk) then
            if rst = '1' then
                cfg_regs <= (others => (others => '0'));
                wr_pipe_valid <= '0';
                rd_valid <= '0';
            else
                -- Write to config regs (addr 0x00 - 0x07)
                if av_write = '1' and av_address < x"08" then
                    cfg_regs(to_integer(unsigned(av_address))) <= av_writedata;
                end if;

                -- Write to SDRAM FIFO (addr 0x10)
                if av_write = '1' and av_address = x"10" then
                    wr_pipe_data  <= av_writedata;
                    wr_pipe_valid <= '1';
                else
                    wr_pipe_valid <= '0';
                end if;

                -- Read from FIFO (addr 0x10)
                if av_read = '1' and av_address = x"10" and rd_pipe_valid = '1' then
                    rd_data  <= rd_pipe_data;
                    rd_valid <= '1';
                    read_fifo_rd_en <= '1';
                else
                    rd_valid <= '0';
                    read_fifo_rd_en <= '0';
                end if;

                -- Read config (addr 0x00 - 0x07)
                if av_read = '1' and av_address < x"08" then
                    rd_data <= cfg_regs(to_integer(unsigned(av_address)));
                    rd_valid <= '1';
                end if;

            end if;
        end if;
    end process;

    av_readdata <= rd_data;
    av_readdatavalid <= rd_valid;
    av_waitrequest <= '0';
    config_regs <= cfg_regs;

    -- FIFO hookup
    write_fifo_data <= wr_pipe_data;
    write_fifo_wr_en <= wr_pipe_valid;

end architecture;
