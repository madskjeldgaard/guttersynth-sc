// PluginGutterSynth.cpp
// Mads Kjeldgaard & Scott Carver (mail@madskjeldgaard.dk)

#include "SC_PlugIn.hpp"
#include "GutterSynth.hpp"

#include <array>

static InterfaceTable* ft;

namespace GutterSynth {

constexpr auto DefaultBankCount = 2;
constexpr auto DefaultFilterCount = 24;

enum class DistortionType {
	Clipping,
	CubicClipping,
	Tanh,
	Atan,
	TanhApprox,
	Sigmoid
};
	
template <size_t BankCount, size_t FilterCount>
struct State {
	std::array<std::array<double, FilterCount>, BankCount> a0, a1, a2, b1, b2;
	
	std::array<std::array<double, FilterCount>, BankCount> filterFreqsArray, filterFreqsArrayTemp;

	std::array<double, FilterCount> Q, QTemp;

	std::array<std::array<double, FilterCount>, BankCount> V, K, norm, prevX1, prevX2, prevY1, prevY2, y;

	std::array<double, BankCount> gains;
	double Fs, singleGain;
	double audioInput;
	bool enableAudioInput;
	int bankCount {BankCount}; // @TODO redundant?

	double smoothing; // for the lowpass. 1 = no lowpass, 5 = quite lowpassed

	double duffX, duffY, dx, dy;
	double gamma, omega, c, t, dt;

	//public float[] x; @TODO unused?
	double finalY;

	DistortionType distortionType {DistortionType::Tanh};

	int filterCount;

	bool filtersOn;
};

using GutterState = State<DefaultBankCount, DefaultFilterCount>;

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
