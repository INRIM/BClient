s??         E?     @N  P          I?                              LIANV       LIA NV CVI Driver                             ?    
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
                       offline.     ?    
 Panel Name:    Set Input Configuration

 Description:   It writes the value of  LIA input configuration  setting 
                to the appropriate network variable of LIANV.     o    
 Panel Name:    Get Input Configuration.

 Description:   It reads the input configuration setting from LIA.     ?    
 Panel Name:    Set Input Configuration

 Description:   It writes the value of  LIA input configuration  setting 
                to the appropriate network variable of LIANV.     e    
 Panel Name:    Get Input Coupling.

 Description:   It reads the input coupling setting from LIA.     ?    
 Panel Name:    Set Input Shield Grounding

 Description:   It writes the value of  LIA input shield grounding setting 
                to the appropriate network variable of LIANV.     t    
 Panel Name:    Get Input Shield Grounding

 Description:   It reads the input shield grounding setting from LIA.     ?    
 Panel Name:    Set Input Line Notch Filter

 Description:   It writes the value of  LIA input line notch filter
                setting to the appropriate network variable of LIANV.     w    
 Panel Name:    Get Input Line Notch Filter.

 Description:   It reads the input line notch filter setting from LIA.     ?    
 Panel Name:    Set Reserve Mode

 Description:   It writes the value of  LIA reserve mode setting 
                to the appropriate network variable of LIANV.     a    
 Panel Name:    Get Reserve Mode.

 Description:   It reads the reserve mode setting from LIA.     ?    
 Panel Name:    Set Low Pass Filter Slope

 Description:   It writes the value of  LIA low pass filter slope setting 
                to the appropriate network variable of LIANV.     u    
 Panel Name:    Get Low Pass Filter Slope.

 Description:   It reads the low pass filter slope parameter from LIA.     ?    
 Panel Name:    Set Time Constant 

 Description:   It writes the value of  LIA time constant  setting 
                to the appropriate network variable of LIANV.     c    
 Panel Name:    Get Time Constant.

 Description:   It reads the time constant setting from LIA.     ?    
 Panel Name:    Set Sensitivity

 Description:   It writes the value of  LIA sensitivity  setting 
                to the appropriate network variable of LIANV.     _    
 Panel Name:    Get Sensitivity.

 Description:   It reads the sensitivity setting from LIA.     a    
 Panel Name:    Get Measure X and Y

 Description:   It reads X and Y outputs from LIA-SR830..     p    
 Panel Name:    Get XY Sampling rate.

 Description:   It reads the XY sampling rate setting from LIA server.     n    
 Panel Name:    GetDeviceStatus.

 Description:   It reads the device status (LIAS) from LIA SR830.
         ???? h ?    ?    NVServer                           "localhost"   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.          

      ?    Control Name:       NV Lia Process Name

 Description:       Define the name of network variable process of LIA.

 Variable Type:     Char (Passed by reference)
    g ????    `    Error                             y uJ    d    Input Configuration               ? u f     ?    NV Lia Process Name                	                       &A 0 A-B 1 I (1 MOhm) 2 I (100 MOhm) 3    "LIA-SR830_1"   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       Input Configuration.

 Description:       Reads the input configuration setting from the network variable of LIA.

 Variable Type:     Int (Passed by reference)

      ?    Control Name:       NV Lia Process Name

 Description:       Define the name of network variable process of LIA.

 Variable Type:     Char (Passed by reference)
    " ????    `    Error                             4 s$     ?    Input Configuration               ? s a     ?    NV Lia Process Name                	            	            "LIA-SR830_1"   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.          

      ?    Control Name:       NV Lia Process Name

 Description:       Define the name of network variable process of LIA.

 Variable Type:     Char (Passed by reference)
    h ????    `    Error                             z vP    P    Input Coupling                    ? v a     ?    NV Lia Process Name                	                       
AC 0 DC 1    "LIA-SR830_1"   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       Input Coupling

 Description:       Reads the input coupling setting from the network variable of LIA.

 Variable Type:     Int (Passed by reference)

      ?    Control Name:       NV Lia Process Name

 Description:       Define the name of network variable process of LIA.

 Variable Type:     Char (Passed by reference)
     ????    `    Error                              s7     ?    Input Coupling                    ? s M     ?    NV Lia Process Name                	            	            "LIA-SR830_1"   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.          

     B ????    `    Error                             T uS    P    Input Shield Grounding          ???? u Q     ?    NV Lia Process Name                	                       Float 0 Ground 1    "LIA-SR830_1"   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       NV Lia Process Name

 Description:       Define the name of network variable process of LIA.

 Variable Type:     Char (Passed by reference)    = ????    `    Error                           ???? o2     ?    Input Shield Grounding            O o d     ?    NV Lia Process Name                	            	            "LIA-SR830_1"   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       Input Line Notch Filter

 Description:       Writes the input line notch filter setting to the network variable of LIA. 
                    DA_DSS selected channel.

 Variable Type:     Int

 Valid Max Range:   0 to 3

     ? ????    `    Error                             ? uS    n    Input Line Notch Filter         ???? u Q     ?    NV Lia Process Name                	                       @No Filters 0 Line Notch 1 2 x Line Notch 2 Both Notch Filters 3    "LIA-SR830_1"   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       NV Lia Process Name

 Description:       Define the name of network variable process of LIA.

 Variable Type:     Char (Passed by reference)    !? ????    `    Error                           ???? o2     ?    Input Line Notch Filter           "? o d     ?    NV Lia Process Name                	            	            "LIA-SR830_1"   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.          

     $] ????    `    Error                             %o vP    d    Reserve Mode                    ???? v l     ?    NV Lia Process Name                	                       $High Reserve 0 Normal 1 Low Noise 2    "LIA-SR830_1"   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       NV Lia Process Name

 Description:       Define the name of network variable process of LIA.

 Variable Type:     Char (Passed by reference)    &k ????    `    Error                           ???? zG     ?    Reserve Mode                      '} z U     ?    NV Lia Process Name                	            	            "LIA-SR830_1"   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.          

     (? ????    `    Error                             * vP    d    Low Pass Filter Slope           ???? v l     ?    NV Lia Process Name                	                       /6 dB/oct 0 12 dB/oct 1 18 dB/oct 2 24 dB/oct 3    "LIA-SR830_1"   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       NV Lia Process Name

 Description:       Define the name of network variable process of LIA.

 Variable Type:     Char (Passed by reference)    + ????    `    Error                           ???? zG     ?    Low Pass Filter Slope             , z U     ?    NV Lia Process Name                	            	            "LIA-SR830_1"   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.     -? ????    `    Error                           ???? ?Q    d    Time Constant                   ???? ? W     ?    NV Lia Process Name                	                	      ?10 ?s 0 30 ?s 1 100 ?s 2 300 ?s 3 1 ms 4 3 ms 5 10 ms 6 30 ms 7 100 ms 8 300 ms 9 1 s 10 3 s 11 10 s 12 30 s 13 100 s 14 300 s 15 1 ks 16 3 ks 17 10 ks 18 30 ks 19    "LIA-SR830_1"   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       NV Lia Process Name

 Description:       Define the name of network variable process of LIA.

 Variable Type:     Char (Passed by reference)    0 ????    `    Error                           ???? }-     ?    Time Constant                     1 } a     ?    Lock-in Amplifier Name             	            	            "LIA-SR830_1"   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       NV Lia Process Name

 Description:       Define the name of network variable process of LIA.

 Variable Type:     Char (Passed by reference)    2? ????    `    Error                           ???? ?Q    d    Sensitivity                       3? ? W     ?    NV Lia Process Name                	                      ?2 nV 0 5 nV 1 10 nV 2 20 nV 3 50 nV 4 100 nV 5 200 nV 6 500 nV 7 1 ?V 8 2 ?V 9 5 ?V 10 10 ?V 11 20 ?V 12 50 ?V 13 100 ?V 14 200 ?V 15 500 ?V 16 1 mV 17 2 mV 18 5 mV 19 10 mV 20 20 mV 21 50 mV 22 100 mV 23 200 mV 24 500 mV 25 1 V 26    "LIA-SR830_1"   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       NV Lia Process Name

 Description:       Define the name of network variable process of LIA.

 Variable Type:     Char (Passed by reference)    6  ????    `    Error                             7 } a     ?    NV Lia Process Name             ???? ~G     ?    Sensitivity                        	            "LIA-SR830_1"    	           
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       NV Lia Process Name

 Description:       Define the name of network variable process of LIA.

 Variable Type:     Char (Passed by reference)    8? ????    `    Error                           ???? w    ?    X and Y Values                    9? w %     ?    NV Lia Process Name                	            	            "LIA-R830_1"   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       NV Lia Process Name

 Description:       Define the name of network variable process of LIA.

 Variable Type:     Char (Passed by reference)    ; ????    `    Error                           ???? zG    ?    XY Sampling Rate                  < z U     ?    NV Lia Process Name                	            	           "LIA-SR830_1"   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       NV Lia Process Name

 Description:       Define the name of network variable process of LIA.

 Variable Type:     Char (Passed by reference)    =? ????    `    Error                             >? } /     ?    NV Lia Process Name             ???? ~o    ?    LIAS                            ???? } ?    ?    Status Byte Name                   	            "LIA-SR830_1"    	            "LIAS" ????       ????  #     K.    SetNameNVServer                   y       ????  1     K.    SetInputConfiguration             4       ????  ?     K.    GetInputConfiguration             ?       ????  2     K.    SetInputCoupling                  f       ????  z     K.    GetInputCoupling                  ?       ????  a     K.    SetInputShieldGrounding           ?       ????  ?     K.    GetInputShieldGrounding                  ????   ?     K.    SetInputLineNotchFilter           ?       ????  #?     K.    GetInputLineNotchFilter           O       ????  %|     K.    SetReserveMode                    ?       ????  ('     K.    GetReserveMode                    	d       ????  *     K.    SetLowPassFilterSlope             
"       ????  ,?     K.    GetLowPassFilterSlope             
?       ????  .?     K.    SetTimeConstant                   O       ????  1?     K.    GetTimeConstant                   ?       ????  4M     K.    SetSensitivity                    e       ????  7?     K.    GetSensitivity                    ?       ????  :@     K.    GetMeasureXY                      5       ????  <?     K.    GetXYSamplingRate                 ?       ????  ?G     K.    GetDeviceStatus                        ?                                     DNVServer Name                     ????Settings                          ????Input                                DSet Input Configuration              DGet Input Configuration              DSet Input Coupling                   DGet Input Coupling                   DSet Input Shield Grounding           DGet Input Shield Grounding           DSet Input Line Notch Filter          DGet Input Line Notch Filter          DSet Reserve Mode                     DGet Reserve Mode                     DSet Low Pass Filter Slope            DGet Low Pass Filter Slope            DSet Time Constant                    DGet Time Constant                    DSet Sensitivity                      DGet Sensitivity                   ????Measure Values                       DGet Measure X and Y                  DGet XY Sampling Rate              ????Status Bytes                         DGet LIAS                              DA_DSS_cvi_driver.fp 