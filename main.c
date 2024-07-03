// Header files
#include <p18f4620.h>                                                                   // Functions for PIC18F4620 microcontroller
#include <stdio.h>                                                                      // Standard I/O functions
#include <math.h>                                                                       // Math library functions
#include <usart.h>                                                                      // USART functions
#include <stdlib.h>                                                                     // Standard library functions
#include <xc.h>                                                                         // XC8 compiler functions
#include <string.h>                                                                     // String manipulation functions

#include "ST7735_TFT.h"                                                                 // Functions for ST7735 TFT display
#include "main.h"                                                                       // Project-specific definitions

// Configuration settings
#pragma config OSC = INTIO67                                                            // Internal clock
#pragma config WDT=OFF                                                                  // Watchdog Timer OFF
#pragma config LVP=OFF                                                                  // Low-Voltage Programming OFF
#pragma config BOREN =OFF                                                               // Brown-Out Reset OFF

// Function Prototypes
unsigned int Get_full_ADC(void);                                                        //
void Init_ADC(void);                                                                    //
void Init_TRIS(void);                                                                   //
void Init_UART(void);                                                                   //
void putch (char);                                                                      //
void Select_ADC_Channel(char ch);                                                       //
void Set_NS(char color);                                                                //
void Set_NS_LT(char color);                                                             //
void Set_EW(char color);                                                                //
void Set_EW_LT(char color);                                                             //
void PED_Control( char Direction, char Num_Sec);                                        //
void Day_Mode();                                                                        //
void Night_Mode();                                                                      //
void Wait_One_Second();                                                                 //
void Wait_Half_Second();                                                                //
void Wait_N_Seconds (char);                                                             //
void Wait_One_Second_With_Beep();                                                       //
void update_LCD_color(char , char );                                                    //
void update_LCD_PED_Count(char direction, char count);                                  //
void Initialize_Screen();                                                               //
void update_LCD_misc();                                                                 //
void update_LCD_count(char, char);                                                      //
void Get_Light_Readout();                                                               //
                                                                                        // Lab 9 New Function Prototypes
void Do_Init();                                                                         //
void interrupt high_priority chkisr();                                                  //
void INT0_ISR();                                                                        //
void INT1_ISR();                                                                        //
void INT2_ISR();                                                                        //
Do_Flashing();                                                                          //
                                                                                        // End of Function Prototypes

#define _XTAL_FREQ  8000000                                                             // Set operation for 8 Mhz
#define TMR_CLOCK   _XTAL_FREQ/4                                                        // Timer Clock 2 Mhz

// Constant Initializations
//colors
#define OFF             0                                                               // Defines OFF as decimal value 0
#define RED             1                                                               // Defines RED as decimal value 1
#define GREEN           2                                                               // Defines GREEN as decimal value 2
#define YELLOW          3                                                               // Defines YELLOW as decimal value 3

#define EW              0                                                               // Number definition of East/West
#define EWLT            1                                                               // Number definition of East/West Left Turn
#define NS              2                                                               // Number definition of North/South
#define NSLT            3                                                               // Number definition of North/South Left Turn

#define Circle_Size     7                                                               // Size of Circle for Light
#define Circle_Offset   15                                                              // Location of Circle
#define TS_1            1                                                               // Size of Normal Text
#define TS_2            2                                                               // Size of PED Text
#define Count_Offset    10                                                              // Location of Count

#define XTXT            30                                                              // X location of Title Text 
#define XRED            40                                                              // X location of Red Circle
#define XYEL            60                                                              // X location of Yellow Circle
#define XGRN            80                                                              // X location of Green Circle
#define XCNT            100                                                             // X location of Sec Count

#define Color_Off       0                                                               // Number definition of Off Color
#define Color_Red       1                                                               // Number definition of Red Color
#define Color_Green     2                                                               // Number definition of Green Color
#define Color_Yellow    3                                                               // Number definition of Yellow Color

#define EW_Txt_Y        20                                                              // Number definition of EW_Txt_Y
#define EW_Cir_Y        EW_Txt_Y + Circle_Offset                                        // Number definition of EW_Cir_Y
#define EW_Count_Y      EW_Txt_Y + Count_Offset                                         // Number definition of EW_Count_Y
#define EW_Color        ST7735_CYAN                                                     // Color definition of EW_Color 

#define EWLT_Txt_Y      50                                                              // Number definition of EWLT_Txt_Y
#define EWLT_Cir_Y      EWLT_Txt_Y + Circle_Offset                                      // Number definition of EWLT_Cir_Y
#define EWLT_Count_Y    EWLT_Txt_Y + Count_Offset                                       // Number definition of EWLT_Count_Y
#define EWLT_Color      ST7735_WHITE                                                    // Color definition of EWLT_Color

#define NS_Txt_Y        80                                                              // Number definition of NS_Txt_Y
#define NS_Cir_Y        NS_Txt_Y + Circle_Offset                                        // Number definition of NS_Cir_Y
#define NS_Count_Y      NS_Txt_Y + Count_Offset                                         // Number definition of NS_Count_Y
#define NS_Color        ST7735_BLUE                                                     // Color definition of NS_Color

#define NSLT_Txt_Y      110                                                             // Number definition of NSLT_Txt_Y
#define NSLT_Cir_Y      NSLT_Txt_Y + Circle_Offset                                      // Number definition of NSLT_Cir_Y
#define NSLT_Count_Y    NSLT_Txt_Y + Count_Offset                                       // Number definition of NSLT_Count_Y
#define NSLT_Color      ST7735_MAGENTA                                                  // Color definition of NSLT_Color

#define PED_EW_Count_Y  30		                                                        // Number definition of PED_EW_Count_Y
#define PED_NS_Count_Y  90		                                                        // Number definition of PED_NS_Count_Y
#define PED_Count_X     2		                                                        // Number definition of PED_Count_X
#define Switch_Txt_Y    140		                                                        // Number definition of Switch_Txt_Y

