/********************************/
/* TunnelX - by M.Brent (Tursi) */
/* Design and Artwork by        */
/* Maurizio Terzo               */
/********************************/
/* Written for the Matsucom     */
/* OnHandPC                     */
/********************************/
/* Random number generator from */
/* the book Graphics Gems       */
/* Bin2Inc tool by Van Helsing  */
/********************************/

/* Libraries */
#include <psdos.h>
#include <wbios.h>
#include <lcdbios.h>

/* sounds - must be first? */
#include "theme2b.inc"
#include "beep.inc"
#include "bonus.inc"
#include "coins.inc"
#include "endgame.inc"
#include "endlevel.inc"
#include "explos.inc"
#include "gameover.inc"
#include "skipenem.inc"

/* Graphics */
#include "0.inc"
#include "1.inc"
#include "2.inc"
#include "3.inc"
#include "4.inc"
#include "5.inc"
#include "6.inc"
#include "7.inc"
#include "8.inc"
#include "9.inc"
#include "bar.inc"
#include "bytursi.inc"
#include "enemy1a.inc"
#include "enemy1b.inc"
#include "enemy1c.inc"
#include "enemy2a.inc"
#include "enemy2b.inc"
#include "enemy2c.inc"
#include "enemy3a.inc"
#include "enemy3b.inc"
#include "enemy3c.inc"
#include "explod1.inc"
#include "explod2.inc"
#include "explod3.inc"
#include "explod4.inc"
#include "explod5.inc"
#include "enemy1amask.inc"
#include "enemy1bmask.inc"
#include "enemy1cmask.inc"
#include "enemy2amask.inc"
#include "enemy2bmask.inc"
#include "enemy2cmask.inc"
#include "enemy3amask.inc"
#include "enemy3bmask.inc"
#include "enemy3cmask.inc"
#include "explod1mask.inc"
#include "explod2mask.inc"
#include "explod3mask.inc"
#include "explod4mask.inc"
#include "explod5mask.inc"
#include "background1.inc"
#include "background2.inc"
#include "background3.inc"
#include "background4.inc"
#include "background1b.inc"
#include "background2b.inc"
#include "background3b.inc"
#include "background4b.inc"
#include "bonus1.inc"
#include "bonus2.inc"
#include "bonus3.inc"
#include "bonus1mask.inc"
#include "bonus2mask.inc"
#include "bonus3mask.inc"
#include "coin1.inc"
#include "coin2.inc"
#include "coin3.inc"
#include "coin4.inc"
#include "coin1mask.inc"
#include "coin2mask.inc"
#include "coin3mask.inc"
#include "coin4mask.inc"
#include "gameoverscr.inc"
#include "lifeleft.inc"
#include "marrisof.inc"
#include "meteora.inc"
#include "meteorb.inc"
#include "meteorc.inc"
#include "meteoramask.inc"
#include "meteorbmask.inc"
#include "meteorcmask.inc"
#include "nolife.inc"
#include "player.inc"
#include "playerl.inc"
#include "playerr.inc"
#include "playermask.inc"
#include "playerlmask.inc"
#include "playerrmask.inc"
#include "sound.inc"
#include "stage-1.inc"
#include "stage-2.inc"
#include "stage-3.inc"
#include "stage-4.inc"
#include "stage-5.inc"
#include "stage-6.inc"
#include "stage-7.inc"
#include "stage-8.inc"
#include "stage-9.inc"
#include "titolo.inc"
#include "tl.inc"
#include "tunnel1.inc"
#include "tunnel2.inc"
#include "tunnel3.inc"
#include "tunnel4.inc"
#include "warntr.inc"
#include "warntl.inc"
#include "warnbr.inc"
#include "warnbl.inc"

#include "man1.inc"
#include "man2.inc"
#include "man3.inc"
#include "ship1.inc"
#include "ship2.inc"
#include "smship.inc"
#include "man1mask.inc"
#include "man2mask.inc"
#include "man3mask.inc"
#include "ship1mask.inc"
#include "ship2mask.inc"
#include "smshipmask.inc"
#include "bgfinala.inc"
#include "bgfinalb.inc"
#include "credits.inc"
#include "congratulations.inc"

/* global variables */
int jiffies=0;
int *tm=0;
int usesound=0;
int debugY=64;
int Lives;
unsigned int Score;
int gx, gy;
struct ENXX {
	int x, y, z, type;
	int xs, ys;
	int xx, yy;
	char *pat, *mask;
} Enemy[10];

/* definitions */
#define GVSET 0
#define GVXOR 1
#define GVOR  2
#define GVAND 3

#define BUZZ (*((int*)0xf27c))
#define BUZZEND() ((usesound != 0) && ((BUZZ == 0xfffe)||(BUZZ == 0xffff)))
#define PLAY(x) if (usesound) { bi_buzzinit(4,x[2]); bi_melody((int*)(&x[4])); }

