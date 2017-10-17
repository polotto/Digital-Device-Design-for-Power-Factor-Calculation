//*****************************************************************************
//
// project0.c - Example to demonstrate minimal TivaWare setup
//
// Copyright (c) 2012-2013 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 1.1 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************

#include <stdint.h>//Padrão C99
#include <stdbool.h>//Padrão C99
#include <string.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_uart.h"
#include "inc/hw_gpio.h"
//#include "inc/hw_ints.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"//Periféricos GPIO
#include "driverlib/interrupt.h"//Interrupções
#include "driverlib/timer.h"//Timer
#include "driverlib/debug.h"//Debug
#include "driverlib/adc.h"//Conversor A/D
#include "driverlib/systick.h"//Configuração do clock
#include "driverlib/fpu.h"//Floating point unit para contas com números reais
#include "driverlib/rom.h"//Acesso à memória ROM para a FPU
#include "driverlib/rom_map.h"
#include "driverlib/uart.h"//Comunicação UART
#include <math.h>//Biblioteca padrão para funções matemáticas
//#include "uartstdio.h"
//#include "ustdlib.h"
//#include <stdio.h>
//#include "utils/cmdline.h"
#include "driverlib/pin_map.h"
//*****************************************************************************
//
// Mapeamento dos periféricos, pinos e funções
//
//*****************************************************************************

/*Periféricos*/
#define PORT_F_PERIPH SYSCTL_PERIPH_GPIOF
#define TIMER0_PERIPH SYSCTL_PERIPH_TIMER0
#define TIMER1_PERIPH SYSCTL_PERIPH_TIMER1
#define PORT_D_PERIPH SYSCTL_PERIPH_GPIOD
#define ANALOG_PERIPH SYSCTL_PERIPH_ADC0
#define PORT_E_PERIPH SYSCTL_PERIPH_GPIOE

/*Saídas*/
#define LED_BASE GPIO_PORTF_BASE
#define RED_LED   GPIO_PIN_1
#define BLUE_LED  GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3

/*Entradas Digitais*/
#define BUTTON_BASE GPIO_PORTF_BASE
#define SW1 GPIO_PIN_4
#define SWITCH_BASE GPIO_PORTE_BASE
#define SW_VOLT GPIO_PIN_4

/*Entradas Analógicas*/
#define ANALOG_BASE ADC0_BASE
#define TEMP_SENS_IN ADC_CTL_TS
#define VOLT_IN ADC_CTL_CH11
#define CURRENT_IN ADC_CTL_CH8

/*Interrupções*/
#define TIME_SAMPLE_BASE TIMER0_BASE
#define TIME_SAMPLE TIMER_A
#define TIME_COUNT_BASE TIMER1_BASE
#define TIME_COUNT TIMER_A
#define ZERO_BASE GPIO_PORTD_BASE
#define ZERO_INT_IN GPIO_INT_PIN_3
#define ZERO_IN GPIO_PIN_3

/*Frequência das interrupções (Hz)*/
//A cada 1s (1Hz) amostra os valores da entrada e recalcula as variáveis
#define FREQ_SAMPLE 1
//Intervalo de espera entre as amostras (Hz), 1/f =
//#define FREQ_COUNT 3840

/*Tensão da rede (V)*/
#define IN127V 1
#define IN220V 0

/*Dimensão dos vetores e valores máximos*/
#define DIM_MAX 300
#define NFFT_t 128 //NFFT_t = (int)pow(2.0, ceil(log((float)MAX_SIZE)/log(2.0)));
#define MAX_SAMPLE 128
#define DELAY_SAMPLE 3840 //Intervalo de espera entre as amostras (Hz), 1/f =
#define F_FUND 60 //Frequência fundamental
#define PI	3.14159265359
#define TWOPI	(2.0*PI)

