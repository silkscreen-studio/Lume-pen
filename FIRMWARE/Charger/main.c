#include "debug.h"

typedef struct {
    GPIO_TypeDef *PORT;
    uint16_t PIN;
} GPIOstruct;

GPIOstruct GPIO_LED[] = {
    {GPIOD, GPIO_Pin_4}, // L0 // RED   T2/CH1
    {GPIOC, GPIO_Pin_0}, // L1    T2/CH3
    {GPIOD, GPIO_Pin_3}, // L2    T2/CH2
    {GPIOD, GPIO_Pin_2}, // L3    T1/CH1
    {GPIOC, GPIO_Pin_4}, // L4    T1/CH4
    {GPIOC, GPIO_Pin_3}, // L5    T1/CH3
};

GPIOstruct GPIO_SENSE = {GPIOD, GPIO_Pin_6};
GPIOstruct GPIO_NFAULT = {GPIOD, GPIO_Pin_0};
GPIOstruct GPIO_5V_POGO_EN = {GPIOA, GPIO_Pin_2};

const uint32_t FREQPWM = 200000;

uint16_t PRSC = 0;
uint16_t ARR = 0;

void GPIO_config() {
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);

    RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM2, ENABLE);  // PWM
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_TIM1, ENABLE);  // PWM
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_ADC1, ENABLE);  // ADC
    RCC_ADCCLKConfig (RCC_PCLK2_Div8);

    GPIO_InitTypeDef GPIO_InitStructure = {0};

    for (uint8_t LED = 0; LED < 6; LED++) {
        GPIO_InitStructure.GPIO_Pin = GPIO_LED[LED].PIN;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
        GPIO_Init (GPIO_LED[LED].PORT, &GPIO_InitStructure);
    }

    GPIO_InitStructure.GPIO_Pin = GPIO_SENSE.PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init (GPIO_SENSE.PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_NFAULT.PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init (GPIO_NFAULT.PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_5V_POGO_EN.PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init (GPIO_5V_POGO_EN.PORT, &GPIO_InitStructure);

    ADC_InitTypeDef ADC_InitStructure = {0};
    ADC_DeInit (ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init (ADC1, &ADC_InitStructure);

    ADC_RegularChannelConfig (ADC1, ADC_Channel_6, 1, ADC_SampleTime_241Cycles);

    ADC_Cmd (ADC1, ENABLE);
    ADC_ResetCalibration (ADC1);
    while (ADC_GetResetCalibrationStatus (ADC1));
    ADC_StartCalibration (ADC1);
    while (ADC_GetCalibrationStatus (ADC1));

    ADC_SoftwareStartConvCmd (ADC1, ENABLE);

    uint32_t timer_clk = SystemCoreClock;  // 48MHz

    PRSC = (timer_clk / (FREQPWM * 65536)) + 1;
    ARR = (timer_clk / ((PRSC + 1) * FREQPWM)) - 1;

    TIM_Cmd (TIM2, DISABLE);
    TIM_OCInitTypeDef TIM_OCInitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

    TIM_TimeBaseInitStructure.TIM_Period = ARR;
    TIM_TimeBaseInitStructure.TIM_Prescaler = PRSC;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit (TIM2, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;


    TIM_OC1Init (TIM2, &TIM_OCInitStructure);
    TIM_OC3Init (TIM2, &TIM_OCInitStructure);
    TIM_OC2Init (TIM2, &TIM_OCInitStructure);
    TIM_CtrlPWMOutputs (TIM2, ENABLE);

    TIM_OC2PreloadConfig (TIM2, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig (TIM2, TIM_OCPreload_Enable);
    TIM_OC1PreloadConfig (TIM2, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig (TIM2, ENABLE);
    TIM_Cmd (TIM2, ENABLE);


    TIM_Cmd (TIM1, DISABLE);

    TIM_TimeBaseInitStructure.TIM_Period = ARR;
    TIM_TimeBaseInitStructure.TIM_Prescaler = PRSC;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit (TIM1, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    TIM_OC1Init (TIM1, &TIM_OCInitStructure);
    TIM_OC4Init (TIM1, &TIM_OCInitStructure);
    TIM_OC3Init (TIM1, &TIM_OCInitStructure);
    TIM_CtrlPWMOutputs (TIM1, ENABLE);

    TIM_OC1PreloadConfig (TIM1, TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig (TIM1, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig (TIM1, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig (TIM1, ENABLE);
    TIM_Cmd (TIM1, ENABLE);
}

void analogWritePWM (uint8_t LED, uint16_t duty_cycle) {
    if (LED >= 6) {
        return;
    }
    if (duty_cycle > 1023) {
        duty_cycle = 1023;
    }
    uint16_t _CCR = ((uint32_t)(ARR + 1) * duty_cycle) / 1023;

    switch (LED) {
    case 0: TIM_SetCompare1 (TIM2, _CCR); break;
    case 1: TIM_SetCompare3 (TIM2, _CCR); break;
    case 2: TIM_SetCompare2 (TIM2, _CCR); break;
    case 3: TIM_SetCompare1 (TIM1, _CCR); break;
    case 4: TIM_SetCompare4 (TIM1, _CCR); break;
    case 5: TIM_SetCompare3 (TIM1, _CCR); break;
    }
}

uint8_t FAULT() {
    return !GPIO_ReadInputDataBit (GPIO_NFAULT.PORT, GPIO_NFAULT.PIN);
}

void set_5V_POGO_EN() {
    GPIO_SetBits (GPIO_5V_POGO_EN.PORT, GPIO_5V_POGO_EN.PIN);
}

void remove_5V_POGO_EN() {
    GPIO_ResetBits (GPIO_5V_POGO_EN.PORT, GPIO_5V_POGO_EN.PIN);
}

uint8_t checkLUMEPresence() {
    float vref = 5.0f;
    float sum = 0.0f;
    for (uint8_t i = 0; i < 20; i++) {
        sum += (vref * ADC_GetConversionValue (ADC1)) / 1024.0f;
        Delay_Ms (5);
    }
    float ADC_avgVoltage = sum / 20;
    if (ADC_avgVoltage >= 2.2 && ADC_avgVoltage <= 2.8) {
        return 1;  // should be around 2.5V if the LUME is connected
    } else {
        return 0;
    }
}

void fade_IN_LED (uint8_t LED, uint32_t duration) {
    for (uint32_t rise = 0; rise < 1020; rise += 6) {  // 170
        analogWritePWM (LED, rise);
        Delay_Ms (duration / 170);
    }
}

void fade_OUT_LED (uint8_t LED, uint32_t duration) {
    for (uint32_t fall = 1020; fall > 0; fall -= 6) {  // 170 steps
        analogWritePWM (LED, fall);
        Delay_Ms (duration / 170);
    }
}

void clear_LED() {
    for (uint8_t LED = 0; LED < 6; LED++) {
        analogWritePWM (LED, 0);
    }
}

int main (void) {

    SystemCoreClockUpdate();
    Delay_Init();

    GPIO_config();
    remove_5V_POGO_EN();

    for (uint8_t LED = 1; LED < 6; LED++) {
        fade_IN_LED (LED, 240);
    }
    for (uint8_t LED = 1; LED < 6; LED++) {
        fade_OUT_LED (LED, 240);
    }
    clear_LED();

    while (1) {
        if (checkLUMEPresence()) {
            for (uint8_t it = 0; it < 2; it++) {
                for (uint8_t LED = 1; LED < 6; LED++) {
                    analogWritePWM (LED, 255);
                    Delay_Ms (350);
                    analogWritePWM (LED, 0);
                    Delay_Ms (150);
                }
            }
            if (checkLUMEPresence() && !FAULT()) {  // Enable charging
                set_5V_POGO_EN();
                while (checkLUMEPresence() && !FAULT()) {
                    for (uint8_t LED = 1; LED < 6; LED++) {
                        fade_IN_LED (LED, 100);
                        Delay_Ms (100);
                        fade_OUT_LED (LED, 100);
                        Delay_Ms (500);
                        if (FAULT()) {
                            break;
                        }
                    }
                }
            }
        }

        if (FAULT()) {
            remove_5V_POGO_EN();
            clear_LED();
            fade_IN_LED (0, 200);  // RED LED
            Delay_Ms (2600);
            fade_OUT_LED (0, 200);
            set_5V_POGO_EN();
            fade_IN_LED (0, 200);
            fade_OUT_LED (0, 200);
        }

        if (!FAULT()) {
            fade_IN_LED (1, 250);
            fade_OUT_LED (1, 250);
        }

        Delay_Ms(500);
    }
}

//Pins:

// PD6 Sense analog IN

// PC0 LEDW1 digital PWM OUT
// PD3 LEDW2 digital PWM OUT
// PD2 LEDW3 digital PWM OUT
// PC4 LEDW4 digital PWM OUT
// PC3 LEDW5 digital PWM OUT
// PD4 LED R digital PWM OUT

// PA2 5V Pogo EN digital OUT
// PD0 NFAULT digital IN
