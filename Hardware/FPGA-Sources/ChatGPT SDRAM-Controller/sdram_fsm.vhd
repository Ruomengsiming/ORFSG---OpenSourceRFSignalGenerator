-- sdram_fsm.vhd
-- State machine to control SDRAM operations: init, read, write, refresh, burst

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity sdram_fsm is
    port (
        clk         : in  std_logic;
        rst         : in  std_logic;
        config_regs : in  std_logic_vector(15 downto 0);
        do_read     : out std_logic;
        do_write    : out std_logic;
        refresh_req : out std_logic,

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

architecture rtl of sdram_fsm is
    type state_type is (
        IDLE, INIT, PRECHARGE, AUTO_REFRESH, LOAD_MODE, READY,
        ACTIVE, READ, WRITE, BURST_RW, NOP
    );
    signal state, next_state : state_type := INIT;
    signal refresh_counter : integer range 0 to 7800 := 0; -- refresh every ~7.8us at 100 MHz
    signal cas_latency : integer range 0 to 3 := 2;

begin

    process(clk)
    begin
        if rising_edge(clk) then
            if rst = '1' then
                state <= INIT;
                refresh_counter <= 0;
            else
                state <= next_state;
                if refresh_counter = 7800 then
                    refresh_counter <= 0;
                else
                    refresh_counter <= refresh_counter + 1;
                end if;
            end if;
        end if;
    end process;

    process(state, refresh_counter)
    begin
        -- Default outputs
        do_read     <= '0';
        do_write    <= '0';
        refresh_req <= '0';

        sdram_clk   <= clk;
        sdram_cke   <= '1';
        sdram_cs_n  <= '0';
        sdram_ras_n <= '1';
        sdram_cas_n <= '1';
        sdram_we_n  <= '1';
        sdram_ba    <= (others => '0');
        sdram_addr  <= (others => '0');

        case state is
            when INIT =>
                next_state <= PRECHARGE;

            when PRECHARGE =>
                sdram_ras_n <= '0';
                sdram_we_n  <= '0';
                next_state  <= AUTO_REFRESH;

            when AUTO_REFRESH =>
                sdram_ras_n <= '0';
                sdram_cas_n <= '0';
                next_state  <= LOAD_MODE;

            when LOAD_MODE =>
                sdram_addr  <= "0000000100100"; -- burst=1, CAS=2 (simplified)
                sdram_we_n  <= '0';
                sdram_ras_n <= '0';
                sdram_cas_n <= '0';
                next_state  <= READY;

            when READY =>
                if refresh_counter = 7800 then
                    refresh_req <= '1';
                    next_state <= AUTO_REFRESH;
                else
                    next_state <= ACTIVE;
                end if;

            when ACTIVE =>
                sdram_ras_n <= '0';
                next_state  <= BURST_RW;

            when BURST_RW =>
                if config_regs(0) = '1' then -- Read enable
                    do_read     <= '1';
                    sdram_cas_n <= '0';
                elsif config_regs(1) = '1' then -- Write enable
                    do_write    <= '1';
                    sdram_cas_n <= '0';
                    sdram_we_n  <= '0';
                end if;
                next_state <= NOP;

            when NOP =>
                next_state <= READY;

            when others =>
                next_state <= READY;
        end case;
    end process;

end architecture;
-- sdram_fsm.vhd