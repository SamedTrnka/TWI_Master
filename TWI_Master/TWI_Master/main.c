/*
 * TWI_Master.c
 *
 * Created: 16.02.2023 08:25:56
 * Author : avm
 */ 

#define F_CPU 3686400UL
#include <avr/io.h>
#include <util/delay.h>

uint8_t TWI_Start()
{
	TWCR=0b10100100; //TWINT=1, TWSTA=1, TWEN=1
	while(!(TWCR&(1<<TWINT))); //warte auf TWINT=1
	return(TWSR&0xf8); //Status zurückgeben
	
}

uint8_t TWI_SLA_W()
{
	TWDR=0b00000010; //Slave-Adresse 0x02+W
	TWCR=0b10000100; //TWINT=1, TWEN=1
	while(!(TWCR&(1<<TWINT))); //warte auf TWINT=1
	return(TWSR&0xf8); //Status zurückgeben
}

uint8_t TWI_Write(uint8_t data)
{
	TWDR = data;
	TWCR = (1<<TWINT)|(1<<TWEN);
	while(!(TWCR&(1<<TWINT))); //warte auf TWINT=1
	return TWDR; //Status zurückgeben
}
int main(void)
{
	DDRC=0x00;								//PORTC zur Eingabe
	PORTC|=(1<<PC4)|(1<<PC5);				//Pull-Ups für PORTC
	DDRD=0x00;								//PORTD zur Eingabe
	PORTD|=(1<<PD2);						//Pull-Ups für PORTD
	DDRB|=(1<<PB0);							//PB0 zur Ausgabe
	TWBR=0x0a;								//Bitratenregister laden
	TWSR=0x00;								//Statusregister, Vorteiler=1
	uint8_t fm=0x00;						//Bit0 = Flankenmerker
	PORTB &=~(1<<PB0);						//Fehleranzeige aus
	while(1)								//wiederhole immer
	{
		if(!(PIND&(1<<PD2)))				//PD2 = 0 ?
		{
			if(!(fm&0x01))					//Flankenmerker=0 ?
			{
				fm|=(1<<0);					//Flankenmerker setzen
				_delay_ms(50);				//Prellzeit
				TWI_Start();
				TWI_Write(0b00000010);
				TWI_Write(0x01);
				TWI_Write(0x01);
				TWCR=0b10010100;			//STOP - TWINT=1, TWSTO=1, TWEN=1
			}
		}
		else								//wenn PC0 =1
		{
			if(fm&0x01)						//Flankenmerker=1 ?
			{
				fm&=~(1<<0); //Flankenmerker zurücksetzen
				_delay_ms(50); //Prellzeit
			}
		}
	}
}
