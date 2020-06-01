
//#include <math.h>
#include <cstdint>
#include <algorithm>
#include <math.h>
#include "main.h"
#include "distorter.h"


Distorter::Distorter(void)
{
	this->thresh= INIT_THRESH;
	this->amp = INIT_AMP;
	this->noise_ratio = 0.0;
}

Distorter::~Distorter() {;}

void Distorter::set_amp(double amplitude)
{
	amp = amplitude;
	return;
}

void Distorter::set_thresh(double threshold)
{
	thresh = threshold;
	return;
}

void Distorter::set_noise(float noise_ratio_usr)
{
	noise_ratio = noise_ratio_usr;
	return;
}

void Distorter::mix_with_noise (float *ibuf, float *obuf, float *noisebuf, int num_frames)
{
    //generate random floating point value and mix with single input sample on each loop
    
    for (int i = 0; i<num_frames; i++)
    {
    obuf[i] = noisebuf[i]*noise_ratio + (1-noise_ratio) * ibuf[i];
    }
    
    return;
}

    
void Distorter::fold_distorter(const float *ibuf, float *obuf, int num_frames) 
{
	double x_amp, x_clamp, x_fold;
	amp = std::max(amp, 1.);
	thresh = std::max(thresh, 0.01);
	double invthresh = thresh * -1.;

    for (int i=0; i<num_frames; i++)
    {
        //input and scale up signal by amp value
        x_amp = ibuf[i];
        //scale up signal by amp value
        x_amp *= amp;
        // create a clamped version of scaled signal at the threshold
        x_clamp = std::clamp(x_amp, invthresh, thresh);

        // folding step 1a: first must deal with scaling past (-1, 1) fmod takes floating point remainder
        // we make the folded term proportional to the difference between signal and threshold, x_amp-thresh
        // since it is the remainder of dividing out thresh*4, it will be equal to x_amp-thresh unless this is greater than thresh*4
        // case 1: for positive x_amp, thresh < x_amp < thresh*4: this scales the folded signal down to less than thresh
        // case 2: x_amp is negative, x_amp < invthresh: x_amp-thresh has magnitude greater than thresh
        // (case 2:) fmod will return negative (so take the absolute value - sign is restored at the end)
        // case 3: if inv_thresh < x_amp < thresh, ignore this whole section (wavefold will not activate on that sample)
        x_fold = abs(fmod((x_amp-thresh), (thresh*4.)));

        // folding step 2: now remove offset, folding closer to 0 by subtracting 2*thresh, and taking absolute value again
        // case 1: x_fold starts less than thresh. so in step 2 it will actually end up greater in magnitude, thresh < x_fold < 2*thresh
        // case 2: initial: thresh < x_fold < 2*thresh.  After step 2, 0 < x_fold < thresh;
        x_fold = abs(x_fold - (thresh*2));

        // folding step 3: restore sign by subtracting thresh.
        //case 1:  after subtracting thresh, x_fold is less than thresh
        //case 2:  after subtracting thresh, 0 > x_fold > invthresh
        x_fold -= thresh;

        // above section adopted from: xp = abs(abs(fmod((xa - thresh), (thresh * 4.))) - (thresh * 2.)) - thresh;

        if (x_amp > thresh || x_amp < invthresh)
        {
            obuf[i] = (float)x_fold;
        }
        else
        {
            obuf[i] = (float)x_clamp;
        }
    }
	
	return;
}