/*Funções*/
void PortDIntHandler(void);
void Timer0IntHandler(void);
void Timer1IntHandler(void);
void setLevels(float *V, float *I, unsigned int size, float Volt, float Amp);
float maxVal(float *V, unsigned int size);
void offset(float *V, float *I, unsigned int size, float refV, float refA);
float offCalc(float *V, unsigned int size);
float valAVG(float *t, float *X, unsigned int size);
float myTrapz(float *X, float *Y, unsigned int size);
float* vectorMult(float *X, float *Y, unsigned int size);
float* vectorPow(float *X, unsigned int size);
void powerFactor(float *t, float *V, float *I, unsigned int size, float *fp, float *S, float *P, float *Vrms, float *Irms);
void ConfigureUART(void);
void reverse(char *str, unsigned int len);
int intToStr(int x, char str[], unsigned int d);
void ftoa(float n, char *res, unsigned int afterpoint);
void send_byte(int data);
void print(char *data);
void println(char *data);
void send_int(int a);
void send_intln(int a);
void four1(float data[], int nn, int isign);
float THD(float *I, unsigned int NFFT, unsigned int size);
char * dtoa(char *s, double n);
int zeroCross(float *X, unsigned int size);
//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Project Zero (project0)</h1>
//!
//! This example demonstrates the use of TivaWare to setup the clocks and
//! toggle GPIO pins to make the LED's blink. This is a good place to start
//! understanding your launchpad and the tools that can be used to program it.
//! See http://www.ti.com/tm4c123g-launchpad/project0 for more information and
//! tutorial videos.
//!
//
//*****************************************************************************


//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line){
	print("Error at line "); send_int(ui32Line); print(" of "); println(pcFilename);
	while(1){
	}
}
#endif

//*****************************************************************************
//
// Main 'C' Language entry point.  Toggle an LED using TivaWare.
// See http://www.ti.com/tm4c123g-launchpad/project0 for more information and
// tutorial videos.
//
//*****************************************************************************
volatile uint32_t ui32ADC0Value[4], ui32ADC0Value2[4];
//volatile unsigned int ui32TempAvg, ui32TempValueC;//volatile uint32_t ui32TempValueF;
volatile unsigned int ui32PotPB5Volt, ui32PotPE5Curr;
//volatile unsigned int counter=0;
//volatile bool getSample=false;
//volatile float freq=0;
volatile float Ans[DIM_MAX];
//volatile unsigned int size = DIM_MAX;
/* calculate NFFT as the next higher power of 2 >= Nx */
/*X = (float *) malloc((2*NFFT_t+1) * sizeof(float));
X_ABS = (float *) malloc((2*NFFT_t+1) * sizeof(float));
X_ANG = (float *) malloc((2*NFFT_t+1) * sizeof(float));
f = (float *) malloc((NFFT_t+1) * sizeof(float));*/
volatile float X[2*NFFT_t+1], X_ABS[2*NFFT_t+1], X_ANG[2*NFFT_t+1];
//volatile float f[2*NFFT_t+1], Fs=DELAY_SAMPLE;
#define N_HARM 10
#define MAX_NUMBER_STRING_SIZE 32 //Deve ser maior q o buffer de string (pcBuffer)
#define PRECISION 0.000001
volatile char pcBuffer[30];
//Fs=1/t_inter
//N_HARM<NFFT/2


//volatile float t_inter=(float)1/(float)DELAY_SAMPLE;

