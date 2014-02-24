#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "driverlib/pin_map.h"

#include "STELLARIS_AD9850.h"

#define BUTTON_1 GPIO_PIN_0
#define BUTTON_2 GPIO_PIN_4

void switch_config(void){ //Configures PF0 and PF4 as inputs (onboard switches)
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY_DD;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, BUTTON_1|BUTTON_2);
	GPIOPadConfigSet(GPIO_PORTF_BASE, BUTTON_1|BUTTON_2, GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
}

int main(void){

	//50MHz clock
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
	switch_config();

	//AD9850 config
	SysCtlPeripheralEnable(PORT_ENABLE);
	GPIOPinTypeGPIOOutput(PORT, W_CLK | FQ_UD | DATA | RESET);
	AD9850_Init();
	AD9850_Reset();

	//Start DDS operation at 1000Hz
	double freq = 1000;
	AD9850_Osc(freq, 0);

	while(1){
		//if button 1 is pressed, decrease frequency
		if(!GPIOPinRead(GPIO_PORTF_BASE, BUTTON_1 && freq > 0 )){
			freq--;
			AD9850_Osc(freq, 0);
		}
		//if button 2 is pressed, increase frequency
		if(!GPIOPinRead(GPIO_PORTF_BASE, BUTTON_2)){
			freq++;
			AD9850_Osc(freq, 0);
		}

		//delay between input checks
		SysCtlDelay(20000);
	}
}