#define PED_Count_NS    8		                                                        // Ped counter for NS
#define PED_Count_EW    9                                                               // Ped counter for EW


char buffer[31];                                                                        // General buffer for display purpose
char *nbr;                                                                              // General pointer used for buffer
char *txt;                                                                              // General pointer used for text

char EW_Count[]     = "00";                                                             // Text storage for EW Count
char EWLT_Count[]   = "00";                                                             // Text storage for EW Left Turn Count
char NS_Count[]     = "00";                                                             // Text storage for NS Count
char NSLT_Count[]   = "00";                                                             // Text storage for NS Left Turn Count

char PED_EW_Count[] = "00";                                                             // Text storage for EW Pedestrian Count
char PED_NS_Count[] = "00";                                                             // Text storage for NS Pedestrian Count

char SW_NSPED_Txt[] = "0";                                                              // Text storage for NS Pedestrian Switch
char SW_NSLT_Txt[]  = "0";                                                              // Text storage for NS Left Turn Switch
char SW_EWPED_Txt[] = "0";                                                              // Text storage for EW Pedestrian Switch
char SW_EWLT_Txt[]  = "0";                                                              // Text storage for EW Left Turn Switch
char SW_MODE_Txt[]  = "D";                                                              // Text storage for Mode Light Sensor
	
char Act_Mode_Txt[]  = "D";                                                             // Text storage for Actual Mode
char FlashingS_Txt[] = "0";                                                             // Text storage for Flashing Status
char FlashingR_Txt[] = "0";                                                             // Text storage for Flashing Request

char dir;                                                                               // RAM variable for dir
char Count;                                                                             // RAM variable for Second Count
char PED_Count;                                                                         // RAM variable for Second Pedestrian Count

char SW_NSPED;                                                                          // RAM variable for NS Pedestrian Switch
char SW_NSLT;                                                                           // RAM variable for NS Left Turn Switch
char SW_EWPED;                                                                          // RAM variable for EW Pedestrian Switch
char SW_EWLT;                                                                           // RAM variable for EW Left Turn Switch
char SW_MODE;                                                                           // RAM variable for Mode Light Sensor
int MODE;                                                                               // Integer value for MODE
char direction;                                                                         // Character value for direction
float volt;                                                                             // Float value for volt (includes decimals)
int nStep;                                                                              // Integer value for nStep

// Lab 9 New Variables
int INT0_flag, INT1_flag, INT2_flag = 0;                                                // Variable Initializations for flags set to the value zero
char FLASHING = 0;                                                                      // Variable Initialization set to zero, variable used in Do_Flashing() method)
char FLASHING_REQUEST = 0;                                                              // Variable Initialization set to the value zero, used to determine to execution
                                                                                        // of Do_Flashing() method

// *************************
// Main Function
// *************************
void main()                                                                             // Beginning of main function
{
    Do_Init();                                                                          // Initialization 
    Init_TRIS();                                                                        // Initialize TRIS/ports
    Initialize_Screen();                                                                // Initialize the TFT screen
    // Get voltage from the photo resistor
    Select_ADC_Channel(0);                                                              // Set value of ADC_channel to 0, AN0
    Get_Light_Readout();                                                                // Performs or calls to Get_Light_Readout()
    SW_MODE = volt < 3.5 ? 1:0;                                                         // Mode = 1, Day_mode, Mode = 0 Night_mode  
    
    // Part 5
    while(1)                                                                            // Forever loop
    {   
        if (SW_MODE)                                                                    // if the variable volt has a value less than 3.5
        {
            Day_Mode();                                                                 // Execute the function Day_Mode()
            if (FLASHING_REQUEST)                                                       // Execute "if block" if variable has the value of 1
            {
                FLASHING_REQUEST = 0;                                                   // Clear the variable value to zero
                Do_Flashing();                                                          // Call the Do_Flashing() method 
            }
        }
        else                                                                            // else the value of volt is greater than 3.5
        {
            // Lab 9 New Code
            NS_PED_SW = 0;                                                              // Assign the value zero for both variables to prevent any
            EW_PED_SW = 0;                                                              // pedestrian process to be handled in night mode    
            Night_Mode();                                                               // Execute the function Night_Mode()
            if (FLASHING_REQUEST)                                                       // Execute "if block" if variable has the value of 1
            {
                FLASHING_REQUEST = 0;                                                   // Clear the variable value to zero
                Do_Flashing();                                                          // Call the Do_Flashing() method 
            }
        }
    }
}

// *************************
// Function Definitions
// *************************
void putch (char c)                                                                     // Routine program for putch
{
    while (!TRMT);                                                                      // While not TMRT
    TXREG = c;                                                                          // TXREG equals value of character, c
}

void Init_UART()                                                                        // Routine program for UART
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &
    USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX &
    USART_BRGH_HIGH, 25);
    OSCCON = 0x60;                                                                      // 0110 0000, sleep mode, 4MHz, OST time-out running, 
                                                                                        // INTOSC frequency not stable, Primary oscillator
}

void Init_ADC(void)                                                                     // General routine to program ADC
{
    ADCON0 = 0x01;                                                                      // Enable A/D converter
    ADCON1=0x0E;                                                                        // Select pins AN0 through AN3 as analog signal, VDD-VSS as 
                                                                                        // reference voltage 
    ADCON2=0xA9;                                                                        // Right justify the result. Set the bit conversion time (TAD) and 
                                                                                        // acquisition time 
}

unsigned int Get_full_ADC(void)                                                         // Routine program to obtain ADC pins
{ 
    int result;                                                                         // Initialize result
    ADCON0bits.GO=1;                                                                    // Start Conversion 
    while(ADCON0bits.DONE==1);                                                          // Wait for conversion to be completed 
    result = (ADRESH * 0x100) + ADRESL;                                                 // Combine result of upper byte and 
                                                                                        // lower byte into result 
    return result;                                                                      // Return the result. 
}

