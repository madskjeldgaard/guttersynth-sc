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

void ResetDuff(GutterState& s) {
    s.duffX = 0.0;
    s.duffY = 0.0;
    s.dx = 0.0;
    s.dy = 0.0;
    s.t = 0.0;
}

void InitGutterState(GutterState& s, double sampleRate) {
	s.Fs = sampleRate;		
	s.filtersOn = true;	// turn off for Duffing only
	s.singleGain = 0.0;

	s.enableAudioInput = false;   // uses the sine forcing for the Duffing by default

	s.smoothing = 1.0; // for the lowpass. 1 = no lowpass, 5 = quite lowpassed
	
	for (auto bank = 0; bank < s.bankCount; bank++) 
	{
		s.gains[bank] = (bank==0) ? 1.0 : 0.0;
			
		s.y[bank].fill(0.0);	
		s.prevX1[bank].fill(0.0); 
		s.prevX2[bank].fill(0.0); 
		s.prevY1[bank].fill(0.0); 
		s.prevY2[bank].fill(0.0);
	}

    ResetDuff(s);

	s.gamma = 0.1;
	s.omega = 1.25; 
	s.c = 0.3;
	s.dt = 1.0;
}

inline double Lowpass(double newVal, double oldVal, double smooth) {
	return (newVal - oldVal) / smooth;
}

inline double Distortion(double value, DistortionType type, double finalY) {
  // value is the temporary final output
  switch (type) {
  case DistortionType::Clipping:
    return sc_max(sc_min(value, 1.0), -1.0);
  case DistortionType::VarClip:
    // the shape argument of this could from 1 to 100
    return varClip(value, sc_min(1.0, 100.0));
  case DistortionType::Tanh: // DIST #2: 	tanh
    return fastatan(value);
  case DistortionType::
      AtanApprox: // DIST #3: 	atan approximation? From
                  // http://www.kvraudio.com/forum/viewtopic.php?p=4402120
    return 0.75 *
           (sc_sqrt(((value * 1.3) * (value * 1.3) + 1.0)) * 1.65 - 1.65) /
           value;
  case DistortionType::
      TanhApprox: // DIST #4: 	tanh approximation? From
                  // http://www.kvraudio.com/forum/viewtopic.php?p=4402120
    return (0.1076 * value * value * value + 3.029 * value) /
           (value * value + 3.124);
  default:
    return value;
        }
}

void SetFilter(GutterState& s, int bank, int filter, double freq, double q)
{
	// printf("bank: %d    filter: %d    freq: %f      q: %f   \n", bank, filter, freq, q);
	s.filterFreqsArray[bank][filter] = freq;
	s.Q[filter] = q;
}

void GutterSynth::UpdateFilters()
{
    constexpr auto filterParamCount = 2; // freq, q
    s.filterCount = (mNumInputs - static_cast<int>(Inputs::FilterParams)) / s.bankCount / filterParamCount;

    for (auto filter = 0; filter < s.filterCount; ++filter) {
        for (auto bank = 0; bank < s.bankCount; ++bank) {
            const auto inputOffset = filterParamCount * ((bank * s.filterCount) + filter);

            const auto freq = in((int)Inputs::FilterParams + inputOffset + 0)[0];
            const auto q = in((int)Inputs::FilterParams + inputOffset + 1)[0];

            SetFilter(s, bank, filter, freq, q);
        }
    }
    CalcCoeffs(s);
}

GutterSynth::GutterSynth() {
  dcfilter1.init();
  dcfilter2.init();

  // Init oversampling
  oversample.reset(sampleRate());
  const auto overSampleInput =
      static_cast<OverSamplingAmounts>(in0((int)Inputs::OverSample));

  int overSampleIndex = 0;

  switch (overSampleInput) {
  case OverSamplingAmounts::None:
    overSampleIndex = 0;
    break;
  case OverSamplingAmounts::Double:
    overSampleIndex = 1;
    break;
  case OverSamplingAmounts::Four:
    overSampleIndex = 2;
    break;
  case OverSamplingAmounts::Eight:
    overSampleIndex = 3;
    break;
  case OverSamplingAmounts::Sixteen:
    overSampleIndex = 4;
    break;
  default:
    overSampleIndex = 0;
    break;
  };

  oversample.setOversamplingIndex(overSampleIndex);
  mCalcFunc = make_calc_function<GutterSynth, &GutterSynth::next>();

  InitGutterState(s, sampleRate());

  // Initialize state variables for the parameter smoothing functions
  m_gamma_past = s.gamma;
  m_dt_past = s.dt;
  m_c_past = s.c;
  m_omega_past = s.omega;
  m_singlegain_past = s.singleGain;
  m_smoothing_past = s.smoothing;

  UpdateFilters();

  // @TODO We need to reset our state after next(1) is called?
  next(1);
}

