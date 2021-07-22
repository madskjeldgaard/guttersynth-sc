# GutterSynth

Gutter synthesis, a physical ish synth using coupled duffing oscillators resonating through a modal synthesis type of physical modelling system. 

Gutter Synth was originally written in [Max/Java and designed by Tom Mudd](https://github.com/tommmmudd/guttersynthesis). 

It was ported to C++ and SuperCollider by Scott Carver and Mads Kjeldgaard and a number of optimizations and changes were made in the process. See the help file for more information.

### Installing

Instructions for most systems:

1. Download one of the prebuilt releases. Choose the one that fits your operating system.
2. Unzip the contents in your extensions directory (If you don't know where this is, open up SuperCollider and evaluate `Platform.userExtensionDir` - the path will be posted in the post window).
3. Recompile class library.

For users of Linux distributions based on Arch Linux, it is possible to install using an AUR helper:

```bash 
paru -S supercollider-guttersynth-git
```

### Compiling

#### Requirements

- CMake >= 3.5
- SuperCollider source code

#### Building

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