int main(void){
	volatile static float  t[DIM_MAX], V[DIM_MAX], I[DIM_MAX];
	volatile static float fp, S, P, Vrms, Irms, Voffset, Ioffset, volt, amp, theta, cos_theta, fp_2;
	volatile static float V_THD, I_THD, cos_lamb, lamb, D, Q, S_proj, cos_phi, phi, aux;
	static float t_inter;
	static float analog_const;
	int i=0, j=0, k=0, Vz=0, Iz=0;//Contadores não podem ser universais e volatile

	/*****FPU*****/
	ROM_FPUEnable();
	ROM_FPUStackingEnable();
	ROM_FPULazyStackingEnable();
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
	/*************/

	/* Configuração do clock
	Configure the system clock to run using a 16MHz crystal on the main oscillator, driving
	the 400MHz PLL. The 400MHz PLL oscillates at only that frequency, but can be driven
	by crystals or oscillators running between 5 and 25MHz. There is a default /2 divider in
	the clock path and we are specifying another /5, which totals 10. That means the System
	Clock will be 40MHz.
	 */
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);//Configurar a frequencia do clock para 40MHz

	/*****GPIO*****/
	//Configuração do GPIO
	//*************Saídas***************
	SysCtlPeripheralEnable(PORT_F_PERIPH);//Habilitar os periféricos GPIO da porta F
	GPIOPinTypeGPIOOutput(LED_BASE, RED_LED|BLUE_LED|GREEN_LED);//Configurar saidas

	//*************Entradas*************
	//GPIOPinTypeGPIOInput(BUTTON_BASE, SW1);//Configurar entradas
	//GPIOPadConfigSet(BUTTON_BASE,SW1,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);//Configurar resistor de pull-up

	SysCtlPeripheralEnable(PORT_E_PERIPH);
	GPIOPinTypeGPIOInput(SWITCH_BASE, SW_VOLT);//Configurar entradas
	GPIOPadConfigSet(SWITCH_BASE,SW_VOLT,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);//Configurar resistor de pull-up
	/***************/

	/*****Timer 0A (AMOSTRAS)*****/
	//Configuração do timer
	/*
	 * Configuração do intervalo de interrupção
	To toggle a GPIO at 10Hz and a 50% duty cycle, you need to generate an interrupt at ½
	of the desired period. First, calculate the number of clock cycles required for a 10Hz
	period by calling SysCtlClockGet() and dividing it by your desired frequency.
	Then divide that by two, since we want a count that is ½ of that for the interrupt.
	 */
	/*SysCtlPeripheralEnable(TIMER0_PERIPH);//Habilitar o clock para o periferico timer 0
	TimerConfigure(TIME_SAMPLE_BASE, TIMER_CFG_PERIODIC);//Configurar o tempo base do timer 0
	ui32Period = (SysCtlClockGet() / FREQ_SAMPLE); /// 2;
	TimerLoadSet(TIME_SAMPLE_BASE, TIME_SAMPLE, ui32Period -1);

	//Habilitar a interrução
	IntEnable(INT_TIMER0A);//Habilitar especificamente o timer 0A
	TimerIntEnable(TIME_SAMPLE_BASE, TIMER_TIMA_TIMEOUT);//Habilitar o estou do timer

	//Habilitar o inicicio das interrupções do timer
	TimerEnable(TIME_SAMPLE_BASE, TIME_SAMPLE);*/
	/***************/

	/*****TIMER 1A (CONTADOR DE FREQUÊNCIA)*******/
	/*SysCtlPeripheralEnable(TIMER1_PERIPH);//Habilitar o clock para o periferico timer 1
	TimerConfigure(TIME_COUNT_BASE, TIMER_CFG_PERIODIC);//Configurar o tempo base do timer 1

	ui32Period = (SysCtlClockGet() / FREQ_COUNT); /// 2;
	TimerLoadSet(TIME_COUNT_BASE, TIME_COUNT, ui32Period -1);

	//Habilitar a interrução
	IntEnable(INT_TIMER1A);//Habilitar especificamente o timer 1A
	TimerIntEnable(TIME_COUNT_BASE, TIMER_TIMA_TIMEOUT);//Habilitar o estou do timer

	//Habilitar o inicicio das interrupções do timer
	TimerEnable(TIME_COUNT_BASE, TIME_COUNT);*/


	/*****GPIO Interrupt***/
	/*SysCtlPeripheralEnable(PORT_D_PERIPH);
	GPIOPadConfigSet(ZERO_BASE ,ZERO_IN,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
	//GPIOIntTypeSet(ZERO_BASE,ZERO_IN,GPIO_FALLING_EDGE);
	GPIOIntTypeSet(ZERO_BASE,ZERO_IN,GPIO_RISING_EDGE);
	GPIOIntRegister(ZERO_BASE,PortDIntHandler);*/

	SysCtlPeripheralEnable(PORT_D_PERIPH);
	GPIOPinTypeGPIOInput(ZERO_BASE, ZERO_IN);//Configurar entradas
	GPIOPadConfigSet(ZERO_BASE,ZERO_IN,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);//Configurar resistor de pull-up
	/**********************/

	/*****ADC0*****/
	//Configuração do ADC
	SysCtlPeripheralEnable(ANALOG_PERIPH);
	//SysCtlADCSpeedSet(SYSCTL_ADCSPEED_125KSPS);
	//ADCSequenceDisable(ANALOG_BASE, 1);
	ADCHardwareOversampleConfigure(ANALOG_BASE,50);

	ADCSequenceConfigure(ANALOG_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
	//ADCSequenceStepConfigure(ANALOG_BASE, 1, 0, TEMP_SENS_IN);
	ADCSequenceStepConfigure(ANALOG_BASE, 1, 0, VOLT_IN|ADC_CTL_IE|ADC_CTL_END);
	ADCSequenceEnable(ANALOG_BASE, 1);

	ADCSequenceConfigure(ANALOG_BASE, 2, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ANALOG_BASE, 2, 0,CURRENT_IN|ADC_CTL_IE|ADC_CTL_END);
	ADCSequenceEnable(ANALOG_BASE, 2);
	/***************/

	IntMasterEnable();//Chave geral das interrupções

	//
	// Enable and Initialize the UART.
	//
	ConfigureUART();

	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0)){}

	/*print("Medidor Eletronico de Fator de Potencia - Angelo Polotto\n");
	print("Resultados:\n");
	print(">\n ");*/
	t_inter=(((float)1/(float)F_FUND)/ (float)DELAY_SAMPLE);
	analog_const=(float)3.3/(float)4023;
	//while(true){

	SysCtlDelay((SysCtlClockGet()/ 10)/2);//20Hz = 50ms

		if(GPIOPinRead(SWITCH_BASE, SW_VOLT)){
			volt=179.61;
			amp=5;
			if(GPIOPinRead(LED_BASE, RED_LED))	GPIOPinWrite(LED_BASE, RED_LED, !RED_LED);//Apaga o LED
			else	GPIOPinWrite(LED_BASE, RED_LED, RED_LED);
		}
		else{
			volt=311.13;
			amp=5;
			if(GPIOPinRead(LED_BASE, BLUE_LED))	GPIOPinWrite(LED_BASE, BLUE_LED, !BLUE_LED);//Apaga o LED
			else	GPIOPinWrite(LED_BASE, BLUE_LED, BLUE_LED);
		}

		//getSample=false;
		//GPIOIntEnable(ZERO_BASE, ZERO_INT_IN);//Habilitar interrupção por borda de subida

		/**************************Aguarda as amostras************************/
		while(GPIOPinRead(ZERO_BASE, ZERO_IN)){}
		/*********************************************************************/

		SysCtlDelay((SysCtlClockGet()/ 20)/2);
		i=0;
		while(i<MAX_SAMPLE){
			//Leitura do ADC
			ADCIntClear(ANALOG_BASE, 1);
			ADCProcessorTrigger(ANALOG_BASE, 1);
			while(!ADCIntStatus(ANALOG_BASE, 1, false)){}
			ADCSequenceDataGet(ANALOG_BASE, 1, ui32ADC0Value);

			ADCIntClear(ANALOG_BASE, 2);
			ADCProcessorTrigger(ANALOG_BASE, 2);
			while(!ADCIntStatus(ANALOG_BASE, 2, false)){}
			ADCSequenceDataGet(ANALOG_BASE, 2, ui32ADC0Value2);

			V[i] = (float)ui32ADC0Value[0];
			I[i] = (float)ui32ADC0Value2[0];
			i++;
			SysCtlDelay(((float)SysCtlClockGet()/ ((float)DELAY_SAMPLE)/(float)4));
		}
		//while(!getSample){}
		SysCtlDelay(1);


		for(i=0;i<MAX_SAMPLE;i++){
			V[i] = analog_const*V[i];
			I[i] = analog_const*I[i];
			t[i] = t_inter*i;
		}

		/*********************************************************************/
		//Voffset=(float)offCalc(V,MAX_SAMPLE);
		Voffset=valAVG(t, V, MAX_SAMPLE);
		Ioffset=1.65;
		//Ioffset=valAVG(t, I, MAX_SAMPLE);
		offset(V, I, MAX_SAMPLE, Voffset, Ioffset);
		setLevels(V, I, MAX_SAMPLE, volt, amp);//OWOOO sometimes!!!

		V_THD=THD(V, NFFT_t, MAX_SAMPLE);
		I_THD=THD(I, NFFT_t, MAX_SAMPLE);

		powerFactor(t, V, I, MAX_SAMPLE, &fp, &S, &P, &Vrms, &Irms);
		aux=(float)I_THD/(float)100;
		cos_lamb=(float)1/(float)sqrt(1+(aux*aux));
		lamb=acos(cos_lamb);
		Vz=zeroCross(V,MAX_SAMPLE);
		Iz=zeroCross(I,MAX_SAMPLE);
		theta=t[Vz]-t[Iz];
		theta=((float)TWOPI*theta)/((float)t_inter*MAX_SAMPLE);
		cos_theta=cos(theta);
		S_proj=S*cos_lamb;
		cos_phi=(float)P/(float)S_proj;
		phi=acos(cos_phi);
		Q=sin(phi)*S_proj;
		D=S*sin(lamb);
		fp_2=cos_lamb*cos_phi;

		//print("*");

		dtoa(pcBuffer, Vrms);
		print("vrms:");print(pcBuffer);print("V;");
		dtoa(pcBuffer, Irms);
		print("irms:");print(pcBuffer);print("A;");

		dtoa(pcBuffer, P);
		print("p:");print(pcBuffer);print("W;");
		dtoa(pcBuffer, S);
		print("s:");print(pcBuffer);print("VA;");
		dtoa(pcBuffer, Q);
		print("q:");print(pcBuffer);print("VA;");
		dtoa(pcBuffer, D);
		print("d:");print(pcBuffer);print("DVA;");
		dtoa(pcBuffer, S_proj);
		print("s_proj:");print(pcBuffer);print("VA;");

		dtoa(pcBuffer, fp);
		print("fp:");print(pcBuffer);print("-");
		if(fp>0.95)	print("resistivo");
		else if (I_THD>=1.5)print("nao linear");
		else if(theta>0 && lamb<=0.1)	print("capacitivo");
		else if(theta<0 && lamb<=0.1)	print("indutivo");
		print(";");

		dtoa(pcBuffer, fp_2);
		print("fpcos:");print(pcBuffer);print(";");

		dtoa(pcBuffer, V_THD);
		print("vthd:");print(pcBuffer);print("%;");
		dtoa(pcBuffer, I_THD);
		print("ithd:");print(pcBuffer);print("%;");

		dtoa(pcBuffer, theta);
		print("theta: ");print(pcBuffer);print(" rad;");
		dtoa(pcBuffer, cos_theta);
		print("Cos theta: ");print(pcBuffer);print(";");

		dtoa(pcBuffer, lamb);
		print("lamb: ");print(pcBuffer);print(" rad;");
		dtoa(pcBuffer, cos_lamb);
		print("coslamb: ");print(pcBuffer);print(";");

		dtoa(pcBuffer, phi);
		print("phi: ");print(pcBuffer);print(" rad;");
		dtoa(pcBuffer, cos_phi);
		print("cosphi: ");print(pcBuffer);print(";");

		/*print("t;V;I;");
		for(k=0;k<MAX_SAMPLE;k++){
			dtoa(pcBuffer, t[k]);
			print(pcBuffer);print(";");
			SysCtlDelay(100);
			dtoa(pcBuffer, V[k]);
			print(pcBuffer);print(";");
			SysCtlDelay(100);
			dtoa(pcBuffer, I[k]);
			print(pcBuffer);print(";");
			println("");
			SysCtlDelay(100);
		}*/

		j=0;
		print("t:");
		while(j<MAX_SAMPLE){
			dtoa(pcBuffer, t[j]);
			print(pcBuffer);print(",");
			j++;
		}
		print(";");
		j=0;
		print("V:");
		while(j<MAX_SAMPLE){
			dtoa(pcBuffer, V[j]);
			print(pcBuffer);print(",");
			j++;
		}
		print(";");
		j=0;
		print("I:");
		while(j<MAX_SAMPLE){
			dtoa(pcBuffer, I[j]);
			print(pcBuffer);print(",");
			j++;
		}
		print(";");

		print("*");

		while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0)){}

		/*i=j=0;
		while(i<MAX_SAMPLE){
			V[i] = 0;
			I[i] = 0;
			t[i] = 0;
			i++;
		}*/
		while(true){}
		SysCtlDelay((SysCtlClockGet()/0.5));
	//}
}

