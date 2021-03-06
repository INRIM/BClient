s??              ?            ?                              DIO         DIO CVI Driver                                ?    
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
                       offline.     x    
 Panel Name:    Set Port

 Description:   It writes the value to selected port of Digital I/O Module. 
                   ?    
 Panel Name:    Get Digital Port Output

 Description:   It reads the current output value from  the network variable of selected port of Digital I/O Module. 
                ???? h ?    ?    NV Server                          "localhost" ???? h ?    ?    NV Process                         "DIO-PXI-NI6509"   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       Output Data

 Description:       Specifies the output data of selected port of 
                    Digital I/O Module.

 Variable Type:     uint8_t

 Valid Range:       0x00 - 0xFF

      ?    Control Name:       Port

 Description:       Specifies the number of digital port of DA_DSS.

 Variable Type:     UInt

 Valid Range:       Port 1 to Port 11    > ????    `    Error                             P p ?    ?    Output Data                       & p L     ?    Port                               	            0               pPort 0 0 Port 1 1 Port 2 3 Port 3 3 Port 4 4 Port 5 5 Port 6 6 Port 7 7 Port 8 8 Port 9 9 Port 10 10 Port 11 11   
    Return value indicating whether the function was successful. 
Unless otherwise stated, zero represents successful execution 
and a negative number indicates that an error occurred.

You can call CNVGetErrorDescription to obtain a string that 
describes the error.      ?    Control Name:       Output Data Status

 Description:       The current value of the output data of selected port of 
                    Digital I/O Module.

 Variable Type:     Unsigned int (reference)

 Valid Range:       0x00000000 - 0x000000FF

      ?    Control Name:       Port

 Description:       Specifies the number of digital port of Digital I/O Module.

 Variable Type:     UInt

 Valid Range:       Port 1 to Port 11    ? ????    `    Error                             
 p ?    ?    Output Data Status                 p L     ?    Port                               	            	                       pPort 0 0 Port 1 1 Port 2 3 Port 3 3 Port 4 4 Port 5 5 Port 6 6 Port 7 7 Port 8 8 Port 9 9 Port 10 10 Port 11 11 ????       ????  ?     K.    SetNameNVServer                 ????       ????  ?     K.    SetNameNVProcess                  y       ????  ?     K.    SetPort                           ?       ????  ?     K.    GetPortStatus                          ?                                     DNVServer Name                        DNVProcess Name                    ????Data                                 DSet Port                             DGet Port Status                       DA_DSS_cvi_driver.fp 