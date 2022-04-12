#include <psdos.h>
#include <wbios.h>
#include <lcdbios.h>
#include "beep.inc"
#include "theme.inc"
#include "bytursi.inc"
#include "marrisof.inc"
#include "titolo.inc"

/* definitions */
#define GVSET 0
#define GVXOR 1
#define GVOR  2
#define GVAND 3

#define BUZZ (*((int*)0xf27c))
#define BUZZEND() ((BUZZ == 0xfffe)||(BUZZ == 0xffff))

#define KEYENTER 0x08

int jiffies=0;

int get_key() {
        int key, ret;

	key=bi_scanbtn();
	if (key & KEYENTER)	{	
		ret=dispYN("Do you want to", "quit TunnelX?", 1);
		if (ret != 1) {
	         dos_exit(0);
		}
	}

        return key;
}

void tm_func() {
 jiffies++;
}

/* Do title page */
void title() {
	bi_buzzinit(7, 3);
	bi_canmelody();
	bi_melody((int*)(theme_snd));
	for (;;) {
		lcdfreeze(1);
		gv_clear(0,0,101,63);
		gv_put(24, 1, TursLogo, GVSET);
		lcdfreeze(0);
		jiffies=0;
		if (BUZZEND()) bi_melody((int*)(theme_snd));
		while (jiffies < 32) {
			if (get_key()&KEYENTER) return;
		}
		lcdfreeze(1);
		gv_clear(0,0,101,63);
		gv_put(22, 24, Marrisoft, GVSET);
		lcdfreeze(0);
		jiffies=0;
		while (jiffies < 32) {
			if (get_key()&KEYENTER) return;
		}
		lcdfreeze(1);
		gv_put(0, 0, Title, GVSET);
		lcdfreeze(0);
		jiffies=0;
		while (jiffies < 64) {
			if (get_key()&KEYENTER) return;
		}
	}
}

int main() {
 int *tm;

 screen(1);
 cls(5);
 gv_place(0,0);
 tm=bi_tmset(2, 255, 2, tm_func);		/* every second 64hz tick */

 title();

 bi_buzzinit(7, 2);
 bi_canmelody();
 bi_melody((int*)(beep_snd));
 
 lcdfreeze(1);
 gv_clear(0,0,101,63);
 gv_put(24, 1, TursLogo, GVSET);
 lcdfreeze(0);

 while (jiffies < 300) {
    if (BUZZEND()) bi_melody((int*)(beep_snd));
    get_key();
 }
 return 0;
}