float valAVG(float *t, float *X, unsigned int size){
	float T = t[size-1]-t[0];
	return (float)myTrapz(t, X, size)/(float)T;
}

void offset(float *V, float *I, unsigned int size, float refV, float refA){
	int i=0;
	for(i=0; i< size; i++){
		V[i]=V[i]-refV;
		I[i]=I[i]-refA;
	}

}

int zeroCross(float *X, unsigned int size){
    /*%zeroCross Detecção de cruzamentos por zero
    %   Detecta cruzamentos e os divide em bordas de descidas e subidas do
    %   vetor X*/
    //QList<int> t_down;
    //QLIst<int> t_up;
    int i=0, j=0, k=0;
    for (i=(size/4)-5; i<size; i++){
        if (X[i]>=0 && X[i+1]<=0){
            return i;
            j++;
        }
        /*if (X[i]<=0 && X[i+1]>=0){
            return i;
            k++;
        }*/
    }
}

void setLevels(float *V, float *I, unsigned int size, float Volt, float Amp){
	//float maxV=maxVal(V,size);
	float maxV=1.65;
	//float maxV=1.65;
	float maxI=1.65;
	int i=0;
	for (i=0;i<size;i++){
		V[i]=((float)Volt/(float)maxV)*V[i];
		I[i]=((float)Amp/(float)maxI)*I[i];
	}
}

