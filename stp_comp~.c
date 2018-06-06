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

// Prototype
t_sample gainComputer(t_sample in, t_float ratio, t_float threshold, t_float knee_width);
t_sample smoothDetection(t_sample x,t_sample y, t_float a_attack, t_float a_release);

t_int *stp_comp_tilde_perform(t_int *w)
{
    stp_comp_tilde *x = (stp_comp_tilde *)(w[1]);
    t_sample  *in = (t_sample *)(w[2]);
    t_sample  *out =  (t_sample *)(w[3]);
    int n =  (int)(w[4]);
    int i;

    t_sample outGain[n];
    t_sample inLevelDetector[n];
    t_sample outLevelDetector = 0;
    t_sample controlVoltage;
    t_sample abs_in[n];
    float fs = sys_getsr(); //Get Sampling Freq from the system
    //Debugger shit
    //post("Fs= %f",fs);

    // Calculate smooth detection filter coefficients eq. 7
    t_sample a_attack = expf(-1/(x->attack*fs));
    t_sample a_release = expf(-1/(x->release*fs));

    for (i=0; i<n; i++) {
    	abs_in[i] = abs(in[i]);
    	outGain[i] = gainComputer(abs_in[i],x->threshold,x->ratio,x->knee_width);
    	post("gain computer %f",outGain[i]);
    	inLevelDetector[i] = abs_in[i]-outGain[i];
    	outLevelDetector = smoothDetection(inLevelDetector[i], outLevelDetector, a_attack, a_release);
    	post("outLevelDetector %f",outLevelDetector);
    	controlVoltage = x->makeup_gain - outLevelDetector;
    	out[i] = in[i] * controlVoltage;
    	post("control voltage %f",controlVoltage);
    }
    /* return a pointer to the dataspace for the next dsp-object */
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

//

t_sample gainComputer(t_sample in, t_float ratio, t_float threshold, t_float knee_width) {
	t_sample out;
	if(2 * (in - threshold) < - knee_width) {
		out = in;
	} else if (2 * fabsf(in - threshold) <= knee_width) {
		out = in + (1 / ratio - 1) * pow(in - threshold + knee_width / 2, 2) / (2 * knee_width);
	} else {
		out = threshold + (in - threshold) / ratio;
	}
	return out;
}

// x is current sample and y is the result of last smoothed sample.
// a_attack and a_relase are filter coefficients. fs sampling frequency in Hz
// returns smoothed sample
t_sample smoothDetection(t_sample x,t_sample y, t_float a_attack, t_float a_release) {
	t_sample out;
	// Calculate filter coefficients for attack and release time eq. 7
	if (x>y){
		out = (a_attack*y) + (1-a_attack)*x;
	}
	else {
		out = (a_release*y) + (1-a_release)*x;
	}
	return out;
}

// Create the stp_comp objectt hat was used for object creation
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
    x->makeup_gain = 0;
    x->threshold = 0.5;
    x->ratio = 2;
    x->knee_width = 1;
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

void stp_comp_set(stp_comp_tilde *x, t_floatarg gain, t_floatarg threshold, t_floatarg ratio)
{
    x->makeup_gain = gain;
    x->threshold = threshold;
    x->ratio = ratio;
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
