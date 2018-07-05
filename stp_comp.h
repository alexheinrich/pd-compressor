#ifndef stp_comp_h
#define stp_comp_h

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef float STP_INPUTVECTOR;
typedef float STP_OUTPUTVECTOR;
/**
 * @brief stp_comp
 * Feedforward dynamic range compressor.The detector is placed in the log domain after
 * the gain computer, since this generates a smooth envelope, has no attack lag, and allows for easy implementation of variable knee width.
 * For the compressor to have smooth performance on a wide variety of signals, with minimal artifacts and minimal modification
 * of timbral characteristics, the smooth, decoupled peak detector design has been used.
 *
 * Algorithms have been taken from:
 * Giannoulis, Dimitrios & Massberg, Michael & Reiss, Joshua. (2012). Digital Dynamic Range Compressor Design—A Tutorial and Analysis. AES: Journal of the Audio Engineering Society. 60.
 *
 */
typedef struct stp_comp
{
    //! Gain computer variables
    float makeup_gain; /*! in dB. The compressor reduces the level (gain) of the signal; therefore, applying a make-up gain to the signal allows for matching the input and output loudness level. */
    float ratio; /*! controls the input/output ratio for signals overshooting the threshold level. It determines the amount of compression applied. */
    float threshold; /*!<  in dB. It defines the level above which compression starts. Any signal overshooting the threshold will be reduced in level. */
    float knee_width; /*!<  in dB. It controls whether the bend in the compression characteristic (see Fig. 1) has a sharp angle or has a rounded edge.*/

    //! Peak detector variables.
    float attack;  /*! Attack time of compressor in s */
    float release; /*! Release time of compressor in s */

    float post_level_detector; /*! Dummy variable for stp_perform function*/
} stp_comp;

stp_comp *stp_comp_new();

void stp_comp_free(stp_comp *x);

void stp_comp_perform(stp_comp *x, STP_INPUTVECTOR *in, STP_OUTPUTVECTOR *out, int vectorSize, float sampling_rate);

#endif /* stp_comp_h */
