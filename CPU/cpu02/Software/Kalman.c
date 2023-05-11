// Tomasz Œwiêchowicz swiechowicz.tomasz@gmail.com

#include <math.h>
#include "stdafx.h" 

void Kalman_calc(struct Kalman_struct *Kalman, float input)
{
    Uint16 i;

    register float prediction = Kalman->states[0];
    Kalman->states[1] = 0;
    for (i = 1; i < KALMAN_HARMONICS; i++)
    {
        register float temp = sincos_table[i - 1].cosine * Kalman->states[2 * i] - sincos_table[i - 1].sine * Kalman->states[2 * i + 1];
        Kalman->states[2 * i + 1] = sincos_table[i - 1].sine * Kalman->states[2 * i] + sincos_table[i - 1].cosine * Kalman->states[2 * i + 1];
        Kalman->states[2 * i] = temp;
        prediction += Kalman->states[2 * i];
    }

    register float error = input - prediction;
    register float *gain = (float*)Kalman->gain.ptr;
    for (i = 0; i < KALMAN_HARMONICS; i++)
    {
        Kalman->states[2 * i] += *gain++ * error;
        Kalman->states[2 * i + 1] += *gain++ * error;
        Kalman->rms_values[i] = sqrtf(Kalman->states[2 * i + 1] * Kalman->states[2 * i + 1] + Kalman->states[2 * i] * Kalman->states[2 * i]);
    }
}


//R = 0.005/(Ts*Ts)
const float Kalman_gain[2 * KALMAN_HARMONICS] =
{
 0.000448842521485309 ,
 0                    ,
 0.000634103002618609 ,
 -2.88548133061653e-05,
 0.000634669298262515 ,
 -1.06817608881194e-05,
 0.000634712723653466 ,
 -7.67962359421781e-06,
 0.000634722176840594 ,
 -6.85393361040006e-06,
 0.000634723172838953 ,
 -6.76106923904479e-06,
 0.000634720478185073 ,
 -7.00947417941968e-06,
 0.000634715452715322 ,
 -7.45065653159282e-06,
 0.000634708546694967 ,
 -8.01741640013077e-06,
 0.000634699881257989 ,
 -8.67634288898916e-06,
 0.000634689421983918 ,
 -9.41040974319991e-06,
 0.000634677036476851 ,
 -1.02116369173533e-05,
 0.000634662510249319 ,
 -1.10777356511361e-05,
 0.000634645542307851 ,
 -1.20105692603990e-05,
 0.000634625726622169 ,
 -1.30155801054626e-05,
 0.000634602517973840 ,
 -1.41018557348941e-05,
 0.000634575177028539 ,
 -1.52827659806226e-05,
 0.000634542679006945 ,
 -1.65772948895905e-05,
 0.000634503562165124 ,
 -1.80124327442099e-05,
 0.000634455657977260 ,
 -1.96274362666893e-05,
 0.000634395580292669 ,
 -2.14817563008641e-05,
 0.000634317667910526 ,
 -2.36709832563470e-05,
 0.000634211495935667 ,
 -2.63627931284139e-05,
 0.000634054915348005 ,
 -2.98928506874042e-05,
 0.000633787960896638 ,
 -3.51004164430066e-05,
 0.000633137770845265 ,
 -4.53407246376031e-05,
};
