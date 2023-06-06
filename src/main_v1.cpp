#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Encoder.h>

#define SCREEN_WIDTH 128 /*128 width of OLED in pixels*/
#define SCREEN_HEIGHT 64 /*64 height of OLED in pixels*/

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); /*OLED display connected at I2C pins (SDA, SCL)*/

Encoder myEnc(2, 6);
const int EncSW = 3;

void enterInterrupt();    //interrupt for the Encoder Button
bool enterState = false;  //
long oldPosition = -999;

const int buzzer = 8;

int prg_cnt = 0;

enum FANSPEED
{
    low,
    mid,
    high
};

void showBoot();

FANSPEED selectFanSpeed();
FANSPEED globalFanspeed = low;

int selectDuration();
int globalDuration = 30;

bool setAlarm();
bool globalAlarm = false;

void displaySetup()
{
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    { /*I2C Address at which OLED will communicate*/
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ;
    }

    display.clearDisplay();                 /*Clear display*/
    display.setTextSize(2);                 /*OLED screen text size defined*/
    display.setTextColor(WHITE);            /*OLED screen text color*/
}

void setup() 
{
    Serial.begin(9600);
    displaySetup();

    pinMode(EncSW, INPUT);
    attachInterrupt(digitalPinToInterrupt(EncSW), enterInterrupt, FALLING);

    pinMode(buzzer, OUTPUT);
    digitalWrite(buzzer, true);

}

void loop() 
{
    switch (prg_cnt)
    {
    case 0:
        //showBoot();
        prg_cnt = 1;
        break;

    case 1:
        globalFanspeed = selectFanSpeed();
        enterState = false;
        prg_cnt = 2;

    case 2:
        globalDuration = selectDuration();
        enterState = false;
        prg_cnt = 3;

    case 3:
        globalAlarm = setAlarm();
        enterState = false;
        prg_cnt = 1;
    
    default:
        break;
    }
}

void enterInterrupt()
{
    enterState = true;
    digitalWrite(buzzer, false);
    Serial.println("Interrupt:: Button pressed!");
    
    delay(1000);
    EIFR = (0b11 << INTF0);
    digitalWrite(buzzer, true);
    delay(2000);
}

void showBoot()
{

}

FANSPEED selectFanSpeed()
{
    FANSPEED fanSpeed = low;
    while(true)
    {
        long newPosition = (-myEnc.read()/4) % 3 + 1;

        if (newPosition != oldPosition)
        {
            oldPosition = newPosition;
            switch (newPosition)
            {
            case 1:
                Serial.println("FanSpeed:: Niedrig");

                display.clearDisplay(); /*Clear display*/
                display.setCursor(25, 24);
                display.println("Niedrig");
                display.display();
                fanSpeed = low;
                break;

            case 2:
                Serial.println("FanSpeed:: Mittel");

                display.clearDisplay(); /*Clear display*/
                display.setCursor(35, 24);
                display.println("Mittel");
                display.display();
                fanSpeed = mid;
                break;
            
            case 3:
                Serial.println("FanSpeed:: Hoch");

                display.clearDisplay(); /*Clear display*/
                display.setCursor(42, 24);
                display.println("Hoch");
                display.display();
                fanSpeed = high;
                break;
            
            default:
                break;
            }
        }
        if (enterState == true)
        {
            oldPosition = -999;
            return fanSpeed;
        }
    }
}

int selectDuration()
{
    myEnc.write(-4);
    long newPosition;

    while (true)
    {    
        newPosition = (-myEnc.read()/4) * 10;

        if (newPosition != oldPosition)
        {
            if(newPosition < 10)
            {
                newPosition = 10;
                myEnc.write(-4);
            }
            if(newPosition > 240)
            {
                newPosition = 240;
                myEnc.write(-96);   //-94 da 1 klick 4 Werte sind
            }
            oldPosition = newPosition;

            Serial.print("Duration:: ");
            Serial.println(newPosition);
            display.clearDisplay();   
            if(newPosition < 100)
            {
                display.setCursor(55, 24);
            }
            else
            {
                display.setCursor(44, 24);
            }

            display.println(newPosition);
            display.display();
        }
        
        if (enterState == true)
        {
            oldPosition = -999;
            return newPosition;
        }
    }
}

bool setAlarm()
{
    myEnc.write(0);
    long newPosition;
    bool alarm = false;

    while(true)
    {
        newPosition = (-myEnc.read()/4) % 2 + 1;

         if (newPosition != oldPosition)
        {
            oldPosition = newPosition;
            switch (newPosition)
            {
            case 1:
                Serial.println("ALARM:: OFF");

                display.clearDisplay();
                display.setCursor(50, 24);
                display.println("OFF");
                display.display();
                alarm = false;
                break;

            case 2:
                Serial.println("Alarm:: ON");
                display.clearDisplay(); 
                display.setCursor(55, 24);
                display.println("ON");
                display.display();
                alarm = true;
                break;
            
            default:
                break;
            }
        }
        if (enterState == true)
        {
            oldPosition = -999;
            return alarm;
        }
    }
}