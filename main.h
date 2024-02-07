#define SEC_LED     PORTDbits.RD7       // Defines SEC_LED as PORTD Bit 7
#define NS_RED      PORTAbits.RA5       // Defines NS_RED as PORTA Bit 5
#define NS_GREEN    PORTCbits.RC0       // Defines NS_GREEN as PORTC Bit 0

#define NSLT_RED    PORTDbits.RD3       // Defines NS_LT RED as PORTD Bit 3
#define NSLT_GREEN  PORTDbits.RD4       // Defines NS_LT GREEN as PORTD Bit 4

#define EW_RED      PORTDbits.RD1       // Defines EW_RED as PORTD Bit 1
#define EW_GREEN    PORTDbits.RD2       // Defines EW_GREEN as PORTD Bit 2

#define EWLT_RED    PORTEbits.RE1       // Defines EWLT_RED as PORTE Bit 1
#define EWLT_GREEN  PORTEbits.RE2       // Defines EWLT_GREEN as PORTE Bit 2

#define NS_LT_SW    PORTAbits.RA3       // Defines NS_LT_SW as PORTA Bit 3
char NS_PED_SW = 0;                     // Variable initialization for SW PUSHBUTTON RB0

#define EW_LT_SW    PORTAbits.RA4       // Defines EW_LT_SW as PORTA Bit 4
char EW_PED_SW = 0;                     // Variable initialization for SW PUSHBUTTON RB1

#define MODE_LED    PORTEbits.RE0       // Defines MODE_LED as PORTE Bit 0