void Select_ADC_Channel(char channel)                                                   // Routine program to select ADC channel
{ 
    ADCON0 = channel * 4 + 1;                                                           // Stores result of channel multiplied by 4 plus 1 in ADCON0
}

void Init_TRIS(void)                                                                    // Program routine for ports
{
    TRISA = 0x1F;                                                                       // Inputs RA0/AN0-RA4 and RA5 as output
    //TRISB = 0x07;                                                                       // Make PORTB as all outputs
    TRISC = 0x00;                                                                       // Make PORTC as all outputs
    TRISD = 0x00;                                                                       // Make PORTD as all outputs
    TRISE = 0x00;                                                                       // Make PORTE as all outputs
}

// Part 1
void Wait_Half_Second()                                                                 // Routine program to delay for half a second
{
    T0CON = 0x03;                                                                       // Timer 0, 16-bit mode, prescaler 1:16
    TMR0L = 0xDB;                                                                       // Set the lower byte of TMR
    TMR0H = 0x0B;                                                                       // Set the upper byte of TMR
    INTCONbits.TMR0IF = 0;                                                              // Clear the Timer 0 flag
    T0CONbits.TMR0ON = 1;                                                               // Turn on the Timer 0
    while (INTCONbits.TMR0IF == 0);                                                     // Wait for the Timer Flag to be 1 for done
    T0CONbits.TMR0ON = 0;                                                               // Turn off the Timer 0
}

void Wait_One_Second()                                                                  // Routine program to delay for one second
{
    SEC_LED = 1;                                                                        // First, turn on the SEC_LED
    strcpy(txt,"*");                                                                    // Copy asterisk to variable txt
    drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);                                // Display variable txt to LCD
    Wait_Half_Second();                                                                 // Wait for half second (or 500 msec)

    SEC_LED = 0;                                                                        //Then turn off the SEC_LED
    strcpy(txt," ");                                                                    // Copy space string to variable txt
    drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);                                // Display variable txt to LCD
    Wait_Half_Second();                                                                 // Wait for half second (or 500 msec)
    update_LCD_misc();                                                                  // Performs or refers to update_LCD_misc()
}

// Part 2
void Wait_N_Seconds (char seconds)                                                      // Routine program to delay for the number of seconds specified
{
    char I;                                                                             // Declares I variable of type char
    for (I = seconds; I > 0; I--)                                                       // For loop to decrement character I, until 0
    {
        update_LCD_count(direction, I);                                                 // Obtains direction and I values to update the LCD
        Wait_One_Second();                                                              // Calls Wait_One_Second for x number of times
    }
    update_LCD_count(direction, 0);                                                     // Obtains direction and zero value to update the LCD
}

// Part 3
void Set_NS(char color)                                                                 // Routine program to set color for NS
{
    direction = NS;                                                                     // Direction is going NS
    update_LCD_color(direction, color);                                                 // Obtains direction and color values to update the LCD
    switch (color)                                                                      // Switch statement with color as argument
    {
        case OFF: NS_RED =0; NS_GREEN=0; break;                                         // Turns off the NS LED
        case RED: NS_RED =1; NS_GREEN=0; break;                                         // Sets NS LED RED
        case GREEN: NS_RED =0; NS_GREEN=1; break;                                       // Sets NS LED GREEN
        case YELLOW: NS_RED =1; NS_GREEN=1; break;                                      // Sets NS LED YELLOW
    }
}

void Set_NS_LT(char color)                                                              // Routine program to set color for NSLT
{
    direction = NSLT;                                                                   // Direction is going NSLT
    update_LCD_color(direction, color);                                                 // Obtains direction and color values to update the LCD                     // Obtains direction and color values to update the LCD
    switch (color)                                                                      // Switch statement with color as argument
    {
        case OFF: NSLT_RED =0; NSLT_GREEN=0; break;                                     // Turns off the NSLT LED
        case RED: NSLT_RED =1; NSLT_GREEN=0; break;                                     // Sets NSLT LED RED
        case GREEN: NSLT_RED =0; NSLT_GREEN=1; break;                                   // Sets NSLT LED GREEN
        case YELLOW: NSLT_RED =1; NSLT_GREEN=1; break;                                  // Sets NSLT LED YELLOW
    }
}

void Set_EW(char color)                                                                 // Routine program to set color for EW
{
    direction = EW;                                                                     // Direction is going EW
    update_LCD_color(direction, color);                                                 // Obtains direction and color values to update the LCD
    switch (color)                                                                      // Switch statement with color as argument
    {
        case OFF: EW_RED =0; EW_GREEN=0; break;                                         // Turns off the EW LED
        case RED: EW_RED =1; EW_GREEN=0; break;                                         // Sets EW LED RED
        case GREEN: EW_RED =0; EW_GREEN=1; break;                                       // Sets EW LED GREEN
        case YELLOW: EW_RED =1; EW_GREEN=1; break;                                      // Sets EW LED YELLOW
    }
}

void Set_EW_LT(char color)                                                              // Routine program to set color for EWLT
{
    direction = EWLT;                                                                   // Direction is going EWLT
    update_LCD_color(direction, color);                                                 // Obtains direction and color values to update the LCD
    switch (color)                                                                      // Switch statement with color as argument
    {
        case OFF: EWLT_RED =0; EWLT_GREEN=0; break;                                     // Turns off the EWLT LED
        case RED: EWLT_RED =1; EWLT_GREEN=0; break;                                     // Sets EWLT LED RED
        case GREEN: EWLT_RED =0; EWLT_GREEN=1; break;                                   // Sets EWLT LED GREEN
        case YELLOW: EWLT_RED =1; EWLT_GREEN=1; break;                                  // Sets EWLT LED YELLOW
    }
}

