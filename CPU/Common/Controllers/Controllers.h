// Tomasz �wi�chowicz swiechowicz.tomasz@gmail.com

#include "stdafx.h"

#pragma once
#ifndef Controllers_H_
#define Controllers_H_

extern const float MATH_PI;
extern const float MATH_2PI;
extern const float MATH_2PI_3;
extern const float MATH_PI_3;
extern const float MATH_1_2PI;
extern const float MATH_1_PI;
extern const float MATH_1_3;
extern const float MATH_1_SQRT3;
extern const float MATH_1_SQRT2;
extern const float MATH_SQRT2_3;
extern const float MATH_SQRT3_2;
extern const float MATH_SQRT2;
extern const float MATH_SQRT3;
extern const float MATH_2_3;
extern const float MATH_1_325;
extern const float MATH_N2_325;
extern const float MATH_E;
extern const float MATH_1_E;
extern const float MATH_1_MINUS_1_E;

struct trigonometric_struct
{
    float cosine;
    float sine;
};

typedef union {
    float *ptr; //Aligned to lower 16-bits
    Uint32 pad; //32-bits
}CLA_FPTR;

typedef union {
    struct trigonometric_struct *ptr; //Aligned to lower 16-bits
    Uint32 pad; //32-bits
}CLA_TRIGPTR;

#define Saturation(input, lim_L, lim_H)   \
(fmaxf(fminf((input), (lim_H)), (lim_L))) \

#define SOGI_calc(SOGI_struct, input, wTs)                                      \
{                                                                               \
    register float wTs_reg = wTs;                                               \
    register float qx_mod = SOGI_struct.qx + 0.5f*SOGI_struct.x*wTs_reg;        \
    SOGI_struct.x += (SOGI_struct.input_err - qx_mod)*wTs_reg;                  \
    SOGI_struct.qx = qx_mod + 0.5f*SOGI_struct.x*wTs_reg;                       \
    SOGI_struct.input_err = input - SOGI_struct.x;                              \
}

#define abc_abg(abg_struct, abc_struct)                        \
{                                                              \
register float in_bc = abc_struct.b + abc_struct.c;            \
abg_struct.alf = (2.0f*abc_struct.a - in_bc) * MATH_1_3;       \
abg_struct.bet = (abc_struct.b - abc_struct.c) * MATH_1_SQRT3; \
abg_struct.gam = MATH_1_3 * (abc_struct.a + in_bc);            \
}

#define abc_ab(abg_struct, abc_struct)                                         \
{                                                                              \
abg_struct.alf = (2.0f*abc_struct.a - abc_struct.b - abc_struct.c) * MATH_1_3; \
abg_struct.bet = (abc_struct.b - abc_struct.c) * MATH_1_SQRT3;                 \
}

#define abg_abcn(abcn_struct, abg_struct)                          \
{                                                                  \
abc_struct.a = abg_struct.alf + abg_struct.gam;                    \
register float out_temp = abg_struct.gam - 0.5f * abg_struct.alf;  \
register float t_struct_bet_temp = abg_struct.bet * MATH_SQRT3_2;  \
abc_struct.b = out_temp + t_struct_bet_temp;                       \
abc_struct.c = out_temp - t_struct_bet_temp;                       \
abc_struct.n = abg_struct.gam * 3.0f;                              \
}

#define ab_abc(abc_struct, abg_struct)                             \
{                                                                  \
abc_struct.a = abg_struct.alf;                                     \
register float out_temp = -0.5f * abg_struct.alf;                  \
register float t_struct_bet_temp = abg_struct.bet * MATH_SQRT3_2;  \
abc_struct.b = out_temp + t_struct_bet_temp;                       \
abc_struct.c = out_temp - t_struct_bet_temp;                       \
}

#define abg_dqz(t_struct, angle)                                 \
{                                                                \
register float sine = sinf(angle);                               \
register float cosine = cosf(angle);                             \
t_struct.d =  cosine * (t_struct.alf) + sine * (t_struct.bet); \
t_struct.q = -sine * (t_struct.alf) + cosine * (t_struct.bet); \
t_struct.z = t_struct.gam;                                     \
}

#define dqz_abg(t_struct, angle)                     \
{                                                    \
register float sine = sinf(angle);                   \
register float cosine = cosf(angle);                 \
t_struct.alf = cosine*t_struct.d - sine*t_struct.q; \
t_struct.bet = sine*t_struct.d + cosine*t_struct.q; \
t_struct.gam = t_struct.z;                         \
}

struct CIC1_struct
{
    int32 integrator;
    int32 decimator_memory[CIC_upsample1];
    float out;
    int32 div_memory;
    float counter;
    float OSR;
    float div_OSR;
    float range_modifier;
    float div_range_modifier;
    float decimation_counter;
    float decimation_ratio;
};

struct CIC1_global_struct
{
    int32 enable_int;
    int32 enable_diff;
    int32 div_memory;
    int32 counter;
    float OSR;
    float div_OSR;
    int32 decimation_counter;
    int32 decimation_ratio;
};

struct CIC1_local_struct
{
    int32 integrator;
    int32 decimator_memory[CIC_upsample1];
    float out;
    float range_modifier;
    float div_range_modifier;
};

struct CIC1_adaptive_global_struct
{
    float cycle_enable[2];
    int32 div_memory[2];
    float OSR_adaptive[2];
    float div_OSR_adaptive[2];
    float counter[2];
    float select_output;
    float change_timer;
    float Ts;
};

struct CIC1_adaptive_struct
{
    float range_modifier;
    float out_temp[2];
    float out;
    float div_range_modifier;
    int32 integrator;
    int32 decimator_memory[2][CIC_upsample2];
};

