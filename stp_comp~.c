#include "m_pd.h"

static t_class *stp_comp_tilde_class;

typedef struct stp_comp_tilde
{
    t_object  x_obj;
    t_sample f;
    t_sample makeup_gain;
    t_sample ratio;
    t_sample threshold;
    t_inlet *x_in;
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

    /* return a pointer to the dataspace for the next dsp-object */
    return (w+5);
}

void stp_comp_tilde_dsp(stp_comp_tilde *x, t_signal **sp)
{
    dsp_add(stp_comp_tilde_perform, 4, x, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n);
}

void stp_comp_tilde_free(stp_comp_tilde *x)
{
	inlet_free(x->x_out);
    outlet_free(x->x_out);
}

void *stp_comp_tilde_new(t_floatarg f)
{
    stp_comp_tilde *x = (stp_comp_tilde *)pd_new(stp_comp_tilde_class);

    x->x_in = floatinlet_new (&x->x_obj, &x->makeup_gain);
    //The main inlet is created automatically
    x->x_out = outlet_new(&x->x_obj, &s_signal);
    x->x_out = outlet_new(&x->x_obj, &s_signal);
    x->makeup_gain = 0;
    x->threshold = 0.5;
    x->ratio = 2;
    return (void *)x;
}

void stp_comp_set(stp_comp_tilde *x, float gain)
{
    x->makeup_gain = gain;
}

void stp_comp_tilde_setup(void)
{
      stp_comp_tilde_class = class_new(gensym("stp_comp~"),
            (t_newmethod)stp_comp_tilde_new,
            (t_method)stp_comp_tilde_free,
        sizeof(stp_comp_tilde),
            CLASS_DEFAULT,
            A_DEFFLOAT, 0);

      class_addmethod(stp_comp_tilde_class, (t_method)stp_comp_tilde_dsp, gensym("dsp"), 0);

      // this adds the gain message to our object
      class_addmethod(stp_comp_tilde_class, (t_method)stp_comp_set, gensym("gain"), A_DEFFLOAT,0);

      CLASS_MAINSIGNALIN(stp_comp_tilde_class, stp_comp_tilde, f);
}
