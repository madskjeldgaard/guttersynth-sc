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
	AtanApprox,
	TanhApprox,
	Sigmoid
};
	
template <size_t BankCount, size_t MaxFilterCount>
struct State {
	static constexpr int bankCount {BankCount};
    
	int filterCount {0};

	std::array<std::array<double, MaxFilterCount>, BankCount> a0, a1, a2, b1, b2;
	
	std::array<std::array<double, MaxFilterCount>, BankCount> filterFreqsArray;

	std::array<double, MaxFilterCount> Q;

	std::array<std::array<double, MaxFilterCount>, BankCount> K, norm, prevX1, prevX2, prevY1, prevY2, y;

	std::array<double, BankCount> gains;
	double Fs, singleGain;
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

class GutterSynth : public SCUnit {
public:
    GutterSynth();

    void UpdateFilters();

    GutterState s;

    // Destructor
    // ~GutterSynth();

private:
    // Calc function
    void next(int nSamples);
	// Gains1 and Gains 2 should be tested using audio rate Line objects
	enum class Inputs {Gamma, Omega, C, Dt, SingleGain, Smoothing, ToggleFilters, DistortionMethod, EnableAudioInput, AudioInput, Gains1, Gains2, FilterParams};	
	enum class Outputs {OutL, OutR};

	float m_gamma_past;
	float m_dt_past;
	float m_singlegain_past;
	float m_smoothing_past;
	float m_c_past;
	float m_omega_past;

	dcblocker::Dcblocker dcfilter1, dcfilter2;
};

} // namespace GutterSynth