// Part 4
void Activate_Buzzer()                                                                  // Routine program to activate buzzer noise/sound
{
    PR2 = 0b11111001 ;                                                                  // PR2 equals 0xF9 (or D?249) number in binary
    T2CON = 0b00000101 ;                                                                // T2CON equals 0x05 (or D?5) number in binary
    CCPR2L = 0b01001010 ;                                                               // CCPR2L equals 0x4A (or D?74) number in binary
    CCP2CON = 0b00111100 ;                                                              // CCP2CON equals 0x3C (or D?60) number in binary
}

void Deactivate_Buzzer()                                                                // Routine program to deactivate buzzer noise/sound
{
    CCP2CON = 0x0;                                                                      // CCPR2CON equals 0
    PORTCbits.RC1 = 0;                                                                  // PORTC bit 1 is set to 0
}

void Wait_One_Second_With_Beep()                                                        // Routine program to wait one second with buzzer sound/beep
{
    SEC_LED = 1;                                                                        // First, turn on the SEC_LED
    strcpy(txt,"*");                                                                    // Copy asterisk to variable txt
    drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);                                // Display variable txt to LCD
    Activate_Buzzer();                                                                  // Call function to activate buzzer
    Wait_Half_Second();                                                                 // Wait for half second (or 500 msec)

    SEC_LED = 0;                                                                        // Then turn off the SEC_LED
    strcpy(txt," ");                                                                    // Copy space string to variable txt
    drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);                                // Display variable txt to LCD
    Deactivate_Buzzer();                                                                // Call function to deactivate buzzer
    Wait_Half_Second();                                                                 // Wait for half second (or 500 msec)
    update_LCD_misc();                                                                  // Execute function call to update LCD
}

void PED_Control(char Direction, char Num_Sec)                                          // Routine program for PED_Control with two arguments
{
    for(char Sec = Num_Sec - 1; Sec > 0; Sec--)                                         // Start of for loop
    {
        update_LCD_PED_Count(Direction, Sec);                                           // Execute function with given two arguments
        Wait_One_Second_With_Beep();                                                    // Call function to wait one second with beep sound
    }
    
    update_LCD_PED_Count(Direction, 0);                                                 // Execute function with given two arguments
    Wait_One_Second_With_Beep();                                                        // Call function to wait one second with beep sound
    // Lab 9 New Step
    if (Direction == NS)                                                                // Evaluates the conditional statement to True or False
    {
        NS_PED_SW = 0;                                                                  // If the conditional statement is True, assign the value zero 
    }
    else                                                                                // Executes this else block if the conditional statement is False
    {
        EW_PED_SW = 0;                                                                  // Assign the value zero to the variable
    }
}

// Part 5
void Day_Mode()                                                                         // Routine program for Day_Mode
{
    printf ("Day Mode \r\n");                                                           // Writes out "Day Mode"
    MODE = 1;                                                                           // Traffic controller running to 1
    MODE_LED = 1;                                                                       // Day mode activated
    Act_Mode_Txt[0] = 'D';                                                              // Display "D" for day mode on LCD
    
    // Step 1
    Set_EW(RED);                                                                        // Set D3 RGB to RED
    Set_EW_LT(RED);                                                                     // Set D4 RGB to RED
    Set_NS_LT(RED);                                                                     // Set D2 RGB to RED
    Set_NS(GREEN);                                                                      // Set D1 RGB to GREEN
    
    // Step 2
    if (NS_PED_SW)                                                                      // Execute this block if NSPED_SW
    {
        PED_Control(NS, 7);                                                             // Function call with arguments 0 and 7 respectively
    }
    
    // Step 3
    Wait_N_Seconds(6);                                                                  // Execute function call to wait 6 seconds
    
    // Step 4
    Set_NS(YELLOW);                                                                     // Set D1 RGB to YELLOW
    Wait_N_Seconds(3);                                                                  // Execute function call to wait 3 seconds
    
    // Step 5
    Set_NS(RED);                                                                        // Set D1 RGB to RED
    
    // Step 6
    if (EW_LT_SW)                                                                       // Execute this block if EWLT_SW
    {
        // Step 7
        Set_EW_LT(GREEN);                                                               // Set D4 RGB to GREEN
        Wait_N_Seconds(8);                                                              // Execute function call to wait 8 seconds
        
        // Step 8
        Set_EW_LT(YELLOW);                                                              // Set D4 RGB to YELLOW
        Wait_N_Seconds(3);                                                              // Execute function call to wait 3 seconds
        
        // Step 9
        Set_EW_LT(RED);                                                                 // Set D4 RGB to RED
    }
    
    // Step 10
    Set_EW(GREEN);                                                                      // Set D3 RGB to RED
    if (EW_PED_SW)                                                                      // Execute this block if EWPED_SW
    {
        PED_Control(EW, 8);                                                             // Function call with arguments 1 and 8 respectively
    }
    
    // Step 11
    Set_EW(GREEN);                                                                      // Set D3 RGB to GREEN
    Wait_N_Seconds(7);                                                                  // Execute function call to wait 7 seconds
    
    // Step 12
    Set_EW(YELLOW);                                                                     // Set D3 RGB to YELLOW
    Wait_N_Seconds(3);                                                                  // Execute function call to wait 3 seconds
    
    // Step 13
    Set_EW(RED);                                                                        // Set D3 RGB to RED
    
    // Step 14
    if (NS_LT_SW)                                                                       // Execute this block if NSLT_SW
    {
        // Step 15
        Set_NS_LT(GREEN);                                                               // Set D2 RGB to GREEN
        Wait_N_Seconds(8);                                                              // Execute function call to wait 8 seconds
        
        // Step 16
        Set_NS_LT(YELLOW);                                                              // Set D2 RGB to YELLOW
        Wait_N_Seconds(3);                                                              // Execute function call to wait 3 seconds
        
        // Step 17
        Set_NS_LT(RED);                                                                 // Set D2 RGB to RED
    }
}

