#include "m_pd.h"
#include <math.h>
#include <stdlib.h>

static t_class *stp_comp_tilde_class;

typedef struct stp_comp_tilde
{
    t_object  x_obj;
    t_int status;
    // Signal Class Related variable
    t_float  f;

    // Variables for Gain computing
    t_float makeup_gain; // dB
    t_float ratio; // linear
    t_float threshold; // dB
    t_float knee_width;

    // Variables for peak detector
    t_float attack;  // ms
    t_float release; // ms

    // Object inlets
    t_inlet *in_makeup;
    t_inlet *in_threshold;
    t_inlet *in_ratio;
    t_inlet *in_attack;
    t_inlet *in_release;

    // Object outlets
    t_outlet*x_out_l;
    t_outlet*x_out_r;
} stp_comp_tilde;

// Prototypes
t_sample gainComputer(t_sample in, t_float ratio, t_float threshold, t_float knee_width);
t_sample simpleGainComputer(t_sample in, t_float ratio, t_float threshold);
t_sample smoothDetection(t_sample x,t_sample y, t_float a_attack, t_float a_release);

t_int *stp_comp_tilde_perform(t_int *w)
{
    stp_comp_tilde *x = (stp_comp_tilde *)(w[1]);
    t_sample  *in = (t_sample *)(w[2]);
    t_sample  *out =  (t_sample *)(w[3]);
    int n =  (int)(w[4]);

    // Calculate smooth detection filter coefficients eq. 7
    float fs = sys_getsr(); // Get sampling freq from the system
    t_sample a_attack = expf(-1 / (x->attack * fs));
    t_sample a_release = expf(-1 / (x->release * fs));

    t_sample in_abs, buffer, outLevelDetector, controlVoltage;
    outLevelDetector = 0;

    for (int i = 0; i < n; i++) {
    	// Calc abs value of sample
    	in_abs = fabsf(in[i]);
    	buffer = in_abs;

    	// Gain computer
    	if (2 * (buffer - x->threshold) < -1 * x->knee_width) {
    		buffer = buffer;
    	}
    	else if (2 * fabsf(buffer - x->threshold) <= x->knee_width) {
    		buffer = buffer + (1 / x->ratio - 1) * pow(buffer - x->threshold + x->knee_width / 2, 2) / (2 * x->knee_width);
    	}
    	else {
    		buffer = x->threshold + (buffer - x->threshold) / x->ratio;
    	}

    	// Before level detector (feed forward loop)
    	buffer = in_abs - buffer;

    	// Seems ok up until here...
    	// post("buffer %f", buffer);

    	// TODO: Debug... value is very small
    	// Level detector
    	if (buffer > outLevelDetector) {
    		outLevelDetector = a_attack * outLevelDetector + (1 - a_attack) * buffer;
    	}
    	else {
    		outLevelDetector = a_release * outLevelDetector + (1 - a_release) * buffer;
    	}

    	controlVoltage = x->makeup_gain - outLevelDetector;

    	out[i] = in[i] * controlVoltage;
    }

    // Return a pointer to the dataspace for the next dsp-object
    return (w+5);
}

void stp_comp_tilde_dsp(stp_comp_tilde *x, t_signal **sp)
{
    dsp_add(stp_comp_tilde_perform, 4, x, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n);
}

void stp_comp_tilde_free(stp_comp_tilde *x)
{
	inlet_free(x->in_makeup);
	inlet_free(x->in_threshold);
	inlet_free(x->in_ratio);
	inlet_free(x->in_attack);
	inlet_free(x->in_release);

    outlet_free(x->x_out_l);
    outlet_free(x->x_out_r);
}

// Create the stp_comp object hat was used for object creation
// Because of the declaration of arguments in the class_new-function with A_GIMME, the constructor has following arguments:
// *s	the symbolic name,
// argc	the number of arguments passed to the object
// argv	a pointer to a list of argc atoms
void *stp_comp_tilde_new(t_symbol *s, int argc, t_atom *argv)
{
    stp_comp_tilde *x = (stp_comp_tilde *)pd_new(stp_comp_tilde_class);

    // Create outputs
    x->x_out_l = outlet_new(&x->x_obj, &s_signal);
    x->x_out_r = outlet_new(&x->x_obj, &s_signal);

    // When creating the object, several arguments should be passed by the user.
    // If the required amount of arguments have not been passed by the user, assign default values
    // Default values of the object
    x->makeup_gain = 1;
    x->threshold = 0.5;
    x->ratio = 5;
    x->knee_width = 0.1;
    x->attack = 10;
    x->release = 100;

    switch (argc) {
		default:
		case 3:
			x->ratio = atom_getfloat(argv+2);
		case 2:
			x->threshold = atom_getfloat(argv+1);
		case 1:
			x->makeup_gain = atom_getfloat(argv);
			break;
		case 0:
			break;
    }

    // Assign parameters
    x->in_makeup = floatinlet_new(&x->x_obj, &x->makeup_gain);
    x->in_threshold = floatinlet_new(&x->x_obj, &x->threshold);
    x->in_ratio = floatinlet_new(&x->x_obj, &x->ratio);
    x->in_attack = floatinlet_new(&x->x_obj, &x->attack);
    x->in_release = floatinlet_new(&x->x_obj, &x->release);

    return (void *)x;
}

void stp_comp_set(stp_comp_tilde *x, t_floatarg gain, t_floatarg threshold, t_floatarg ratio, t_floatarg attack, t_floatarg release)
{
    x->makeup_gain = gain;
    x->threshold = threshold;
    x->ratio = ratio;
    x->attack = attack;
    x->release = release;
}

void stp_comp_tilde_setup(void)
{
      stp_comp_tilde_class = class_new(gensym("stp_comp~"),
            (t_newmethod)stp_comp_tilde_new,
            (t_method)stp_comp_tilde_free,
            sizeof(stp_comp_tilde),
            CLASS_DEFAULT,
            A_GIMME, 0);

      class_addmethod(stp_comp_tilde_class, (t_method)stp_comp_tilde_dsp, gensym("dsp"), 0);

      // this adds the gain message to our object
      class_addmethod(stp_comp_tilde_class, (t_method)stp_comp_set, gensym("set"), A_DEFFLOAT, 0);
      CLASS_MAINSIGNALIN(stp_comp_tilde_class, stp_comp_tilde, f);
}
