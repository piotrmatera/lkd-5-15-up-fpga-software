// modbus-id  0x0100

// typy danych 
enum types_e{
        t_u16_128,
        t_bit16,
        t_float,
        t_float3,
        t_float4,
        t_time,
        t_u32,
        t_u16,
        t_i16,
        
    // bity i stany z nazwami symbolicznymi    
        t_bit16_alarm0,
        t_bit16_alarm1,
        t_bit16_alarm2,
        t_bit16_status0,
        t_bit16_status1,
        t_enum16_machine_states,
        t_enum16_conv_states,
        t_bit16_h_odd,
        t_bit16_h_oddH,
        t_bit16_h_even,
        t_bit16_ctrl,
        t_bit16_ctrlH
        
};

uint16_t types_len[]={
        128,//t_u16_128,
        1,//t_bit16,
        2,//t_float,
        6,//t_float3,
        8,//t_float4,
        3,//t_time,
        2,//t_u32,
        1,//t_u16,
        1,//t_i16,
        
    // bity i stany z nazwami symbolicznymi    
        1,//t_bit16_alarm0,
        1,//t_bit16_alarm1,
        1,//t_bit16_alarm2,
        1,//t_bit16_status0,
        1,//t_bit16_status1,
        1,//t_enum16_machine_states,
        1,//t_enum16_conv_states,
        1,//t_bit16_h_odd,
        1,//t_bit16_h_oddH,
        1,//t_bit16_h_even,
        1,//t_bit16_ctrl
        1,//t_bit16_ctrlH
};

struct dsc_record{
   uint16_t address;
   enum types_e type;
   const char * name;
};



