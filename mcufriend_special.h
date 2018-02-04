#define SSD1289_JUMPERS 2       //Uno Shield with VERY different pin-out to Mcufriend
// only define one "USE_XXX" macro at any time
//#define USE_SSD1289_SHIELD_UNO 
//#define USE_SSD1289_SHIELD_MEGA 
//#define USE_SSD1289_SHIELD_DUE 
//#define USE_MEGA_8BIT_PROTOSHIELD
//#define USE_MEGA_8BIT_SHIELD
//#define USE_MEGA_16BIT_SHIELD     //RD on PL6 (D43)
//#define USE_BLD_BST_MEGA32U4
//#define USE_BLD_BST_MEGA2560
//#define USE_DUE_8BIT_PROTOSHIELD
//#define USE_DUE_16BIT_SHIELD        //RD on PA15 (D24) 

#if 0
#elif defined(__AVR_ATmega328P__) && defined(USE_SSD1289_SHIELD_UNO)    //on UNO
#warning using SSD1289 Shield for mega328
#define RD_PORT PORTC
#define RD_PIN  3
#define WR_PORT PORTC
#define WR_PIN  2
#define CD_PORT PORTC
#define CD_PIN  1
#define CS_PORT PORTC
#define CS_PIN  0
#define RESET_PORT PORTB
#define RESET_PIN  1      //actually SD_CS

// SSD1289 shield has LCD_D0 on RXD0.   Fine for write-only
// For any Read operations,  put jumper from D0 to D8, Switch #2 to OFF.
// If using Serial,  jumper D1 to A5, Switch #1 to OFF
#if SSD1289_JUMPERS == 0
#warning no jumpers.  Switch #1=ON, #2=ON
#define BMASK    0x00       //0x00 for output, 0x01 for Read + Serial
#define CMASK    0x00       //0x20 for Read + Serial 
#define DMASK    (~BMASK)
#define write8(x)     { PORTD = x; WR_STROBE; }
#define read_8()      ( PIND )
#elif SSD1289_JUMPERS == 1
#warning jumper D0 to D8.  Switch #1=ON, #2=OFF
#define BMASK    0x01       //0x00 for output, 0x01 for Read + Serial
#define CMASK    0x00       //0x20 for Read + Serial
#define DMASK    (~BMASK)
#define write8(x)     { PORTD = (PORTD & ~DMASK) | (x & DMASK); PORTB = (PORTB & ~BMASK) | (x & BMASK); WR_STROBE; }
#define read_8()      ( (PIND & DMASK)|(PINB & BMASK) )
#elif SSD1289_JUMPERS == 2
#warning jumper D0 to D8, D1 to A5.  Switch #1=OFF, #2=OFF
#define BMASK    (1<<0)       //0x00 for output, 0x01 for Read + Serial
#define CMASK    (1<<5)       //0x20 for Read + Serial
#define DMASK    (0xFC)
#define write8(x)     { PORTC = (PORTC & ~CMASK) | ((x<<4) & CMASK);\
                        PORTD = (PORTD & ~DMASK) | (x & DMASK);\
						PORTB = (PORTB & ~BMASK) | (x & BMASK); WR_STROBE; }
#define read_8()      ( ((PINC & CMASK)>>4)|(PIND & DMASK)|(PINB & BMASK) )
#endif
#define setWriteDir() { DDRC |=  CMASK; DDRD |=  DMASK; DDRB |=  BMASK; }
#define setReadDir()  { DDRC &= ~CMASK; DDRD &= ~DMASK; DDRB &= ~BMASK; }
#define write16(x)    { uint8_t h = (x)>>8, l = x; write8(h); write8(l); }
#define READ_8(dst)   { RD_STROBE; dst = read_8(); RD_IDLE; }
#define READ_16(dst)  { uint8_t hi; READ_8(hi); READ_8(dst); dst |= (hi << 8); }

#define PIN_LOW(p, b)        (p) &= ~(1<<(b))
#define PIN_HIGH(p, b)       (p) |= (1<<(b))
#define PIN_OUTPUT(p, b)     *(&p-1) |= (1<<(b))

#elif defined(__AVR_ATxmega128A1__)   // Home made shield with Xplained
#warning Home made shield with Xplained
#define RD_PORT VPORT3
#define RD_PIN  0
#define WR_PORT VPORT3
#define WR_PIN  1
#define CD_PORT VPORT3
#define CD_PIN  2
#define CS_PORT VPORT3
#define CS_PIN  3
#define RESET_PORT VPORT3
#define RESET_PIN  4

// VPORTs are very fast.   CBI, SBI are only one cycle.    Hence all those RD_ACTIVEs
// ILI9320 data sheet says tDDR=100ns.    We need 218ns to read REGs correctly.
#define write_8(x)    { VPORT2.OUT = x; }
#define read_8()      ( VPORT2.IN )
#define setWriteDir() { PORTCFG.VPCTRLB=PORTCFG_VP3MAP_PORTF_gc | PORTCFG_VP2MAP_PORTC_gc; VPORT2.DIR = 0xFF; }
#define setReadDir()  { VPORT2.DIR = 0x00; }
#define write8(x)     { write_8(x); WR_STROBE; }
#define write16(x)    { uint8_t h = (x)>>8, l = x; write8(h); write8(l); }
#define READ_8(dst)   { RD_STROBE; RD_ACTIVE; RD_ACTIVE; RD_ACTIVE; dst = read_8(); RD_IDLE; }
#define READ_16(dst)  { uint8_t hi; READ_8(hi); READ_8(dst); dst |= (hi << 8); }

#define PIN_LOW(p, b)        (p).OUT &= ~(1<<(b))
#define PIN_HIGH(p, b)       (p).OUT |= (1<<(b))
#define PIN_OUTPUT(p, b)     (p).DIR |= (1<<(b))
#elif defined(__AVR_ATxmega32A4U__)   // Home made shield with Batsocks module
#warning Home made shield with Batsocks module
#define RD_PORT VPORT3
#define RD_PIN  0
#define WR_PORT VPORT3
#define WR_PIN  1
#define CD_PORT VPORT3
#define CD_PIN  2
#define CS_PORT VPORT3
#define CS_PIN  3
#define RESET_PORT PORTE
#define RESET_PIN  0

