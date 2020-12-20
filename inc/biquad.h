#ifndef BIQUAD_H_
#define BIQUAD_H_


#define BIQUAD_CROSSOVER_FREQ_HZ  340.0
#define BIQUAD_Q                  0.707 // butterworth filter



int biquad_loadCoeffs_LR(double fsHz);

#endif
