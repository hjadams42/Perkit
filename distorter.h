#ifndef _DISTORTER_H
#define _DISTORTER_H

#define INIT_AMP		2.0
#define INIT_THRESH		0.5


class Distorter {
public:
	Distorter();
	~Distorter();
	void set_thresh(double thresh);
	void set_amp(double amp);
	void set_noise(float noise_ratio);
    void mix_with_noise (float *ibuf, float *obuf, float *noisebuf, int num_frames);
	void fold_distorter(const float *ibuf, float *obuf, int num_frames);
private:
	double amp;
	double thresh;
	float noise_ratio;
};

#endif