//nazwy rejestrow INPUT
struct dsc_record dsc_table_inputs[] ={
        
{0,t_u16_128,"FatFS_response"},
{128,t_bit16_alarm0,"Alarm_0"},
{129,t_bit16_alarm1,"Alarm_1"},
{130,t_bit16_alarm2,"Alarm_2"},
{131,t_bit16,"Alarm_3"},
{132,t_bit16_alarm0,"Alarm_0_snapshot"},
{133,t_bit16_alarm1,"Alarm_1_snapshot"},
{134,t_bit16_alarm2,"Alarm_2_snapshot"},
{135,t_bit16,"Alarm_3_snapshot"},
{136,t_bit16_status0,"STATUS_0"},
{137,t_bit16_status1,"STATUS_1"},
{138,t_bit16,"STATUS_2"},
{139,t_bit16,"STATUS_3"},
{140,t_enum16_machine_states,"Machine_state"},
{142,t_enum16_conv_states,"Converter_state"},
{144,t_i16,"Temp1"},
{145,t_i16,"Temp2"},
{146,t_i16,"Temp3"},
{147,t_i16,"Temp_CPU"},
{148,t_float,"L_grid_previous_0"},
{150,t_float,"L_grid_previous_1"},
{152,t_float,"L_grid_previous_2"},
{154,t_float,"L_grid_previous_3"},
{156,t_float,"L_grid_previous_4"},
{158,t_float,"L_grid_previous_5"},
{160,t_float,"L_grid_previous_6"},
{162,t_float,"L_grid_previous_7"},
{164,t_float,"L_grid_previous_8"},
{166,t_float,"L_grid_previous_9"},
{168,t_u16,"file_head"},
{169,t_time,"RTC_current_t_time"},
{256,t_float3,"I_grid_1h"},
{262,t_float3,"U_grid_1h"},
{268,t_float3,"P_grid_1h"},
{274,t_float3,"P_load_1h"},
{280,t_float3,"P_conv_1h"},
{286,t_float3,"Q_grid_1h"},
{292,t_float3,"Q_load_1h"},
{298,t_float3,"Q_conv_1h"},
{304,t_float3,"S_grid_1h"},
{310,t_float3,"S_load_1h"},
{316,t_float3,"S_conv_1h"},
{322,t_float3,"PF_grid_1h"},
{328,t_float3,"THD_I_grid"},
{334,t_float3,"THD_U_grid"},
{340,t_float3,"U_grid"},
{346,t_float3,"I_grid"},
{352,t_float4,"I_conv"},
{360,t_float3,"S_grid"},
{366,t_float3,"S_conv"},
{372,t_float4,"Used_resources"},
{380,t_float,"PF_grid_1h"},
{382,t_float,"P_load_1h"},
{384,t_float,"Q_load_1h"},

{386,t_float,"S_load_1h"},//+
{388,t_float,"P_grid_1h"},//+

{390,t_float,"Q_grid_1h"},
{392,t_float,"S_grid_1h"},
{394,t_float,"U_grid_1h"},
{396,t_float,"frequency"},
{512,t_float,"I_grid_L1_harmonic_amplitudes_00"},
{514,t_float,"I_grid_L1_harmonic_amplitudes_01"},
{516,t_float,"I_grid_L1_harmonic_amplitudes_02"},
{518,t_float,"I_grid_L1_harmonic_amplitudes_03"},
{520,t_float,"I_grid_L1_harmonic_amplitudes_04"},
{522,t_float,"I_grid_L1_harmonic_amplitudes_05"},
{524,t_float,"I_grid_L1_harmonic_amplitudes_06"},
{526,t_float,"I_grid_L1_harmonic_amplitudes_07"},
{528,t_float,"I_grid_L1_harmonic_amplitudes_08"},
{530,t_float,"I_grid_L1_harmonic_amplitudes_09"},
{532,t_float,"I_grid_L1_harmonic_amplitudes_10"},
{534,t_float,"I_grid_L1_harmonic_amplitudes_11"},
{536,t_float,"I_grid_L1_harmonic_amplitudes_12"},
{538,t_float,"I_grid_L1_harmonic_amplitudes_13"},
{540,t_float,"I_grid_L1_harmonic_amplitudes_14"},
{542,t_float,"I_grid_L1_harmonic_amplitudes_15"},
{544,t_float,"I_grid_L1_harmonic_amplitudes_16"},
{546,t_float,"I_grid_L1_harmonic_amplitudes_17"},
{548,t_float,"I_grid_L1_harmonic_amplitudes_18"},
{550,t_float,"I_grid_L1_harmonic_amplitudes_19"},
{552,t_float,"I_grid_L1_harmonic_amplitudes_20"},
{554,t_float,"I_grid_L1_harmonic_amplitudes_21"},
{556,t_float,"I_grid_L1_harmonic_amplitudes_22"},
{558,t_float,"I_grid_L1_harmonic_amplitudes_23"},
{560,t_float,"I_grid_L1_harmonic_amplitudes_24"},
{562,t_float,"I_grid_L1_harmonic_amplitudes_25"},
{564,t_float,"I_grid_L2_harmonic_amplitudes_00"},
{566,t_float,"I_grid_L2_harmonic_amplitudes_01"},
{568,t_float,"I_grid_L2_harmonic_amplitudes_02"},
{570,t_float,"I_grid_L2_harmonic_amplitudes_03"},
{572,t_float,"I_grid_L2_harmonic_amplitudes_04"},
{574,t_float,"I_grid_L2_harmonic_amplitudes_05"},
{576,t_float,"I_grid_L2_harmonic_amplitudes_06"},
{578,t_float,"I_grid_L2_harmonic_amplitudes_07"},
{580,t_float,"I_grid_L2_harmonic_amplitudes_08"},
{582,t_float,"I_grid_L2_harmonic_amplitudes_09"},
{584,t_float,"I_grid_L2_harmonic_amplitudes_10"},
{586,t_float,"I_grid_L2_harmonic_amplitudes_11"},
{588,t_float,"I_grid_L2_harmonic_amplitudes_12"},
{590,t_float,"I_grid_L2_harmonic_amplitudes_13"},
{592,t_float,"I_grid_L2_harmonic_amplitudes_14"},
{594,t_float,"I_grid_L2_harmonic_amplitudes_15"},
{596,t_float,"I_grid_L2_harmonic_amplitudes_16"},
{598,t_float,"I_grid_L2_harmonic_amplitudes_17"},
{600,t_float,"I_grid_L2_harmonic_amplitudes_18"},
{602,t_float,"I_grid_L2_harmonic_amplitudes_19"},
{604,t_float,"I_grid_L2_harmonic_amplitudes_20"},
{606,t_float,"I_grid_L2_harmonic_amplitudes_21"},
{608,t_float,"I_grid_L2_harmonic_amplitudes_22"},
{610,t_float,"I_grid_L2_harmonic_amplitudes_23"},
{612,t_float,"I_grid_L2_harmonic_amplitudes_24"},
{614,t_float,"I_grid_L2_harmonic_amplitudes_25"},
{616,t_float,"I_grid_L3_harmonic_amplitudes_00"},
{618,t_float,"I_grid_L3_harmonic_amplitudes_01"},
{620,t_float,"I_grid_L3_harmonic_amplitudes_02"},
{622,t_float,"I_grid_L3_harmonic_amplitudes_03"},
{624,t_float,"I_grid_L3_harmonic_amplitudes_04"},
{626,t_float,"I_grid_L3_harmonic_amplitudes_05"},
{628,t_float,"I_grid_L3_harmonic_amplitudes_06"},
{630,t_float,"I_grid_L3_harmonic_amplitudes_07"},
{632,t_float,"I_grid_L3_harmonic_amplitudes_08"},
{634,t_float,"I_grid_L3_harmonic_amplitudes_09"},
{636,t_float,"I_grid_L3_harmonic_amplitudes_10"},
{638,t_float,"I_grid_L3_harmonic_amplitudes_11"},
{640,t_float,"I_grid_L3_harmonic_amplitudes_12"},
{642,t_float,"I_grid_L3_harmonic_amplitudes_13"},
{644,t_float,"I_grid_L3_harmonic_amplitudes_14"},
{646,t_float,"I_grid_L3_harmonic_amplitudes_15"},
{648,t_float,"I_grid_L3_harmonic_amplitudes_16"},
{650,t_float,"I_grid_L3_harmonic_amplitudes_17"},
{652,t_float,"I_grid_L3_harmonic_amplitudes_18"},
{654,t_float,"I_grid_L3_harmonic_amplitudes_19"},
{656,t_float,"I_grid_L3_harmonic_amplitudes_20"},
{658,t_float,"I_grid_L3_harmonic_amplitudes_21"},
{660,t_float,"I_grid_L3_harmonic_amplitudes_22"},
{662,t_float,"I_grid_L3_harmonic_amplitudes_23"},
{664,t_float,"I_grid_L3_harmonic_amplitudes_24"},
{666,t_float,"I_grid_L3_harmonic_amplitudes_25"},
{668,t_float,"U_grid_L1_harmonic_amplitudes_00"},
{670,t_float,"U_grid_L1_harmonics_amplitudes_01"},
{672,t_float,"U_grid_L1_harmonics_amplitudes_02"},
{674,t_float,"U_grid_L1_harmonics_amplitudes_03"},
{676,t_float,"U_grid_L1_harmonics_amplitudes_04"},
{678,t_float,"U_grid_L1_harmonics_amplitudes_05"},
{680,t_float,"U_grid_L1_harmonics_amplitudes_06"},
{682,t_float,"U_grid_L1_harmonics_amplitudes_07"},
{684,t_float,"U_grid_L1_harmonics_amplitudes_08"},
{686,t_float,"U_grid_L1_harmonics_amplitudes_09"},
{688,t_float,"U_grid_L1_harmonics_amplitudes_10"},
{690,t_float,"U_grid_L1_harmonics_amplitudes_11"},
{692,t_float,"U_grid_L1_harmonics_amplitudes_12"},
{694,t_float,"U_grid_L1_harmonics_amplitudes_13"},
{696,t_float,"U_grid_L1_harmonics_amplitudes_14"},
{698,t_float,"U_grid_L1_harmonics_amplitudes_15"},
{700,t_float,"U_grid_L1_harmonics_amplitudes_16"},
{702,t_float,"U_grid_L1_harmonics_amplitudes_17"},
{704,t_float,"U_grid_L1_harmonics_amplitudes_18"},
{706,t_float,"U_grid_L1_harmonics_amplitudes_19"},
{708,t_float,"U_grid_L1_harmonics_amplitudes_20"},
{710,t_float,"U_grid_L1_harmonics_amplitudes_21"},
{712,t_float,"U_grid_L1_harmonics_amplitudes_22"},
{714,t_float,"U_grid_L1_harmonics_amplitudes_23"},
{716,t_float,"U_grid_L1_harmonics_amplitudes_24"},
{718,t_float,"U_grid_L1_harmonics_amplitudes_25"},
{720,t_float,"U_grid_L2_harmonic_amplitudes_00"},
{722,t_float,"U_grid_L2_harmonic_amplitudes_01"},
{724,t_float,"U_grid_L2_harmonic_amplitudes_02"},
{726,t_float,"U_grid_L2_harmonic_amplitudes_03"},
{728,t_float,"U_grid_L2_harmonic_amplitudes_04"},
{730,t_float,"U_grid_L2_harmonic_amplitudes_05"},
{732,t_float,"U_grid_L2_harmonic_amplitudes_06"},
{734,t_float,"U_grid_L2_harmonic_amplitudes_07"},
{736,t_float,"U_grid_L2_harmonic_amplitudes_08"},
{738,t_float,"U_grid_L2_harmonic_amplitudes_09"},
{740,t_float,"U_grid_L2_harmonic_amplitudes_10"},
{742,t_float,"U_grid_L2_harmonic_amplitudes_11"},
{744,t_float,"U_grid_L2_harmonic_amplitudes_12"},
{746,t_float,"U_grid_L2_harmonic_amplitudes_13"},
{748,t_float,"U_grid_L2_harmonic_amplitudes_14"},
{750,t_float,"U_grid_L2_harmonic_amplitudes_15"},
{752,t_float,"U_grid_L2_harmonic_amplitudes_16"},
{754,t_float,"U_grid_L2_harmonic_amplitudes_17"},
{756,t_float,"U_grid_L2_harmonic_amplitudes_18"},
{758,t_float,"U_grid_L2_harmonic_amplitudes_19"},
{760,t_float,"U_grid_L2_harmonic_amplitudes_20"},
{762,t_float,"U_grid_L2_harmonic_amplitudes_21"},
{764,t_float,"U_grid_L2_harmonic_amplitudes_22"},
{766,t_float,"U_grid_L2_harmonic_amplitudes_23"},
{768,t_float,"U_grid_L2_harmonic_amplitudes_24"},
{770,t_float,"U_grid_L2_harmonic_amplitudes_25"},
{772,t_float,"U_grid_L3_harmonic_amplitudes_00"},
{774,t_float,"U_grid_L3_harmonic_amplitudes_01"},
{776,t_float,"U_grid_L3_harmonic_amplitudes_02"},
{778,t_float,"U_grid_L3_harmonic_amplitudes_03"},
{780,t_float,"U_grid_L3_harmonic_amplitudes_04"},
{782,t_float,"U_grid_L3_harmonic_amplitudes_05"},
{784,t_float,"U_grid_L3_harmonic_amplitudes_06"},
{786,t_float,"U_grid_L3_harmonic_amplitudes_07"},
{788,t_float,"U_grid_L3_harmonic_amplitudes_08"},
{790,t_float,"U_grid_L3_harmonic_amplitudes_09"},
{792,t_float,"U_grid_L3_harmonic_amplitudes_10"},
{794,t_float,"U_grid_L3_harmonic_amplitudes_11"},
{796,t_float,"U_grid_L3_harmonic_amplitudes_12"},
{798,t_float,"U_grid_L3_harmonic_amplitudes_13"},
{800,t_float,"U_grid_L3_harmonic_amplitudes_14"},
{802,t_float,"U_grid_L3_harmonic_amplitudes_15"},
{804,t_float,"U_grid_L3_harmonic_amplitudes_16"},
{806,t_float,"U_grid_L3_harmonic_amplitudes_17"},
{808,t_float,"U_grid_L3_harmonic_amplitudes_18"},
{810,t_float,"U_grid_L3_harmonic_amplitudes_19"},
{812,t_float,"U_grid_L3_harmonic_amplitudes_20"},
{814,t_float,"U_grid_L3_harmonic_amplitudes_21"},
{816,t_float,"U_grid_L3_harmonic_amplitudes_22"},
{818,t_float,"U_grid_L3_harmonic_amplitudes_23"},
{820,t_float,"U_grid_L3_harmonic_amplitudes_24"},
{822,t_float,"U_grid_L3_harmonic_amplitudes_25"},
{824,t_float,"I_grid_L1_harmonic_THD_00"},
{826,t_float,"I_grid_L1_harmonic_THD_01"},
{828,t_float,"I_grid_L1_harmonic_THD_02"},
{830,t_float,"I_grid_L1_harmonic_THD_03"},
{832,t_float,"I_grid_L1_harmonic_THD_04"},
{834,t_float,"I_grid_L1_harmonic_THD_05"},
{836,t_float,"I_grid_L1_harmonic_THD_06"},
{838,t_float,"I_grid_L1_harmonic_THD_07"},
{840,t_float,"I_grid_L1_harmonic_THD_08"},
{842,t_float,"I_grid_L1_harmonic_THD_09"},
{844,t_float,"I_grid_L1_harmonic_THD_10"},
{846,t_float,"I_grid_L1_harmonic_THD_11"},
{848,t_float,"I_grid_L1_harmonic_THD_12"},
{850,t_float,"I_grid_L1_harmonic_THD_13"},
{852,t_float,"I_grid_L1_harmonic_THD_14"},
{854,t_float,"I_grid_L1_harmonic_THD_15"},
{856,t_float,"I_grid_L1_harmonic_THD_16"},
{858,t_float,"I_grid_L1_harmonic_THD_17"},
{860,t_float,"I_grid_L1_harmonic_THD_18"},
{862,t_float,"I_grid_L1_harmonic_THD_19"},
{864,t_float,"I_grid_L1_harmonic_THD_20"},
{866,t_float,"I_grid_L1_harmonic_THD_21"},
{868,t_float,"I_grid_L1_harmonic_THD_22"},
{870,t_float,"I_grid_L1_harmonic_THD_23"},
{872,t_float,"I_grid_L1_harmonic_THD_24"},
{874,t_float,"I_grid_L1_harmonic_THD_25"},
{876,t_float,"I_grid_L2_harmonic_THD_00"},
{878,t_float,"I_grid_L2_harmonic_THD_01"},
{880,t_float,"I_grid_L2_harmonic_THD_02"},
{882,t_float,"I_grid_L2_harmonic_THD_03"},
{884,t_float,"I_grid_L2_harmonic_THD_04"},
{886,t_float,"I_grid_L2_harmonic_THD_05"},
{888,t_float,"I_grid_L2_harmonic_THD_06"},
{890,t_float,"I_grid_L2_harmonic_THD_07"},
{892,t_float,"I_grid_L2_harmonic_THD_08"},
{894,t_float,"I_grid_L2_harmonic_THD_09"},
{896,t_float,"I_grid_L2_harmonic_THD_10"},
{898,t_float,"I_grid_L2_harmonic_THD_11"},
{900,t_float,"I_grid_L2_harmonic_THD_12"},
{902,t_float,"I_grid_L2_harmonic_THD_13"},
{904,t_float,"I_grid_L2_harmonic_THD_14"},
{906,t_float,"I_grid_L2_harmonic_THD_15"},
{908,t_float,"I_grid_L2_harmonic_THD_16"},
{910,t_float,"I_grid_L2_harmonic_THD_17"},
{912,t_float,"I_grid_L2_harmonic_THD_18"},
{914,t_float,"I_grid_L2_harmonic_THD_19"},
{916,t_float,"I_grid_L2_harmonic_THD_20"},
{918,t_float,"I_grid_L2_harmonic_THD_21"},
{920,t_float,"I_grid_L2_harmonic_THD_22"},
{922,t_float,"I_grid_L2_harmonic_THD_23"},
{924,t_float,"I_grid_L2_harmonic_THD_24"},
{926,t_float,"I_grid_L2_harmonic_THD_25"},
{928,t_float,"I_grid_L3_harmonic_THD_00"},
{930,t_float,"I_grid_L3_harmonic_THD_01"},
{932,t_float,"I_grid_L3_harmonic_THD_02"},
{934,t_float,"I_grid_L3_harmonic_THD_03"},
{936,t_float,"I_grid_L3_harmonic_THD_04"},
{938,t_float,"I_grid_L3_harmonic_THD_05"},
{940,t_float,"I_grid_L3_harmonic_THD_06"},
{942,t_float,"I_grid_L3_harmonic_THD_07"},
{944,t_float,"I_grid_L3_harmonic_THD_08"},
{946,t_float,"I_grid_L3_harmonic_THD_09"},
{948,t_float,"I_grid_L3_harmonic_THD_10"},
{950,t_float,"I_grid_L3_harmonic_THD_11"},
{952,t_float,"I_grid_L3_harmonic_THD_12"},
{954,t_float,"I_grid_L3_harmonic_THD_13"},
{956,t_float,"I_grid_L3_harmonic_THD_14"},
{958,t_float,"I_grid_L3_harmonic_THD_15"},
{960,t_float,"I_grid_L3_harmonic_THD_16"},
{962,t_float,"I_grid_L3_harmonic_THD_17"},
{964,t_float,"I_grid_L3_harmonic_THD_18"},
{966,t_float,"I_grid_L3_harmonic_THD_19"},
{968,t_float,"I_grid_L3_harmonic_THD_20"},
{970,t_float,"I_grid_L3_harmonic_THD_21"},
{972,t_float,"I_grid_L3_harmonic_THD_22"},
{974,t_float,"I_grid_L3_harmonic_THD_23"},
{976,t_float,"I_grid_L3_harmonic_THD_24"},
{978,t_float,"I_grid_L3_harmonic_THD_25"},
{980,t_float,"U_grid_L1_harmonic_THD_00"},
{982,t_float,"U_grid_L1_harmonic_THD_01"},
{984,t_float,"U_grid_L1_harmonic_THD_02"},
{986,t_float,"U_grid_L1_harmonic_THD_03"},
{988,t_float,"U_grid_L1_harmonic_THD_04"},
{990,t_float,"U_grid_L1_harmonic_THD_05"},
{992,t_float,"U_grid_L1_harmonic_THD_06"},
{994,t_float,"U_grid_L1_harmonic_THD_07"},
{996,t_float,"U_grid_L1_harmonic_THD_08"},
{998,t_float,"U_grid_L1_harmonic_THD_09"},
{1000,t_float,"U_grid_L1_harmonic_THD_10"},
{1002,t_float,"U_grid_L1_harmonic_THD_11"},
{1004,t_float,"U_grid_L1_harmonic_THD_12"},
{1006,t_float,"U_grid_L1_harmonic_THD_13"},
{1008,t_float,"U_grid_L1_harmonic_THD_14"},
{1010,t_float,"U_grid_L1_harmonic_THD_15"},
{1012,t_float,"U_grid_L1_harmonic_THD_16"},
{1014,t_float,"U_grid_L1_harmonic_THD_17"},
{1016,t_float,"U_grid_L1_harmonic_THD_18"},
{1018,t_float,"U_grid_L1_harmonic_THD_19"},
{1020,t_float,"U_grid_L1_harmonic_THD_20"},
{1022,t_float,"U_grid_L1_harmonic_THD_21"},
{1024,t_float,"U_grid_L1_harmonic_THD_22"},
{1026,t_float,"U_grid_L1_harmonic_THD_23"},
{1028,t_float,"U_grid_L1_harmonic_THD_24"},
{1030,t_float,"U_grid_L1_harmonic_THD_25"},
{1032,t_float,"U_grid_L2_harmonic_THD_00"},
{1034,t_float,"U_grid_L2_harmonic_THD_01"},
{1036,t_float,"U_grid_L2_harmonic_THD_02"},
{1038,t_float,"U_grid_L2_harmonic_THD_03"},
{1040,t_float,"U_grid_L2_harmonic_THD_04"},
{1042,t_float,"U_grid_L2_harmonic_THD_05"},
{1044,t_float,"U_grid_L2_harmonic_THD_06"},
{1046,t_float,"U_grid_L2_harmonic_THD_07"},
{1048,t_float,"U_grid_L2_harmonic_THD_08"},
{1050,t_float,"U_grid_L2_harmonic_THD_09"},
{1052,t_float,"U_grid_L2_harmonic_THD_10"},
{1054,t_float,"U_grid_L2_harmonic_THD_11"},
{1056,t_float,"U_grid_L2_harmonic_THD_12"},
{1058,t_float,"U_grid_L2_harmonic_THD_13"},
{1060,t_float,"U_grid_L2_harmonic_THD_14"},
{1062,t_float,"U_grid_L2_harmonic_THD_15"},
{1064,t_float,"U_grid_L2_harmonic_THD_16"},
{1066,t_float,"U_grid_L2_harmonic_THD_17"},
{1068,t_float,"U_grid_L2_harmonic_THD_18"},
{1070,t_float,"U_grid_L2_harmonic_THD_19"},
{1072,t_float,"U_grid_L2_harmonic_THD_20"},
{1074,t_float,"U_grid_L2_harmonic_THD_21"},
{1076,t_float,"U_grid_L2_harmonic_THD_22"},
{1078,t_float,"U_grid_L2_harmonic_THD_23"},
{1080,t_float,"U_grid_L2_harmonic_THD_24"},
{1082,t_float,"U_grid_L2_harmonic_THD_25"},
{1084,t_float,"U_grid_L3_harmonic_THD_00"},
{1086,t_float,"U_grid_L3_harmonic_THD_01"},
{1088,t_float,"U_grid_L3_harmonic_THD_02"},
{1090,t_float,"U_grid_L3_harmonic_THD_03"},
{1092,t_float,"U_grid_L3_harmonic_THD_04"},
{1094,t_float,"U_grid_L3_harmonic_THD_05"},
{1096,t_float,"U_grid_L3_harmonic_THD_06"},
{1098,t_float,"U_grid_L3_harmonic_THD_07"},
{1100,t_float,"U_grid_L3_harmonic_THD_08"},
{1102,t_float,"U_grid_L3_harmonic_THD_09"},
{1104,t_float,"U_grid_L3_harmonic_THD_10"},
{1106,t_float,"U_grid_L3_harmonic_THD_11"},
{1108,t_float,"U_grid_L3_harmonic_THD_12"},
{1110,t_float,"U_grid_L3_harmonic_THD_13"},
{1112,t_float,"U_grid_L3_harmonic_THD_14"},
{1114,t_float,"U_grid_L3_harmonic_THD_15"},
{1116,t_float,"U_grid_L3_harmonic_THD_16"},
{1118,t_float,"U_grid_L3_harmonic_THD_17"},
{1120,t_float,"U_grid_L3_harmonic_THD_18"},
{1122,t_float,"U_grid_L3_harmonic_THD_19"},
{1124,t_float,"U_grid_L3_harmonic_THD_20"},
{1126,t_float,"U_grid_L3_harmonic_THD_21"},
{1128,t_float,"U_grid_L3_harmonic_THD_22"},
{1130,t_float,"U_grid_L3_harmonic_THD_23"},
{1132,t_float,"U_grid_L3_harmonic_THD_24"},
{1134,t_float,"U_grid_L3_harmonic_THD_25"}
};

