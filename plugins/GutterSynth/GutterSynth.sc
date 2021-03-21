GutterSynth : MultiOutUGen {
	// dt: (0..10)
	// c: (0.0001..1)
	// singlegain: (0..1)
	// smoothing: (0..)
	// togglefilters: [0,1]
	
	*ar { |gamma=0.1, omega=0.02, c=0.1, dt=5, singlegain=1.0, smoothing=0.5, togglefilters=0, distortionmethod=0, enableaudioinput=0, audioinput, gains1=1, gains2=0|
		^this.multiNew('audio', gamma, omega, c, dt, singlegain, smoothing, togglefilters, distortionmethod, enableaudioinput, audioinput, gains1, gains2);
	}

		init { arg ... theInputs;
				var numChannelsOut = 2;
				inputs = theInputs;

				^this.initOutputs(numChannelsOut, rate)
			}
		
	checkInputs {
		/* TODO */
		^this.checkValidInputs;
	}
}