// VPORTs are very fast.   CBI, SBI are only one cycle.    Hence all those RD_ACTIVEs
// ILI9320 data sheet says tDDR=100ns.    We need 218ns to read REGs correctly.
// S6D0154 data sheet says tDDR=250ns.    We need ~500ns to read REGs correctly.
#define write_8(x)    { VPORT2.OUT = x; }
#define read_8()      ( VPORT2.IN )
#define setWriteDir() { PORTCFG.VPCTRLB=PORTCFG_VP13MAP_PORTB_gc | PORTCFG_VP02MAP_PORTC_gc; VPORT2.DIR = 0xFF; }
#define setReadDir()  { VPORT2.DIR = 0x00; }
#define write8(x)     { write_8(x); WR_STROBE; }
#define write16(x)    { uint8_t h = (x)>>8, l = x; write8(h); write8(l); }
#define READ_8(dst)   { RD_STROBE; RD_ACTIVE; RD_ACTIVE; RD_ACTIVE; dst = read_8(); RD_IDLE; }
#define READ_16(dst)  { uint8_t hi; READ_8(hi); READ_8(dst); dst |= (hi << 8); }

#define PIN_LOW(p, b)        (p).OUT &= ~(1<<(b))
#define PIN_HIGH(p, b)       (p).OUT |= (1<<(b))
#define PIN_OUTPUT(p, b)     (p).DIR |= (1<<(b))

#elif defined(__AVR_ATmega2560__) && defined(USE_BLD_BST_MEGA2560)   //regular UNO shield on MEGA2560 using BLD/BST
#warning regular UNO shield on MEGA2560 using BLD/BST
#define RD_PORT PORTF
#define RD_PIN  0
#define WR_PORT PORTF
#define WR_PIN  1
#define CD_PORT PORTF
#define CD_PIN  2
#define CS_PORT PORTF
#define CS_PIN  3
#define RESET_PORT PORTF
#define RESET_PIN  4

#define EMASK         0x38
#define GMASK         0x20
#define HMASK         0x78
static inline void write_8(uint8_t val)
{
	asm volatile("lds __tmp_reg__,0x0102" "\n\t"
	"BST %0,0" "\n\t" "BLD __tmp_reg__,5" "\n\t"
	"BST %0,1" "\n\t" "BLD __tmp_reg__,6" "\n\t"
	"BST %0,6" "\n\t" "BLD __tmp_reg__,3" "\n\t"
	"BST %0,7" "\n\t" "BLD __tmp_reg__,4" "\n\t"
	"sts 0x0102,__tmp_reg__" : : "a" (val));
	asm volatile("in __tmp_reg__,0x0E" "\n\t"
	"BST %0,2" "\n\t" "BLD __tmp_reg__,4" "\n\t"
	"BST %0,3" "\n\t" "BLD __tmp_reg__,5" "\n\t"
	"BST %0,5" "\n\t" "BLD __tmp_reg__,3" "\n\t"
	"out 0x0E,__tmp_reg__" : : "a" (val));
	asm volatile("in __tmp_reg__,0x14" "\n\t"
	"BST %0,4" "\n\t" "BLD __tmp_reg__,5" "\n\t"
	"out 0x14,__tmp_reg__" : : "a" (val));
}

#define read_8()      ( ((PINH & (3<<5)) >> 5)\
| ((PINE & (3<<4)) >> 2)\
| ((PING & (1<<5)) >> 1)\
| ((PINE & (1<<3)) << 2)\
| ((PINH & (3<<3)) << 3)\
)
#define setWriteDir() { DDRH |=  HMASK; DDRG |=  GMASK; DDRE |=  EMASK;  }
#define setReadDir()  { DDRH &= ~HMASK; DDRG &= ~GMASK; DDRE &= ~EMASK;  }
#define write8(x)     { write_8(x); WR_STROBE; }
#define write16(x)    { uint8_t h = (x)>>8, l = x; write8(h); write8(l); }
#define READ_8(dst)  { RD_STROBE; dst = read_8(); RD_IDLE; }
#define READ_16(dst)  { RD_STROBE; dst = read_8(); RD_IDLE; RD_STROBE; dst = (dst<<8) | read_8(); RD_IDLE; }

#define PIN_LOW(p, b)        (p) &= ~(1<<(b))
#define PIN_HIGH(p, b)       (p) |= (1<<(b))
#define PIN_OUTPUT(p, b)     *(&p-1) |= (1<<(b))

#elif defined(__AVR_ATmega2560__) && defined(USE_SSD1289_SHIELD_MEGA)   //on MEGA2560
#warning using SSD1289 Shield for mega2560
#define RD_PORT PORTF
#define RD_PIN  3      //A3
#define WR_PORT PORTF
#define WR_PIN  2      //A2
#define CD_PORT PORTF
#define CD_PIN  1      //A1
#define CS_PORT PORTF
#define CS_PIN  0      //A0
#define RESET_PORT PORTH
#define RESET_PIN  6   //D9 DS_CS,  D10=T_CS, D9=SD_CS, D8=n.c.

// only for SSD1289 data bus on D2..D9  UNTESTED
#if (SSD1289_JUMPERS == 0)    //Switch #1=ON, #2=ON
#warning no jumpers Switch #1=ON, #2=ON
#define EMASK         0x3B
#define FMASK         0x00
#define HMASK         0x18
#define GMASK         0x20
#define write_8(x)   {  PORTH &= ~HMASK; PORTG &= ~GMASK; PORTE &= ~EMASK; \
	PORTE |= (((x) & (1<<0)) << 0); \
	PORTE |= (((x) & (1<<1)) << 0); \
	PORTE |= (((x) & (3<<2)) << 2); \
	PORTG |= (((x) & (1<<4)) << 1); \
	PORTE |= (((x) & (1<<5)) >> 2); \
	PORTH |= (((x) & (3<<6)) >> 3); \
}