//nazwy rejestrow HOLDING
struct dsc_record dsc_table_holds[] ={
{0, t_u16_128, "FatFS_request"},
{128, t_bit16_h_odd, "H_odd_a"},		
{129, t_bit16_h_oddH, "H_oddH_a"},
{130, t_bit16_h_odd, "H_odd_b"},
{131, t_bit16_h_oddH, "H_oddH_b"},
{132, t_bit16_h_odd, "H_odd_c"},
{133, t_bit16_h_oddH, "H_oddH_c"},
{134, t_bit16_h_even, "H_even_a"},
{136, t_bit16_h_even, "H_even_b"},
{138, t_bit16_h_even, "H_even_c"},
{140, t_float3, "Q_set"},
{146, t_bit16_ctrl,  "control_bits"},
{147, t_bit16_ctrlH, "control_bitsH"},
{148, t_time, 	"RTC_new_time"}
};

//typy bitow i stanow (te enumy na razie nieuzywane)
enum bit16_alarm0_bits{
I_conv_a_H=0,
I_conv_a_L,
I_conv_b_H,
I_conv_b_L,
I_conv_c_H,
I_conv_c_L,
I_conv_n_H,
I_conv_n_L,
I_conv_a_SDH,
I_conv_a_SDL,
I_conv_b_SDH,
I_conv_b_SDL,
I_conv_c_SDH,
I_conv_c_SDL,
I_conv_n_SDH,
I_conv_n_SDL,      
};

