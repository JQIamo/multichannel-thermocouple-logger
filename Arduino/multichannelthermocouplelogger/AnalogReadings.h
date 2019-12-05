#define analogCLK 32
#define analogSDI 33
#define analogCS 22
#define analogSDO 23

#define adc_rst 0x8500
#define auto_chan 0xA000
#define chan0 0xC000
#define chan1 0xC400
#define chan2 0xC800
#define chan3 0xCC00
#define chan4 0xD000
#define chan5 0xD400
#define chan6 0xD800
#define chan7 0xDC00
#define no_op 0x0000

int write32(int cs, int sck, int sdo, int sdi, int data);
void exit_standby(int cs, int sck, int sdi, int data);