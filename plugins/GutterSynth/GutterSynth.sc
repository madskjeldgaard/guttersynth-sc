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

      // Index of first array argument. The rate of the array style arguments will be checked seperately
      var firstArrayArgument = 11;

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
        \gains1 -> [\scalar, \control],          
        \gains2 -> [\scalar, \control],          

        \freqs1 -> [\scalar],          
        \qs1 -> [\scalar],          
        \freqs2 -> [\scalar],          
        \qs2 -> [\scalar]

      ];

      // Iterate over all singular inputs (not the arrayed inputs for the bank settings) and check if they comply
      inputs.do{|input, inputNum| 
        if(inputNum < firstArrayArgument, {
          var name = this.argNameForInputAt(inputNum);
          var inrate = input.rate;
          var expected = allowedRates[name];

          var rateIsExpected = expected.indexOfEqual(inrate).notNil;

          // "Num args: %".format(allowedRates.size).postln;
          input.class.postln; name.postln; inrate.postln; expected.postln; inputNum.postln;

          if(rateIsExpected.not, {
            ^"%'s input % not % (it is %)".format(this.name, name, expected, inrate)
          })
        })

      };

      // Check the rate of the arrays that are input for the bank settings
      inputs[firstArrayArgument..].do{|array|
        array.do{|argument|
          if(argument.rate != \scalar, { ^"All array arguments for %'s bank arguments have to be scalar".format(this.name) })
        }
      };

      ^this.checkValidInputs;
    }
}