//Função para manipular o estouro do timer 0
void Timer0IntHandler(void){
	// Clear the timer interrupt
	TimerIntClear(TIME_SAMPLE_BASE, TIMER_TIMA_TIMEOUT);
	if(GPIOPinRead(LED_BASE, RED_LED))	GPIOPinWrite(LED_BASE, RED_LED, !RED_LED);//Apaga o LED
	else	GPIOPinWrite(LED_BASE, RED_LED, RED_LED);
}

//Função para manipular a interrupção de borda de subida no GPIO
void PortDIntHandler(void){
	unsigned int i=0;
	uint32_t status=0;
	status = GPIOIntStatus(ZERO_BASE,true);
	GPIOIntClear(ZERO_BASE,status);
	GPIOIntDisable(ZERO_BASE, ZERO_INT_IN);

	//if(!getSample){
	/*SysCtlDelay(2);
	while(i<MAX_SAMPLE){
		//Leitura do ADC
		ADCIntClear(ANALOG_BASE, 1);
		ADCProcessorTrigger(ANALOG_BASE, 1);
		while(!ADCIntStatus(ANALOG_BASE, 1, false)){}
		ADCSequenceDataGet(ANALOG_BASE, 1, ui32ADC0Value);

		ADCIntClear(ANALOG_BASE, 2);
		ADCProcessorTrigger(ANALOG_BASE, 2);
		while(!ADCIntStatus(ANALOG_BASE, 2, false)){}
		ADCSequenceDataGet(ANALOG_BASE, 2, ui32ADC0Value2);

		V[i] = (float)ui32ADC0Value[0];
		I[i] = (float)ui32ADC0Value2[0];
		i++;
		SysCtlDelay(((uint32_t)SysCtlClockGet()/ (uint32_t)((float)DELAY_SAMPLE)/(float)4));
	}*/

	// Read the current state of the GPIO pin and
	// write back the opposite state
	if(GPIOPinRead(LED_BASE, BLUE_LED))	GPIOPinWrite(LED_BASE, GREEN_LED, !GREEN_LED);//Apaga o LED
	else	GPIOPinWrite(LED_BASE, GREEN_LED, GREEN_LED);//Acende o LED
	//getSample=true;
	//}
}

