#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <cmath>
#include "main.h"
#include "filter.h"


Filter::Filter(void)
{
	this->fs = F_SAMP; //s sampling rate
	this->freq= (float)fs/2; // max cutoff init
	
    for (int i=0; i<MAX_CHAN; i++)
    {
        filt.y_state[i] = INIT_Y; // 0 initialization
    }
    env.status = -1;
	this->ln_v = LN_V; // delay factor constant
	this->decay = DECAY_INIT; // in seconds
	set_decay(this->decay);
	env.attack_factor = ATTACK;
    env.decay_amp = 1.;
    env.attack_amp = 1.;
}

Filter::~Filter() {;}

void Filter::set_fs(int FS)
{
    fs = FS;
    return;
}

void Filter::set_freq(float frequency)
{
	freq = frequency;
	return;
}

void  Filter::set_decay(float decay)
{
  	//calculate decay factor
    float n;
    double t_const, exp_const;
    n = decay * fs;
    t_const =(double) n / (ln_v * fs);
    exp_const = (-1./fs)/t_const;
    env.decay_factor = exp(exp_const);
	return;
}

void  Filter::set_status(int status)
{
    env.status = status;
    return;
}

int Filter::get_status()
{
    return env.status;
}

//float mix_interp(x, y, factor)
//{
	// y is previous sample
	//float y_out;
	//y_out = y + factor*(x-y)	
//}

void Filter::filter_gate(const float *ibuf, float *obuf, int num_frames, int chan)
{

	for (int i=0; i<num_frames; i++)
	{
		x = ibuf[i];
		//calculate point in envelope
		amp = envelope(chan);
		//sweep filter frequency
		filt.freq_filt = freq * amp;
		//filter, output one sample and also store it in y_state
		float z = filter_onepole(x, chan);
		// output
		obuf[i] = (z * amp * MAX_AMP);
	}
	return; 
}

float Filter::envelope(int chan)
{
    /*
    if (env.status == 2) // envelope bypass switch
    {
        env.amp = 1.;
        return env.amp;
    } */
    if (env.decay_amp < DROP_LEVEL)
    {
        env.status = -1;
    }
    
    if (env.status < 0) // default, waiting for new note
    {
        env.amp = 0.;
        env.decay_amp = 1.;
        env.attack_amp = 1.;
        return env.amp;
    }
    else if (env.status > 2 ) // bypass: working
    {
        env.amp = 1.;
        env.decay_amp = 1.;
        env.attack_amp = 1.;
        return env.amp;
    }
   
    /* else {
        env.status = 0;
        env.attack_amp = 1.;
        env.decay_amp = 1.;
        //filt.y_state[chan] = 0;
        return env.amp;
    } */
    else
    {
        if (env.status == 1) //on trigger
        {
            env.amp = 0.;
            env.decay_amp = 1.;
            env.attack_amp = 1.;
        }
        env.amp = (1-pe->attack_amp)*pe->decay_amp;
        env.decay_amp *= pe->decay_factor;
        env.attack_amp *= pe->attack_factor;
        env.status = 0; //running, ready for retrigger
        return env.amp;
    }
}

float Filter::filter_onepole (float x, int chan)
{
	// calculate factor: use sin to scale frequency to a factor between 0 and 1
	double factor = std::clamp(sin(filt.freq_filt*2*PI/fs), 0.00001, 0.99999);
	y = filt.y_state[chan];
	// this->y_state = (y + factor*(x-y));
	filt.y_state[chan] += (factor*x - factor*y);
	return filt.y_state[chan];
}
