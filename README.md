# GutterSynth

Gutter synthesis, a physical ish synth using coupled duffing oscillators resonating through a modal synthesis type of physical modelling system. 

Gutter Synth was originally written in [Max/Java and designed by Tom Mudd](https://github.com/tommmmudd/guttersynthesis). 

It was ported to C++ and SuperCollider by Scott Carver and Mads Kjeldgaard and was in the process cleaned up, made safer to use and optimizations were made to increase performance of the synth. See the help file for more information.

### Requirements

- CMake >= 3.5
- SuperCollider source code

### Building

Clone the project:

    git clone https://github.com/madskjeldgaard/guttersynth-sc
    cd guttersynth-sc
    mkdir build
    cd build

Then, use CMake to configure, build and install to your extensions directory:

    cmake .. -DCMAKE_BUILD_TYPE=Release -DSC_PATH=../supercollider -DCMAKE_INSTALL_PREFIX=/path/to/extensions
    cmake --build . --config Release
    cmake --build . --config Release --target install


It's expected that the SuperCollider repo is cloned at `../supercollider` relative to this repo. If
it's not: change the option to wherever the source code is: `-DSC_PATH=/path/to/sc/source`.

### Acknowledgements

Thank you:
- Tom Mudd for the original design in Java. 
- Jatin Chowdhury for the oversampling code.
