/*
 * ADC.h
 *
 *  Created on: Feb 10, 2018
 *      Author: lourw and joe
 */

#ifndef ADC_H_
#define ADC_H_


void adcInit(void)
{
    // Setting reference voltage to 1.2V and enabling reference
    MAP_REF_A_setReferenceVoltage(REF_A_VREF1_2V);
    MAP_REF_A_enableReferenceVoltage();

    // Enable floating point operations
    MAP_FPU_enableModule();
    MAP_FPU_enableLazyStacking();

    // Initializing ADC MCLK / 16 = 3MHz
    MAP_ADC14_enableModule();
    MAP_ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_4,
                ADC_DIVIDER_4, 0);

    // Configuring GPIO 5.4 and 5.5 for analog input
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5,
                GPIO_PIN4 | GPIO_PIN5, GPIO_TERTIARY_MODULE_FUNCTION);

    // Configure ADC Memory ADC_MEM0 -> ADC_MEM1, internal 1.2V
    MAP_ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM1, true);
    MAP_ADC14_configureConversionMemory(ADC_MEM0,
                ADC_VREFPOS_INTBUF_VREFNEG_VSS, ADC_INPUT_A0, false);
    MAP_ADC14_configureConversionMemory(ADC_MEM1,
                ADC_VREFPOS_INTBUF_VREFNEG_VSS, ADC_INPUT_A1, false);

    // Timer_A0 sourced with automatic iteration
    // Ensure Timer_A0 has been configured first
    MAP_ADC14_setSampleHoldTrigger(ADC_TRIGGER_SOURCE1, false);

    // Enable interrupt when conversion on end channel (1) completes
    //MAP_ADC14_enableInterrupt(ADC_INT0);
    MAP_ADC14_enableInterrupt(ADC_INT1);
    MAP_Interrupt_enableInterrupt(INT_ADC14);
    //MAP_Interrupt_enableMaster();

    MAP_ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);
    MAP_ADC14_enableConversion();

}


#endif /* ADC_H_ */
