#include <wiringPi.h>
#include <time.h>
#include <stdio.h>
#include "ifttt.h"

void ALARM_OFF();
void ALARM_ARMING();
void ALARM_ARMED();
void ALARM_TRIGGERED();
void ALARM_SOUNDING();

/*This is an alarm system that sends the user an email when the alarm gets triggered*/

int main()
{
	wiringPiSetup();
	pinMode (1, OUTPUT);/*LED 1*/
	pinMode (0, OUTPUT); /*LED 2*/
	pinMode (4, OUTPUT); /*Buzzer*/
	pinMode (2, INPUT); /*Button*/
	pullUpDnControl(2, PUD_UP);
	
	ALARM_OFF();
	
	for(;;) 
	{ 
		int x = digitalRead(2);				    		                if(x == 0)						       
		{ 									        	ALARM_ARMING(); 							} 								        }
	return 0;
}

/*LED1 on, LED2 off, buzzer silent. If button pressed call ALARM_ARMING*/
void ALARM_OFF()
{        	
	printf("ALARM OFF");
	digitalWrite(1, HIGH);
	digitalWrite(0, LOW);
	digitalWrite(4, LOW);	
	
}

/*The system waits to go live. Buzzer silent, for 10 seconds LED 1 blinks on and off for 1 second duration. At the end of the 10 seconds, LED 1 turns off and turns LED2 on. The system then calls ALARM_ARMED*/
void ALARM_ARMING()
{
	time_t endTime = time(NULL) + 10;
	printf("Arming alarm");
	
	while(time(NULL) <= endTime)
	{
		digitalWrite(1, HIGH); delay(1000);
		digitalWrite(1, LOW); delay(1000);
	}
	
	ALARM_ARMED();

}

/*LED 1 is off, LED 2 is on. The buzzer is silent. Remains in ALARM_ARMED mode until: the button is pressed(call ALARM_OFF), or the alarm is triggered(call ALARM_TRIGGERED). */
void ALARM_ARMED()
{
	digitalWrite(1, LOW);
	digitalWrite(0, HIGH);
	
	for(;;)
	{
		if(digitalRead(2) == 0)
		{
			ALARM_OFF();
			while(digitalRead(2) == 0);	
			break;
		}
		if(digitalRead(7) == 1)
		{
			ALARM_TRIGGERED();
			break;
		}
	}	
}

/*An intruder has been detected and the user has 10 seconds to disable the alarm. The buzzer is silent and two LEDs blink on and off once every 2 seconds. This continues for 10 seconds. If during the 10 seconds the button is pressed, the system is deactivated and calls ALARM_OFF. If the button is not pressed, call ALARM_SOUNDING*/
void ALARM_TRIGGERED()
{
	time_t endTime = time(NULL) + 10;
	int disabled = 0;

	while(time(NULL) <= endTime)
	{
		digitalWrite(1, HIGH);
		digitalWrite(0, HIGH); delay(2000);
		digitalWrite(1, LOW);
		digitalWrite(0, LOW); delay(2000);
		if(digitalRead(2) == 0)
		{
			disabled = 1;
			ALARM_OFF();
			while(digitalRead(2) == 0);
		}	

	}
	if(disabled == 0)
	{
		ALARM_SOUNDING();
	}
}
/*The buzzer and 2 LEDs are on for 2 seconds, then off for 2 seconds and a single alarm message is sent to the IFTTT trigger. The system remains in this state until btn is pressed then calls ALARM_OFF*/
void ALARM_SOUNDING()
{
	int disabled = 0; 
	system("curl -X POST https://maker.ifttt.com/trigger/alarm_triggered/with/key/cZpdqyVIgxsOoM4ckMAgqp");
	while (disabled == 0)
	{
		digitalWrite(1, HIGH);
		digitalWrite(0, HIGH);
		digitalWrite(4, HIGH); delay(2000);
		digitalWrite(1, LOW);
		digitalWrite(0, LOW);
		digitalWrite(4, LOW); delay(2000);

		if(digitalRead(2) == 0)
		{
			disabled = 1;
		}

		ALARM_OFF();
		while(digitalRead(2) == 0);
	}
}
