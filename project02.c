/*
 * Project Name: Automatic Temperature Control System
 * MCU : PIC16F877A
 * IDE: mikroC PRO
 * Author: Mahamudul Hasan
 * Email: hasaneee717@gmail.com
*/

// Keypad module connections
char  keypadPort at PORTD;

// LCD module connections
sbit LCD_RS at RB4_bit;
sbit LCD_EN at RB5_bit;
sbit LCD_D4 at RB3_bit;
sbit LCD_D5 at RB2_bit;
sbit LCD_D6 at RB1_bit;
sbit LCD_D7 at RB0_bit;

sbit LCD_RS_Direction at TRISB4_bit;
sbit LCD_EN_Direction at TRISB5_bit;
sbit LCD_D4_Direction at TRISB3_bit;
sbit LCD_D5_Direction at TRISB2_bit;
sbit LCD_D6_Direction at TRISB1_bit;
sbit LCD_D7_Direction at TRISB0_bit;
// End LCD module connections

#define HEATER PORTC.F0
#define FAN PORTC.F1
#define ON 1
#define OFF 0
#define CLEAR 13
#define ENTER 15

void main() {
    unsigned short kp, text[12];
    unsigned short tempRef = 0;
    unsigned char inTemp;
    unsigned int temp;
    float mV, actualTemp;

    //ADCON1 = 0x0F;
    TRISC.F0 = 0;
    TRISC.F1 = 0;
    TRISE.F1 = 1;

    Lcd_Init();                           // Initialize LCD
    Keypad_Init();                        // Initialize keyPad

    Lcd_Cmd(_LCD_CLEAR);               // Clear display
    Lcd_Cmd(_LCD_CURSOR_OFF);          // Cursor off
    Lcd_Out(1, 1, "Automatic");
    Lcd_Out(2, 5, "Temp Control");
    Delay_ms(500);

    HEATER = OFF;
    FAN = OFF;

    START:
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Out(1, 1, "Enter Temp Ref");
    //tempRef = 0;
    Lcd_Out(2, 1, "Ref Temp: ");

    while(1){
        do
        kp = Keypad_Key_Click();
        while(!kp);
        if(kp == ENTER) break;
        if(kp>3 && kp <8) kp = kp-1;
        if(kp>8 && kp <12) kp = kp-2;
        if(kp == 14) kp = 0;
        if(kp == CLEAR) goto START;
        Lcd_Chr_Cp(kp + '0');
        tempRef = (tempRef*10) + kp;
    }
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Out(1, 1, "Ref Temp: ");
    intToStr(tempRef, text);
    inTemp = Ltrim(text);
    Lcd_Out_Cp(inTemp);
    Lcd_Out(2, 1, "Press # to Cont..");
    
    kp = 0;
    while(kp != ENTER){
        do{
            kp = Keypad_Key_Click();
        } while(!kp);
    }
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Out(1, 1, "Ref Temp: ");
    //Lcd_Chr(1, 12, 176);
    Lcd_Chr(1, 13, 'C');
    
    ADC_Init();
    while(1){
        temp = ADC_Read(6);
        actualTemp = (temp * 5000.0) / 1024.0;
        actualTemp = actualTemp / 10.0;
        intToStr(tempRef, text);
        inTemp = Ltrim(text);
        Lcd_Out(1, 11, inTemp);
        Lcd_Out(2, 1, "Temp: ");
        FloatToStr(actualTemp, text);
        text[4] = 0;
        Lcd_Out(2, 7, text);
        
        if(tempRef > actualTemp){
            HEATER = ON;
            FAN = OFF;
        }
        if(tempRef < actualTemp){
            HEATER = OFF;
            FAN = ON;
        }
        if(tempRef == actualTemp){
            HEATER = OFF;
            FAN = OFF;
        }
        Delay_ms(500);
    }
}