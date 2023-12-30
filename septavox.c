#include <stdint.h>
#include "m_pd.h"  
#include "pp6.h"

static t_class *septavox_class;  


typedef struct _septavox {  
    t_object  x_obj; 
    t_int i_note_out;
    t_int i_note_num;
    t_int i_note_vel;
    t_int i_voices[4];


} t_septavox;  

static void allocate_voices(t_septavox *x){
    int j, already_playing = 0;
    
    t_int num = x->i_note_num;
    t_int vel = x->i_note_vel;

    // if the key is up
    if (vel == 0){
        for (j=0; j < 4; j++){
            if (x->i_voices[j] == num) x->i_voices[j] = 0;
        }
    }
    // if its down
    else {
        // see that its not already playing
        already_playing = 0;
        for (j=0; j < 4; j++) {
            if (x->i_voices[j] == num) already_playing = 1;
        }
        // if its not, try to allocate
        if (!already_playing){
            for (j=0; j < 4; j++) {
                if (x->i_voices[j] == 0) {
                    x->i_voices[j] = num;  
                    break;
                }
            }
        }
    }
}

void septavox_bang(t_septavox *x) {
}

void septavox_notein(t_septavox *x, t_floatarg num) {  
    x->i_note_num = (t_int)num;
    allocate_voices(x); 
}  

void septavox_velin(t_septavox *x, t_floatarg vel) {  
    x->i_note_vel = (t_int)vel;
}

void *septavox_new(void) {  
    t_septavox *x = (t_septavox *)pd_new(septavox_class);  
    
    outlet_new(&x->x_obj, &s_float);

    inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("notein"));
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("velin"));

    pp6_init();
    
    return (void *)x;  
}  

void septavox_setup(void) {  
    septavox_class = class_new(gensym("septavox"),  
        (t_newmethod)septavox_new,  
        0, sizeof(t_septavox),  
        CLASS_DEFAULT, 0);  
    class_addbang(septavox_class, septavox_bang);  
    class_addmethod(septavox_class,  
        (t_method)septavox_notein, gensym("notein"),  
        A_DEFFLOAT, 0);  
    class_addmethod(septavox_class,  
        (t_method)septavox_velin, gensym("velin"),  
        A_DEFFLOAT, 0);  
}
