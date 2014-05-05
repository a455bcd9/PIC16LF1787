unsigned short ADC_Read()
{
	unsigned short ADCResult ;

	__delay_us(1); //Waits for the acquisition to complete 1 microseconde
	ADCON0bits.GO = 1;   //Starts ADC conversion
	while(ADCON0bits.DONE);   //wait till ADC conversion is over
	ADCResult = (ADRESH<<8) + ADRESL ;   //Merging the MSB and LSB
	// ADRESH<<8 ou bien ADRESHbits.AD ?
   return ADCResult ;
}