void Timer1IntHandler(void){
	// Clear the timer interrupt
	TimerIntClear(TIME_SAMPLE_BASE, TIMER_TIMA_TIMEOUT);
}

/*******************FP*************************************/
float myTrapz(float *X, float *Y, unsigned int size){
	//myTrapz Função para cálculo da integral numérica de uma função discreta
	//   Efetua uma aproximação usando o método de soma de trapézios.
	int i=0;
	float Int=0, H, B, b;
	for(i=0; i< size-1; i++){
		H = X[i+1]-X[i];
		B = Y[i];
		b = Y[i+1];
		Int = Int + (B+b)*H/2;
	}
	return Int;
}

float* vectorMult(float *X, float *Y, unsigned int size){
	int i=0;
	for(i=0;i<size;i++){
		Ans[i] = X[i]*Y[i];
	}
	return Ans;
}

float* vectorPow(float *X, unsigned int size){
	int i=0;
	for(i=0;i<size;i++){
		Ans[i] = X[i]*X[i];
	}
	return Ans;
}

void powerFactor(float *t, float *V, float *I, unsigned int size, float *fp, float *S, float *P, float *Vrms, float *Irms){
	/*powerFactor Função para calculo do fator de potência
%   Essa função retorna o fator de potência (fp), potência ativa(P) e
%   potêcia aparente(S) com base no tempo(t), tensão(V) e corrente(I).
%   Todos os valores de entrada devem ser vetores vindos da amostragem.
%   Esses valores devem também serem correspondentes a um período de
%   amostra, para estimativa correta dos resultados.*/
	float fp_t=0, S_t=0, P_t=0, Vrms_t=0, Irms_t=0;
	float T = t[size-1]-t[0];
	P_t = myTrapz(t, vectorMult(V, I, size), size)/T;
	Vrms_t = sqrt(myTrapz(t, vectorPow(V, size), size)/T);
	Irms_t = sqrt(myTrapz(t, vectorPow(I, size), size)/T);
	S_t = Vrms_t*Irms_t;
	fp_t = P_t/S_t;

	*fp=fp_t;
	*S=S_t;
	*P=P_t;
	*Vrms=Vrms_t;
	*Irms=Irms_t;
}
/**********************************************************/