#define read_8()      ( ((PINE & (1<<0)) >> 0)\
                      | ((PINE & (1<<1)) >> 0)\
                      | ((PINE & (3<<4)) >> 2)\
                      | ((PING & (1<<5)) >> 1)\
                      | ((PINE & (1<<3)) << 2)\
                      | ((PINH & (3<<3)) << 3)\
                      )
#elif (SSD1289_JUMPERS == 1)    //jumper D0 to D8.  Switch #1=ON, #2=OFF
#warning jumper D0 to D8.  Switch #1=ON, #2=OFF
#define EMASK         0x3A
#define FMASK         0x00
#define HMASK         0x38
#define GMASK         0x20
#define write_8(x)   {  PORTH &= ~HMASK; PORTG &= ~GMASK; PORTE &= ~EMASK; \
	PORTH |= (((x) & (1<<0)) << 5); \
	PORTE |= (((x) & (1<<1)) << 0); \
	PORTE |= (((x) & (3<<2)) << 2); \
	PORTG |= (((x) & (1<<4)) << 1); \
	PORTE |= (((x) & (1<<5)) >> 2); \
	PORTH |= (((x) & (3<<6)) >> 3); \
}

#define read_8()      ( ((PINH & (1<<5)) >> 5)\
                      | ((PINE & (1<<1)) >> 0)\
                      | ((PINE & (3<<4)) >> 2)\
                      | ((PING & (1<<5)) >> 1)\
                      | ((PINE & (1<<3)) << 2)\
                      | ((PINH & (3<<3)) << 3)\
                      )
#elif (SSD1289_JUMPERS == 2)    //jumper D0 to D8, D1 to A5.  Switch #1=OFF, #2=OFF
#warning jumper D0 to D8, D1 to A5.  Switch #1=OFF, #2=OFF
#define FMASK         0x20
#define EMASK         0x38
#define HMASK         0x38
#define GMASK         0x20
#define write_8(x)   {  PORTH &= ~HMASK; PORTG &= ~GMASK; PORTF &= ~FMASK; PORTE &= ~EMASK; \
	PORTH |= (((x) & (1<<0)) << 5); \
	PORTF |= (((x) & (1<<1)) << 4); \
	PORTE |= (((x) & (3<<2)) << 2); \
	PORTG |= (((x) & (1<<4)) << 1); \
	PORTE |= (((x) & (1<<5)) >> 2); \
	PORTH |= (((x) & (3<<6)) >> 3); \
}

#define read_8()      ( ((PINH & (1<<5)) >> 5)\
                      | ((PINF & (1<<5)) >> 4)\
                      | ((PINE & (3<<4)) >> 2)\
                      | ((PING & (1<<5)) >> 1)\
                      | ((PINE & (1<<3)) << 2)\
                      | ((PINH & (3<<3)) << 3)\
                      )
#endif
#define setWriteDir() { DDRH |=  HMASK; DDRG |=  GMASK; DDRF |=  FMASK; DDRE |=  EMASK;  }
#define setReadDir()  { DDRH &= ~HMASK; DDRG &= ~GMASK; DDRF &= ~FMASK; DDRE &= ~EMASK;  }
#define write8(x)     { write_8(x); WR_STROBE; }
#define write16(x)    { uint8_t h = (x)>>8, l = x; write8(h); write8(l); }
#define READ_8(dst)  { RD_STROBE; dst = read_8(); RD_IDLE; }
#define READ_16(dst)  { RD_STROBE; dst = read_8(); RD_IDLE; RD_STROBE; dst = (dst<<8) | read_8(); RD_IDLE; }

#define PIN_LOW(p, b)        (p) &= ~(1<<(b))
#define PIN_HIGH(p, b)       (p) |= (1<<(b))
#define PIN_OUTPUT(p, b)     *(&p-1) |= (1<<(b))

#elif defined(__AVR_ATmega2560__) && defined(USE_MEGA_16BIT_SHIELD)
#warning USE_MEGA_16BIT_SHIELD
#define RD_PORT PORTL
#define RD_PIN  6        //PL6 (D43).   Graham has PA15 (D24) on Due Shield 
#define WR_PORT PORTG
#define WR_PIN  2        //D39 CTE
#define CD_PORT PORTD
#define CD_PIN  7        //D38 CTE
#define CS_PORT PORTG
#define CS_PIN  1        //D40 CTE
#define RESET_PORT PORTG
#define RESET_PIN  0     //D41 CTE

#define write_8(x)    { PORTC = x; }
#define write_16(x)   { PORTA = (x) >> 8; PORTC = x; }

#define read_16()     ( (PINA<<8) | (PINC) )
#define setWriteDir() { DDRC = 0xFF; DDRA = 0xff; }
#define setReadDir()  { DDRC = 0x00; DDRA = 0x00; }
#define write8(x)     { write_8(x); WR_STROBE; }
#define write16(x)    { write_16(x); WR_STROBE; }
#define READ_16(dst)  { RD_STROBE; dst = read_16(); RD_IDLE; }
#define READ_8(dst)   { READ_16(dst); dst &= 0xFFFF; }

#define PIN_LOW(p, b)        (p) &= ~(1<<(b))
#define PIN_HIGH(p, b)       (p) |= (1<<(b))
#define PIN_OUTPUT(p, b)     *(&p-1) |= (1<<(b))

#elif defined(__AVR_ATmega2560__) && defined(USE_MEGA_8BIT_SHIELD)
#warning USE_MEGA_8BIT_SHIELD for vagos21
#define RD_PORT PORTL
#define RD_PIN  6        //PL6 (D43).   Graham has PA15 (D24) on Due Shield 
#define WR_PORT PORTG
#define WR_PIN  2        //D39 CTE
#define CD_PORT PORTD
#define CD_PIN  7        //D38 CTE
#define CS_PORT PORTG
#define CS_PIN  1        //D40 CTE
#define RESET_PORT PORTG
#define RESET_PIN  0     //D41 CTE