void Night_Mode()                                                                       // Routine program for Night_Mode
{
    printf ("Night Mode \r\n");                                                         // Writes out "Night Mode"
    MODE = 0;                                                                           // Traffic controller running to 0
    MODE_LED = 0;                                                                       // Night mode activated
    Act_Mode_Txt[0] = 'N';                                                              // Display "N" for night mode on LCD
    
    // Step 1
    Set_EW(RED);                                                                        // Set D3 RGB to RED
    Set_EW_LT(RED);                                                                     // Set D4 RGB to RED
    Set_NS_LT(RED);                                                                     // Set D2 RGB to RED
    Set_NS(GREEN);                                                                      // Set D1 RGB to GREEN
    
    // Step 2
    Wait_N_Seconds(5);                                                                  // Execute function call to wait 5 seconds
    
    // Step 3
    Set_NS(YELLOW);                                                                     // Set D1 RGB to YELLOW
    Wait_N_Seconds(3);                                                                  // Execute function call to wait 3 seconds
    
    // Step 4
    Set_NS(RED);                                                                        // Set D1 RGB to RED
    
    // Step 5
    if (EW_LT_SW == 1)                                                                  // Execute this if block if EWLT_SW is equal to one
    {
        // Step 6
        Set_EW_LT(GREEN);                                                               // Set D4 RGB to GREEN
        Wait_N_Seconds(5);                                                              // Execute function call to wait 5 seconds
        
        // Step 7
        Set_EW_LT(YELLOW);                                                              // Set D4 RGB to YELLOW
        Wait_N_Seconds(3);                                                              // Execute function call to wait 3 seconds
        
        // Step 8
        Set_EW_LT(RED);                                                                 // Set D4 RGB to RED
    }
    
    // Step 9
    Set_EW(GREEN);                                                                      // Set D3 RGB to GREEN
    Wait_N_Seconds(7);                                                                  // Execute function call to wait 7 seconds
    
    // Step 10
    Set_EW(YELLOW);                                                                     // Set D3 RGB to YELLOW
    Wait_N_Seconds(3);                                                                  // Execute function call to wait 3 seconds
    
    // Step 11
    Set_EW(RED);                                                                        // Set D3 RGB to RED
    
    // Step 12
    if (NS_LT_SW == 1)                                                                  // Execute this if block if NSLT_SW is equal to one
    {
        // Step 13
        Set_NS_LT(GREEN);                                                               // Set D2 RGB to GREEN
        Wait_N_Seconds(8);                                                              // Execute function call to wait 8 seconds
        
        // Step 14
        Set_NS_LT(YELLOW);                                                              // Set D2 RGB to YELLOW
        Wait_N_Seconds(3);                                                              // Execute function call to wait 3 seconds
        
        // Step 15
        Set_NS_LT(RED);                                                                 // Set D2 RGB to RED
    }
}

// New function Definitions for Lab 8
void update_LCD_color(char direction, char color)                                       // Routine to update LCD colors of lights
{
    char Circle_Y;                                                                      // Obtains character value for Circle_Y
    Circle_Y = EW_Cir_Y + direction * 30;                                               // Value for Circle_Y equals EW_Cir_Y + direction * 30
    
    if (color == Color_Off)                                                             // If Color off make all circles black but leave outline
    {  
        fillCircle(XRED, Circle_Y, Circle_Size, ST7735_BLACK);                          // No fill for red circle
        fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_BLACK);                          // No fill for yellow circle
        fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK);                          // No fill for green circle
        drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);                            // Outline for red circle   
        drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);                         // Outline for yellow circle
        drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);                          // Outline for green circle                   
    }    
    
    if (color == Color_Red)                                                             // If the color is red only fill the red circle with red
    {
        fillCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);                            // Fill in red circle
        fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_BLACK);                          // No fill for yellow circle
        fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK);                          // No fill for green circle
        drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);                            // Outline for red circle   
        drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);                         // Outline for yellow circle
        drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);                          // Outline for green circle
    }
          
    if (color == Color_Yellow)                                                          // If the color is yellow only fill the yellow circle with yellow
    {
        fillCircle(XRED, Circle_Y, Circle_Size, ST7735_BLACK);                          // No fill for red circle
        fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);                         // Fill in yellow circle
        fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK);                          // No fill for green circle
        drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);                            // Outline for red circle
        drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);                         // Outline for yellow circle
        drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);                          // Outline for green circle           
    }
          
    if (color == Color_Green)                                                           // If the color is green only fill the green circle with green
    {
        fillCircle(XRED, Circle_Y, Circle_Size, ST7735_BLACK);                          // No fill for red circle
        fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_BLACK);                          // No fill for yellow circle
        fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);                          // Fill in green circle
        drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);                            // Outline for red circle     
        drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);                         // Outline for yellow circle
        drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);                          // Outline for green circle                    
    }     
}

void update_LCD_PED_Count(char direction, char count)                                   // Routine for update_LCD_count with two inputs of characters: direction
                                                                                        // and count to take into consideration and determine which case to use
{
    switch (direction)                                                                  // Update traffic light with ped time
    {
        case EW:                                                                        // Switch case for EW
            PED_EW_Count[0] = count/10  + '0';                                          // PED count upper digit
            PED_EW_Count[1] = count%10  + '0';                                          // PED Lower
            drawtext(PED_Count_X, PED_EW_Count_Y, PED_EW_Count, EW_Color, ST7735_BLACK, TS_2);  // Put counter digit on screen        
            break;                                                                      // Breaks from case EW
        
        case NS:                                                                        // Switch case for NS
            PED_NS_Count[0] = count/10  + '0';                                          // PED count upper digit
            PED_NS_Count[1] = count%10  + '0';                                          // PED Lower
            drawtext(PED_Count_X, PED_NS_Count_Y, PED_NS_Count, NS_Color, ST7735_BLACK, TS_2);  // Put counter digit on screen
            break;                                                                      // Breaks from case NS
    }
}