#define KEYFILER 0x01
#define KEYMENU  0x02
#define KEYEL    0x04
#define KEYENTER 0x08
#define KEYLEFT  0x10
#define KEYUP    0x20
#define KEYRIGHT 0x40
#define KEYDOWN  0x80

static char *Tunnel[4]={ Tunnel4, Tunnel3, Tunnel2, Tunnel1 };
static char *Digit[10]={Digit0, Digit1, Digit2, Digit3, Digit4, Digit5, Digit6,
			Digit7, Digit8, Digit9 };

/* Wrapper for gv_place */
void place(int x, int y) {
	gx=x; gy=y;
	gv_place(x,y);
}

/* timer function - count ticks */
void tm_func() {
	jiffies++;
}

/* Random function adapted from Graphics Gems */
static int nLastRand=1234;
#define MASK 0x6000; /* 15 bit */
unsigned int myrand() {
  if (nLastRand&1) {
    nLastRand>>=1;
    nLastRand^=MASK;
  } else {
    nLastRand>>=1;
  }
  return(nLastRand);
}

void debug(char *str) {
	gv_kput(0, debugY, str, 0, 0, GVSET);
	debugY+=12;
	if (debugY > 230) {
		debugY=120;
		gv_scroll(120);
	}
}

/* disable debug calls */
#define debug(x)

int sgn(x) {
	if (x>0) return 1;
	if (x<0) return -1;
	return 0;
}

/* do any cleanup, and exit the program */
void shutdown() {
	debug("Shutting down...");
        bi_canmelody();
        bi_tmdel(tm);
	dos_exit(0);
}

/* read the keyboard */
int get_key() {
        int key, ret, old, x, y;

	key=bi_scanbtn();
	if (key & KEYMENU)	{	/* Menu */
		debug("Menu key detected");
		gv_place(0,0);		/* deliberately not using override */
		ret=dispYN("Do you want to", "quit TunnelX?", 1);
		if (ret != 1) {
			shutdown();
		}
		gv_place(gx, gy);	/* put the screen back */
	}

        return key;
}

/* draws the status bar with Score */
void DrawBar() {
	unsigned int x, y, tmp, z;

	gv_put(0, 55, Bar, GVSET);		/* Overwrite with bar */
		
	/* add the score */
	gv_put(8, 58, Digit0, GVSET);		/* Fixed digit */
	x=13;
	tmp=Score;
	/* save a loop and 5 divisions by breaking it out */
	y=tmp/10000;
	tmp%=10000;
	gv_put(x, 58, Digit[y], GVSET);
	x+=5;
	y=tmp/1000;
	tmp%=1000;
	gv_put(x, 58, Digit[y], GVSET);
	x+=5;
	y=tmp/100;
	tmp%=100;
	gv_put(x, 58, Digit[y], GVSET);
	x+=5;
	y=tmp/10;
	tmp%=10;
	gv_put(x, 58, Digit[y], GVSET);
	x+=5;
	gv_put(x, 58, Digit[tmp], GVSET);

	/* and the remaining lives */
	x=76;
	for (z=0; z<Lives; z++) {
		gv_put(x, 59, LifeLeft, GVSET);
		x+=5;
	}
	for (z=Lives; z<3; z++) {
		gv_put(x, 59, NoLife, GVSET);
		x+=5;
	}
}

/* Do title page */
void title() {
	int cnt;

	cnt=0;	/* we exit after 20 times through */
	jiffies=0;
	bi_canmelody();
	while (jiffies<1);
	Lives=0;
	PLAY(theme_snd);
	for (;;) {
		debug("Title loop starts...");
		lcdfreeze(1);
		gv_clear(0,0,101,63);
		gv_put(0, 1, TursLogo, GVSET);
		lcdfreeze(0);
		jiffies=0;
		if (BUZZEND()) PLAY(theme_snd);
		while (jiffies < 48) {
			if (get_key()&KEYENTER) return;
		}
		lcdfreeze(1);
		gv_clear(0,0,101,63);
		gv_put(22, 24, Marrisoft, GVSET);
		if (Score>0) DrawBar();
		lcdfreeze(0);
		jiffies=0;
		while (jiffies < 48) {
			if (get_key()&KEYENTER) return;
		}
		lcdfreeze(1);
		gv_put(0, 0, Title, GVSET);
		lcdfreeze(0);
		jiffies=0;
		while (jiffies < 120) {
			if (get_key()&KEYENTER) return;
		}
		cnt++;
		if (cnt > 20) {
			shutdown();
		} 
	}
}

