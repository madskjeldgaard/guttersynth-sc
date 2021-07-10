GutterSynth : UGen {
	// dt: (0..10)
	// c: (0.0001..1)
	// singlegain: (0..1)
	// smoothing: (0..)
	// togglefilters: [0,1]
	
	*ar { |gamma=0.1, omega=0.02, c=0.1, dt=5, singlegain=1.0, smoothing=0.5, 
			togglefilters=0, distortionmethod=0, oversampling=1, enableaudioinput=0, audioinput, gains1=1, gains2=0,
			freqs1, qs1, freqs2, qs2|

		^this.multiNewList(['audio', gamma, omega, c, dt, singlegain, smoothing, togglefilters, distortionmethod, oversampling, enableaudioinput, audioinput, gains1, gains2]
			++ ([freqs1.value, qs1.value].flop ++ [freqs2.value, qs2.value].flop).flatten
		);
	}

	checkInputs {

		// This dictionary maps what rates are allowed for each parameter of the UGen
        var allowedRates = IdentityDictionary[
          \audioinput -> [\scalar, \control, \audio],          

          \gamma -> [\scalar, \control], 
          \omega -> [\scalar, \control], 
          \c -> [\scalar, \control], 
          \dt -> [\scalar, \control], 
          \singlegain -> [\scalar, \control], 
          \smoothing -> [\scalar, \control], 
          \togglefilters -> [\scalar], 
          \oversampling -> [\scalar], 

          \distortionmethod -> [\scalar, \control], 

          \enableaudioinput -> [\scalar],

          // Banks are scalar only for now
          \gains1 -> [\scalar],          
          \freqs1 -> [\scalar],          
          \qs1 -> [\scalar],          

          \gains2 -> [\scalar],          
          \freqs2 -> [\scalar],          
          \qs2 -> [\scalar]

        ];
        

		// Iterate over all inputs and check if they comply
		inputs.do{|input, inputNum| 
			var name = this.argNameForInputAt(inputNum);
			var inrate = input.rate;
			var expected = allowedRates[name];
            
            // @FIXME: Why does this become nil?
            if(inrate.isNil.not and: { name.isNil.not }, { 
              var rateIsExpected = expected.indexOfEqual(inrate).notNil;

              // "Num args: %".format(allowedRates.size).postln;
              // input.class.postln; name.postln; inrate.postln; expected.postln; inputNum.postln;

              if(rateIsExpected.not, {
                ^"%'s input % not % (it is %)".format(this.name, name, expected, inrate).error
              })

            })
			
		};

		^this.checkValidInputs;
	}
}
