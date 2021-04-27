GutterSynth : UGen {
	// dt: (0..10)
	// c: (0.0001..1)
	// singlegain: (0..1)
	// smoothing: (0..)
	// togglefilters: [0,1]
	
	*ar { |gamma=0.1, omega=0.02, c=0.1, dt=5, singlegain=1.0, smoothing=0.5, 
			togglefilters=0, distortionmethod=0, oversampling=1, enableaudioinput=0, audioinput, gains1=1, gains2=0,
			freqs1, qs1, freqs2, qs2
			|

		^this.multiNewList(['audio', gamma, omega, c, dt, singlegain, smoothing, togglefilters, distortionmethod, oversampling, enableaudioinput, audioinput, gains1, gains2]
			++ ([freqs1.value, qs1.value].flop ++ [freqs2.value, qs2.value].flop).flatten
		);
	}

		// init { arg ... theInputs;
		// 		var numChannelsOut = 2;
		// 		inputs = theInputs;

		// 		^this.initOutputs(numChannelsOut, rate)
		// 	}
		
	checkInputs {
		/* TODO */
		^this.checkValidInputs;
	}
}
