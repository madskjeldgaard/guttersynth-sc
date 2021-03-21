// PluginGutterSynth.cpp
// Mads Kjeldgaard & Scott Carver (mail@madskjeldgaard.dk)

#include "SC_PlugIn.hpp"
#include "GutterSynth.hpp"

static InterfaceTable* ft;

namespace GutterSynth {

void InitGutterState(GutterState&) {
	M_PI = 3.14159265358979323846;	
	Fs = 44100;		
	filterCount = 24;
	filtersOn = true;	// turn off for Duffing only
	singleGain = 0.0;

	enableAudioInput = false;   // uses the sine forcing for the Duffing by default

	smoothing = 1; // for the lowpass. 1 = no lowpass, 5 = quite lowpassed

	// TO KEEP after having moved all this to the init functions at the bottom:
	initMainArrays(bankCount, filterCount);
	
	gains = new double[bankCount];
	
	for (int i=0; i<bankCount; i++) 
	{
		if (i==0) 
		{ 
			gains[i] = 1;
		} else {
			gains[i] = 0;
		}
			
		for (int j=0; j<filterCount; j++) 
		{ 	
			filterFreqsArray[i][j] = (j/2.0)*20 * (i+1)*1.2 + 80;		// INIT arbitrary filter freqs
			y[i][j] = 0;	prevX1[i][j] = 0; prevX2[i][j] = 0; prevY1[i][j] = 0; prevY2[i][j] = 0;
		}
	}

	initTempArrays(bankCount, filterCount);
	for (int j=0; j<filterCount; j++) {	Q[j] = 30; QTemp[j] = 30;	}	

	calcCoeffs();
	
	duffX = 0; duffY = 0; dx = 0; dy = 0;
	gamma = 0.1; omega = 1.25; c = 0.3;
	t = 0;
	dt = 1;
}

GutterSynth::GutterSynth() {
    mCalcFunc = make_calc_function<GutterSynth, &GutterSynth::next>();
    next(1);
}

void GutterSynth::next(int nSamples) {
    const float* input = in(0);
    const float* gain = in(1);
    float* outbuf = out(0);

    // simple gain function
    for (int i = 0; i < nSamples; ++i) {
        outbuf[i] = input[i] * gain[i];
    }
}

} // namespace GutterSynth

PluginLoad(GutterSynthUGens) {
    // Plugin magic
    ft = inTable;
    registerUnit<GutterSynth::GutterSynth>(ft, "GutterSynth", false);
}
