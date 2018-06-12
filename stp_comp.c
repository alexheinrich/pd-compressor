#include "stp_comp.h"

stp_comp *stp_comp_new()
{
    stp_comp *x = (stp_comp *) malloc(sizeof(stp_comp));

    x->makeup_gain = 1;
    x->threshold = 0.5;
    x->ratio = 5;
    x->knee_width = 0.1;
    x->attack = 0.02;
    x->release = 0.02;

    return x;
}

void stp_comp_free(stp_comp *x)
{
    free(x);
}

void stp_comp_perform(stp_comp *x, STP_INPUTVECTOR *in, STP_OUTPUTVECTOR *out, int n, float sampling_rate)
{
	float a_attack, a_release, in_abs, buffer, post_level_detector, control_voltage;

	post_level_detector = 0;

    // Calculate smooth detection filter coefficients eq. 7
    a_attack = expf(-1 / (x->attack * sampling_rate));
    a_release = expf(-1 / (x->release * sampling_rate));

    for (int i = 0; i < n; i++) {
    	in_abs = fabsf(in[i]);

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

    	// Pre level detector (feed forward loop)
    	buffer = in_abs - buffer;

    	// Level detector
    	if (buffer > post_level_detector) {
    		post_level_detector = a_attack * post_level_detector + (1 - a_attack) * buffer;
    	}
    	else {
    		post_level_detector = a_release * post_level_detector + (1 - a_release) * buffer;
    	}

    	control_voltage = x->makeup_gain - post_level_detector;

    	out[i] = in[i] * control_voltage;
    }
}
