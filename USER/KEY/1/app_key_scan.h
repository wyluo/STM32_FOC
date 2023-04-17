#ifndef APP_KEY_SCAN_H
#define APP_KEY_SCAN_H

enum{
	KEY_SCLICK_EVENT   = 0x01,
	KEY_DCLICK_EVENT   = 0x02,
	KEY_TCLICK_EVENT   = 0x04,
	KEY_HOLD_EVENT     = 0x08,
	KEY_LONG_EVENT     = 0x10,
};

void KeyScan(void);
void key_progess(void);

#endif
