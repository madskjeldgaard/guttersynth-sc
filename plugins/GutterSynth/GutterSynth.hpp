// PluginGutterSynth.hpp
// Mads Kjeldgaard & Scott Carver (mail@madskjeldgaard.dk)

#pragma once

#include "SC_PlugIn.hpp"
#include <array>

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
	static constexpr int bankCount {BankCount};
    static constexpr int filterCount {FilterCount};

	std::array<std::array<double, FilterCount>, BankCount> a0, a1, a2, b1, b2;
	
	std::array<std::array<double, FilterCount>, BankCount> filterFreqsArray, filterFreqsArrayTemp;

	std::array<double, FilterCount> Q, QTemp;

	std::array<std::array<double, FilterCount>, BankCount> V, K, norm, prevX1, prevX2, prevY1, prevY2, y;

	std::array<double, BankCount> gains;
	double Fs, singleGain;
	double audioInput;
	bool enableAudioInput;

	double smoothing; // for the lowpass. 1 = no lowpass, 5 = quite lowpassed

	double duffX, duffY, dx, dy;
	double gamma, omega, c, t, dt;

	//public float[] x; @TODO unused?
	double finalY;

	DistortionType distortionType {DistortionType::Tanh};

	bool filtersOn;
};

using GutterState = State<DefaultBankCount, DefaultFilterCount>;

void InitGutterState(GutterState&);

class GutterSynth : public SCUnit {
public:
    GutterSynth();

    GutterState state;

    // Destructor
    // ~GutterSynth();

private:
    // Calc function
    void next(int nSamples);

    // Member variables
};

} // namespace GutterSynth