void gameWin2(int x, int y) {
	int mx, my, i, frame;
	
	frame=0;

	/* phase 4 - walk MAN1 down the ramp */
	my=y+15;
	for (mx=x+35; mx<176; mx++) {
		jiffies=0;

		get_key();

		if (mx==x+37) my++;
		else if (mx==x+43) my++;
		else if (mx==x+47) my++;

		lcdfreeze(1);
		gv_put(102, 0, frame&1?BGFINALA:BGFINALB, GVSET);
		gv_put(mx-1, my-1, MAN1MASK, GVAND);
		gv_put(mx, my, Man1, GVOR);
		gv_put(x-1, y-1, SHIP2MASK, GVAND);
		gv_put(x, y, SHIP2, GVOR);
		lcdfreeze(0);
		while (jiffies<3);
		frame++;
	}

	/* phase 4 - delay */
	for (i=0; i<6; i++) {
		jiffies=0;

		get_key();

		lcdfreeze(1);
		gv_put(102, 0, frame&1?BGFINALA:BGFINALB, GVSET);
		gv_put(mx-1, my-1, MAN1MASK, GVAND);
		gv_put(mx, my, Man1, GVOR);
		gv_put(x-1, y-1, SHIP2MASK, GVAND);
		gv_put(x, y, SHIP2, GVOR);
		lcdfreeze(0);
		while (jiffies<3);
		frame++;
	}

	/* phase 5 - man walks up into the distance size 1 */
	for (i=0; i<12; i++) {
		my--;
		if (frame%3 == 0) mx--;

		jiffies=0;

		get_key();

		lcdfreeze(1);
		gv_put(102, 0, frame&1?BGFINALA:BGFINALB, GVSET);
		gv_put(mx-1, my-1, MAN1MASK, GVAND);
		gv_put(mx, my, Man1, GVOR);
		gv_put(x-1, y-1, SHIP2MASK, GVAND);
		gv_put(x, y, SHIP2, GVOR);
		lcdfreeze(0);
		while (jiffies<3);
		frame++;
	}
 
	/* phase 6 - man walks up into the distance size 2 */
	for (i=0; i<9; i++) {
		if (frame%3 != 0) my--;
		if (frame%4 == 0) mx--;

		jiffies=0;

		get_key();

		lcdfreeze(1);
		gv_put(102, 0, frame&1?BGFINALA:BGFINALB, GVSET);
		gv_put(mx-1, my-1, MAN2MASK, GVAND);
		gv_put(mx, my, Man2, GVOR);
		gv_put(x-1, y-1, SHIP2MASK, GVAND);
		gv_put(x, y, SHIP2, GVOR);
		lcdfreeze(0);
		while (jiffies<3);
		frame++;
	}

	/* phase 7 - man walks up into the distance size 3 */
	for (i=0; i<9; i++) {
		if (frame%3 == 0) my--;
		if (frame%5 == 0) mx--;

		jiffies=0;

		get_key();

		lcdfreeze(1);
		gv_put(102, 0, frame&1?BGFINALA:BGFINALB, GVSET);
		gv_put(mx-1, my-1, MAN3MASK, GVAND);
		gv_put(mx, my, Man3, GVOR);
		gv_put(x-1, y-1, SHIP2MASK, GVAND);
		gv_put(x, y, SHIP2, GVOR);
		lcdfreeze(0);
		while (jiffies<3);
		frame++;
	}

	x-=102;
	lcdfreeze(1);
	gv_put(0, 0, frame&1?BGFINALA:BGFINALB, GVSET);
	gv_put(x-1, y-1, SHIP2MASK, GVAND);
	gv_put(x, y, SHIP2, GVOR);
	if (Lives > 0) DrawBar();
	lcdfreeze(0);
	place(0,0);
	while (jiffies < 20);
	jiffies=0;

	if (Lives > 0) {
		/* tally up bonus points */
		bi_canmelody();
		while (Lives > 0) {
			while (jiffies < 20);
			PLAY(beep_snd);
			Lives--;
			Score+=50;
			jiffies=0;
			lcdfreeze(1);
			gv_put(0, 0, frame&1?BGFINALA:BGFINALB, GVSET);
			gv_put(x-1, y-1, SHIP2MASK, GVAND);
			gv_put(x, y, SHIP2, GVOR);
			DrawBar();
			lcdfreeze(0);
		}
		while (jiffies < 40);
	}
}

