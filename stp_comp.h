#ifndef stp_comp_h
#define stp_comp_h

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef float STP_INPUTVECTOR;
typedef float STP_OUTPUTVECTOR;

typedef struct stp_comp
{
    // Gain computer variables
    float makeup_gain; // dB
    float ratio; // linear
    float threshold; // dB
    float knee_width;

    // Peak detector variables
    float attack;  // in s
    float release; // in s
} stp_comp;

stp_comp *stp_comp_new();

void stp_comp_free(stp_comp *x);

void stp_comp_perform(stp_comp *x, STP_INPUTVECTOR *in, STP_OUTPUTVECTOR *out, int vectorSize, float sampling_rate);

#endif /* stp_comp_h */