enum bit16_alarm1_bits{
Temperature_H=0,
Temperature_L,
U_dc_H,
U_dc_L,
Driver_PWM_a_A,
Driver_PWM_a_B,
Driver_PWM_b_A,
Driver_PWM_b_B,
Driver_PWM_c_A,
Driver_PWM_c_B,
Driver_PWM_n_A,
Driver_PWM_n_B,
No_calibration,
CT_char_error,
PLL_UNSYNC,
CONV_SOFTSTART,        
};

enum bit16_alarm2_bits{
FUSE_BROKEN=0,
TZ_FLT_SUPPLY,
TZ_DRV_FLT,
TZ_CLOCKFAIL,
TZ_EMUSTOP,
TZ_SD_COMP,
TZ,
};

enum bit16_status0_bits{
Init_done=0,
ONOFF_state, //dopisane _state aby odroznic od ONOFF w ctrl_bits
DS1_switch_SD_CT,
DS2_enable_Q_comp,
DS3_enable_P_sym,
DS4_enable_H_comp,
DS5_limit_to_9odd_harmonics,
DS6_limit_to_14odd_harmonics,
DS7_limit_to_19odd_harmonics,
DS8_DS_override,
calibration_procedure_error,
L_grid_measured,
Scope_snapshot_pending,
Scope_snapshot_error,
SD_card_not_enough_data,
SD_no_CT_characteristic,      
};

