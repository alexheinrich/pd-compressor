#include "stp_comp.h"
#include "m_pd.h"

stp_comp *stp_comp_new()
{
    stp_comp *x = (stp_comp *) malloc(sizeof(stp_comp));

    x->makeup_gain = 0.0f;
    x->threshold = 0.0f;
    x->ratio = 1.0f;
    x->knee_width = 1.0f;
    x->attack = 0.01f;
    x->release = 0.02f;
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
	// Knee width 1 sets 20 dB knee (soft). Rest is hard knee as default.
	if (x->knee_width == 1){
		x->knee_width = 20;
	}
	else {
		x->knee_width = 0;
	}
	// Ratio exception handling
	if (x->ratio < 1.0f) {
		x->ratio = 1.0f;
	}

	// Level detector exception handling
	if (x->attack <= 0.0f) {
		x->attack = 0.01f;
	}

	if (x->release <= 0.0f) {
		x->release = 0.001f;
	}

    // Calculate smooth detection filter coefficients eq. 7
    a_attack = expf(-1.0f / (x->attack * sampling_rate));
    a_release = expf(-1.0f / (x->release * sampling_rate));

    // Generate the control voltage
    for (int i = 0; i < vector_size; ++i) {
    	in_abs = fabsf(in[i]);

    	if (in_abs != 0.0f) {
    		// Convert values in dB
    		in_abs = 20.0f * logf(in_abs);
    	}
		else {
			in_abs = MINUS_INF;
    	}

    	// Gain computer
    	if (2.0f * (in_abs - x->threshold) < -1.0f * x->knee_width) {
    		buffer = in_abs;
    	}
    	else if (2.0f * fabsf(in_abs - x->threshold) <= x->knee_width) {
    		buffer = in_abs + (1.0f / x->ratio - 1.0f) * powf(in_abs - x->threshold + x->knee_width / 2.0f, 2.0f) / (2.0f * x->knee_width);
    	}
    	else {
    		buffer = x->threshold + (in_abs - x->threshold) / x->ratio;
    	}

    	// Pre-level detector (feed forward loop)
    	buffer = in_abs - buffer;

    	// Level detector
    	if (buffer > x->post_level_detector) {
    		x->post_level_detector = a_attack * x->post_level_detector + (1.0f - a_attack) * buffer;
    	}
    	else {
    		x->post_level_detector = a_release * x->post_level_detector + (1.0f - a_release) * buffer;
    	}

    	// Control voltage
    	control_voltage = x->makeup_gain - x->post_level_detector;

    	// Convert the control voltage to linear and apply it to the audio.
    	out[i] = in[i] * powf(10.0f, control_voltage / 20.0f);
    }
}
