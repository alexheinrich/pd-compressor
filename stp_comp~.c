#include "m_pd.h"

static t_class *stp_comp_tilde_class;

typedef struct stp_comp_tilde
{
    t_object  x_obj;

    t_int status;
    t_float  f;
    t_float makeup_gain;
    t_float ratio;
    t_float threshold;

    t_inlet *x_in2;
    t_inlet *x_in3;
    t_inlet *x_in4;
    t_outlet*x_out;
} stp_comp_tilde;

t_int *stp_comp_tilde_perform(t_int *w)
{
    stp_comp_tilde *x = (stp_comp_tilde *)(w[1]);
    t_sample  *in = (t_sample *)(w[2]);
    t_sample  *out =  (t_sample *)(w[3]);
    int n =  (int)(w[4]);
    int i;

    for(i=0; i<n; i++) {
    	// formel 3 aus dem Paper
    	if(in[i] <= x->threshold) {
    		out[i] = in[i];
    	} else {
    		out[i] = x->threshold + (in[i] - x->threshold) / x->ratio;
    	}

    	out[i] = in[i] * x->makeup_gain;
    }
    if (x->status == 0) {
    post("%f", x->threshold);
    x->status = 1;
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
	inlet_free(x->x_in2);
	inlet_free(x->x_in3);
	inlet_free(x->x_in4);
    outlet_free(x->x_out);
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
    x->x_out = outlet_new(&x->x_obj, &s_signal);
    x->x_out = outlet_new(&x->x_obj, &s_float);
    // When creating the object, several arguments should be passed by the user.
    // If the required amount of arguments have not been passed by the user, assign default values
    // Default values of the object
    x->makeup_gain = 0;
    x->threshold = 0.5;
    x->ratio = 2;

    switch(argc){
    default:
    case 3:
      x->ratio=atom_getfloat(argv+2);
    case 2:
      x->threshold=atom_getfloat(argv+1);
    case 1:
      x->makeup_gain=atom_getfloat(argv);
      break;
    case 0:
      break;
    }
    // Assign parameters
    x->x_in2 = floatinlet_new (&x->x_obj, &x->makeup_gain);
    x->x_in3 = floatinlet_new (&x->x_obj, &x->threshold);
    x->x_in4 = floatinlet_new (&x->x_obj, &x->ratio);
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
      //class_addmethod(stp_comp_tilde_class, (t_method)stp_comp_set, gensym("gain"), A_DEFFLOAT,0);
      class_addmethod(stp_comp_tilde_class, (t_method)stp_comp_set, gensym("set"), A_DEFFLOAT, 0);
      CLASS_MAINSIGNALIN(stp_comp_tilde_class, stp_comp_tilde, f);
}