void Initialize_Screen()                                                                // Routine to initialize LCD screen
{
    LCD_Reset();                                                                        // Resets LCD
    TFT_GreenTab_Initialize();                                                          // Function execution call
    fillScreen(ST7735_BLACK);                                                           // LCD screen turn off/ black out
  
    /* TOP HEADER FIELD */
    txt = buffer;                                                                       // Assign txt variable to buffer
    strcpy(txt, "ECE3301L S3-T06");                                                     // Copy string into variable txt
    drawtext(2, 2, txt, ST7735_WHITE, ST7735_BLACK, TS_1);                              // Display text to LCD
  
    /* MODE FIELD */
    strcpy(txt, "Mode:");                                                               // Copy string into variable txt
    drawtext(2, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);                             // Display text to LCD
    drawtext(35,10, Act_Mode_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);                    // DIsplay Act_Mode_Txt to LCD

    /* EMERGENCY REQUEST FIELD */
    strcpy(txt, "FR:");                                                                 // Copy string into variable txt
    drawtext(50, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);                            // Display text to LCD
    drawtext(70, 10, FlashingR_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);                  // Display FlashingR_Txt to LCD
  
    /* EMERGENCY STATUS FIELD */
    strcpy(txt, "FS:");                                                                 // Copy string into variable txt
    drawtext(80, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);                            // Display text to LCD
    drawtext(100, 10, FlashingS_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);                 // Display FlashingS_Txt to LCD
  
    /* SECOND UPDATE FIELD */
    strcpy(txt, "*");                                                                   // Copy string into variable txt
    drawtext(120, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);                           // Display text to LCD
      
    /* EAST/WEST UPDATE FIELD */
    strcpy(txt, "EAST/WEST");                                                           // Copy string into variable txt
    drawtext  (XTXT, EW_Txt_Y, txt, EW_Color, ST7735_BLACK, TS_1);                      // Display text to LCD
    drawRect  (XTXT, EW_Cir_Y-8, 60, 18, EW_Color);                                     // Execute function to draw Rectangle
    fillCircle(XRED, EW_Cir_Y, Circle_Size, ST7735_RED);                                // Execute function to fill circle red
    drawCircle(XYEL, EW_Cir_Y, Circle_Size, ST7735_YELLOW);                             // Execute function to draw circle for yellow
    drawCircle(XGRN, EW_Cir_Y, Circle_Size, ST7735_GREEN);                              // Execute function to draw circle for green
    drawtext  (XCNT, EW_Count_Y, EW_Count, EW_Color, ST7735_BLACK, TS_2);               // Display EW_Count text

    /* EAST/WEST LEFT TURN UPDATE FIELD */
    strcpy(txt, "E/W LT");                                                              // Copy string into variable txt
    drawtext  (XTXT, EWLT_Txt_Y, txt, EWLT_Color, ST7735_BLACK, TS_1);                  // Display text to LCD
    drawRect  (XTXT, EWLT_Cir_Y-8, 60, 18, EWLT_Color);                                 // Execute function to draw Rectangle
    fillCircle(XRED, EWLT_Cir_Y, Circle_Size, ST7735_RED);                              // Execute function to fill circle red
    drawCircle(XYEL, EWLT_Cir_Y, Circle_Size, ST7735_YELLOW);                           // Execute function to draw circle for yellow
    drawCircle(XGRN, EWLT_Cir_Y, Circle_Size, ST7735_GREEN);                            // Execute function to draw circle for green
    drawtext  (XCNT, EWLT_Count_Y, EWLT_Count, EWLT_Color, ST7735_BLACK, TS_2);         // Display EWLT_Count text

    /* NORTH/SOUTH UPDATE FIELD */
    strcpy(txt, "NORTH/SOUTH");                                                         // Copy string into variable txt
    drawtext  (XTXT, NS_Txt_Y  , txt, NS_Color, ST7735_BLACK, TS_1);                    // Display text to LCD
    drawRect  (XTXT, NS_Cir_Y-8, 60, 18, NS_Color);                                     // Execute function to draw Rectangle
    drawCircle(XRED, NS_Cir_Y  , Circle_Size, ST7735_RED);                              // Execute function to fill circle red
    drawCircle(XYEL, NS_Cir_Y  , Circle_Size, ST7735_YELLOW);                           // Execute function to draw circle for yellow
    fillCircle(XGRN, NS_Cir_Y  , Circle_Size, ST7735_GREEN);                            // Execute function to draw circle for green
    drawtext  (XCNT, NS_Count_Y, NS_Count, NS_Color, ST7735_BLACK, TS_2);               // Display NS_Count text
    
    /* NORTH/SOUTH LEFT TURN UPDATE FIELD */
    strcpy(txt, "N/S LT");                                                              // Copy string into variable txt
    drawtext  (XTXT, NSLT_Txt_Y, txt, NSLT_Color, ST7735_BLACK, TS_1);                  // Display text to LCD
    drawRect  (XTXT, NSLT_Cir_Y-8, 60, 18, NSLT_Color);                                 // Execute function to draw Rectangle
    fillCircle(XRED, NSLT_Cir_Y, Circle_Size, ST7735_RED);                              // Execute function to fill circle red
    drawCircle(XYEL, NSLT_Cir_Y, Circle_Size, ST7735_YELLOW);                           // Execute function to draw circle for yellow
    drawCircle(XGRN, NSLT_Cir_Y, Circle_Size, ST7735_GREEN);                            // Execute function to draw circle for green
    drawtext  (XCNT, NSLT_Count_Y, NSLT_Count, NSLT_Color, ST7735_BLACK, TS_2);         // Display NSLT_Count text
  

    /* EAST/WEST PEDESTRIAM UPDATE FIELD */  
    drawtext(2, PED_EW_Count_Y, PED_EW_Count, EW_Color, ST7735_BLACK, TS_2);            // Display PED_EW_Count text
    strcpy(txt, "PEW");                                                                 // Copy string into variable txt
    drawtext(3, EW_Txt_Y, txt, EW_Color, ST7735_BLACK, TS_1);                           // Display text to LCD

    /* NORTH/SOUTH PEDESTRIAM UPDATE FIELD */
    strcpy(txt, "PNS");                                                                 // Copy string into variable txt
    drawtext(3, NS_Txt_Y, txt, NS_Color, ST7735_BLACK, TS_1);                           // Display text to LCD
    drawtext(2, PED_NS_Count_Y, PED_NS_Count, NS_Color, ST7735_BLACK, TS_2);            // Display PED_NS_Count text
    
    /* MISCELLANEOUS UPDATE FIELD */  
    strcpy(txt, "EWP EWLT NSP NSLT MD");                                                // Copy string into variable txt
    drawtext(1,  Switch_Txt_Y, txt, ST7735_WHITE, ST7735_BLACK, TS_1);                  // Display text from variable txt
    drawtext(6,  Switch_Txt_Y+9, SW_EWPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);       // Display SW_EWPED_Txt to LCD
    drawtext(32, Switch_Txt_Y+9, SW_EWLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);        // Display SW_EWLT_Txt to LCD
    drawtext(58, Switch_Txt_Y+9, SW_NSPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);       // Display SW_NSPED_Txt to LCD
    drawtext(87, Switch_Txt_Y+9, SW_NSLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);        // Display SW_NSLT_Txt to LCD
  
    drawtext(112,Switch_Txt_Y+9, SW_MODE_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);        // Display SW_MODE_Txt to LCD
}