void GutterSynth::next(int nSamples) {

  float *out1 = out((int)Outputs::OutL);
  float *out2 = out((int)Outputs::OutR);

  SlopeSignal<float> slopedGamma =
      makeSlope(in0((int)Inputs::Gamma), m_gamma_past);
  SlopeSignal<float> slopedOmega =
      makeSlope(in0((int)Inputs::Omega), m_omega_past);
  SlopeSignal<float> slopedC = makeSlope(in0((int)Inputs::C), m_c_past);
  SlopeSignal<float> slopedDt = makeSlope(in0((int)Inputs::Dt), m_dt_past);
  SlopeSignal<float> slopedSingleGain =
      makeSlope(in0((int)Inputs::SingleGain), m_singlegain_past);
  SlopeSignal<float> slopedSmoothing =
      makeSlope(in0((int)Inputs::Smoothing), m_smoothing_past);

  s.filtersOn = 0 < in((int)Inputs::ToggleFilters)[0];
  s.distortionType =
      static_cast<DistortionType>(in((int)Inputs::DistortionMethod)[0]);
  s.enableAudioInput = 0 < in((int)Inputs::EnableAudioInput)[0];

  s.gains[0] = in((int)Inputs::Gains1)[0];
  s.gains[1] = in((int)Inputs::Gains2)[0];

  float const *audioInput = in((int)Inputs::AudioInput);

  UpdateFilters();

  /*-------------------------------------------------------------------*
   * Go through the required number of samples
   *-------------------------------------------------------------------*/
  for (int i = 0; i < nSamples; i++) {
    // Input sample
    float inputSample = audioInput[i];

    // Smoothed parameters
    const float gamma = slopedGamma.consume();
    s.gamma = gamma;

    const float omega = slopedOmega.consume();
    s.omega = omega;

    const float c = slopedC.consume();
    s.c = c;

    const float dt = slopedDt.consume();
    s.dt = dt;

    const float singlegain = slopedSingleGain.consume();
    s.singleGain = sc_clip(singlegain, 0.0f, 5.0f);

    const float smoothing = slopedSmoothing.consume();
    s.smoothing = 1.0 + smoothing; // 0 = no smoothing

    // band pass filters
    s.finalY = 0;
    if (s.filtersOn) {
      for (int bank = 0; bank < s.bankCount; bank++) {
        for (int filter = 0; filter < s.filterCount; filter++) {
          // @TODO Potential for performance increase: Swap out biquad filter
          // for SVF
          s.y[bank][filter] = s.a0[bank][filter] * s.duffX +
                              s.a1[bank][filter] * s.prevX1[bank][filter] +
                              s.a2[bank][filter] * s.prevX2[bank][filter] -
                              s.b1[bank][filter] * s.prevY1[bank][filter] -
                              s.b2[bank][filter] * s.prevY2[bank][filter];
          s.prevX2[bank][filter] = zapgremlins(s.prevX1[bank][filter]);
          s.prevX1[bank][filter] = zapgremlins(s.duffX);
          s.prevY2[bank][filter] = zapgremlins(s.prevY1[bank][filter]);
          s.prevY1[bank][filter] = zapgremlins(s.y[bank][filter]);
          s.finalY += zapgremlins(
              s.y[bank][filter] * s.gains[bank] *
              s.singleGain); // retain singleGain for overall control
        }
      }
    } else { // if filters are disabled then pass directly
      s.finalY = s.duffX;
    }

    // DUFFING with audioInput or with OSC?
    if (s.enableAudioInput) {
      s.dy = s.finalY - (s.finalY * s.finalY * s.finalY) - (s.c * s.duffY) +
             s.gamma * inputSample;
    } else {
      s.dy = s.finalY - (s.finalY * s.finalY * s.finalY) - (s.c * s.duffY) +
             s.gamma * std::sin(s.omega * s.t);
    }

    s.duffY += s.dy;
    s.dx = s.duffY;
    s.duffX = Lowpass(s.finalY + s.dx, s.duffX, s.smoothing);

    if (s.filtersOn) { // If the filters are enabled use variable distortion
                       // (function above)

      oversample.upsample(s.duffX);
      float *osBuffer = oversample.getOSBuffer();
      for (int k = 0; k < oversample.getOversamplingRatio(); k++)
        osBuffer[k] = Distortion(osBuffer[k], s.distortionType, s.finalY);
      s.duffX = oversample.downsample();

      /* s.duffX = Distortion(s.duffX, s.distortionType, s.finalY); */
      out1[i] = (float)(s.finalY * 0.125); // output the value from the filter,
                                           // not from the distortion
    } else { // If the filters are OFF use reset() function to reignite process
             // (snazzy clicks?)
      s.duffX = sc_clip(s.duffX, -100.0, 100.0);
      if (sc_abs(s.duffX) > 99.0) {
        ResetDuff(s);
      }
      out1[i] = static_cast<float>(sc_clip(s.duffX * s.singleGain, -1.0, 1.0));
    }

    out2[i] = static_cast<float>(s.duffX);

    out1[i] = dcfilter1.process(out1[i]);
    out2[i] = dcfilter2.process(out2[i]);

    s.t += s.dt;

    // @TODO Zapgremlins instead?
    if (std::isnan(s.duffX)) {
      ResetDuff(s);
    }
  }

  m_gamma_past = slopedGamma.value;
  m_dt_past = slopedDt.value;
  m_singlegain_past = slopedSingleGain.value;
  m_smoothing_past = slopedSmoothing.value;
  m_c_past = slopedC.value;
  m_omega_past = slopedOmega.value;
}

} // namespace GutterSynth

PluginLoad(GutterSynthUGens) {
    // Plugin magic
    ft = inTable;
    registerUnit<GutterSynth::GutterSynth>(ft, "GutterSynth", false);
}
