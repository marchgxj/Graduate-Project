/*******************************************************
*   核心板 飞线 管教 对应
*
*      P6.6 --- MISO
*      P6.5 --- SCK
*      P6.4 --- DRDY
*      P3.7 --- SSN
*      P3.6 --- MOSI  -- connect to pin_miso of pni
*      P5.4 --- clear  
*
*******************************************************/
#ifndef PNI_H
#define PNI_H
/******************* pin number **************************/
#define SCS_1   (P3OUT |= BIT7)
#define SCS_0   (P3OUT &= ~BIT7)
#define SCK_1   (P6OUT |= BIT5)
#define SCK_0   (P6OUT &= ~BIT5)
#define MOSI_1  (P3OUT |= BIT6)
#define MOSI_0  (P3OUT &= ~BIT6)
#define CLEAR_1  (P5OUT |= BIT4)
#define CLEAR_0  (P5OUT &= ~BIT4)
#define read_MISO       ((P6IN & BIT6)>>6)
#define DRDY_PIN        ((P6IN & BIT4)>>4)

/******************* configuration **************************/
#define PNI_CIRCLR 400

/*********  TMRC value ***********
92      ~1.7 ms      ~600 Hz
93      ~3 ms        ~300 Hz
94      ~7 ms        ~150 Hz
95      ~13 ms       ~75 Hz
96      ~27 ms       ~37 Hz
97      ~55 ms       ~18 Hz
98      ~110 ms      ~9 Hz
99      ~220 ms      ~4.5 Hz
9A      ~440 ms      ~2.3 Hz
9B      ~0.8 s       ~1.2 Hz
9C      ~1.6 s       ~0.6 Hz
9D      ~3.3 s       ~0.3 Hz
9E      ~6.7 s       ~0.015 Hz
9F      ~13 s        ~0.075 Hz

频率最终是由 circle相关的频率 和 TMRC相关的频率 两者中最低的决定
**********************************/
#define TMRC_VALUE 0x92
/******************* Variables **************************/
extern uint16 XValue;
extern uint16 YValue;
extern uint16 ZValue;
extern uint8 circle_cfg[6];

/******************* function **************************/
extern unsigned char SPI_read_write(unsigned char data);
extern void PNI_write_circle(uint16 circle_num);
extern void PNI_read_circle(void);
extern void PNI_read_data(uint16* Xn,uint16* Yn,uint16* Zn);
extern int DataReady(void);
extern void Init_PNI(void);
extern uint8 PNI_read_TMRC(void);
extern void PNI_write_TMRC(uint8 TMRC);
extern void getOffset();
#endif