void update_LCD_misc()                                                                  // Routine for update_LCD_misc() to update the states of the switches
                                                                                        // and the photo sensor
{
    Get_Light_Readout();                                                                // Execute function call
    SW_MODE = volt < 3.5 ? 1:0;                                                         // Mode = 1, Day_mode, Mode = 0 Night_mode  
    SW_EWPED = EW_PED_SW;                                                               // SW_EWPED gets value EW_PED_SW
    SW_EWLT = EW_LT_SW;                                                                 // SW_EWLT gets value EW_LT_SW
    SW_NSPED = NS_PED_SW;                                                               // SW_NSPED gets value NS_PED_SW
    SW_NSLT = NS_LT_SW;                                                                 // SW_NSLT gets value NS_LT_SW
    
    if (SW_EWPED == 0) SW_EWPED_Txt[0] = '0'; else SW_EWPED_Txt[0] = '1';               // Set Text at bottom of screen to switch states
    if (SW_EWLT == 0) SW_EWLT_Txt[0] = '0'; else SW_EWLT_Txt[0] = '1';                  // Set Text at bottom of screen to switch states
    if (SW_NSPED == 0) SW_NSPED_Txt[0] = '0'; else SW_NSPED_Txt[0] = '1';               // Set Text at bottom of screen to switch states
    if (SW_NSLT == 0) SW_NSLT_Txt[0] = '0'; else SW_NSLT_Txt[0] = '1';                  // Set Text at bottom of screen to switch states

    if (SW_MODE == 0) SW_MODE_Txt[0] = 'N'; else SW_MODE_Txt[0] = 'D';                  // Set Text at bottom of screen to switch states
    
    // Lab 9 NEW CODE
    if (FLASHING_REQUEST == 0) FlashingR_Txt[0] = '0'; else FlashingR_Txt[0] = '1';     // Set Text at top of screen to switch logic states
    if (FLASHING == 0) FlashingS_Txt[0] = '0'; else FlashingS_Txt[0] = '1';             // Set Text at top of screen to switch logic states
   
    drawtext(35,10, Act_Mode_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);                    // Show Act_Mode_Txt on the screen
    drawtext(70,10, FlashingR_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);                   // Show FlashingR_Txt on the screen
    drawtext(100,10, FlashingS_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);                  // Show FlashingS_Txt on the screen
    drawtext(6,   Switch_Txt_Y+9, SW_EWPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);      // Show switch and sensor states at bottom of the screen
    drawtext(32,  Switch_Txt_Y+9, SW_EWLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);       // Show switch and sensor states at bottom of the screen
    drawtext(58,  Switch_Txt_Y+9, SW_NSPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);      // Show switch and sensor states at bottom of the screen
    drawtext(87,  Switch_Txt_Y+9, SW_NSLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);       // Show switch and sensor states at bottom of the screen
    drawtext(112,  Switch_Txt_Y+9, SW_MODE_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);      // Show switch and sensor states at bottom of the screen
}

