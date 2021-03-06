s??         ?k   '  zc  	          ??                              DADSS       DA DSS CVI Driver                             ?    
 Instrument Name:  Sample Oscilloscope

 Description:      Provides the following func-
                   tions for programming a sample
                   oscilloscope:

                   (1) Initialize
                       Initialize the oscilloscope
                       and set default to a de-
                       fault configuration.

                   (2) Configure
                       Configures the horizontal
                       and vertical settings of
                       the oscilloscope.

                   (3) Read Waveform
                       Read a waveform from the
                       oscilloscope.

                   (4) Close
                       Take the oscilloscope
                       offline.    ?    
 Panel Name:    Configure of MDAC1 Range

 Description:   It writes the setting of MDAC1 range to the appropriate 
                network variable of DA_DSS.
                To physical changes in the MDAC1 range it is required 
                to stop and start the generation process. 


        Vout(i) = MDAC2Range*(MDAC2/(2^18 - 1) * (MDAC1Range*Vref * V(i) +DCoffset)
        where
           V(i) = (MDAC1/(2^17 - 1)) * sin(2PI*i/NUM_SAMPLES + Phase)        
 Panel Name:    Get MDAC1 Range

 Description:   It reads the setting of MDAC1 range.
               


        Vout(i) = MDAC2Range*(MDAC2/(2^18 - 1) * (MDAC1Range*Vref * V(i) +DCoffset)
        where
           V(i) = (MDAC1/(2^17 - 1)) * sin(2PI*i/NUM_SAMPLES + Phase)    ?    
 Panel Name:    Configure of MDAC2 Range

 Description:   It writes the setting of MDAC2 range to the appropriate 
                network variable of DA_DSS.
                To physical changes in the MDAC2 range it is required 
                to stop and start the generation process.



        Vout(i) = MDAC2Range*(MDAC2/(2^18 - 1) * (MDAC1Range*Vref * V(i) +DCoffset)
        where
           V(i) = (MDAC1/(2^17 - 1)) * sin(2PI*i/NUM_SAMPLES + Phase)        
 Panel Name:    Get MDAC2 Range

 Description:   It reads the setting of MDAC2 range.
               


        Vout(i) = MDAC2Range*(MDAC2/(2^18 - 1) * (MDAC1Range*Vref * V(i) +DCoffset)
        where
           V(i) = (MDAC1/(2^17 - 1)) * sin(2PI*i/NUM_SAMPLES + Phase)    (    
 Panel Name:    Configure of CLK Source

 Description:   It writes the setting of CLK source to the appropriate 
                network variable of DA_DSS.
                To physical changes in the CLK source it is required 
                to stop and start the generation process.



          o    
 Panel Name:    Get CLK Source

 Description:   It reads the setting of CLK source.
               


              
 Panel Name:    Configure of output 10 MHz reference clock

 Description:   It writes the setting to the appropriate 
                network variable of DA_DSS.
                To physical changes it is required 
                to stop and start the generation process.



          o    
 Panel Name:    Get CLK Source

 Description:   It reads the setting of CLK source.
               


           ?    
 Panel Name:    Update Configuration

 Description:   Updates the configuration parameters of DA_DSS.
                To changes in the configuration parameters is required 
                to stop the generation process.     ?    
 Panel Name:    Set Amplitude

 Description:   It writes the value of  amplitude setting 
                to the appropriate network variable of DA_DSS.     ]    
 Panel Name:    Get Amplitude

 Description:   It reads the amplitude setting from DA_DSS.     ?    
 Panel Name:    Set Phase

 Description:   It writes the value of  phase setting 
                to the appropriate network variable of DA_DSS.     U    
 Panel Name:    Get Phase

 Description:   It reads the phase setting from DA_DSS.     ?    
 Panel Name:    Update Waveform

 Description:   Updates the amplitude and phase settings during generation proccess.
                This function writes the waveform samples to NCO memory withot stop
                the generation proccess.    '    
 Panel Name:    Set Frequency

 Description:   Writes the value of  frequency setting to the appropriate 
                network variable of DA_DSS.
                To physical changes in the frequency of output signals is required 
                to stop and start the generation process.      ]    
 Panel Name:    Get Frequency

 Description:   It reads the frequency setting from DA_DSS.     f    
 Panel Name:    Get Frequency

 Description:   It reads the real frequency value of output signals.     ?    
 Panel Name:    Set MDAC1 Amplitude

 Description:   It writes the value of  amplitude setting 
                to the appropriate network variable of DA_DSS.     ]    
 Panel Name:    Get Amplitude

 Description:   It reads the amplitude setting from DA_DSS.     ?    
 Panel Name:    Set MDAC1 Offset 

 Description:   It writes the offset value associated with MDAC1 converter
                to the appropriate network variable of DA_DSS.     u    
 Panel Name:    Get MDAC1 Offset  

 Description:   It reads the MDAC1 offset setting from DA_DSS.
                    ?    
 Panel Name:    Set MDAC2 

 Description:   It writes the setting of MDAC2 converter
                to the appropriate network variable of DA_DSS.     ?    
 Panel Name:    Set MDAC2 

 Description:   It writes the setting of MDAC2 converter
                to the appropriate network variable of DA_DSS.     ?    
 Panel Name:    Get Amplitude Max

 Description:   It reads the max amplitude setting based on the MDAC1 and MDAC2 
                range settings.     ?    
 Panel Name:    Get Maximum Amplitude

 Description:   It returns the max possible amplitude taking into accunt 
                ranges of MDAC1 and MDAC2, and setting of MDAC2.     O    
 Panel Name:    Update MDAC2

 Description:   It updates the MDAC2 settings.    J    
 Panel Name:    Set CLK Frequency

 Description:   Writes the value of  NCO reference clock 
                frequency  to the appropriate  network variable of DA_DSS.
                
                To physical changes in the frequency of output signals is required 
                to stop and start the generation process.      a    
 Panel Name:    Get CLK Frequency

 Description:   It reads the frequency setting from DA_DSS.     [    
 Panel Name:    Set Number Samples 

 Description:   It specifies the number of samples.     v    
 Panel Name:    Get Number of Samples

 Description:   It reads the defined number of wavefrom samples from DA_DSS.     p    
 Panel Name:    Start/Stop Generation

 Description:   Starts or stops the output signal generation proccess.     s    
 Panel Name:    Status Start/Stop Generation

 Description:   Gets the status of start/stop generation proccess.  ???? h ?    ?    NV Server                          "localhost" ???? h ?    ?    NV Process                         "REGUZG_DADSS"   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       Channel

 Description:       Specifies the number of channel of DA_DSS.

 Variable Type:     Double

 Valid Range:       Chan1 to Chan7     ?    Control Name:       Range

 Description:       Specifies the range of MDAC1.

 Variable Type:     int

 Valid Value:       2 : range x0.5  (+/- 0.5)
                    3 : range x1    (+/- 1)
                    4 : range x2    (+/- 2)      ? ????    `    Error                             ? p L     ?    Channel                            p   ?    Range                              	                       TChannel 1 1 Channel 2 2 Channel 3 3 Channel 4 4 Channel 5 5 Channel 6 6 Channel 7 7              x0.5 2 x1 3 x2 4   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       Channel

 Description:       Specifies the number of channel of DA_DSS.

 Variable Type:     Double

 Valid Range:       Chan1 to Chan7        Control Name:       Range

 Description:       Takes the value of the MDAC1 range setting.

 Variable Type:     int (Passed by reference)

 Valid Value:       2 : range x0.5  (+/- 0.5)
                    3 : range x1    (+/- 1)
                    4 : range x2    (+/- 2)    ? ????    `    Error                              ? p L     ?    Channel                           !\ p   ?    Range                              	                       TChannel 1 1 Channel 2 2 Channel 3 3 Channel 4 4 Channel 5 5 Channel 6 6 Channel 7 7    	           
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       Channel

 Description:       Specifies the number of channel of DA_DSS.

 Variable Type:     Double

 Valid Range:       Chan1 to Chan7     ?    Control Name:       Range

 Description:       Specifies the range of MDAC2.

 Variable Type:     int

 Valid Value:       0 : range x1  (+1)
                    1 : range x2  (+2)    #? ????    `    Error                             $? p L     ?    Channel                           %G p   ?    Range                              	                       TChannel 1 1 Channel 2 2 Channel 3 3 Channel 4 4 Channel 5 5 Channel 6 6 Channel 7 7               
x1 0 x2 1   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       Channel

 Description:       Specifies the number of channel of DA_DSS.

 Variable Type:     Double

 Valid Range:       Chan1 to Chan7     ?    Control Name:       Range

 Description:       Takes the value of the MDAC2 range setting.

 Variable Type:     int (Passed by reference)

 Valid Value:       
                    0 : range x1  (+1)
                    1 : range x2  (+2)    ', ????    `    Error                             (> p L     ?    Channel                           (? p   ?    Range                              	                       TChannel 1 1 Channel 2 2 Channel 3 3 Channel 4 4 Channel 5 5 Channel 6 6 Channel 7 7    	           
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.         Control Name:       CLK Source

 Description:       Specifies the CLK source.

 Variable Type:     unsigned int

 Valid Value:       0 : Internal (6541)
                    1 : Internal (6653)
                    2 : External (6541)
                    3:  External 10MHz (6653)    *? ????    `    Error                             , p ?    ?    CLK Source                         	                       OInternal (6541) 0 Internal (6653) 1 External (6541) 2 External  10MHz (6653) 3   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       Status of 10 MHz reference clock signal.

 Description:       Takes the status of 10 MHz reference signal.

 Variable Type:     unsigned int (Passed by reference)

 Valid Value:       0 : off
                    1 : on    -? ????    `    Error                             / p ?    ?    CLK Source                         	            	           
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       10 MHz Reference Clock 

 Description:       Set on the PXI6653 CLK OUT the 10 MHz.

 Variable Type:     unsigned int

 Valid Value:       0 : off
                    1 : on
                    0? ????    `    Error                             1? p ?    ?    10 MHz Reference Clock             	                       Off 0 On 1   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.     ;    Control Name:       CLK Source

 Description:       Takes the configuration of CLK source.

 Variable Type:     unsigned int (Passed by reference)

 Valid Value:       0 : Internal (6541)
                    1 : Internal (6653)
                    2 : External (6541)
                    3:  External 10MHz (6653)    3 ????    `    Error                             4 p ?    ?    CLK Source                         	            	           
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.     5? ????    `    Error                              	           
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       Amplitude

 Description:       Specifies the amplitude of output signal of 
                    DA_DSS selected channel.

 Variable Type:     Double

 Valid Max Range:   0.0 (V) to 10.0 (V)

      ?    Control Name:       Channel

 Description:       Specifies the number of channel of DA_DSS.

 Variable Type:     Int

 Valid Range:       Chan1 to Chan7    7. ????    `    Error                             8@ p ?    ?    Amplitude                         9 p L     ?    Channel                            	            0               TChannel 1 1 Channel 2 2 Channel 3 3 Channel 4 4 Channel 5 5 Channel 6 6 Channel 7 7   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       Amplitude

 Description:       Reads the amplitude setting value of output signal.

 Variable Type:     Double (Passed by reference)

      ?    Control Name:       Channel

 Description:       Specifies the number of channel of DA_DSS.

 Variable Type:     Int

 Valid Range:       Chan1 to Chan7    :? ????    `    Error                             ;? p ?    ?    Amplitude                         <? p L     ?    Channel                            	            	                      TChannel 1 1 Channel 2 2 Channel 3 3 Channel 4 4 Channel 5 5 Channel 6 6 Channel 7 7   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       Phase

 Description:       Specifies the phase of output signal of 
                    DA_DSS selected channel.

 Variable Type:     Double

 Valid Range:       -PI to +PI

      ?    Control Name:       Channel

 Description:       Specifies the number of channel of DA_DSS.

 Variable Type:     Int

 Valid Range:       Chan1 to Chan7    >F ????    `    Error                             ?X p ?    ?    Phase                             @% p L     ?    Channel                            	            0               TChannel 1 1 Channel 2 2 Channel 3 3 Channel 4 4 Channel 5 5 Channel 6 6 Channel 7 7   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       Phase

 Description:       Phase setting value of output signal.

 Variable Type:     Double (Passed by reference)



      ?    Control Name:       Channel

 Description:       Specifies the number of channel of DA_DSS.

 Variable Type:     Int

 Valid Range:       Chan1 to Chan7    A? ????    `    Error                             B? p ?    ?    Phase                             C? p L     ?    Channel                            	            	                      TChannel 1 1 Channel 2 2 Channel 3 3 Channel 4 4 Channel 5 5 Channel 6 6 Channel 7 7   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.     E= ????    `    Error                              	           
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:  Frequency

 Description:   Specifies the frequency of
                DA_DSS output signals.

 Variable Type: Double

 Valid Range:   20 (Hz) to 20000 (Hz)    F? ????    `    Error                             G? p ?     ?    Frequency                          	            1000   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       Frequency

 Description:       Frequency setting value of output signal.

 Variable Type:     Double (Passed by reference)



     H? ????    `    Error                             I? p ?     ?    Frequency                          	            	          
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       Frequency

 Description:       Real frequency value of output signals.

 Variable Type:     Double (Passed by reference)



     K ????    `    Error                             L p ?     ?    Frequency                          	            	          
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       MDAC1 Amplitude

 Description:       Specifies the amplitude of MDAC1 output signal of 
                    DA_DSS selected channel.

 Variable Type:     Double

 Valid Max Range:   0.0 (V) to 6.0 (V)

      ?    Control Name:       Channel

 Description:       Specifies the number of channel of DA_DSS.

 Variable Type:     Int

 Valid Range:       Chan1 to Chan7    M4 ????    `    Error                             NF p ?   ?    MDAC1 Amplitude                   O/ p L     ?    Channel                            	         ??      @                                              TChannel 1 1 Channel 2 2 Channel 3 3 Channel 4 4 Channel 5 5 Channel 6 6 Channel 7 7   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       Phase

 Description:       Reads the phase setting value of output signal.

 Variable Type:     Double (Passed by reference)

      ?    Control Name:       Channel

 Description:       Specifies the number of channel of DA_DSS.

 Variable Type:     Int

 Valid Range:       Chan1 to Chan7    Q
 ????    `    Error                             R p ?    ?    Amplitude                         R? p L     ?    Channel                            	            	                      TChannel 1 1 Channel 2 2 Channel 3 3 Channel 4 4 Channel 5 5 Channel 6 6 Channel 7 7   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.     ?    Control Name:       Setting

 Description:       Specifies the offset value associated with MDAC1 converter 
                    of DA_DSS selected channel. The value is expressed in MDAC code. 
                    The offset resolution can be calculated with following fomula:

                    Offset_resolution =  (Setting/2^17) * MDAC1_Range * Vref
                    Vref = 3 V

 Variable Type:     Int

 Valid Max Range:   from -1000 to 1000

      ?    Control Name:       Channel

 Description:       Specifies the number of channel of DA_DSS.

 Variable Type:     Int

 Valid Range:       Chan1 to Chan7    Tu ????    `    Error                             U? p ?     ?    Setting                           WW p L     ?    Channel                            	            0               TChannel 1 1 Channel 2 2 Channel 3 3 Channel 4 4 Channel 5 5 Channel 6 6 Channel 7 7   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       Setting

 Description:       Reads the MDAC1 offset setting from DA_DSS

 Variable Type:     Int (Passed by reference)

      ?    Control Name:       Channel

 Description:       Specifies the number of channel of DA_DSS.

 Variable Type:     Int

 Valid Range:       Chan1 to Chan7    Y ????    `    Error                             Z p ?     ?    Setting                           Z? p L     ?    Channel                            	            	                       TChannel 1 1 Channel 2 2 Channel 3 3 Channel 4 4 Channel 5 5 Channel 6 6 Channel 7 7   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       Amplitude

 Description:       Specifies the amplitude of output signal of 
                    DA_DSS selected channel.

 Variable Type:     Double

 Valid Max Range:   0.0 (V) to 10.0 (V)

      ?    Control Name:       Channel

 Description:       Specifies the number of channel of DA_DSS.

 Variable Type:     Int

 Valid Range:       Chan1 to Chan7    \q ????    `    Error                             ]? p ?    ?    Setting                           ^a p L     ?    Channel                            	            0               TChannel 1 1 Channel 2 2 Channel 3 3 Channel 4 4 Channel 5 5 Channel 6 6 Channel 7 7   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       Setting

 Description:       Reads the MDAC2 setting from DA_DSS

 Variable Type:     UInt (Passed by reference)

      ?    Control Name:       Channel

 Description:       Specifies the number of channel of DA_DSS.

 Variable Type:     Int

 Valid Range:       Chan1 to Chan7    ` ????    `    Error                             a( p ?    ?    Setting                           a? p L     ?    Channel                            	            	                       TChannel 1 1 Channel 2 2 Channel 3 3 Channel 4 4 Channel 5 5 Channel 6 6 Channel 7 7   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       Amplitude

 Description:       Reads the amplitude setting value of output signal.

 Variable Type:     Double (Passed by reference)

      ?    Control Name:       Channel

 Description:       Specifies the number of channel of DA_DSS.

 Variable Type:     Int

 Valid Range:       Chan1 to Chan7    cu ????    `    Error                             d? p ?    ?    Amplitude Max                     e, p L     ?    Channel                            	            	                      TChannel 1 1 Channel 2 2 Channel 3 3 Channel 4 4 Channel 5 5 Channel 6 6 Channel 7 7   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.     B    Control Name:       Waveform

 Description:       Reads the waveform samples used by DA_DSS to generate the output sinewave.
                    The format data is integer and the smaple values are in the range 
                    from -2^17   to (2^17 - 1).


 Variable Type:     Integer array (Passed by reference)

      ?    Control Name:       Channel

 Description:       Specifies the number of channel of DA_DSS.

 Variable Type:     Int

 Valid Range:       Chan1 to Chan7    f? ????    `    Error                             g? p ?    ?    Waveform                          iD p      ?    Channel                         ???? p?    ?    NumSamples                         	            	                       TChannel 1 1 Channel 2 2 Channel 3 3 Channel 4 4 Channel 5 5 Channel 6 6 Channel 7 7    0   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.     k: ????    `    Error                              	           
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:  Frequency

 Description:   Specifies the frequency of CLK.

 Variable Type: Double

 Valid Range:   1.0 (MHz) to 40.0 (MHz)    l? ????    `    Error                             m? p ?    ?    CLK Frequency                      	         ??      @D      ??      @4                  
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       CLK Frequency

 Description:       Frequency setting value of output signal.

 Variable Type:     Double (Passed by reference)



     n? ????    `    Error                             o? p ?     ?    CLK Frequency                      	            	          
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       Samples

 Description:       Specifies the number of wavform samples.

 Variable Type:     unsigned int

 Valid Max Range:   25 to 25000

     q	 ????    `    Error                             r p ?    ?    Samples                            	              ?P     ?          
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       Samples

 Description:       Number of wavform samples.

 Variable Type:     unsigned int (Passed by reference)



     sQ ????    `    Error                             tc p ?     ?    Samples                            	            	           
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       Start/Stop Generation

 Description:      Start/stop generation process.

 Variable Type:     Unsigned Char

 Valid Values:      0 - stop
                    1 - start    uw ????    `    Error                             v? ~ ?    ?    StartStop                          	                      Start 1 Stop 0   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       Status Start/Stop Generation

 Description:       Status of start/stop generation process.

 Variable Type:     Unsigned Char (Passed by reference)

 Valid Values:      0 - stop
                    1 - start    w? ????    `    Error                             x? ~ ?    ?    StatusStartStop                    	            	         ????       ????  =     K.    SetNameNVServer                 ????       ????  ?     K.    SetNameNVProcess                  y       ????  w     K.    SetRangeMDAC1                     N       ????  "v     K.    GetRangeMDAC1                     h       ????  &     K.    SetRangeMDAC2                     =       ????  )?     K.    GetRangeMDAC2                     	W       ????  -&     K.    SetCLKSource                      
?       ????  0     K.    GetCLKSource                      
?       ????  2t     K.    SetCLKExport10MHz                 !       ????  5Y     K.    GetCLKExport10MHzStatus           ?       ????  6?     K.    UpdateConfiguration               ?       ????  9?     K.    SetAmplitude                      #       ????  =,     K.    GetAmplitude                      ?       ????  @?     K.    SetPhase                          #       ????  D#     K.    GetPhase                          ?       ????  FO     K.    UpdateWaveform                    }       ????  HV     K.    SetFrequency                      ?       ????  J?     K.    GetFrequency                             ????  L?     K.    GetRealFrequency                         ????  O?     K.    SetMDAC1Amplitude                 (       ????  S[     K.    GetMDAC1Amplitude                 ?       ????  W?     K.    SetMDAC1Offset                    D       ????  [W     K.    GetMDAC1Offset                    ?       ????  _     K.    SetMDAC2                          _       ????  b[     K.    GetMDAC2                          ?       ????  e?     K.    GetAmplitudeMax                   ?       ????  i?     K.    GetWaveform                       W       ????  lL     K.    UpdateMDAC2                       ?       ????  n3     K.    SetCLKFrequency                           ????  p?     K.    GetCLKFrequency                   i       ????  r?     K.    SetNumberSamples                  ?       ????  t?     K.    GetNumberSamples                  J       ????  wN     K.    StartStop                         ?       ????  y?     K.    StartStop_Status                       ?                                     DNVServer Name                        DNVProcess Name                    ????Settings                          ????Configuration                        DSet MDAC1 Range                      DGet MDAC1 Range                      DSet MDAC2 Range                      DGet MDAC2 Range                      DSet Clock Signal Source              DGet Clock Signal Source              DOutput 10MHz Ref Clock               DOutput 10MHz Ref Clock - Status      DUpdate Configuration                 DSet Amplitude                        DGet Amplitude                        DSet Phase                            DGet Phase                            DUpdate Amplitude-Phase Settings      DSet Frequency                        DGet Frequency                        DGet Real Frequency                ????Advanced                             DSet MDAC1 Amplitude                  DGet MDAC1 Amplitude                  DSet MDAC1 Offset Value               DGet MDAC1 Offset Value               DSet MDAC2 Value                      DGet MDAC2 Value                      DGet Amplitude Max                    DGet Waveform                         DUpdate MDAC2                      ????NCO                                  DSet CLK Frequency                    DGet CLK Frequency                    DSet Number of Samples                DGet Number of Samples                DStart/Stop Generation                DStatus Start/Stop Generation          DA_DSS_cvi_driver.fp 