/* Game win sequence animation */
void gameWin() {
	int frame, x, y, i;
	
	jiffies=0;
	bi_canmelody();
	while (jiffies<1);
	PLAY(SNDWIN);

	gv_clear(102,0,203,63);
	place(102,0);
	frame=0;

	/* phase 1 - flyby of SMSHIP, right to left, slowly descending */
	y=1;

	for (x=204; x>79; x--) {
		jiffies=0;

		get_key();

		lcdfreeze(1);
		gv_put(102, 0, frame&1?BGFINALA:BGFINALB, GVSET);
		gv_put(x-1, y-1, SMSHIPMASK, GVAND);
		gv_put(x, y, SMSHIP, GVOR);
		lcdfreeze(0);
		while (jiffies < 3);
		frame++;
		if (frame%8 == 0) y++;
	}

	/* phase 2 - landing of SHIP1, left to right, partially */
	y=29;
	for (x=54; x<113; x++) {
		jiffies=0;

		get_key();

		lcdfreeze(1);
		gv_put(102, 0, frame&1?BGFINALA:BGFINALB, GVSET);
		gv_put(x-1, y-1, SHIP1MASK, GVAND);
		gv_put(x, y, SHIP1, GVOR);
		lcdfreeze(0);
		while (jiffies < 3);
		frame++;
		if (frame%12 == 0) y++;
	}

	while (jiffies<3);
		
	for (x=113; x<118; x++) {
		jiffies=0;

		get_key();

		lcdfreeze(1);
		gv_put(102, 0, frame&1?BGFINALA:BGFINALB, GVSET);
		gv_put(x-1, y-1, SHIP1MASK, GVAND);
		gv_put(x, y, SHIP1, GVOR);
		lcdfreeze(0);
		while (jiffies < 4);
		frame++;
		if (frame%12 == 0) y++;
	}

	while (jiffies<5);
	jiffies=0;
	get_key();
	y++;
	lcdfreeze(1);
	gv_put(102, 0, frame&1?BGFINALA:BGFINALB, GVSET);
	gv_put(x-1, y-1, SHIP2MASK, GVAND);
	gv_put(x, y, SHIP2, GVOR);
	lcdfreeze(0);
	while (jiffies<3);
	frame++;
	jiffies=0;
	get_key();
	lcdfreeze(1);
	gv_put(102, 0, frame&1?BGFINALA:BGFINALB, GVSET);
	gv_put(x-1, y-1, SHIP2MASK, GVAND);
	gv_put(x, y, SHIP2, GVOR);
	lcdfreeze(0);
	while (jiffies<3);
	frame++;
	jiffies=0;
	get_key();
	y++;
	lcdfreeze(1);
	gv_put(102, 0, frame&1?BGFINALA:BGFINALB, GVSET);
	gv_put(x-1, y-1, SHIP2MASK, GVAND);
	gv_put(x, y, SHIP2, GVOR);
	lcdfreeze(0);
	while (jiffies<3);
	frame++;

	/* phase 3 - delay */
	for (i=0; i<5; i++) {
		jiffies=0;

		get_key();

		lcdfreeze(1);
		gv_put(102, 0, frame&1?BGFINALA:BGFINALB, GVSET);
		gv_put(x-1, y-1, SHIP2MASK, GVAND);
		gv_put(x, y, SHIP2, GVOR);
		lcdfreeze(0);
		while (jiffies<3);
		frame++;
	}

	gameWin2(x,y);

	/* Window is placed back at 0,0 */

	debug("Credits loop starts...");

	i=0;
	for (;; i++) {
		jiffies=0;
		lcdfreeze(1);
		gv_put(0, 0, CONGRAT, GVSET);
		if (i) DrawBar();
		lcdfreeze(0);
			
		if (BUZZEND()) PLAY(SNDWIN);
		while (jiffies < 64) {
			if (get_key()&KEYENTER) return;
		}

		jiffies=0;
		gv_put(0, 0, CREDITS, GVSET);
		if (BUZZEND()) PLAY(SNDWIN);
		while (jiffies < 96) {
			if (get_key()&KEYENTER) return;
		}
	}
}

void newLife() {
	if (Lives < 3) {
		Lives++;
		/* TODO: Play new life sound */
	}
}

