#include "stp_comp.h"

stp_comp *stp_comp_new()
{
    stp_comp *x = (stp_comp *) malloc(sizeof(stp_comp));

    x->makeup_gain = 0.0f;
    x->threshold = 0.0f;
    x->ratio = 1.0f;
    x->knee_width = 1.0f;
    x->attack = 0.01;
    x->release = 0.02;
    x->post_level_detector = 0.0f;
    return x;
}

void stp_comp_free(stp_comp *x)
{
    free(x);
}


void stp_comp_perform(stp_comp *x, STP_INPUTVECTOR *in, STP_OUTPUTVECTOR *out, int vector_size, float sampling_rate)
{
	float a_attack, a_release, in_abs, buffer, control_voltage;
	// Set knee soft or hard
	if (x->knee_width == 0.0){ // hard knee
		x->knee_width = 0.0;
	}
	else if (x->knee_width == 1.0){ // soft knee
		x->knee_width = 20.0;
	}
	// Exception handling
	else {
		x->knee_width = 0.0f;
	}
	// Ratio exception handling
	if (x->ratio < 1.0) {
		x->ratio = 1.0;
	}
	// Level detector exception handling
	if (x->attack <= 0) {
		x->attack = 0.001;
	}
	if (x->release <= 0) {
		x->release = 0.001;
	}
    // Calculate smooth detection filter coefficients eq. 7
    a_attack = expf(-1 / (x->attack * sampling_rate));
    a_release = expf(-1 / (x->release * sampling_rate));
    // Generate the control voltage
    for (int i = 0; i < vector_size; ++i) {
    	in_abs = fabsf(in[i]);
    	// Convert values in dB
    	in_abs = 20*log(in_abs);
    	// Gain computer
    	if (2 * (in_abs - x->threshold) < -1 * x->knee_width) {
    		buffer = in_abs;
    	}
    	else if (2 * fabsf(in_abs - x->threshold) <= x->knee_width) {
    		buffer = in_abs + (1 / x->ratio - 1) * pow(in_abs - x->threshold + x->knee_width / 2, 2) / (2 * x->knee_width);
    	}
    	else {
    		buffer = x->threshold + (in_abs - x->threshold) / x->ratio;
    	}
    	// Pre-level detector (feed forward loop)
    	buffer = in_abs - buffer;
    	// Level detector
    	if (buffer > x->post_level_detector) {
    		x->post_level_detector = a_attack * x->post_level_detector + (1 - a_attack) * buffer;
    	}
    	else {
    		x->post_level_detector = a_release * x->post_level_detector + (1 - a_release) * buffer;
    	}
    	// Control voltage
    	control_voltage = x->makeup_gain - x->post_level_detector;
    	// Convert the control voltage to linear and apply it to the audio.
    	out[i] = in[i] * pow(10,control_voltage/20);
    }
}