enum bit16_status1_bits{
SD_no_calibration=0,
SD_no_harmonic_settings,
SD_no_settings,
FLASH_not_enough_data,
FLASH_no_CT_characteristic,
FLASH_no_calibration,
FLASH_no_harmonic_settings,
FLASH_no_settings,
in_limit_Q,
in_limit_P,
in_limit_H,
};

enum enum16_machine_states{
    Machine_init=0,
    Machine_calibrate_offsets,
    Machine_calibrate_curent_gain,
    Machine_calibrate_AC_voltage_gain,
    Machine_calibrate_DC_voltage_gain,
    Machine_idle,
    Machine_start,
    Machine_Lgrid_meas,
    Machine_operational,
    Machine_cleanup,
    Machine_cleanup_delay,
};

enum enum16_conv_states{
    CONV_softstart=0,
    CONV_grid_relay,
    CONV_active,

};

enum bit16_h_odd_bits{
        //reserved 0
        harm3 = 1,
        harm5, //2
        harm7, //3
        harm9, //4
        harm11, //5
        harm13, //6
        harm15, //7
        harm17, //8
        harm19, //9
        harm21, //10
        harm23, //11
        harm25, //12
        harm27, //13
        harm29, //14
        harm31, //15
};
enum bit16_h_oddH_bits{
        harm33 = 0,
        harm35,
        harm37,
        harm39,
        harm41,
        harm43,
        harm45,
        harm47,
        harm49
};

