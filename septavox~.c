#include <stdint.h>
#include "m_pd.h"  
#include "pp6.h"
#include "wave_synth.h"


static t_class *septavox_tilde_class;

typedef struct _septavox_tilde {  
    t_object  x_obj; 
    t_int i_note_out;
    t_int i_note_num;
    t_int i_note_vel;
    t_outlet *out; 
    pocket_piano pp6;
} t_septavox_tilde;  

void septavox_tilde_bang(t_septavox_tilde *x) {
}

void septavox_tilde_notein(t_septavox_tilde *x, t_floatarg num) {  
}  

void septavox_tilde_waveform(t_septavox_tilde *x, t_floatarg num) {  
    x->pp6.aux_button_count = (uint8_t)num;
    post("my waveform: %d", x->pp6.aux_button_count);
}

void *septavox_tilde_new(void) {  
    t_septavox_tilde *x = (t_septavox_tilde *)pd_new(septavox_tilde_class);  
    
    //inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("notein"));
    //inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("waveform"));

    x->out = outlet_new(&x->x_obj, &s_signal);

    pp6_init(&(x->pp6));
    wave_synth_init(&(x->pp6));
    
    x->pp6.freqs[0] = 440.f;
    x->pp6.freqs[1] = 1.f;
    x->pp6.freqs[2] = 1.f;
    x->pp6.freqs[3] = 1.f;

	x->pp6.amps[0] = 1.f;
	x->pp6.amps[1] = 0.f;
	x->pp6.amps[2] = 0.f;
	x->pp6.amps[3] = 0.f;
    x->pp6.aux_button_count = 0;


    return (void *)x;  
}  

t_int *septavox_tilde_perform(t_int *w) {
    t_septavox_tilde *x = (t_septavox_tilde *)(w[1]);
    t_sample *out = (t_sample *)(w[2]);
    int n = (int)(w[3]);

    while (n--) {
        *out++ = wave_synth_process(&(x->pp6));
    }
    return (w + 4);
}

void septavox_tilde_dsp(t_septavox_tilde *x, t_signal **sp) {
    dsp_add(septavox_tilde_perform, 3, x, sp[0]->s_vec, sp[0]->s_n);
}

void septavox_tilde_setup(void) {  
    septavox_tilde_class = class_new(gensym("septavox~"),  
        (t_newmethod)septavox_tilde_new,  
        0, sizeof(t_septavox_tilde),  
        CLASS_DEFAULT, 0);  
    class_addbang(septavox_tilde_class, septavox_tilde_bang);  
    class_addmethod(septavox_tilde_class,  
        (t_method)septavox_tilde_notein, gensym("notein"),  
        A_DEFFLOAT, 0);  
    class_addmethod(septavox_tilde_class,  
        (t_method)septavox_tilde_waveform, gensym("waveform"),  
        A_DEFFLOAT, 0);  

    class_addmethod(septavox_tilde_class, (t_method)septavox_tilde_dsp, gensym("dsp"), A_CANT, 0);
}
