// PluginGutterSynth.cpp
// Mads Kjeldgaard & Scott Carver (mail@madskjeldgaard.dk)

#include "SC_PlugIn.hpp"
#include "SC_Constants.h"
#include "GutterSynth.hpp"

#include <cmath>

static InterfaceTable* ft;

namespace GutterSynth {

void CalcCoeffs(GutterState& s) {						
	for (auto bank = 0; bank < s.bankCount; bank++) {
		for (int filter = 0; filter < s.filterCount; filter++) {
			s.V[bank][filter] = sc_pow(10.0, 1.0 / 20.0);
			s.K[bank][filter] = std::tan(pi * s.filterFreqsArray[bank][filter] / s.Fs);

			s.norm[bank][filter] = 1.0 / (1.0 + s.K[bank][filter] / s.Q[filter] + s.K[bank][filter] * s.K[bank][filter]);

			s.a0[bank][filter] = s.K[bank][filter] / s.Q[bank] * s.norm[bank][filter];
			s.a1[bank][filter] = 0.0;
			s.a2[bank][filter] = -s.a0[bank][filter];

			s.b1[bank][filter] = 2.0 * (s.K[bank][filter] * s.K[bank][filter] - 1) * s.norm[bank][filter];
			s.b2[bank][filter] = (1.0 - s.K[bank][filter] / s.Q[bank] + s.K[bank][filter] * s.K[bank][filter]) * s.norm[bank][filter];  	
		}
	}
}

void InitTempArrays(GutterState& s) {
	s.QTemp = s.Q;

	for (auto bank = 0; bank < s.bankCount; bank++) {
		s.filterFreqsArrayTemp[bank] = s.filterFreqsArray[bank];
	}
}

void InitGutterState(GutterState& s) {
	s.Fs = 44100.0;		
	s.filtersOn = true;	// turn off for Duffing only
	s.singleGain = 0.0;

	s.enableAudioInput = false;   // uses the sine forcing for the Duffing by default

	s.smoothing = 1.0; // for the lowpass. 1 = no lowpass, 5 = quite lowpassed
	
	for (auto bank = 0; bank < s.bankCount; bank++) 
	{
		s.gains[bank] = (bank==0) ? 1.0 : 0.0;
			
		for (int filter = 0; filter < s.filterCount; filter++) 
		{ 	
			s.filterFreqsArray[bank][filter] = (filter / 2.0) * 20.0 * (bank + 1) * 1.2 + 80.0;		// INIT arbitrary filter freqs
		}

		s.y[bank].fill(0.0);	
		s.prevX1[bank].fill(0.0); 
		s.prevX2[bank].fill(0.0); 
		s.prevY1[bank].fill(0.0); 
		s.prevY2[bank].fill(0.0);
	}

	InitTempArrays(s);
	s.Q.fill(30.0);
	s.QTemp.fill(30.0);

	CalcCoeffs(s);
	
	s.duffX = 0.0; 
	s.duffY = 0.0; 
	s.dx = 0.0; 
	s.dy = 0.0;
	s.gamma = 0.1; 
	s.omega = 1.25; 
	s.c = 0.3;
	s.t = 0.0;
	s.dt = 1.0;
}


GutterSynth::GutterSynth() {
    mCalcFunc = make_calc_function<GutterSynth, &GutterSynth::next>();

	InitGutterState(state);

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