/*******************THD************************************/
void four1(float data[], int nn, int isign)
{
    int n, mmax, m, j, istep, i;
    float wtemp, wr, wpr, wpi, wi, theta;
    float tempr, tempi;

    n = nn << 1;
    j = 1;
    for (i = 1; i < n; i += 2) {
	if (j > i) {
	    tempr = data[j];     data[j] = data[i];     data[i] = tempr;
	    tempr = data[j+1]; data[j+1] = data[i+1]; data[i+1] = tempr;
	}
	m = n >> 1;
	while (m >= 2 && j > m) {
	    j -= m;
	    m >>= 1;
	}
	j += m;
    }
    mmax = 2;
    while (n > mmax) {
	istep = 2*mmax;
	theta = TWOPI/(isign*mmax);
	wtemp = sin(0.5*theta);
	wpr = -2.0*wtemp*wtemp;
	wpi = sin(theta);
	wr = 1.0;
	wi = 0.0;
	for (m = 1; m < mmax; m += 2) {
	    for (i = m; i <= n; i += istep) {
		j =i + mmax;
		tempr = wr*data[j]   - wi*data[j+1];
		tempi = wr*data[j+1] + wi*data[j];
		data[j]   = data[i]   - tempr;
		data[j+1] = data[i+1] - tempi;
		data[i] += tempr;
		data[i+1] += tempi;
	    }
	    wr = (wtemp = wr)*wpr - wi*wpi + wr;
	    wi = wi*wpr + wtemp*wpi + wi;
	}
	mmax = istep;
    }
}

float THD(float *I, unsigned int NFFT, unsigned int size){
    int Nx, i;
	float maximum, sum, aux;
	int location;
	Nx=size;

    /* Storing x(n) in a complex array to make it work with four1.
	This is needed even though x(n) is purely real in this case. */
	for(i=0; i<Nx; i++){
		X[2*i+1] = I[i];//******************************************Input signal
		X[2*i+2] = 0.0;
	}
	/* pad the remainder of the array with zeros (0 + 0 j) */
	for(i=Nx; i<NFFT; i++){
		X[2*i+1] = 0.0;
		X[2*i+2] = 0.0;
	}

	/*print("\nInput complex sequence (padded to next highest power of 2):\n");
	print("x[]: \n");
	for(i=0; i<NFFT; i++){
		send_int(i);print(";");
		dtoa(pcBuffer, X[2*i+1]);
		print(pcBuffer);print(";");
		dtoa(pcBuffer, X[2*i+2]);
		print(pcBuffer);print(";\n");
	}*/

	/* calculate FFT */
	four1(X, NFFT, 1);

    /* Compute the X signal ABS*/
	for(i=0; i<NFFT; i++){
		//X_ANG[2*i+1]=sqrt(pow(X[2*i+1],2)+pow(X[2*i+2],2));
		X_ABS[i]=sqrt(pow(X[2*i+1],2)+pow(X[2*i+2],2));
		//X_ANG[2*i+2]=tan((float)X[2*i+2]/(float)X[2*i+1]);
	}

	/*print("x_abs[]: \n");
	for(i=0; i<NFFT; i++){
		send_int(i);print(";");
		dtoa(pcBuffer,X_ABS[i]);
		print(pcBuffer);print(";\n");
	}*/

	/*for(i=0; i<NFFT; i++){
		f[i]=i*((float)Fs/(float)NFFT);
	}*/
	maximum = 0.0;
	location=0;
    for (i = 0; i < NFFT/2; i++){
        if (X_ABS[i] > maximum){
           maximum  = X_ABS[i];
           location = i;
        }
    }
    /*print("Maximum: ");
	dtoa(pcBuffer,maximum);
	print(pcBuffer);print(";\n");
	print("Location: ");
	dtoa(pcBuffer,location);
	print(pcBuffer);print(";\n");*/
    sum=0;

    /*for(i=0; i<=NFFT/2; i++){
		X_ABS[i]=10*log(X_ABS[i]);
	}*/

	for(i=location+1; i<=N_HARM; i++){
		//aux=X_ABS[i]*X_ABS[i];
        sum=sum+pow(X_ABS[i],2);
        /*print("sum: ");
		dtoa(pcBuffer,sum);
		print(pcBuffer);print(";\n");
		print("aux: ");
		dtoa(pcBuffer,aux);
		print(pcBuffer);print(";\n");*/
	}
	/*print("sum: ");
	dtoa(pcBuffer,sum);
	print(pcBuffer);print(";\n");*/
	maximum=(float)maximum*(float)10;
	float THD_val=(sqrt((float)sum)/(float)maximum)*100;
	/*dtoa(pcBuffer,THD_val);
	print("THD_val: ");
	print(pcBuffer);print(";\n");*/
    /**********************************************/
    return THD_val;
}
/**********************************************************/