enum bit16_h_even_bits{
    harm2 = 1,
    harm4
};

enum bit16_ctrl_bits{
        Scope_snapshot=0,
        Modbus_FatFS_repeat,
        save_to_FLASH,
        SD_save_H_Settings,
        SD_save_Settings,
        CPU_reset,
        ONOFF_override,
        ONOFF,
        
        enable_Q_comp_a,      //8
        enable_Q_comp_b,      //9
        enable_Q_comp_c,      //10
        enable_P_sym,         //11
        enable_H_comp,        //12
        version_Q_comp_a,     //13
        version_Q_comp_b,     //14
        version_Q_comp_c,     //15
};
        
enum bit16_ctrlH_bits{
        version_P_sym
};




//nazwy bitow i stanow (uwaga! tablice indeksowane enumami - musi byc zgodne)
const char * bit16_alarm0_bits_s[]={
"I_conv_a_H",
"I_conv_a_L",
"I_conv_b_H",
"I_conv_b_L",
"I_conv_c_H",
"I_conv_c_L",
"I_conv_n_H",
"I_conv_n_L",
"I_conv_a_SDH",
"I_conv_a_SDL",
"I_conv_b_SDH",
"I_conv_b_SDL",
"I_conv_c_SDH",
"I_conv_c_SDL",
"I_conv_n_SDH",
"I_conv_n_SDL"      
};

