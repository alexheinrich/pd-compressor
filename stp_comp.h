/**
 * @file stp_comp.h
 * @author Alex Heinrich, Mert Aslantürk, Simon Gapp <br>
 * Technical University of Berlin <br>
 * <br>
 * @brief stp_comp
 * Feed-forward design dynamic range compressor. The detector is placed in the log domain after <br>
 * the gain computer, since this generates a smooth envelope, has no attack lag, and allows <br>
 * for easy implementation of variable knee width. For the compressor to have smooth performance on a wide <br>
 * variety of signals, with minimal artifacts and minimal modification of timbral characteristics, <br>
 *  the smooth, decoupled peak detector design has been used. <br>
 * <br>
 * Algorithms have been taken from: <br>
 * Giannoulis, Dimitrios & Massberg, Michael & Reiss, Joshua. (2012). Digital Dynamic Range Compressor Design—A Tutorial and Analysis. AES: Journal of the Audio Engineering Society. 60. <br>
 *
 */

#ifndef stp_comp_h
#define stp_comp_h

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef float STP_INPUTVECTOR;
typedef float STP_OUTPUTVECTOR;

typedef struct stp_comp
{
    //! Gain computer variables
    float makeup_gain; /*! in dB. The compressor reduces the level (gain) of the signal; therefore, applying a make-up gain to the signal allows for matching the input and output loudness level. */
    float ratio; /*! controls the input/output ratio for signals overshooting the threshold level. It determines the amount of compression applied. */
    float threshold; /*!<  in dB. It defines the level above which compression starts. Any signal overshooting the threshold will be reduced in level. */
    float knee_width; /*!<  Important: It only accepts 0 or 1 as input. 1 Sets soft knee (20dB) and 0 hard knee (0dB). It controls whether the bend in the compression characteristic (see Fig. 1) has a sharp angle or has a rounded edge.*/

    //! Peak detector variables.
    float attack;  /*! Attack time of compressor in s */
    float release; /*! Release time of compressor in s */

    float post_level_detector; /*! Dummy variable for stp_perform function*/
} stp_comp;

/**
* @related stp_comp
* @brief Creates a new compressor object<br>
* @return a pointer to the newly created stp_comp object <br>
*/

stp_comp *stp_comp_new();
/**
* @related stp_comp
* @brief Frees a compressor object<br>
* @param x My compressor object <br>
* The function frees the allocated memory<br>
* of a compressor object
*/

void stp_comp_free(stp_comp *x);
/**
* @related stp_gain
* @brief Performs the dynamic compression in realtime <br>
* @param x My compressor object <br>
* @param in The input vector <br>
* @param out The output vector <br>
* @param vectorSize The vectorSize <br>
* The function stp_comp_perform performs the compression of <br>
* the incoming signal and copies the result to the output vector <br>
*/
void stp_comp_perform(stp_comp *x, STP_INPUTVECTOR *in, STP_OUTPUTVECTOR *out, int vectorSize, float sampling_rate);

#endif /* stp_comp_h */
