/*
 * Scope.h
 *
 *  Created on: 19 cze 2019
 *      Author: Mr.Tea
 */

#ifndef SCOPE_H_
#define SCOPE_H_

#include "stdafx.h"

#define SCOPE_BUFFER 1250
#define SCOPE_CHANNEL 12
typedef float scope_data_type;

#ifdef __cplusplus
extern "C" {
#endif

struct Scope_v1{
    scope_data_type data[SCOPE_CHANNEL][SCOPE_BUFFER];
    scope_data_type *data_in[SCOPE_CHANNEL];
    Uint32 index;
    Uint32 decimation;

    int32 acquire_counter;
    Uint32 acquire_before_counter;

    Uint32 finished_sorting;
    Uint32 acquire_before_trigger;
};

void Scope_start();
Uint32 Scope_trigger(float input, float *input_last, float trigger_val, float edge);
void Scope_trigger_unc();
void Scope_task();

#ifdef __cplusplus
}
#endif // extern "C"

#endif /* SCOPE_H_ */