/* main function */
int main(int argc, char *argv[]) {
	int frame, lastframe, n, x, y, z, tmp, tmp2;
	int tunnelN, lasttunnelN;
	int gameSpeed, state;
	int key, ret, distanceleft;
	int numEn, idx;
	char *playershape, *fog1, *fog2;
	char *playermask;
	char buf[16];

	int playerX, playerY;			/* player ship */
	int level;

	/* init */
	screen(1);
	cls(5);
	gv_clear(0,0,101,63);
	place(0,0);

	debug("Starting up...");
	tm=bi_tmset(2, 255, 2, tm_func);		/* every second 64hz tick */

	/* check whether the user wants sound */
	for (;;) {			/* make sure neither left or right are pressed yet */
		key=get_key();
		if ((!(key&KEYLEFT))&&(!(key&KEYRIGHT))) break;
	}
	lcdfreeze(1);
	gv_put(0, 0, SoundPic, GVSET);
	lcdfreeze(0);
	debug("Waiting for sound input");
	for (;;) {
		key=get_key();
		if (key&KEYLEFT) {
			usesound=1;
			break;
		}
		if (key&KEYRIGHT) {
			usesound=0;
			break;
		}
	}
	
	/* all initialization that shutdown() cleans up should now be done */
TITLELOOP:
	jiffies=0;
	bi_canmelody();
	while (jiffies<1);
	title();

	jiffies=0;
	bi_canmelody();
	while (jiffies<1);
	debug("Initializing level...");

	frame=0;
	tunnelN=0;		/* we keep the last 2 frames to combat the slow LCD */
	lasttunnelN=0;		/* enabled only at the maximum gamespeed, though */
	key=0;

	Score=0;
	level=1;
	Lives=3;

	/* per-level stuff */
	playerX=60;
	playerY=30;
	gameSpeed=3;		/* higher = slower */

	numEn=0;
	for (x=0; x<10; x++) {
		Enemy[x].x=-1;
	}
	
	state=0;		/* states are: */
				/* 0 = Level start (lasts 32? frames) */
				/* 1 = Invincible (lasts 32? frames) */
				/* 2 = Game on */
				/* 3 = exploding */

	distanceleft=32;	/* Used in the states, too */

	fog1=BG1b;
	fog2=BG1;

	jiffies=0;

	debug("Starting main loop.");
	/* Main loop starts */
	for (;;) {
		/* Handle player control */
		playershape=Player;
		playermask=PMASK;
		key=get_key();
		if ((key&KEYLEFT)&&(playerX>2)) { playerX-=2; playershape=PlayerL; playermask=PLMASK; }
		if ((key&KEYRIGHT)&&(playerX<83)) { playerX+=2; playershape=PlayerR; playermask=PRMASK; }
		if ((key&KEYUP)&&(playerY>2)) playerY-=2;
		if ((key&KEYDOWN)&&(playerY<45)) playerY+=2;
                /* keep out of the corners */
                if ((playerY<18)&&(playerX<(18-playerY))) playerX=18-playerY;
		if ((playerY<18)&&(playerX>(83-(18-playerY)))) playerX=83-(18-playerY);
		if ((playerY>34)&&(playerX<(playerY-34))) playerX=playerY-34;
		if ((playerY>34)&&(playerX>(83-(playerY-34)))) playerX=83-(playerY-34);

		/* flash the backlight twice */
		if ((state == 3) && (distanceleft == 9)) bi_el(1);

		/* delay */
		while (jiffies < 1);

		/* draw the screen */		
		jiffies=0;
		lcdfreeze(1);
		
		gv_put(0, 0, (frame&1)?fog1:fog2, GVSET);	/* Overwrite with Fog */

		if (lasttunnelN<4) {
	                /* calculate tunnel offset for center */
			x=*Tunnel[lasttunnelN];		/* x size */
			y=*(Tunnel[lasttunnelN]+2);	/* y size */
			x=51-(x>>1);			/* centering */
			y=32-(y>>1);			/* centering */
			gv_put(x, y, Tunnel[lasttunnelN], GVOR);/* OR the tunnel */
		}
		
		if (gameSpeed == 1) {	/* double up the tunnel at top speed */
			if (tunnelN<4) {
		                /* calculate tunnel offset for center */
				x=*Tunnel[tunnelN];		/* x size */
				y=*(Tunnel[tunnelN]+2);		/* y size */
				x=51-(x>>1);			/* centering */
				y=32-(y>>1);			/* centering */
				gv_put(x, y, Tunnel[tunnelN], GVOR);/* OR the tunnel */
			}
		}

		/* Handle enemy creation and movement */
		if (numEn <= (level>>1)) {
			/* create a new one - no gaps */
			Enemy[numEn].x=(myrand())%88+1;
			Enemy[numEn].y=(myrand())%41+1;
			Enemy[numEn].type=myrand()%5;
			if (level < 5) Enemy[numEn].type=0;
			if (level == 5) Enemy[numEn].type=5;
			if ((level<7)&&(Enemy[numEn].type==2)) Enemy[numEn].type=0;
			if ((level<7)&&(Enemy[numEn].type==3)) Enemy[numEn].type=0;
			if ((level<8)&&(Enemy[numEn].type==3)) Enemy[numEn].type=0;
			if ((Enemy[numEn].type==4)&&(!(myrand()&0x2))) Enemy[numEn].type=0;

			switch (Enemy[numEn].type) {
			case 0:
				Enemy[numEn].xs=9-myrand()%level;
				Enemy[numEn].ys=9-myrand()%level;
				if (myrand()&1) Enemy[numEn].xs=-Enemy[numEn].xs;
				if (myrand()&1) Enemy[numEn].ys=-Enemy[numEn].ys;
				Enemy[numEn].pat=MeteorA;
				Enemy[numEn].mask=MAMASK;
				Enemy[numEn].z=15;	/* max distance */
				break;
			
			case 1:
				Enemy[numEn].xs=9-(playerX-Enemy[numEn].x)%level;
				Enemy[numEn].ys=9-(playerY-Enemy[numEn].y)%level;
				Enemy[numEn].pat=Enemy1a;
				Enemy[numEn].mask=E1AMASK;
				Enemy[numEn].z=7;	/* max distance */
				break;

			case 2:
				Enemy[numEn].xs=99;
				Enemy[numEn].ys=99;
				Enemy[numEn].pat=Enemy2a;
				Enemy[numEn].mask=E2AMASK;
				Enemy[numEn].z=3;	/* max distance */
				break;
				
			case 3:
				Enemy[numEn].xs=9-(playerX-Enemy[numEn].x)%level;
				Enemy[numEn].ys=9-(playerY-Enemy[numEn].y)%level;
				Enemy[numEn].pat=Enemy3a;
				Enemy[numEn].mask=E3AMASK;
				Enemy[numEn].z=7;	/* max distance */
				break;

			case 4:
				Enemy[numEn].xs=99;
				Enemy[numEn].ys=99;
				Enemy[numEn].pat=BONUS3;
				Enemy[numEn].mask=BONUS3MASK;
				Enemy[numEn].z=7;	/* max distance */
				break;

			case 5:
				Enemy[numEn].xs=99;
				Enemy[numEn].ys=99;
				Enemy[numEn].pat=COIN1_1;
				Enemy[numEn].mask=COIN1MASK;
				Enemy[numEn].z=15;	/* max distance */
				break;
			}
			Enemy[numEn].xx=abs(Enemy[numEn].xs);
			Enemy[numEn].yy=abs(Enemy[numEn].ys);
			
			numEn++;
		}

		/* Move the enemies */
		for (idx=0; idx<numEn; idx++) {
			if ((level>=4)||((level>=2)&&(frame&1))||((level<2)&&(frame%3==0))) {
                          Enemy[idx].z--;
                        }
			switch (Enemy[idx].type) {
			case 0: /* z= 16->0 */
				switch ((Enemy[idx].z) >> 2) {
				case 0:	
				case 1:	Enemy[idx].pat=MeteorC;
					Enemy[idx].mask=MCMASK;
					break;
				case 2: Enemy[idx].pat=MeteorB;
					Enemy[idx].mask=MBMASK;
					break;
				case 3: Enemy[idx].pat=MeteorA;
					Enemy[idx].mask=MAMASK;
					break;
				}
				
				if (level > 2) {
					Enemy[idx].xx--;
					if (Enemy[idx].xx<1) {
						tmp=Enemy[idx].x+sgn(Enemy[idx].xs);
						if ((tmp>87)||(tmp<1)) {
							Enemy[idx].xs=-Enemy[idx].xs;
						} else {
							Enemy[idx].x=tmp;
						}
						Enemy[idx].xx=abs(Enemy[idx].xs);
					}
					Enemy[idx].yy--;
					if (Enemy[idx].yy<1) {
						tmp=Enemy[idx].y+sgn(Enemy[idx].ys);
						if ((tmp>40)||(tmp<1)) {
							Enemy[idx].ys=-Enemy[idx].ys;
						} else {
							Enemy[idx].y=tmp;
						}
						Enemy[idx].yy=abs(Enemy[idx].ys);
					}
				}
				break;

			case 1: /* z = 7->0 */
				switch ((Enemy[idx].z) >> 1) {
				case 0:	
				case 1:	Enemy[idx].pat=Enemy1c;
					Enemy[idx].mask=E1CMASK;
					break;
				case 2: Enemy[idx].pat=Enemy1b;
					Enemy[idx].mask=E1BMASK;
					break;
				case 3: Enemy[idx].pat=Enemy1a;
					Enemy[idx].mask=E1AMASK;
					break;
				}

				Enemy[idx].xs=9-(playerX-Enemy[idx].x)%level;
				Enemy[idx].ys=9-(playerY-Enemy[idx].y)%level;
				
				Enemy[idx].xx--;
				if (Enemy[idx].xx<1) {
					tmp=Enemy[idx].x+sgn(Enemy[idx].xs);
					if ((tmp>87)||(tmp<1)) {
						Enemy[idx].xs=-Enemy[idx].xs;
					} else {
						Enemy[idx].x=tmp;
					}
					Enemy[idx].xx=abs(Enemy[idx].xs);
				}
				Enemy[idx].yy--;
				if (Enemy[idx].yy<1) {
					tmp=Enemy[idx].y+sgn(Enemy[idx].ys);
					if ((tmp>40)||(tmp<1)) {
						Enemy[idx].ys=-Enemy[idx].ys;
					} else {
						Enemy[idx].y=tmp;
					}
					Enemy[idx].yy=abs(Enemy[idx].ys);
				}
				break;

			case 2: /* z = 3->0 */
				switch (Enemy[idx].z) {
				case 0:
				case 1:	Enemy[idx].pat=Enemy2c;
					Enemy[idx].mask=E2CMASK;
					break;
				case 2: Enemy[idx].pat=Enemy2b;
					Enemy[idx].mask=E2BMASK;
					break;
				case 3: Enemy[idx].pat=Enemy2a;
					Enemy[idx].mask=E2AMASK;
					break;
				}
				break;

			case 3:	/* z = 7->0 */
				switch ((Enemy[idx].z) >> 1) {
				case 0:	
				case 1:	Enemy[idx].pat=Enemy3c;
					Enemy[idx].mask=E3CMASK;
					break;
				case 2: Enemy[idx].pat=Enemy3b;
					Enemy[idx].mask=E3BMASK;
					break;
				case 3: Enemy[idx].pat=Enemy3a;
					Enemy[idx].mask=E3AMASK;
					break;
				}

				Enemy[idx].xs=9-(120-Enemy[idx].x)%level;
				Enemy[idx].ys=9-(22-Enemy[idx].y)%level;
				
				Enemy[idx].xx--;
				if (Enemy[idx].xx<1) {
					tmp=Enemy[idx].x+sgn(Enemy[idx].xs);
					if ((tmp>87)||(tmp<1)) {
						Enemy[idx].xs=-Enemy[idx].xs;
					} else {
						Enemy[idx].x=tmp;
					}
					Enemy[idx].xx=abs(Enemy[idx].xs);
				}
				Enemy[idx].yy--;
				if (Enemy[idx].yy<1) {
					tmp=Enemy[idx].y+sgn(Enemy[idx].ys);
					if ((tmp>40)||(tmp<1)) {
						Enemy[idx].ys=-Enemy[idx].ys;
					} else {
						Enemy[idx].y=tmp;
					}
					Enemy[idx].yy=abs(Enemy[idx].ys);
				}
				break;

			case 4: /* z = 7->0 */
				switch ((Enemy[idx].z) >> 1) {
				case 0:	
				case 1:	Enemy[idx].pat=BONUS1;
					Enemy[idx].mask=BONUS1MASK;
					break;
				case 2: Enemy[idx].pat=BONUS2;
					Enemy[idx].mask=BONUS2MASK;
					break;
				case 3: Enemy[idx].pat=BONUS3;
					Enemy[idx].mask=BONUS3MASK;
					break;
				}
				break;

			case 5: /* z = 16->0 (animation timer only) */
				switch ((Enemy[idx].z >> 1)&0x03) {
				case 0:	Enemy[idx].pat=COIN1_1;
					Enemy[idx].mask=COIN1MASK;
					break;
				case 1:	Enemy[idx].pat=COIN1_2;
					Enemy[idx].mask=COIN2MASK;
					break;
				case 2: Enemy[idx].pat=COIN1_3;
					Enemy[idx].mask=COIN3MASK;
					break;
				case 3: Enemy[idx].pat=COIN1_4;
					Enemy[idx].mask=COIN4MASK;
					break;
				}
				break;
			}
		}

		for (idx=0; idx<numEn; idx++) {
			/* check for collision and off screen (z==0) */
			
			if (Enemy[idx].z < 1) {
				if (idx<9) {
					memmove(&Enemy[idx], &Enemy[idx+1], sizeof(Enemy[idx])*(9-idx));
				}
				numEn--;
				idx--;
				bi_canmelody();
				/*PLAY(SNDENEMY); //too noisy */
				continue;
			}
			
			tmp=Enemy[idx].x;
			tmp2=Enemy[idx].y;		
			gv_put(tmp-1, tmp2-1, Enemy[idx].mask, GVAND);
			gv_put(tmp, tmp2, Enemy[idx].pat, GVOR);
			
			/* Check collision - fixed 10x6 collide range */
			if ((state==2)&&(abs(Enemy[idx].x-(playerX+3))<=10)&&(abs(Enemy[idx].y-playerY)<=6)) {
				if (Enemy[idx].type == 4) {
					/* Bonus icon - change all enemies into coins */
					jiffies=0;
					bi_canmelody();
					while (jiffies<1);
					PLAY(SNDBONUS);
					Score+=10;
					if (Score/150 > (Score-10)/150) newLife();
					for (z=0; z<9; z++) {
						Enemy[z].type=5;
						Enemy[z].z=15;
					}
					Enemy[idx].z=1;
				} else if (Enemy[idx].type == 5) {
					bi_canmelody();
					PLAY(SNDBONUS);
					Score+=5;
					if (Score/150 > (Score-5)/150) newLife();
					Enemy[idx].z=1;
				} else {
					if (Enemy[idx].z == 1) {
						debug("Enemy collision!");
						jiffies=0;
						bi_canmelody();
						while (jiffies<1);
						PLAY(SNDDIE);
 					    	bi_el(1);	/* backlight on */
						state=3;
						distanceleft=10;
						Enemy[idx].z=0;
					} else {
						debug("Pending collision!");
						gv_put(0,  0, warnTL, GVOR);
						gv_put(82, 0, warnTR, GVOR);
						gv_put(82,35, warnBR, GVOR);
						gv_put(0, 35, warnBL, GVOR);
					}
				}
			}
		}

		DrawBar();

		/* now the player or level banner */
		switch (state) {
		case 0:  /* level banner */
			switch (level) {
				case 1: gv_put(0, 26, Stage1, GVSET); break;
				case 2: gv_put(0, 26, Stage2, GVSET); break;
				case 3: gv_put(0, 26, Stage3, GVSET); break;
				case 4: gv_put(0, 26, Stage4, GVSET); break;
				case 5: gv_put(0, 26, Stage5, GVSET); break;
				case 6: gv_put(0, 26, Stage6, GVSET); break;
				case 7: gv_put(0, 26, Stage7, GVSET); break;
				case 8: gv_put(0, 26, Stage8, GVSET); break;
				case 9: gv_put(0, 26, Stage9, GVSET); break;
			}
			break;

		case 1:  /* invincible */
			if ((frame&3) >= 2) break;
			/* else, fall through */

		case 2:	 /* game on regular */
                        gv_put(playerX-1, playerY-1, playermask, GVAND);
			gv_put(playerX, playerY, playershape, GVOR);
                        break;

                case 3:  /* exploding */
			switch (distanceleft>>1) {
			case 5: gv_put(playerX-1, playerY-1, EX1MASK, GVAND);
				gv_put(playerX, playerY, Explod1, GVOR); break;
			case 4: gv_put(playerX-1, playerY-1, EX2MASK, GVAND);
				gv_put(playerX, playerY, Explod2, GVOR); break;
			case 3: gv_put(playerX-1, playerY-1, EX3MASK, GVAND);
				gv_put(playerX, playerY, Explod3, GVOR); break;
			case 2: gv_put(playerX-1, playerY-1, EX4MASK, GVAND);
				gv_put(playerX, playerY, Explod4, GVOR); break;
			case 1: gv_put(playerX-1, playerY-1, EX5MASK, GVAND);
				gv_put(playerX, playerY, Explod5, GVOR); break;
			}
			if (distanceleft>=9) bi_el(0);		/* backlight off */
		}

		lcdfreeze(0);
		lasttunnelN=tunnelN;
		frame++;
		if (frame%gameSpeed == 0) {	/* Frame update speed - this one should increase for difficulty */
			tunnelN++;
			if (tunnelN>4) {
				tunnelN=0;
				if ((state == 1) || (state == 2)) {
					PLAY(beep_snd);
					Score++;
					if (Score%150 == 0) newLife();
				}
			}
		}

		distanceleft--;
		if (distanceleft < 1) {
			switch (state) {
			case 0:	state=1; distanceleft=48; break;
			case 1: state=2; distanceleft=500; break;
			case 2: /* end level, just restart faster ;) */
				if (usesound) {
					jiffies=0;
					bi_canmelody();
					while (jiffies<1);
					PLAY(SNDLEVEL);			
					while (!BUZZEND());
				}
				if (level < 9) level++; else { gameWin(); goto TITLELOOP; }
				if (((level-1)%3==0)&&(gameSpeed > 1)) gameSpeed--;
				if (level == 3) { fog2=BG2; fog1=BG2b; }
				if (level == 5) { fog2=BG3; fog1=BG3b; }
				if (level == 7) { fog2=BG4; fog1=BG4b; }
				distanceleft=32;
				state=0;
				break;
			case 3: /* end explosion, check lives left, restart level */
				Lives--;
				if (Lives >= 0) {
					state=0; distanceleft=32;
					break;
				} 
				jiffies=0;
				bi_canmelody();
				while (jiffies<1);
				PLAY(SNDLOSE);
				lcdfreeze(1);
				gv_clear(0,0,101,63);
				gv_put(0, 0, GameOver, GVSET);
				lcdfreeze(0);
				jiffies=0;
				for (;;) {
					if (usesound) {
						if (!BUZZEND()) jiffies=64;
					} 
					if (jiffies>96) break;
					if (get_key()&KEYENTER) break;
				}
				goto TITLELOOP;
			}
		}
		/* Main loop continues... */
	}
}