#define write_8(x)   { PORTA = x;}

#define read_8()      ( PINA )
#define setWriteDir() { DDRA = 0xFF; }
#define setReadDir()  { DDRA = 0x00; }
#define write8(x)     { write_8(x); WR_STROBE; }
#define write16(x)    { uint8_t h = (x)>>8, l = x; write8(h); write8(l); }
#define READ_8(dst)   { RD_STROBE; dst = read_8(); RD_IDLE; }
#define READ_16(dst)  { RD_STROBE; dst = read_8(); RD_IDLE; RD_STROBE; dst = (dst<<8) | read_8(); RD_IDLE; }

#define PIN_LOW(p, b)        (p) &= ~(1<<(b))
#define PIN_HIGH(p, b)       (p) |= (1<<(b))
#define PIN_OUTPUT(p, b)     *(&p-1) |= (1<<(b))

#elif defined(__AVR_ATmega2560__) && defined(USE_MEGA_8BIT_PROTOSHIELD)
#warning USE_MEGA_8BIT_PROTOSHIELD
#define RD_PORT PORTF
#define RD_PIN  0
#define WR_PORT PORTF
#define WR_PIN  1
#define CD_PORT PORTF
#define CD_PIN  2
#define CS_PORT PORTF
#define CS_PIN  3
#define RESET_PORT PORTF
#define RESET_PIN  4

#define write_8(x)   { PORTA = x;}

#define read_8()      ( PINA )
#define setWriteDir() { DDRA = 0xFF; }
#define setReadDir()  { DDRA = 0x00; }
#define write8(x)     { write_8(x); WR_STROBE; }
#define write16(x)    { uint8_t h = (x)>>8, l = x; write8(h); write8(l); }
#define READ_8(dst)   { RD_STROBE; dst = read_8(); RD_IDLE; }
#define READ_16(dst)  { RD_STROBE; dst = read_8(); RD_IDLE; RD_STROBE; dst = (dst<<8) | read_8(); RD_IDLE; }

#define PIN_LOW(p, b)        (p) &= ~(1<<(b))
#define PIN_HIGH(p, b)       (p) |= (1<<(b))
#define PIN_OUTPUT(p, b)     *(&p-1) |= (1<<(b))

#elif defined(__AVR_ATmega32U4__) && defined(USE_BLD_BST_MEGA32U4)  //regular UNO shield on Leonardo using BST/BLD
#warning regular UNO shield on Leonardo using BST/BLD
#define RD_PORT PORTF
#define RD_PIN  7
#define WR_PORT PORTF
#define WR_PIN  6
#define CD_PORT PORTF
#define CD_PIN  5
#define CS_PORT PORTF
#define CS_PIN  4
#define RESET_PORT PORTF
#define RESET_PIN  1

#define BMASK         (3<<4)
#define CMASK         (1<<6)
#define DMASK         ((1<<7)|(1<<4)|(3<<0))
#define EMASK         (1<<6)
static inline void write_8(uint8_t val)
{
	asm volatile("in __tmp_reg__,0x05" "\n\t"
	"BST %0,0" "\n\t" "BLD __tmp_reg__,4" "\n\t"
	"BST %0,1" "\n\t" "BLD __tmp_reg__,5" "\n\t"
	"out 0x05,__tmp_reg__" : : "a" (val));
	asm volatile("in __tmp_reg__,0x0B" "\n\t"
	"BST %0,2" "\n\t" "BLD __tmp_reg__,1" "\n\t"
	"BST %0,3" "\n\t" "BLD __tmp_reg__,0" "\n\t"
	"BST %0,4" "\n\t" "BLD __tmp_reg__,4" "\n\t"
	"BST %0,6" "\n\t" "BLD __tmp_reg__,7" "\n\t"
	"out 0x0B,__tmp_reg__" : : "a" (val));
	asm volatile("in __tmp_reg__,0x08" "\n\t"
	"BST %0,5" "\n\t" "BLD __tmp_reg__,6" "\n\t"
	"out 0x08,__tmp_reg__" : : "a" (val));
	asm volatile("in __tmp_reg__,0x0E" "\n\t"
	"BST %0,7" "\n\t" "BLD __tmp_reg__,6" "\n\t"
	"out 0x0E,__tmp_reg__" : : "a" (val));
}
#define read_8()      ( ((PINB & (3<<4)) >> 4)\
| ((PIND & (1<<1)) << 1)\
| ((PIND & (1<<0)) << 3)\
| ((PIND & (1<<4)) >> 0)\
| ((PINC & (1<<6)) >> 1)\
| ((PIND & (1<<7)) >> 1)\
| ((PINE & (1<<6)) << 1)\
)
#define setWriteDir() { DDRB |=  BMASK; DDRC |=  CMASK; DDRD |=  DMASK; DDRE |=  EMASK;  }
#define setReadDir()  { DDRB &= ~BMASK; DDRC &= ~CMASK; DDRD &= ~DMASK; DDRE &= ~EMASK;  }
#define write8(x)     { write_8(x); WR_STROBE; }
#define write16(x)    { uint8_t h = (x)>>8, l = x; write8(h); write8(l); }
#define READ_8(dst)  { RD_STROBE; dst = read_8(); RD_IDLE; }
#define READ_16(dst)  { RD_STROBE; dst = read_8(); RD_IDLE; RD_STROBE; dst = (dst<<8) | read_8(); RD_IDLE; }

#define PIN_LOW(p, b)        (p) &= ~(1<<(b))
#define PIN_HIGH(p, b)       (p) |= (1<<(b))
#define PIN_OUTPUT(p, b)     *(&p-1) |= (1<<(b))

