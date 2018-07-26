#include "m_pd.h"
#include "stp_comp.h"

static t_class *stp_comp_tilde_class;

typedef struct stp_comp_tilde
{
    t_object  x_obj;
    t_int status;
    t_float  f;
    stp_comp *comp;
    t_inlet *in_makeup_gain;
    t_inlet *in_threshold;
    t_inlet *in_ratio;
    t_inlet *in_attack;
    t_inlet *in_release;
    t_inlet *in_knee_width;
    t_outlet*x_out_l;
    t_outlet*x_out_r;
} stp_comp_tilde;

t_int *stp_comp_tilde_perform(t_int *w)
{
    stp_comp_tilde *x = (stp_comp_tilde *)(w[1]);
    t_sample  *in = (t_sample *)(w[2]);
    t_sample  *out =  (t_sample *)(w[3]);
    int n =  (int)(w[4]);

    stp_comp_perform(x->comp, in, out, n, sys_getsr());

    return (w+5);
}

void stp_comp_tilde_dsp(stp_comp_tilde *x, t_signal **sp)
{
    dsp_add(stp_comp_tilde_perform, 4, x, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n);
}

void stp_comp_tilde_free(stp_comp_tilde *x)
{
    stp_comp_free(x->comp);

	inlet_free(x->in_makeup_gain);
	inlet_free(x->in_threshold);
	inlet_free(x->in_ratio);
	inlet_free(x->in_attack);
	inlet_free(x->in_release);
	inlet_free(x->in_knee_width);

    outlet_free(x->x_out_l);
    outlet_free(x->x_out_r);
}

void *stp_comp_tilde_new(t_floatarg f)
{
	// Avoid unused parameter warning
	(void) f;

    stp_comp_tilde *x = (stp_comp_tilde *)pd_new(stp_comp_tilde_class);

    x->x_out_l = outlet_new(&x->x_obj, &s_signal);
    x->x_out_r = outlet_new(&x->x_obj, &s_signal);

    x->comp = stp_comp_new();

    // Map inlets to x_obj members
    x->in_makeup_gain = floatinlet_new(&x->x_obj, &x->comp->makeup_gain);
    x->in_threshold = floatinlet_new(&x->x_obj, &x->comp->threshold);
    x->in_ratio = floatinlet_new(&x->x_obj, &x->comp->ratio);
    x->in_attack = floatinlet_new(&x->x_obj, &x->comp->attack);
    x->in_release = floatinlet_new(&x->x_obj, &x->comp->release);
    x->in_knee_width =floatinlet_new(&x->x_obj, &x->comp->knee_width);

    return (void *)x;
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
      CLASS_MAINSIGNALIN(stp_comp_tilde_class, stp_comp_tilde, f);
}