void ConfigureUART(void){
	//
	// Enable the GPIO Peripheral used by the UART.
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)){}
	//
	// Enable UART0
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0)){}
	//
	// Configure GPIO Pins for UART mode.
	//
	UARTDisable(UART0_BASE);
	GPIOPadConfigSet(GPIO_PORTA_BASE,GPIO_PIN_0|GPIO_PIN_1,GPIO_STRENGTH_8MA,GPIO_PIN_TYPE_STD);
	UARTFIFOEnable(UART0_BASE);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_TWO | UART_CONFIG_PAR_EVEN));
	UARTEnable(UART0_BASE);

	//UARTDisable(UART0_BASE);
	/*GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_TWO | UART_CONFIG_PAR_EVEN));*/
	//UARTEnable(UART0_BASE);


	//
	// Use the internal 16MHz oscillator as the UART clock source.
	//
	//UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

	//
	// Initialize the UART for console I/O.
	//
	//UARTStdioConfig(0, 115200, 16000000);
}

void send_byte(int data){
	UARTCharPut(UART0_BASE, data);
}
void print(char *data){
	while(*data){
		send_byte(*data);
		data++;
	}
}
void println(char *data){
	while(*data){
		send_byte(*data);
		data++;
	}
	send_byte('\n');
	send_byte('\r');
}
void send_int(int a){
	int temp;
	int rev=0;
	int dummy =a;
	if(a==0)	send_byte('0');
	while (dummy){
		rev = rev * 10;
		rev = rev + dummy%10;
		dummy = dummy/10;
	}
	while(rev){
		temp=rev%10;
		send_byte(0x30+temp);
		rev /=10;
	}
}
void send_intln(int a){
	int temp;
	int rev=0;
	int dummy =a;
	if(a==0)	send_byte('0');
	while (dummy){
		rev = rev * 10;
		rev = rev + dummy%10;
		dummy = dummy/10;
	}
	while(rev){
		temp=rev%10;
		send_byte(0x30+temp);
		rev /=10;
	}
	send_byte('\n');
	send_byte('\r');
}

/***********************************************************************************************************/
/**
 * Double to ASCII
 */
char * dtoa(char *s, double n) {
    // handle special cases
    if (isnan(n)) {
        strcpy(s, "nan");
    } else if (isinf(n)) {
        strcpy(s, "inf");
    } else if (n == 0.0) {
        strcpy(s, "0");
    } else {
        int digit, m, m1;
        char *c = s;
        int neg = (n < 0);
        if (neg)
            n = -n;
        // calculate magnitude
        m = log10(n);
        int useExp = (m >= 14 || (neg && m >= 9) || m <= -9);
        if (neg)
            *(c++) = '-';
        // set up for scientific notation
        if (useExp) {
            if (m < 0)
               m -= 1.0;
            n = n / pow(10.0, m);
            m1 = m;
            m = 0;
        }
        if (m < 1.0) {
            m = 0;
        }
        // convert the number
        while (n > PRECISION || m >= 0) {
            double weight = pow(10.0, m);
            if (weight > 0 && !isinf(weight)) {
                digit = floor(n / weight);
                n -= (digit * weight);
                *(c++) = '0' + digit;
            }
            if (m == 0 && n > 0)
                *(c++) = '.';
            m--;
        }
        if (useExp) {
            // convert the exponent
            int i, j;
            *(c++) = 'e';
            if (m1 > 0) {
                *(c++) = '+';
            } else {
                *(c++) = '-';
                m1 = -m1;
            }
            m = 0;
            while (m1 > 0) {
                *(c++) = '0' + m1 % 10;
                m1 /= 10;
                m++;
            }
            c -= m;
            for (i = 0, j = m-1; i<j; i++, j--) {
                // swap without temporary
                c[i] ^= c[j];
                c[j] ^= c[i];
                c[i] ^= c[j];
            }
            c += m;
        }
        *(c) = '\0';
    }
    return s;
}