void update_LCD_count(char direction, char count)                                       // Routine for update_LCD_count with two inputs of characters: direction
                                                                                        // and count to take into consideration and determine which case to use
{
    switch (direction)                                                                  // Update traffic light no ped time
    {
        case NS:                                                                        // Switch case for NS direction
            NS_Count[0] = count/10  + '0';                                              // NS count upper digit
            NS_Count[1] = count%10  + '0';                                              // NS count lower digit
            drawtext(XCNT, NS_Count_Y, NS_Count, NS_Color, ST7735_BLACK, TS_2);         // Put counter digit on screen
            break;                                                                      // Breaks from case NS
      
        case NSLT:                                                                      // Switch case for NSLT direction
            NSLT_Count[0] = count/10  + '0';                                            // NSLT count upper digit
            NSLT_Count[1] = count%10  + '0';                                            // NSLT count lower digit
            drawtext(XCNT, NSLT_Count_Y, NSLT_Count, NSLT_Color, ST7735_BLACK, TS_2);   // put counter digit on screen         
            break;                                                                      // Breaks from case NSLT
      
        case EW:                                                                        // Switch case for EW direction
            EW_Count[0] = count/10  + '0';                                              // EW count upper digit
            EW_Count[1] = count%10  + '0';                                              // EW count lower digit
            drawtext(XCNT, EW_Count_Y, EW_Count, EW_Color, ST7735_BLACK, TS_2);         // Put counter digit on screen        
            break;                                                                      // Breaks from case EW
            
        case EWLT:                                                                      // Switch case for EWLT direction
            EWLT_Count[0] = count/10  + '0';                                            // EWLT count upper digit
            EWLT_Count[1] = count%10  + '0';                                            // EWLT count lower digit
            drawtext(XCNT, EWLT_Count_Y, EWLT_Count, EWLT_Color, ST7735_BLACK, TS_2);   // Put counter digit on screen     
            break;                                                                      // Breaks from case EWLT
    }  
}

void Get_Light_Readout()                                                                // Routine for Get_Light_Readout()
{
    nStep = Get_full_ADC();                                                             // calculates the # of steps for analog conversion
    volt = nStep * 5 /1024.0;                                                           // gets the voltage in Volts, using 5V as reference s 
                                                                                        // instead of 4, also divide by 1024 
}

// Lab 9 New Functions
void Do_Init()                                                                          // Initialize the ports 
{ 
    Init_UART();                                                                        // Initialize the uart 
    Init_ADC();                                                                         // Initialize the ADC with the 
                                                                                        // programming of ADCON1 
    RBPU = 0;                                                                           // Make sure that PORTB has pull-up 
    OSCCON=0x70;                                                                        // Set oscillator to 8 MHz 
    TRISB = 0x07;                                                                       // Configure the PORTB to make sure
                                                                                        // that all the INTx pins are 
                                                                                        // inputs 
    INTCONbits.INT0IF = 0 ;                                                             // Clear INT0IF 
    INTCON3bits.INT1IF = 0;                                                             // Clear INT1IF 
    INTCON3bits.INT2IF =0;                                                              // Clear INT2IF 
    INTCON2bits.INTEDG0=0 ;                                                             // INT0 EDGE falling 
    INTCON2bits.INTEDG1=0;                                                              // INT1 EDGE falling 
    INTCON2bits.INTEDG2=1;                                                              // INT2 EDGE rising 
    INTCONbits.INT0IE =1;                                                               // Set INT0 IE 
    INTCON3bits.INT1IE=1;                                                               // Set INT1 IE 
    INTCON3bits.INT2IE=1;                                                               // Set INT2 IE 
    INTCONbits.GIE=1;                                                                   // Set the Global Interrupt Enable 
}

void interrupt high_priority chkisr()                                                   // Routine to check if interrupts have occurred
{ 
    if (INTCONbits.INT0IF == 1) INT0_ISR();                                             // Check if INT0 has occurred
    if (INTCON3bits.INT1IF == 1) INT1_ISR();                                            // Check if INT1 has occurred
    if (INTCON3bits.INT2IF == 1) INT2_ISR();                                            // Check if INT2 has occurred
} 

void INT0_ISR()                                                                         // Routine for INTO_ISR
{ 
    INTCONbits.INT0IF=0;                                                                // Clear the interrupt flag 
    if (MODE == 1)                                                                      // Checks if Day Mode
    {
        NS_PED_SW = 1;                                                                  // If it is day mode then assign the value one
    }
    else                                                                                // Execute else block if it is night mode
    {
        NS_PED_SW = 0;                                                                  // And assign the value zero to the variable
    }
} 

void INT1_ISR()                                                                         // Routine for INT1_ISR
{ 
    INTCON3bits.INT1IF=0;                                                               // Clear the interrupt flag
    if (MODE == 1)                                                                      // Checks if Day Mode
    {
        EW_PED_SW = 1;                                                                  // If it is day mode then assign the value one
    }
    else                                                                                // Execute else block if it is night mode
    {
        EW_PED_SW = 0;                                                                  // And assign the value zero to the variable
    }
} 

void INT2_ISR()                                                                         // Routine for INT2_ISR
{ 
    INTCON3bits.INT2IF=0;                                                               // Clear the interrupt flag
    FLASHING_REQUEST = 1;                                                               // Assign the value one to the variable
} 

Do_Flashing()                                                                           // Routine for Do_Flashing() method
{
    FLASHING = 1;                                                                       // Assign the value of 1 to the variable
    
    while (FLASHING == 1)                                                               // Continue looping while the conditional statement is True
    {
        if (FLASHING_REQUEST == 1)                                                      // If the conditional statement is True, execute this block
        {
            FLASHING_REQUEST = 0;                                                       // Assign the value of zero to the variable
            FLASHING = 0;                                                               // Assign the value of zero to the variable
        }
        else                                                                            // Execute this block if the conditional statement above is False
        {
            Set_EW(RED);                                                                // Set D3 RGB to RED
            Set_EW_LT(RED);                                                             // Set D4 RGB to RED
            Set_NS_LT(RED);                                                             // Set D2 RGB to RED
            Set_NS(RED);                                                                // Set D1 RGB to RED
            Wait_One_Second();                                                          // Delay by one second
            
            Set_EW(OFF);                                                                // Set D3 RGB to OFF
            Set_EW_LT(OFF);                                                             // Set D4 RGB to OFF
            Set_NS_LT(OFF);                                                             // Set D2 RGB to OFF
            Set_NS(OFF);                                                                // Set D1 RGB to OFF
            Wait_One_Second();                                                          // Delay by one second
        }
    }
    
}