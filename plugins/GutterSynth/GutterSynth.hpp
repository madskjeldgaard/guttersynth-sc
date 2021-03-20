// PluginGutterSynth.hpp
// Mads Kjeldgaard & Scott Carver (mail@madskjeldgaard.dk)

#pragma once

#include "SC_PlugIn.hpp"

namespace GutterSynth {

class GutterSynth : public SCUnit {
public:
    GutterSynth();

    // Destructor
    // ~GutterSynth();

private:
    // Calc function
    void next(int nSamples);

    // Member variables
};

} // namespace GutterSynth