struct CIC1_adaptive2_global_struct
{
    int32 write_ptr;
    int32 read_ptr;
    int32 counter_WDIST;
    int32 counter_WDIST_read_val;
    int32 WDIST;
    int32 read_enable;
    int32 write_enable;
    int32 inc_delay;
    int32 dec_delay;
    float new_OSR;
    float OSR_adaptive;
    float div_OSR_adaptive;
    float Ts;
};

struct CIC1_adaptive2_struct
{
    int32 integrator;
    int32 decimator_memory[CIC_upsample2];
    float out;
    float range_modifier;
    float div_range_modifier;
};

struct CIC2_struct
{
    int32 integrator[2];
    int32 subtractor[CIC_upsample1];
    int32 decimator_memory[CIC_upsample1];
    float out;
    int32 div_memory;
    float counter;
    float OSR;
    float div_OSR;
    float range_modifier;
    float div_range_modifier;
    float decimation_counter;
    float decimation_ratio;
};

struct abcn_struct
{
    float a;
    float b;
    float c;
    float n;
};

struct abc_struct
{
    float a;
    float b;
    float c;
};

struct abg_struct
{
    float alf;
    float bet;
    float gam;
};

struct transformation_struct
{
    float a;
    float b;
    float c;
    float n;
    float alf;
    float bet;
    float gam;
    float d;
    float q;
    float z;
};

struct SOGI_struct
{
    float x;
    float qx;
    float input_err;
};

struct PI_struct
{
    float Kp;
    float Ts_Ti;
    float integrator;
    float proportional;
    float lim_H;
    float lim_L;
    float out;
};

struct Filter1_struct
{
    float Ts_Ti;
    float out;
    float Kahan;
};

struct PR_struct
{
    float y0;
    float y1;
    float y2;
    float x0;
    float x1;
    float Kp;
    float Ki;
    float Ts;
    float out;
    float w;
};

struct Resonant_struct
{
    float input;
    float gain;
    float x0;
    float x1;
    float y0;
    float y1;
    CLA_TRIGPTR trigonometric;
    CLA_TRIGPTR trigonometric_comp;
};

void PR_calc(struct PR_struct* PR, float error);
void PR_calc_imp(struct PR_struct* PR, float error);

extern float CIC1_adaptive_filter_CLAasm(struct CIC1_adaptive_global_struct *CIC_global, struct CIC1_adaptive_struct *CIC, float input);
float CIC1_adaptive_filter(struct CIC1_adaptive_global_struct *CIC_global, struct CIC1_adaptive_struct *CIC, float input);

extern void CIC1_adaptive_global_CLAasm(struct CIC1_adaptive_global_struct *CIC_global, float frequency);
void CIC1_adaptive_global_calc(struct CIC1_adaptive_global_struct *CIC_global, float frequency);

extern float CIC1_adaptive2_filter_CLAasm(struct CIC1_adaptive2_global_struct* CIC_global, struct CIC1_adaptive2_struct* CIC, float input);
float CIC1_adaptive2_filter(struct CIC1_adaptive2_global_struct* CIC_global, struct CIC1_adaptive2_struct* CIC, float input);

extern void CIC1_adaptive2_global_CLAasm(struct CIC1_adaptive2_global_struct* CIC_global, float frequency);
void CIC1_adaptive2_global_calc(struct CIC1_adaptive2_global_struct* CIC_global, float frequency);

extern void CIC1_filter_global_CLAasm(struct CIC1_global_struct* CIC_global);
void CIC1_filter_global(struct CIC1_global_struct* CIC_global);

extern void CIC1_filter_local_CLAasm(struct CIC1_global_struct* CIC_global, struct CIC1_local_struct* CIC, float input);
void CIC1_filter_local(struct CIC1_global_struct* CIC_global, struct CIC1_local_struct* CIC, float input);

extern void CIC1_filter_CLAasm(struct CIC1_struct *CIC, float input);
void CIC1_filter(struct CIC1_struct *CIC, float input);

extern void CIC2_filter_CLAasm(struct CIC2_struct *CIC, float input);
void CIC2_filter(struct CIC2_struct *CIC, float input);

extern void Resonant_filter_calc_CLAasm(struct Resonant_struct* Resonant, float input);
void Resonant_filter_calc(struct Resonant_struct* Resonant, float input);

extern float Resonant_calc_CLAasm(struct Resonant_struct* Resonant, float error);
float Resonant_calc(struct Resonant_struct* Resonant, float error);

extern float Resonant_mult_calc_CLAasm(struct Resonant_struct* Resonant, float error, Uint16 harmonics);
float Resonant_mult_calc(struct Resonant_struct* Resonant, float error, Uint16 harmonics);

extern float Resonant_mult_calc2_CLAasm(struct Resonant_struct *Resonant, Uint16 harmonics);
float Resonant_mult_calc2(struct Resonant_struct *Resonant, Uint16 harmonics);

extern void PI_antiwindup_fast_ff_CLAasm(struct PI_struct *PI, float error, float feedforward);
void PI_antiwindup_fast_ff(struct PI_struct *PI, float error, float feedforward);

extern void PI_antiwindup_fast_CLAasm(struct PI_struct *PI, float error);
void PI_antiwindup_fast(struct PI_struct *PI, float error);

extern void PI_antiwindup_CLAasm(struct PI_struct *PI, float error);
void PI_antiwindup(struct PI_struct *PI, float error);

extern void Filter1_calc_CLAasm(struct Filter1_struct *Filter1, float input);
void Filter1_calc(struct Filter1_struct *Filter1, float input);
#endif /* Controllers_H_ */
