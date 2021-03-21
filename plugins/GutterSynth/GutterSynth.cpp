// PluginGutterSynth.cpp
// Mads Kjeldgaard & Scott Carver (mail@madskjeldgaard.dk)

#include "SC_PlugIn.hpp"
#include "GutterSynth.hpp"

static InterfaceTable* ft;

namespace GutterSynth {
	
struct State {
	public double[][] a0, a1, a2, b1, b2;
	
	public double[][] filterFreqsArray, filterFreqsArrayTemp;
	//public double[] filterFreqsTempArray = {68, 97, 170, 248, 391, 449, 531, 589, 658, 711, 879, 771, 807, 1053, 1200, 1255, 1460, 1478, 1521, 1685, 1666, 1784, 1921, 1954, 68, 97, 170, 248, 391, 449, 531, 589, 658, 711, 879, 771, 807, 1053, 1200, 1255, 1460, 1478, 1521, 1685, 1666, 1784, 1921, 1954};
	public double[] Q, QTemp;
	//public double[] QTemp = {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30};
	public double[][] V, K, norm, prevX1, prevX2, prevY1, prevY2, y;
	public double[] gains;
	public double M_PI, Fs, singleGain;
	public double audioInput;
	public boolean enableAudioInput;
	public int bankCount = 2;

	public double smoothing; // for the lowpass. 1 = no lowpass, 5 = quite lowpassed
	
	public double duffX, duffY, dx, dy;
	public double gamma, omega, c, t, dt;
	
	public float[] x;
	public double finalY;

	public int distMethod = 2;
	
	public int filterCount;

	public boolean filtersOn;
};

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