const char *bit16_alarm1_bits_s[]={
"Temperature_H",
"Temperature_L",
"U_dc_H",
"U_dc_L",
"Driver_PWM_a_A",
"Driver_PWM_a_B",
"Driver_PWM_b_A",
"Driver_PWM_b_B",
"Driver_PWM_c_A",
"Driver_PWM_c_B",
"Driver_PWM_n_A",
"Driver_PWM_n_B",
"No_calibration",
"CT_char_error",
"PLL_UNSYNC",
"CONV_SOFTSTART"     
};

const char * bit16_alarm2_bits_s[] = {
"FUSE_BROKEN",
"TZ_FLT_SUPPLY",
"TZ_DRV_FLT",
"TZ_CLOCKFAIL",
"TZ_EMUSTOP",
"TZ_SD_COMP",
"TZ"
};

const char * bit16_status0_bits_s[]={
"Init_done",
"ONOFF",
"DS1_switch_SD_CT",
"DS2_enable_Q_comp",
"DS3_enable_P_sym",
"DS4_enable_H_comp",
"DS5_limit_to_9odd_harmonics",
"DS6_limit_to_14odd_harmonics",
"DS7_limit_to_19odd_harmonics",
"DS8_DS_override",
"calibration_procedure_error",
"L_grid_measured",
"Scope_snapshot_pending",
"Scope_snapshot_error",
"SD_card_not_enough_data",
"SD_no_CT_characteristic"
};