#elif defined(__SAMD21J18A__)   //regular UNO shield on D21_XPRO
#warning regular UNO shield on D21_XPRO
#include "samd21.h"
// configure macros for the control pins
#define RD_PORT PORT->Group[1]
#define RD_PIN  0
#define WR_PORT PORT->Group[1]
#define WR_PIN  1
#define CD_PORT PORT->Group[0]
#define CD_PIN  10
#define CS_PORT PORT->Group[0]
#define CS_PIN  11
#define RESET_PORT PORT->Group[0]
#define RESET_PIN  8
// configure macros for data bus
#define AMASK 0x00220000
#define BMASK 0x0000C0E4
#define write_8(d) { \
	PORT->Group[0].OUT.reg = (PORT->Group[0].OUT.reg & ~AMASK) \
	| (((d) & (1<<5)) << 16) \
	| (((d) & (1<<7)) << 10); \
	PORT->Group[1].OUT.reg = (PORT->Group[1].OUT.reg & ~BMASK) \
	| (((d) & (3<<0)) << 6) \
	| (((d) & (1<<2)) << 12) \
	| (((d) & (1<<3)) >> 1) \
	| (((d) & (1<<4)) << 1) \
	| (((d) & (1<<6)) << 9); \
}
#define read_8() (          (((PORT->Group[0].IN.reg & (1<<21)) >> 16) \
| ((PORT->Group[0].IN.reg & (1<<17)) >> 10) \
| ((PORT->Group[1].IN.reg & (3<<6)) >> 6) \
| ((PORT->Group[1].IN.reg & (1<<14)) >> 12) \
| ((PORT->Group[1].IN.reg & (1<<2))  << 1) \
| ((PORT->Group[1].IN.reg & (1<<5))  >> 1) \
| ((PORT->Group[1].IN.reg & (1<<15))  >> 9)))
#define setWriteDir() { \
	PORT->Group[0].DIRSET.reg = AMASK; \
	PORT->Group[1].DIRSET.reg = BMASK; \
	PORT->Group[0].WRCONFIG.reg = (AMASK>>16) | (0<<22) | (0<<28) | (1<<30) | (1<<31); \
	PORT->Group[1].WRCONFIG.reg = (BMASK & 0xFFFF) | (0<<22) | (0<<28) | (1<<30); \
}
#define setReadDir()  { \
	PORT->Group[0].DIRCLR.reg = AMASK; \
	PORT->Group[1].DIRCLR.reg = BMASK; \
	PORT->Group[0].WRCONFIG.reg = (AMASK>>16) | (1<<17) | (0<<28) | (1<<30) | (1<<31); \
	PORT->Group[1].WRCONFIG.reg = (BMASK & 0xFFFF) | (1<<17) | (0<<28) | (1<<30); \
}

#define write8(x)     { write_8(x); WR_STROBE; }
#define write16(x)    { uint8_t h = (x)>>8, l = x; write8(h); write8(l); }
#define READ_8(dst)  { RD_STROBE; dst = read_8(); RD_IDLE; }
#define READ_16(dst)  { RD_STROBE; dst = read_8(); RD_IDLE; RD_STROBE; dst = (dst<<8) | read_8(); RD_IDLE; }
// Shield Control macros.
#define PIN_LOW(port, pin)    (port).OUTCLR.reg = (1<<(pin))
#define PIN_HIGH(port, pin)   (port).OUTSET.reg = (1<<(pin))
#define PIN_OUTPUT(port, pin) (port).DIR.reg |= (1<<(pin))

#elif defined(__SAM3X8E__) && defined(USE_SSD1289_SHIELD_DUE)   // on DUE
#warning USE_SSD1289_SHIELD_DUE
// configure macros for the control pins
#define RD_PORT PIOA
#define RD_PIN  22     //A3
#define WR_PORT PIOA
#define WR_PIN  23     //A2
#define CD_PORT PIOA
#define CD_PIN  24     //A1
#define CS_PORT PIOA
#define CS_PIN  16     //A0
#define RESET_PORT PIOC
#define RESET_PIN  21  //D9 Touch CS
// configure macros for data bus
// only for SSD1289 data bus on D2..D9  UNTESTED
#if SSD1289_JUMPERS == 0
#warning no jumpers Switch #1=ON, #2=ON
#define AMASK         (3<<8)
#define BMASK         (1<<25)
#define CMASK         (0xBC << 21)
#define write_8(x)   { PIOA->PIO_CODR = AMASK; PIOB->PIO_CODR = BMASK; PIOC->PIO_CODR = CMASK; \
	PIOA->PIO_SODR = (((x) & (1<<0)) << 8); \
	PIOA->PIO_SODR = (((x) & (1<<1)) << 8); \
	PIOB->PIO_SODR = (((x) & (1<<2)) << 23); \
	PIOC->PIO_SODR = (((x) & (1<<3)) << 25); \
	PIOC->PIO_SODR = (((x) & (1<<4)) << 22); \
	PIOC->PIO_SODR = (((x) & (1<<5)) << 20); \
	PIOC->PIO_SODR = (((x) & (1<<6)) << 18); \
	PIOC->PIO_SODR = (((x) & (1<<7)) << 16); \
}

#define read_8()      ( ((PIOA->PIO_PDSR & (1<<8)) >> 8)\
                      | ((PIOA->PIO_PDSR & (1<<9)) >> 8)\
                      | ((PIOB->PIO_PDSR & (1<<25)) >> 23)\
                      | ((PIOC->PIO_PDSR & (1<<28)) >> 25)\
                      | ((PIOC->PIO_PDSR & (1<<26)) >> 22)\
                      | ((PIOC->PIO_PDSR & (1<<25)) >> 20)\
                      | ((PIOC->PIO_PDSR & (1<<24)) >> 18)\
                      | ((PIOC->PIO_PDSR & (1<<23)) >> 16)\
                      )
