-- sdram_fifo.vhd
-- Simple synchronous FIFO buffer for SDRAM read/write pipelines

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity sdram_fifo is
    generic (
        DATA_WIDTH : integer := 16;
        DEPTH      : integer := 64
    );
    port (
        clk     : in  std_logic;
        rst     : in  std_logic;

        wr_en   : in  std_logic;
        wr_data : in  std_logic_vector(DATA_WIDTH-1 downto 0);

        rd_en   : in  std_logic;
        rd_data : out std_logic_vector(DATA_WIDTH-1 downto 0);
        valid   : out std_logic
    );
end entity;

architecture rtl of sdram_fifo is
    type mem_t is array(0 to DEPTH-1) of std_logic_vector(DATA_WIDTH-1 downto 0);
    signal mem : mem_t := (others => (others => '0'));

    signal wr_ptr : integer range 0 to DEPTH-1 := 0;
    signal rd_ptr : integer range 0 to DEPTH-1 := 0;
    signal count  : integer range 0 to DEPTH := 0;

begin

    process(clk)
    begin
        if rising_edge(clk) then
            if rst = '1' then
                wr_ptr <= 0;
                rd_ptr <= 0;
                count  <= 0;
            else
                -- Write
                if wr_en = '1' and count < DEPTH then
                    mem(wr_ptr) <= wr_data;
                    wr_ptr <= (wr_ptr + 1) mod DEPTH;
                    count  <= count + 1;
                end if;

                -- Read
                if rd_en = '1' and count > 0 then
                    rd_data <= mem(rd_ptr);
                    rd_ptr <= (rd_ptr + 1) mod DEPTH;
                    count  <= count - 1;
                end if;
            end if;
        end if;
    end process;

    valid <= '1' when count > 0 else '0';

end architecture;
