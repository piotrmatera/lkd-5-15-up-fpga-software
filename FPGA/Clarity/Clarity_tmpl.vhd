--VHDL instantiation template

component Clarity is
    port (IDDR_datain: in std_logic_vector(0 downto 0);
        IDDR_q: out std_logic_vector(3 downto 0);
        PLL_SerDes_PHASESEL: in std_logic_vector(1 downto 0);
        IDDR_alignwd: in std_logic;
        IDDR_clkin: in std_logic;
        IDDR_ready: out std_logic;
        IDDR_sclk: out std_logic;
        IDDR_start: in std_logic;
        IDDR_sync_clk: in std_logic;
        IDDR_sync_reset: in std_logic;
        PLL_DSP_CLKI: in std_logic;
        PLL_DSP_CLKOP: out std_logic;
        PLL_SerDes_CLKI: in std_logic;
        PLL_SerDes_CLKOP: out std_logic;
        PLL_SerDes_CLKOS: out std_logic;
        PLL_SerDes_CLKOS2: out std_logic;
        PLL_SerDes_CLKOS3: out std_logic;
        PLL_SerDes_LOCK: out std_logic;
        PLL_SerDes_PHASEDIR: in std_logic;
        PLL_SerDes_PHASELOADREG: in std_logic;
        PLL_SerDes_PHASESTEP: in std_logic
    );
    
end component Clarity; -- sbp_module=true 
_inst: Clarity port map (IDDR_datain => __,IDDR_q => __,IDDR_alignwd => __,
            IDDR_clkin => __,IDDR_ready => __,IDDR_sclk => __,IDDR_start => __,
            IDDR_sync_clk => __,IDDR_sync_reset => __,PLL_DSP_CLKI => __,
            PLL_DSP_CLKOP => __,PLL_SerDes_PHASESEL => __,PLL_SerDes_CLKI => __,
            PLL_SerDes_CLKOP => __,PLL_SerDes_CLKOS => __,PLL_SerDes_CLKOS2 => __,
            PLL_SerDes_CLKOS3 => __,PLL_SerDes_LOCK => __,PLL_SerDes_PHASEDIR => __,
            PLL_SerDes_PHASELOADREG => __,PLL_SerDes_PHASESTEP => __);