#elif SSD1289_JUMPERS == 1
#warning jumper D0 to D8.  Switch #1=ON, #2=OFF
#define AMASK         (1<<9)
#define BMASK         (1<<25)
#define CMASK         (0xBE << 21)
#define write_8(x)   { PIOA->PIO_CODR = AMASK; PIOB->PIO_CODR = BMASK; PIOC->PIO_CODR = CMASK; \
	PIOC->PIO_SODR = (((x) & (1<<0)) << 22); \
	PIOA->PIO_SODR = (((x) & (1<<1)) << 8); \
	PIOB->PIO_SODR = (((x) & (1<<2)) << 23); \
	PIOC->PIO_SODR = (((x) & (1<<3)) << 25); \
	PIOC->PIO_SODR = (((x) & (1<<4)) << 22); \
	PIOC->PIO_SODR = (((x) & (1<<5)) << 20); \
	PIOC->PIO_SODR = (((x) & (1<<6)) << 18); \
	PIOC->PIO_SODR = (((x) & (1<<7)) << 16); \
}

#define read_8()      ( ((PIOC->PIO_PDSR & (1<<22)) >> 22)\
| ((PIOA->PIO_PDSR & (1<<9)) >> 8)\
| ((PIOB->PIO_PDSR & (1<<25)) >> 23)\
| ((PIOC->PIO_PDSR & (1<<28)) >> 25)\
| ((PIOC->PIO_PDSR & (1<<26)) >> 22)\
| ((PIOC->PIO_PDSR & (1<<25)) >> 20)\
| ((PIOC->PIO_PDSR & (1<<24)) >> 18)\
| ((PIOC->PIO_PDSR & (1<<23)) >> 16)\
)
#elif SSD1289_JUMPERS == 2
#warning jumper D0 to D8, D1 to A5.  Switch #1=OFF, #2=OFF
#define AMASK         (1<<4)
#define BMASK         (1<<25)
#define CMASK         (0xBE << 21)
#define write_8(x)   { PIOA->PIO_CODR = AMASK; PIOB->PIO_CODR = BMASK; PIOC->PIO_CODR = CMASK; \
	                   PIOC->PIO_SODR = (((x) & (1<<0)) << 22); \
	                   PIOA->PIO_SODR = (((x) & (1<<1)) << 3); \
	                   PIOB->PIO_SODR = (((x) & (1<<2)) << 23); \
	                   PIOC->PIO_SODR = (((x) & (1<<3)) << 25); \
	                   PIOC->PIO_SODR = (((x) & (1<<4)) << 22); \
	                   PIOC->PIO_SODR = (((x) & (1<<5)) << 20); \
	                   PIOC->PIO_SODR = (((x) & (1<<6)) << 18); \
	                   PIOC->PIO_SODR = (((x) & (1<<7)) << 16); \
                     }

#define read_8()      ( ((PIOC->PIO_PDSR & (1<<22)) >> 22)\
                      | ((PIOA->PIO_PDSR & (1<<4)) >> 3)\
                      | ((PIOB->PIO_PDSR & (1<<25)) >> 23)\
                      | ((PIOC->PIO_PDSR & (1<<28)) >> 25)\
                      | ((PIOC->PIO_PDSR & (1<<26)) >> 22)\
                      | ((PIOC->PIO_PDSR & (1<<25)) >> 20)\
                      | ((PIOC->PIO_PDSR & (1<<24)) >> 18)\
                      | ((PIOC->PIO_PDSR & (1<<23)) >> 16)\
                      )
#endif
#define setWriteDir() { PIOA->PIO_OER = AMASK; PIOB->PIO_OER = BMASK; PIOC->PIO_OER = CMASK; }
#define setReadDir()  { \
	PMC->PMC_PCER0 = (1 << ID_PIOA)|(1 << ID_PIOB)|(1 << ID_PIOC);\
	PIOA->PIO_ODR = AMASK; PIOB->PIO_ODR = BMASK; PIOC->PIO_ODR = CMASK;\
}
#define write8(x)     { write_8(x); WR_ACTIVE; WR_STROBE; WR_IDLE; }
#define write16(x)    { uint8_t h = (x)>>8, l = x; write8(h); write8(l); }
#define READ_8(dst)   { RD_STROBE; RD_ACTIVE; RD_ACTIVE; RD_ACTIVE; RD_ACTIVE; dst = read_8(); RD_IDLE; RD_IDLE; RD_IDLE; }
#define READ_16(dst)  { uint8_t hi; READ_8(hi); READ_8(dst); dst |= (hi << 8); }
// Shield Control macros.
#define PIN_LOW(port, pin)    (port)->PIO_CODR = (1<<(pin))
#define PIN_HIGH(port, pin)   (port)->PIO_SODR = (1<<(pin))
#define PIN_OUTPUT(port, pin) (port)->PIO_OER = (1<<(pin))

#elif defined(__SAM3X8E__) && defined(USE_DUE_8BIT_PROTOSHIELD)  //regular UNO shield on DUE
#warning USE_DUE_8BIT_PROTOSHIELD
// configure macros for the control pins
  #define RD_PORT PIOA
  #define RD_PIN  16    //A0
  #define WR_PORT PIOA
  #define WR_PIN  24    //A1
  #define CD_PORT PIOA
  #define CD_PIN  23    //A2
  #define CS_PORT PIOA
  #define CS_PIN  22    //A3
  #define RESET_PORT PIOA
  #define RESET_PIN  6  //A4
// configure macros for data bus
#define DMASK         (0xFF<<0)
#define write_8(x)   {  PIOD->PIO_CODR = DMASK; PIOD->PIO_SODR = x; }

#define read_8()      ( PIOD->PIO_PDSR & DMASK)
  #define setWriteDir() { PIOD->PIO_OER = DMASK; PIOD->PIO_PER = DMASK; }
  #define setReadDir()  { PMC->PMC_PCER0 = (1 << ID_PIOD); PIOD->PIO_ODR = DMASK;}      
