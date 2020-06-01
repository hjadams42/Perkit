#ifndef _FILTER_H
#define _FILTER_H

#define INIT_Y			0.0
#define F_SAMP			48000
#define PI              3.14159265358979323846

//Envelope constants, v=-60db
#define ATTACK 			0.99800
#define DROP_LEVEL		0.001
#define DECAY_INIT		0.2
#define LN_V			6.90775527898
#define MAX_AMP         0.8

typedef struct {
	int status;
	double decay_factor;
	double attack_factor;
	double decay_amp;
	float attack_amp;
	float amp;
} Env;

typedef struct {
	float y_state[MAX_CHAN];
	float freq_filt;
} Filt;


class Filter {
public:
	Filter();
	~Filter();
    void set_fs(int FS);
	void set_freq(float freq);
	void set_decay(float decay);
    void set_status(int status);
    int  get_status();
	void filter_gate(const float *ibuf, float *obuf, int num_frames, int chan);
	float envelope (int chan);
	float filter_onepole (float x, int chan);
private:
	//filtering
    Filt filt;
    float x, y, freq;
    int fs;  // in, out, max cutoff, sampling rate
	//envelope
	Env env;
    Env *pe = &env; //env struct contains status, decay and attack factor and amplitudes
	float decay;    //in seconds, provided from slider
	float ln_v;     //decay_factor calculation constant
	float amp;      //amplitude output by envelope
};

#endif
