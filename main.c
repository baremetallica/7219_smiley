/*
 * Bare metal programming for STM32F407G-DISC
 * Peripheral D
 * Slave- MAX7219CNG
 * Protocol- SPI
 */

#include <stdint.h>
#define CLK 10
#define CS 11
#define DIN 12

uint32_t* enableClock = (uint32_t*)0x40023830;
uint32_t* pd_mode = (uint32_t*)0x40020C00;
uint32_t* pd_data = (uint32_t*)0x40020C14;

uint16_t smiley[]=
{
		0x3c,0x42,0xa5,0x81,0xa5,0x99,0x42,0x3c
};

uint16_t sad_smiley[]=
{
		0x3c,0x42,0xa5,0x81,0x99,0xa5,0x42,0x3c
};

void send_cmd(uint16_t cmd, uint16_t data) {
	uint16_t rc_data = (cmd << 8) | data;
	uint16_t data_b = 1;

	*pd_data &= (~(1<<CS)); // Set chip select to low
	*pd_data &= (~(1<<CLK)); // set clock to low

	for(int i = 0; i<16;++i)
	{
		uint16_t data_mask = 0x8000;
		data_b = 1;
		data_b = (data_mask & rc_data) >> 15;
		rc_data = rc_data << 1;

		uint32_t dat = *pd_data;
		switch(data_b)
		{
			case 0:
				dat &= (~(1<<DIN)); // calc the data pin
				break;
			case 1:
				dat |= (1<<DIN); // calc the data pin
				break;
		}
		*pd_data = dat; //Set the DIN before CLK

		dat |= (1<<CLK); // Set the clock
		*pd_data = dat;

		dat &= (~(1<<CLK)); // set the clock
		*pd_data = dat;
	}
	*pd_data |= (1<<CS); // Set chip select to high
}

void delay()
{
	for(int i = 0;i<800000;++i);
}
void clearAllBits()
{
	for(int i = 1; i < 9; ++i)
	{
		send_cmd(i, 0x0);
	}
}

void showHappyFace()
{
	for(int i = 1; i < 9; ++i)
	{
		send_cmd(i, smiley[i-1]);
	}
}

void showSadFace()
{
	for(int i = 1; i < 9; ++i)
	{
		send_cmd(i, sad_smiley[i-1]);
	}
}

int main(void)
{
	*enableClock |=0x08; // enable clock for D group of peripherals

	*pd_mode |= ( 1 << (CLK << 1)); // set clock pin as output
	*pd_mode |= ( 1 << (CS << 1)); // set chip select pin as output
	*pd_mode |= ( 1 << (DIN << 1)); // set data pin as output

	send_cmd(0xC, 0x01); // Disable shutdown mode
	clearAllBits();

	for(;;)
	{
		showHappyFace();
		delay();
		showSadFace();
		delay();
	}
}