#define write8(x)     { write_8(x); WR_ACTIVE; WR_STROBE; WR_IDLE; WR_IDLE; }
#define write16(x)    { uint8_t h = (x)>>8, l = x; write8(h); write8(l); }
#define READ_8(dst)   { RD_STROBE; RD_ACTIVE; RD_ACTIVE; RD_ACTIVE; RD_ACTIVE; dst = read_8(); RD_IDLE; RD_IDLE; RD_IDLE; }
#define READ_16(dst)  { uint8_t hi; READ_8(hi); READ_8(dst); dst |= (hi << 8); }
// Shield Control macros.
#define PIN_LOW(port, pin)    (port)->PIO_CODR = (1<<(pin))
#define PIN_HIGH(port, pin)   (port)->PIO_SODR = (1<<(pin))
#define PIN_OUTPUT(port, pin) (port)->PIO_OER = (1<<(pin))

#elif defined(__SAM3X8E__) && defined(USE_DUE_16BIT_SHIELD)  //regular CTE shield on DUE
#warning USE_DUE_16BIT_SHIELD
// configure macros for the control pins
#define RD_PORT PIOA
#define RD_PIN  15     //D24 Graham
#define WR_PORT PIOD
#define WR_PIN  1      //D26
#define CD_PORT PIOD
#define CD_PIN  0      //D25
#define CS_PORT PIOD
#define CS_PIN  2      //D27
#define RESET_PORT PIOD
#define RESET_PIN  3   //D28
// configure macros for data bus 
// DB0..DB7 on PIOC1..PIOC8,  DB8..DB15 on PIOC12..PIOC19
// 
#define CMASKH        (0xFF00<<4)
#define CMASKL        (0x00FF<<1)
#define CMASK         (CMASKH | CMASKL)
#define write_8(x)    { PIOC->PIO_CODR = CMASKL; PIOC->PIO_SODR = (((x)&0x00FF)<<1); }
#define write_16(x)   { PIOC->PIO_CODR = CMASK; \
                        PIOC->PIO_SODR = (((x)&0x00FF)<<1)|(((x)&0xFF00)<<4); }
#define read_16()     (((PIOC->PIO_PDSR & CMASKH)>>4)|((PIOC->PIO_PDSR & CMASKL)>>1) )
#define read_8()      (read_16() & 0xFF)
#define setWriteDir() { PIOC->PIO_OER = CMASK; PIOC->PIO_PER = CMASK; }
#define setReadDir()  { PMC->PMC_PCER0 = (1 << ID_PIOC); PIOC->PIO_ODR = CMASK; }
#define write8(x)     { write16(x & 0xFF); }
#define write16(x)    { write_16(x); WR_ACTIVE; WR_STROBE; WR_IDLE; WR_IDLE; }
#define READ_16(dst)  { RD_STROBE; RD_ACTIVE; RD_ACTIVE; RD_ACTIVE; RD_ACTIVE; dst = read_16(); RD_IDLE; RD_IDLE; RD_IDLE; }
#define READ_8(dst)   { READ_16(dst); dst &= 0xFF; }

// Shield Control macros.
#define PIN_LOW(port, pin)    (port)->PIO_CODR = (1<<(pin))
#define PIN_HIGH(port, pin)   (port)->PIO_SODR = (1<<(pin))
#define PIN_OUTPUT(port, pin) (port)->PIO_OER = (1<<(pin))

#elif defined(__SAM3X8E__) && defined(USE_MEGA_16BIT_SHIELD)  //regular CTE shield on DUE
#warning USE_MEGA_16BIT_SHIELD
// configure macros for the control pins
#define RD_PORT PIOA
#define RD_PIN  20     //D43
#define WR_PORT PIOC
#define WR_PIN  7      //D39
#define CD_PORT PIOC
#define CD_PIN  6      //D38
#define CS_PORT PIOC
#define CS_PIN  8      //D40
#define RESET_PORT PIOC
#define RESET_PIN  9   //D41
// configure macros for data bus 
// 
#define AMASK         ((1<<7)|(3<<14))          //PA7, PA14-PA15
#define BMASK         (1<<26)                   //PB26
#define CMASK         (31<<1)                   //PC1-PC5
#define DMASK         ((15<<0)|(1<<6)|(3<<9))   //PD0-PD3, PD6, PD9-PD10

#define write_16(x)   { PIOA->PIO_CODR = AMASK; PIOB->PIO_CODR = BMASK; PIOC->PIO_CODR = CMASK; PIOD->PIO_CODR = DMASK; \
                        PIOA->PIO_SODR = (((x)&(1<<6))<<1)|(((x)&(3<<9))<<5); \
                        PIOB->PIO_SODR = (((x)&(1<<8))<<18); \
                        PIOC->PIO_SODR = (((x)&(1<<0))<<5); \
                        PIOC->PIO_SODR = (((x)&(1<<1))<<3); \
                        PIOC->PIO_SODR = (((x)&(1<<2))<<1); \
                        PIOC->PIO_SODR = (((x)&(1<<3))>>1); \
                        PIOC->PIO_SODR = (((x)&(1<<4))>>3); \
                        PIOD->PIO_SODR = (((x)&(1<<7))<<2)|(((x)&(1<<5))<<5)|(((x)&(15<<11))>>11)|(((x)&(1<<15))>>9); \
					  }

/*
#define write_16(VL)   { PIOA->PIO_CODR = AMASK; PIOC->PIO_CODR = CMASK; PIOD->PIO_CODR = DMASK; \
		REG_PIOA_SODR=((((VL)>>8) & 0x06)<<13) | ((VL & 0x40)<<1);\
		if ((VL)&(1<<8)) REG_PIOB_SODR=(1<<26); else REG_PIOB_CODR=(1<<26);\
		REG_PIOC_SODR=((VL & 0x01)<<5) | ((VL & 0x02)<<3) | ((VL & 0x04)<<1) | ((VL & 0x08)>>1) | ((VL & 0x10)>>3);\
		REG_PIOD_SODR=((((VL)>>8) & 0x78)>>3) | ((((VL)>>8) & 0x80)>>1) | ((VL & 0x20)<<5) | ((VL & 0x80)<<2);\
}
*/
#define read_16()     ( 0\
                        |((PIOC->PIO_PDSR & (1<<5))>>5)\
                        |((PIOC->PIO_PDSR & (1<<4))>>3)\
                        |((PIOC->PIO_PDSR & (1<<3))>>1)\
                        |((PIOC->PIO_PDSR & (1<<2))<<1)\
                        |((PIOC->PIO_PDSR & (1<<1))<<3)\
                        |((PIOD->PIO_PDSR & (1<<10))>>5)\
                        |((PIOA->PIO_PDSR & (1<<7))>>1)\
                        |((PIOD->PIO_PDSR & (1<<9))>>2)\
                        |((PIOB->PIO_PDSR & (1<<26))>>18)\
                        |((PIOA->PIO_PDSR & (3<<14))>>5)\
                        |((PIOD->PIO_PDSR & (15<<0))<<11)\
                        |((PIOD->PIO_PDSR & (1<<6))<<9)\
                      )