const char * bit16_status1_bits_s[]={
"SD_no_calibration",
"SD_no_harmonic_settings",
"SD_no_settings",
"FLASH_not_enough_data",
"FLASH_no_CT_characteristic",
"FLASH_no_calibration",
"FLASH_no_harmonic_settings",
"FLASH_no_settings",
"in_limit_Q",
"in_limit_P",
"in_limit_H"
};

const char * enum16_machine_states_s[]={
"Machine_init",
"Machine_calibrate_offsets",
"Machine_calibrate_curent_gain",
"Machine_calibrate_AC_voltage_gain",
"Machine_calibrate_DC_voltage_gain",
"Machine_idle",
"Machine_start",
"Machine_Lgrid_meas",
"Machine_operational",
"Machine_cleanup",
"Machine_cleanup_delay"
};

const char * enum16_conv_states_s[] = {
"CONV_softstart",
"CONV_grid_relay",
"CONV_active"
};

const char * bit16_h_odd_bits_s[]={
        "???",
        "harm3",    
        "harm5",
        "harm7",
        "harm9",
        "harm11",
        "harm13",
        "harm15",
        "harm17",
        "harm19",
        "harm21",
        "harm23",
        "harm25",
        "harm27",
        "harm29",
        "harm31"
};
const char * bit16_h_oddH_bits_s[]={
        "harm33",
        "harm35",
        "harm37",
        "harm39",
        "harm41",
        "harm43",
        "harm45",
        "harm47",
        "harm49"
};

const char * bit16_h_even_bits_s[]={
    "???",
    "harm2",
    "harm4"
};

const char * bit16_ctrl_bits_s[]={
"Scope_snapshot",       //0
"Modbus_FatFS_repeat",  //1
"save_to_FLASH",        //2
"SD_save_H_Settings",   //3
"SD_save_Settings",     //4
"CPU_reset",            //5        
"ONOFF_override",       //6
"ONOFF",                //7

"enable_Q_comp_a",      //8
"enable_Q_comp_b",      //9
"enable_Q_comp_c",      //10
"enable_P_sym",         //11
"enable_H_comp",        //12
"version_Q_comp_a",     //13
"version_Q_comp_b",     //14
"version_Q_comp_c",     //15
};

const char * bit16_ctrlH_bits_s[]={
"version_P_sym",
};

// rozpoznawanie przy wydruku - dodac analogiczna linie po dodaniu nowego typu
#define CASE_BIT_ENUM_PRINT \
        CASE_BIT16( bit16_alarm0 )\
        CASE_BIT16( bit16_alarm1 )\
        CASE_BIT16( bit16_alarm2 )\
        \
        CASE_BIT16( bit16_status0 )\
        CASE_BIT16( bit16_status1 )\
        \
        CASE_BIT16( bit16_h_odd )  \
        CASE_BIT16( bit16_h_oddH )  \
        CASE_BIT16( bit16_h_even )\
        CASE_BIT16( bit16_ctrl )\
        CASE_BIT16( bit16_ctrlH )\
        \
        CASE_ENUM16( enum16_machine_states )\
        CASE_ENUM16( enum16_conv_states )

#define CASE_BIT_WRITE \
        CASE_BIT16_WRITE( bit16_h_odd )\
        CASE_BIT16_WRITE( bit16_h_oddH )\
        CASE_BIT16_WRITE( bit16_h_even )\
        CASE_BIT16_WRITE( bit16_ctrl )\
        CASE_BIT16_WRITE( bit16_ctrlH )