#define read_8()      (read_16() & 0xFF)
#define setWriteDir() {\
                        PIOA->PIO_OER = AMASK; PIOA->PIO_PER = AMASK; \
                        PIOB->PIO_OER = BMASK; PIOB->PIO_PER = BMASK; \
                        PIOC->PIO_OER = CMASK; PIOC->PIO_PER = CMASK; \
                        PIOD->PIO_OER = DMASK; PIOD->PIO_PER = DMASK; \
                      }
#define setReadDir()  { \
                        PMC->PMC_PCER0 = (1 << ID_PIOA)|(1 << ID_PIOB)|(1 << ID_PIOC)|(1 << ID_PIOD); \
						PIOA->PIO_ODR = AMASK; \
						PIOB->PIO_ODR = BMASK; \
						PIOC->PIO_ODR = CMASK; \
						PIOD->PIO_ODR = DMASK; \
					  }
#define write8(x)     { write16(x & 0xFF); }
// ILI9486 is slower than ILI9481
#define write16(x)    { write_16(x); WR_ACTIVE; WR_ACTIVE; WR_STROBE; }
#define READ_16(dst)  { RD_STROBE; RD_ACTIVE; RD_ACTIVE; RD_ACTIVE; RD_ACTIVE; dst = read_16(); RD_IDLE; RD_IDLE; RD_IDLE; }
#define READ_8(dst)   { READ_16(dst); dst &= 0xFF; }

// Shield Control macros.
#define PIN_LOW(port, pin)    (port)->PIO_CODR = (1<<(pin))
#define PIN_HIGH(port, pin)   (port)->PIO_SODR = (1<<(pin))
#define PIN_OUTPUT(port, pin) (port)->PIO_OER = (1<<(pin))

#elif defined(__SAM3X8E__) && defined(USE_MEGA_8BIT_SHIELD)  //regular CTE shield on DUE
#warning USE_MEGA_8BIT_SHIELD for peloxp
// configure macros for the control pins
#define RD_PORT PIOA
#define RD_PIN  20     //D43
#define WR_PORT PIOC
#define WR_PIN  7      //D39
#define CD_PORT PIOC
#define CD_PIN  6      //D38
#define CS_PORT PIOC
#define CS_PIN  8      //D40
#define RESET_PORT PIOC
#define RESET_PIN  9   //D41
// configure macros for data bus 
// 
#define AMASK         ((3<<14))                 //PA14-PA15    D23-D24
#define BMASK         (1<<26)                   //PB26         D22
#define DMASK         ((15<<0)|(1<<6))          //PD0-PD3, PD6 D25-D28,D29

#define write_8(x)   { PIOA->PIO_CODR = AMASK; PIOB->PIO_CODR = BMASK; PIOD->PIO_CODR = DMASK; \
                        PIOB->PIO_SODR = (((x)&(1<<0))<<26); \
                        PIOA->PIO_SODR = (((x)&(3<<1))<<13); \
                        PIOD->PIO_SODR = (((x)&(15<<3))>>3); \
                        PIOD->PIO_SODR = (((x)&(1<<7))>>1); \
					  }

#define read_8()     ( 0\
                        |((PIOB->PIO_PDSR & (1<<26))>>26)\
                        |((PIOA->PIO_PDSR & (3<<14))>>13)\
                        |((PIOD->PIO_PDSR & (15<<0))<<3)\
                        |((PIOD->PIO_PDSR & (1<<6))<<1)\
                      )

#define setWriteDir() {\
                        PIOA->PIO_OER = AMASK; PIOA->PIO_PER = AMASK; \
                        PIOB->PIO_OER = BMASK; PIOB->PIO_PER = BMASK; \
                        PIOD->PIO_OER = DMASK; PIOD->PIO_PER = DMASK; \
                      }
#define setReadDir()  { \
                        PMC->PMC_PCER0 = (1 << ID_PIOA)|(1 << ID_PIOB)|(1 << ID_PIOC)|(1 << ID_PIOD); \
						PIOA->PIO_ODR = AMASK; \
						PIOB->PIO_ODR = BMASK; \
						PIOD->PIO_ODR = DMASK; \
					  }

// ILI9486 is slower than ILI9481
#define write8(x)     { write_8(x); WR_ACTIVE; WR_ACTIVE; WR_STROBE; WR_IDLE; WR_IDLE; }
#define write16(x)    { uint8_t h = (x)>>8, l = x; write8(h); write8(l); }
#define READ_8(dst)   { RD_STROBE; RD_ACTIVE; RD_ACTIVE; RD_ACTIVE; RD_ACTIVE; dst = read_8(); RD_IDLE; RD_IDLE; RD_IDLE; }
#define READ_16(dst)  { uint8_t hi; READ_8(hi); READ_8(dst); dst |= (hi << 8); }

// Shield Control macros.
#define PIN_LOW(port, pin)    (port)->PIO_CODR = (1<<(pin))
#define PIN_HIGH(port, pin)   (port)->PIO_SODR = (1<<(pin))
#define PIN_OUTPUT(port, pin) (port)->PIO_OER = (1<<(pin))

#else
#define USE_SPECIAL_FAIL
#endif
