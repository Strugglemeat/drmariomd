//dr mario ported to mega drive in SGDK 1.60 by Strugglemeat in Jan/Feb 2021
#include <genesis.h>
#include <resources.h>

void drawFallingPill();
void drawBGpills();
void joyEvent(u16 joy, u16 changed, u16 state);
void handleInput();
void drawScreenText();
void createVirus();
void animateVirus();
void drawGameOver(u8 which);
void checkMatches();
void adjustGraphics();
void doGravity();

typedef struct {
	u8 x;//from 0 to 7
	u8 y;//from 0 to 15
	u8 color1;//1=red,2=yellow,3=blue -- can't use 0 for red because 0 stands for a blank cell in the playfield array
	u8 color2;
	bool rotated;//0=horiz,1=vert
	Sprite* sprite1;//first section of the pill - in horiz, it's the left half. in vert, it's the bottom
	Sprite* sprite2;//second section of the pill - in horiz, it's the right half. in vert, it's the top
} Pill;

Pill fallingpill = {3,0,0,0,0,0,0};
Pill previewpill = {0,0,0,0,0,0,0};

#define VERT_TOP 0
#define VERT_BOT 1
#define HORI_LEFT 2
#define HORI_RIGHT 3

u8 playfield[9][17][2];//game board is 8x16 but in my experience using arrays in SGDK, you want to make them 1 bigger to avoid funky stuff from happening
//playfield dimensions are: 0=tile color (1,2,3), 1=frame to draw

#define playfieldX 96//96
#define playfieldY 72//80
#define wellMaxX 8
#define wellMaxY 16

u8 inputDelay,rotateDelay;
const u8 inputDelayAmt=6, downInputDelayAmt=4;
u8 displayDev=0,gameoverdrawn;
bool paused, pieceLanded=1, donedrawing=0, OKtopress;
int score, topscore=10000;
u8 level=0,virus,speed=1,music,gameState=0,menuItem;//state 0=title, state 1=options, state2=load game, state3=play game, state4=game over, state5=game won
s8 virusred,virusyel,virusblu;
char hud_string[40] = "";
u8 chain=0; u8 scoreMultiplier[11]={1,2,4,8,16,32,32,32,32,32,32};
u8 throwindex;

Sprite* virus1;
Sprite* virus2;
Sprite* virus3;
Sprite* drmario;
Sprite* drmario_dead;
Sprite* drmario_hand;

Sprite* titlelogoA;Sprite* titlelogoB;Sprite* titlefoot;Sprite* titlevirus;

Sprite* options_level;
Sprite* options_diff;
Sprite* options_musbox;
Sprite* options_musboxoff;
Sprite* optionssel1; Sprite* optionssel2; Sprite* optionssel3;

Sprite* start_spr;

u8 fallingDelay=50,fallingDelayAmt,animDelay=255;

int main()
{
VDP_setScreenWidth256();//we use 256x224 - same as NTSC NES. note that code probably needs to be changed for this to play properly in PAL regions

VDP_loadFontData(tileset_Font_Namco.tiles, 96, CPU);

SPR_init();

JOY_init();
JOY_setEventHandler(&joyEvent);

u8 titleAnim;u8 titleAnim2;
	while(1)
	{

if(gameState==0)
{
if(donedrawing==0)
	{
	VDP_drawImage(BG_B,&title,0,0);
	titlelogoA = SPR_addSprite(&title_logoA,32,55,TILE_ATTR(PAL0,0,FALSE,FALSE));
	titlelogoB = SPR_addSprite(&title_logoB,160,55,TILE_ATTR(PAL0,0,FALSE,FALSE));
	titlefoot = SPR_addSprite(&title_foot,52,192,TILE_ATTR(PAL0,0,FALSE,FALSE));
	titlevirus= SPR_addSprite(&title_virus,191,170,TILE_ATTR(PAL0,0,FALSE,FALSE));SPR_setAnim(titlevirus,1);
	SPR_update();
	XGM_setLoopNumber(-1);
	//XGM_startPlay(titlemus);
	donedrawing=1;
	}
u16 value = JOY_readJoypad(JOY_1);
if (value & BUTTON_START){gameState=1;inputDelay=inputDelayAmt;donedrawing=0;XGM_setLoopNumber(-1);XGM_startPlay(optionsmus);//VDP_clearPlane(BG_A,FALSE);
	SPR_reset();
	}

titleAnim++;titleAnim2++;
if(titleAnim>32)titleAnim=0;
if(titleAnim2>64)titleAnim2=0;
if(titleAnim==0){
	SPR_setVisibility(titlelogoA,VISIBLE);SPR_setVisibility(titlelogoB,VISIBLE);SPR_setVisibility(titlefoot,VISIBLE);
	SPR_update();}
else if(titleAnim==16) {
	SPR_setVisibility(titlelogoA,HIDDEN);SPR_setVisibility(titlelogoB,HIDDEN);SPR_setVisibility(titlefoot,HIDDEN);
	SPR_update();}

if(titleAnim2==16){SPR_setAnim(titlevirus,0);SPR_update();}//dancing LEFT
else if(titleAnim2==32){SPR_setAnim(titlevirus,1);SPR_update();}//neutral
else if(titleAnim2==48){SPR_setVisibility(titlevirus,HIDDEN);SPR_update();}//dancing RIGHT
else if(titleAnim2==64){SPR_setVisibility(titlevirus,VISIBLE);SPR_setAnim(titlevirus,1);SPR_update();}//neutral
}

if(gameState==1)
{
	if(donedrawing==0)
	{
	gameoverdrawn=0;//re-initialize after they lost
	VDP_clearPlane(BG_A,TRUE);VDP_clearPlane(BG_B,TRUE);
	VDP_drawImageEx(BG_B,&options,16,0,0,TRUE,TRUE);
	VDP_fillTileData(0, 155, 1,FALSE);
	menuItem=0;score=0;//reset everything        level=0;speed=1;music=0;
	options_level = SPR_addSprite(&optlevel,87,76,TILE_ATTR(PAL0,0,FALSE,FALSE));
	options_diff = SPR_addSprite(&optdiff,128,128,TILE_ATTR(PAL0,0,FALSE,FALSE));
	options_musbox = SPR_addSprite(&musicbox,59,179,TILE_ATTR(PAL0,0,FALSE,FALSE));
	options_musboxoff = SPR_addSprite(&musicboxoff,170,179,TILE_ATTR(PAL0,0,FALSE,FALSE));
	SPR_setVisibility(options_musboxoff,HIDDEN);
	optionssel1 = SPR_addSprite(&optionssel1spr,40,49,TILE_ATTR(PAL0,0,FALSE,FALSE));
	optionssel2 = SPR_addSprite(&optionssel2spr,40,105,TILE_ATTR(PAL0,0,FALSE,FALSE));
	SPR_setVisibility(optionssel2,HIDDEN);
	optionssel3 = SPR_addSprite(&optionssel3spr,40,153,TILE_ATTR(PAL0,0,FALSE,FALSE));
	SPR_setVisibility(optionssel3,HIDDEN);
	SPR_update();
	VDP_setPaletteColor(15,RGB24_TO_VDPCOLOR(0xFFFFFF));
	VDP_drawText("00",23,9);
	XGM_setPCM(64, rotatewav, sizeof(rotatewav));
	XGM_setPCM(65, pillmovewav, sizeof(pillmovewav));
	XGM_setPCM(66, menuchangewav, sizeof(menuchangewav));
	XGM_setPCM(67,matchwav,sizeof(matchwav));
	//XGM_setPCM(68,killviruswav,sizeof(killviruswav));
	//XGM_setPCM(68,landedwav,sizeof(landedwav));

	if(level>0){
	SPR_setPosition(options_level,87+(level*4),76);
	
	if(level<10)sprintf(hud_string,"0%d",level);
	else if(level>=10)sprintf(hud_string,"%d",level);
	VDP_drawText(hud_string,23,9);
	}

	if(speed!=0){SPR_setPosition(options_diff,88+(speed*40),128);SPR_update();}

	if(music!=0){SPR_setVisibility(options_musboxoff,VISIBLE);SPR_setVisibility(options_musbox,HIDDEN);SPR_update();}

	donedrawing=1;
	}

if(inputDelay>0)inputDelay--;
u16 value = JOY_readJoypad(JOY_1);
if(!(value & BUTTON_START))OKtopress=1;
if (value & BUTTON_START && inputDelay==0 && OKtopress==1){
	VDP_clearText(23,9,2);
	SPR_reset();
	gameState=2;
	}

if(value & BUTTON_RIGHT && inputDelay==0 && level<20 && menuItem==0){level++;inputDelay=6;SPR_setPosition(options_level,87+(level*4),76);SPR_update();
	if(level<10)sprintf(hud_string,"0%d",level);
	else if(level>=10)sprintf(hud_string,"%d",level);
	VDP_drawText(hud_string,23,9);
	XGM_startPlayPCM(65,1,SOUND_PCM_CH2);
	}
if(value & BUTTON_LEFT && inputDelay==0 && level>0 && menuItem==0){level--;inputDelay=6;SPR_setPosition(options_level,87+(level*4),76);SPR_update();
	if(level<10)sprintf(hud_string,"0%d",level);
	else if(level>=10)sprintf(hud_string,"%d",level);
	VDP_drawText(hud_string,23,9);
	XGM_startPlayPCM(65,1,SOUND_PCM_CH2);
	}

if(menuItem!=0){SPR_setVisibility(optionssel1,HIDDEN);SPR_update();}
else if(menuItem==0){SPR_setVisibility(optionssel1,VISIBLE);SPR_update();}

if(menuItem==1){SPR_setVisibility(optionssel2,VISIBLE);SPR_update();}
else if(menuItem!=1){SPR_setVisibility(optionssel2,HIDDEN);SPR_update();}

if(menuItem==2){SPR_setVisibility(optionssel3,VISIBLE);SPR_update();}
else if(menuItem!=2){SPR_setVisibility(optionssel3,HIDDEN);SPR_update();}
}

if(gameState==2){//loading the game
waitMs(20);
gameoverdrawn=0;//re-initialize after winning
VDP_drawImageEx(BG_B,&background,16,0,0,TRUE,TRUE);
#define BGend 131
VDP_loadTileSet(orangetile.tileset,8,DMA);//re-initialize after setting this as black after player won
VDP_setPaletteColor(8,RGB24_TO_VDPCOLOR(0xf0bc3c));//rescomp auto-picks a funny color for orange, fixes
VDP_setPaletteColor(2,RGB24_TO_VDPCOLOR(0x887000));//same thing but for mario's hair
if(speed==1)VDP_setPaletteColor(5,RGB24_TO_VDPCOLOR(0x44009C));//replace colors for bg in MED and HI
else if(speed==2)VDP_setPaletteColor(5,RGB24_TO_VDPCOLOR(0x747474));

VDP_loadTileSet(pillbg.tileset,BGend,DMA);//load the pill graphics into TILES after the background

if(speed==0)fallingDelayAmt=70;
else if(speed==1)fallingDelayAmt=50;
else if(speed==2)fallingDelayAmt=30;

fallingpill.sprite1 = SPR_addSprite(&pillsprite,(fallingpill.x*8)+playfieldX,(fallingpill.y*8)+playfieldY,TILE_ATTR(PAL0,0,FALSE,FALSE));
fallingpill.sprite2 = SPR_addSprite(&pillsprite,(fallingpill.x*8)+playfieldX+8,(fallingpill.y*8)+playfieldY,TILE_ATTR(PAL0,0,FALSE,FALSE));
SPR_setVisibility(fallingpill.sprite1,HIDDEN);SPR_setVisibility(fallingpill.sprite2,HIDDEN);
fallingpill.color1=(random() %  (3-1+1))+1, fallingpill.color2=(random() %  (3-1+1))+1;//drawFallingPill();

drmario = SPR_addSprite(&mario,192,69,TILE_ATTR(PAL0,1,FALSE,FALSE));
SPR_setDepth(drmario,1);
drmario_hand = SPR_addSprite(&mariohand,184,80,TILE_ATTR(PAL0,1,FALSE,FALSE));
SPR_setVisibility(drmario_hand,HIDDEN);

previewpill.sprite1 = SPR_addSprite(&pillsprite,190,62,TILE_ATTR(PAL0,0,FALSE,FALSE));
previewpill.sprite2 = SPR_addSprite(&pillsprite,198,62,TILE_ATTR(PAL0,0,FALSE,FALSE));
SPR_setDepth(previewpill.sprite1,0);
SPR_setDepth(previewpill.sprite2,0);
previewpill.color1=(random() %  (3-1+1))+1, previewpill.color2=(random() %  (3-1+1))+1;
SPR_setAnim(previewpill.sprite1,HORI_LEFT);
SPR_setAnim(previewpill.sprite2,HORI_RIGHT);
SPR_setFrame(previewpill.sprite1,previewpill.color1-1);
SPR_setFrame(previewpill.sprite2,previewpill.color2-1);

virus=(level+1)*4;

drawScreenText();

virus1= SPR_addSprite(&virusguy1,44,140,TILE_ATTR(PAL0,0,FALSE,FALSE));//red - starts at top right
virus2= SPR_addSprite(&virusguy2,32,170,TILE_ATTR(PAL0,0,FALSE,FALSE));//yel - starts at bot mid
virus3= SPR_addSprite(&virusguy3,20,140,TILE_ATTR(PAL0,0,FALSE,FALSE));//blue - starts at top left

createVirus();

start_spr = SPR_addSprite(&startspr,110,184,TILE_ATTR(PAL1,0,FALSE,FALSE));//because it was crashing the game when I put it in drawGameOver()
SPR_setVisibility(start_spr,HIDDEN);
VDP_setPalette(PAL1, startspr.palette->data);

//XGM_stopPlay();
XGM_setLoopNumber(-1);
if(music==0)XGM_startPlay(fever);
gameState=3;
	}
	if(gameState==3){//playing the game
	if(inputDelay>0)inputDelay--;
	if(rotateDelay>0)rotateDelay--;
	if(fallingDelay>0)fallingDelay--;
	if(paused==0 && gameoverdrawn==0){
		if(pieceLanded==0){
SPR_setVisibility(fallingpill.sprite1,VISIBLE);
SPR_setVisibility(fallingpill.sprite2,VISIBLE);
SPR_update();
		if(fallingpill.rotated==0 && fallingpill.y<wellMaxY-1 && playfield[fallingpill.x][fallingpill.y+1][0]==0 && playfield[fallingpill.x+1][fallingpill.y+1][0]==0 && fallingDelay==0){fallingpill.y++;fallingDelay=fallingDelayAmt;}
		else if(fallingpill.rotated==1 && fallingpill.y<wellMaxY-1 && playfield[fallingpill.x][fallingpill.y+1][0]==0 && fallingDelay==0){fallingpill.y++;fallingDelay=fallingDelayAmt;}//is the falling pill NOT at the bottom AND is it NOT directly above another pill? if so, let it fall
		
		else if(fallingpill.rotated==0 && (playfield[fallingpill.x][fallingpill.y+1][0]>0 || playfield[fallingpill.x+1][fallingpill.y+1][0]>0 || fallingpill.y==wellMaxY-1)){
			playfield[fallingpill.x][fallingpill.y][1]=fallingpill.color1+6;playfield[fallingpill.x][fallingpill.y][0]=fallingpill.color1;
			playfield[fallingpill.x+1][fallingpill.y][1]=fallingpill.color2+3+6;playfield[fallingpill.x+1][fallingpill.y][0]=fallingpill.color2;
			pieceLanded=1;fallingDelay=20;
			//XGM_startPlayPCM(XX,1,SOUND_PCM_CH2);
			SPR_setPosition(fallingpill.sprite1,(fallingpill.x*8)+playfieldX,(fallingpill.y*8)+1+playfieldY);
			SPR_setPosition(fallingpill.sprite2,((fallingpill.x+1)*8)+playfieldX,(fallingpill.y*8)+1+playfieldY);
			SPR_update();//need this to make the pill fall +1 and then come back
			waitMs(animDelay);
			checkMatches();throwindex=0;
		}
		else if(fallingpill.rotated==1 && (playfield[fallingpill.x][fallingpill.y+1][0]>0 || fallingpill.y==wellMaxY-1)){
			playfield[fallingpill.x][fallingpill.y][1]=fallingpill.color1+3;playfield[fallingpill.x][fallingpill.y][0]=fallingpill.color1;
			playfield[fallingpill.x][fallingpill.y-1][1]=fallingpill.color2;playfield[fallingpill.x][fallingpill.y-1][0]=fallingpill.color2;
			pieceLanded=1;fallingDelay=20;
			//XGM_startPlayPCM(XX,1,SOUND_PCM_CH2);
			SPR_setPosition(fallingpill.sprite1,(fallingpill.x*8)+playfieldX,(fallingpill.y*8)+1+playfieldY);
			SPR_setPosition(fallingpill.sprite2,(fallingpill.x*8)+playfieldX,((fallingpill.y-1)*8)+1+playfieldY);
			SPR_update();//need this to make the pill fall +1 and then come back
			waitMs(animDelay);
			checkMatches();throwindex=0;
		}
		if(inputDelay==0)handleInput();
		}
	drawFallingPill();

	while(throwindex<25){
	if(throwX[throwindex]<240 && throwindex%2!=0){
	SPR_setPosition(previewpill.sprite1,throwX[throwindex]+8,throwY[throwindex]-8-8);
	SPR_setPosition(previewpill.sprite2,throwX[throwindex]+8,throwY[throwindex]-8);
	SPR_setAnim(previewpill.sprite1,VERT_TOP);
	SPR_setAnim(previewpill.sprite2,VERT_BOT);
}
	else if(throwX[throwindex]<240 && throwindex%2==0){
	SPR_setPosition(previewpill.sprite1,throwX[throwindex]-8+8,throwY[throwindex]-8);
	SPR_setPosition(previewpill.sprite2,throwX[throwindex]+8,throwY[throwindex]-8);
	SPR_setAnim(previewpill.sprite1,HORI_LEFT);
	SPR_setAnim(previewpill.sprite2,HORI_RIGHT);
}
	else if(throwX[throwindex]==240)SPR_setFrame(drmario,0);
	else if(throwX[throwindex]==241){SPR_setVisibility(drmario_hand,VISIBLE);SPR_setFrame(drmario,1);}
	else if(throwX[throwindex]==242){SPR_setVisibility(drmario_hand,HIDDEN);SPR_setFrame(drmario,2);}
	else if(throwX[throwindex]==243)SPR_setFrame(drmario,0);

		SPR_setFrame(previewpill.sprite1,previewpill.color1-1);
		SPR_setFrame(previewpill.sprite2,previewpill.color2-1);

	SPR_update();
	waitMs(32);
	throwindex++;
	}
	//return it to normal:
		if(pieceLanded==1 && playfield[3][0][0]==0){
		fallingpill.rotated=0, fallingpill.x=3,fallingpill.y=0;
	SPR_setAnim(previewpill.sprite1,HORI_LEFT);
	SPR_setAnim(previewpill.sprite2,HORI_RIGHT);	
	SPR_setPosition(previewpill.sprite1,190,62);
	SPR_setPosition(previewpill.sprite2,198,62);
		fallingpill.color1=previewpill.color1; fallingpill.color2=previewpill.color2;
		previewpill.color1=(random() %  (3-1+1))+1, previewpill.color2=(random() %  (3-1+1))+1;
		SPR_setFrame(previewpill.sprite1,previewpill.color1-1);
		SPR_setFrame(previewpill.sprite2,previewpill.color2-1);
		SPR_setPosition(fallingpill.sprite1,-8,-8);
		SPR_setPosition(fallingpill.sprite2,-8,-8);
		pieceLanded=0;
		}

		if (playfield[3][0][0]>0 && pieceLanded==1){
			XGM_stopPlay();
			drawGameOver(1);
			}

		drawBGpills();
		drawScreenText();
		if(score>topscore)topscore=score;
		}
		animateVirus();
		if(displayDev>100){VDP_setTextPalette(PAL1);VDP_drawText("Strugglemeat",10,27);}
		//sprintf(hud_string,"%d",chain); VDP_drawText(hud_string,3,5); //<- chaining debug 
	}
		SYS_doVBlankProcess();
	}
	return(0);
}

void drawFallingPill()
{
SPR_setFrame(fallingpill.sprite1,fallingpill.color1-1);//can't use 0 for the color red
SPR_setFrame(fallingpill.sprite2,fallingpill.color2-1);

SPR_setPosition(fallingpill.sprite1,(fallingpill.x*8)+playfieldX,(fallingpill.y*8)+playfieldY);//first section of pill is always drawn in the same spot

if(fallingpill.rotated==0){
SPR_setAnim(fallingpill.sprite1,HORI_LEFT);
SPR_setAnim(fallingpill.sprite2,HORI_RIGHT);
SPR_setPosition(fallingpill.sprite2,(fallingpill.x*8)+playfieldX+8,(fallingpill.y*8)+playfieldY);}
else if(fallingpill.rotated==1){
SPR_setAnim(fallingpill.sprite1,VERT_BOT);
SPR_setAnim(fallingpill.sprite2,VERT_TOP);
SPR_setPosition(fallingpill.sprite2,(fallingpill.x*8)+playfieldX,(fallingpill.y*8)+playfieldY-8);}
}

u8 virusTimer;
void drawBGpills()
{//go through the entire platfield and draw the pills that are set into the playfield
u8 iX,iY;
for(iX=0;iX<wellMaxX;iX++)
	{for(iY=0;iY<wellMaxY;iY++)
		{
		if(playfield[iX][iY][0]==0 && playfield[iX][iY][1]<15)VDP_setTileMapXY(BG_A, BGend+24, (playfieldX/8)+iX, (playfieldY/8)+iY);//BGend+24 is blank
		else if(playfield[iX][iY][1]!=0 && playfield[iX][iY][1]<=18)VDP_setTileMapXY(BG_A, BGend+playfield[iX][iY][1]-1, (playfieldX/8)+iX, (playfieldY/8)+iY);
		else if(playfield[iX][iY][1]!=0 && playfield[iX][iY][1]>18 && virusTimer<=8)VDP_setTileMapXY(BG_A, BGend+playfield[iX][iY][1]-1, (playfieldX/8)+iX, (playfieldY/8)+iY);
		else if(playfield[iX][iY][1]!=0 && playfield[iX][iY][1]>18 && virusTimer>8)VDP_setTileMapXY(BG_A, BGend+playfield[iX][iY][1]-1+3, (playfieldX/8)+iX, (playfieldY/8)+iY);		
		}
	}
	virusTimer++;
	if(virusTimer>16)virusTimer=0;
}

void joyEvent(u16 joy, u16 changed, u16 state)//abc start - this allows for single press (can't hold down the button to continuously rotate)
{
u8 tempcolor=0;
if(joy==JOY_1)
{
if(gameState==1)
{
if(state & BUTTON_DOWN && inputDelay==0 && menuItem<2){menuItem++;inputDelay=6;XGM_startPlayPCM(66,1,SOUND_PCM_CH2);}
if(state & BUTTON_UP && inputDelay==0 && menuItem>0){menuItem--;inputDelay=6;XGM_startPlayPCM(66,1,SOUND_PCM_CH2);}

if(state & BUTTON_RIGHT && inputDelay==0 && speed<2 && menuItem==1){speed++;inputDelay=6;SPR_setPosition(options_diff,88+(speed*40),128);SPR_update();XGM_startPlayPCM(65,1,SOUND_PCM_CH2);}
if(state & BUTTON_LEFT && inputDelay==0 && speed>0 && menuItem==1){speed--;inputDelay=6;SPR_setPosition(options_diff,88+(speed*40),128);SPR_update();XGM_startPlayPCM(65,1,SOUND_PCM_CH2);}

if(state & BUTTON_RIGHT && inputDelay==0 && music==0 && menuItem==2){music=2;inputDelay=6;SPR_setVisibility(options_musboxoff,VISIBLE);SPR_setVisibility(options_musbox,HIDDEN);SPR_update();XGM_startPlayPCM(65,1,SOUND_PCM_CH2);}
if(state & BUTTON_LEFT && inputDelay==0 && music==2 && menuItem==2){music=0;inputDelay=6;SPR_setVisibility(options_musboxoff,HIDDEN);SPR_setVisibility(options_musbox,VISIBLE);SPR_update();XGM_startPlayPCM(65,1,SOUND_PCM_CH2);}
}
	if(gameState==3){
    if (((state & BUTTON_A) || (state & BUTTON_C)) && rotateDelay==0 && fallingpill.rotated==0 && playfield[fallingpill.x][fallingpill.y-1][0]==0 && pieceLanded==0 && fallingpill.y>0){
    	fallingpill.rotated=1;
    	rotateDelay=inputDelayAmt;
    	//drawFallingPill();
    	XGM_startPlayPCM(64,1,SOUND_PCM_CH2);
    }
    else if (((state & BUTTON_A) || (state & BUTTON_C)) && rotateDelay==0 && fallingpill.rotated==1 && playfield[fallingpill.x+1][fallingpill.y][0]==0 && fallingpill.x < wellMaxX-1 && pieceLanded==0){
    	fallingpill.rotated=0;
    	rotateDelay=inputDelayAmt;
    	tempcolor=fallingpill.color1;
    	fallingpill.color1=fallingpill.color2; 
    	fallingpill.color2=tempcolor;
    	//drawFallingPill();
    	XGM_startPlayPCM(64,1,SOUND_PCM_CH2);
    }
    else if (((state & BUTTON_A) || (state & BUTTON_C)) && rotateDelay==0 && fallingpill.rotated==1 && playfield[fallingpill.x+1][fallingpill.y][0]!=0 && playfield[fallingpill.x-1][fallingpill.y][0]==0 && pieceLanded==0){//shift position 1 to the left if we can rotate to hori and it's open
    	fallingpill.x--;
    	fallingpill.rotated=0;
    	rotateDelay=inputDelayAmt;
    	tempcolor=fallingpill.color1;
    	fallingpill.color1=fallingpill.color2; 
    	fallingpill.color2=tempcolor;
    	//drawFallingPill();
    	XGM_startPlayPCM(64,1,SOUND_PCM_CH2);
    }
    else if (((state & BUTTON_A) || (state & BUTTON_C)) && rotateDelay==0 && fallingpill.rotated==1 && playfield[fallingpill.x-1][fallingpill.y][0]==0 && fallingpill.x == wellMaxX-1 && pieceLanded==0){//shift position 1 to the left if we can rotate to hori and it's open
    	fallingpill.x--;
    	fallingpill.rotated=0;
    	rotateDelay=inputDelayAmt;
    	tempcolor=fallingpill.color1;
    	fallingpill.color1=fallingpill.color2; 
    	fallingpill.color2=tempcolor;
    	//drawFallingPill();
    	XGM_startPlayPCM(64,1,SOUND_PCM_CH2);
    }//the above is for rotating from vert to horiz while at the right wall... it SHOULD be combined into the 3rd condition, but it got too confusing and I suck

    if (state & BUTTON_START && paused==0 && inputDelay==0 && gameoverdrawn==0)//pause the game
    {
	paused=1;
	VDP_setPalette(PAL0, pausedpalette.palette->data);
	SPR_setVisibility(fallingpill.sprite1,HIDDEN);
	SPR_setVisibility(fallingpill.sprite2,HIDDEN);
	SPR_update();
	VDP_clearPlane(BG_A,FALSE);
	inputDelay=inputDelayAmt;
	XGM_pausePlay();
    }
    else if(state & BUTTON_START && paused==1 && inputDelay==0 && gameoverdrawn==0)//unpause the game
    {
    paused=0;
    SPR_setVisibility(fallingpill.sprite1,VISIBLE);
    SPR_setVisibility(fallingpill.sprite2,VISIBLE);
    SPR_update();
    VDP_setPalette(PAL0, background.palette->data);
    VDP_setPaletteColor(8,RGB24_TO_VDPCOLOR(0xf0bc3c));//rescomp auto-picks a funny color for orange, fixes
	if(speed==1)VDP_setPaletteColor(5,RGB24_TO_VDPCOLOR(0x44009C));//replace colors for bg in MED and HI
	else if(speed==2)VDP_setPaletteColor(5,RGB24_TO_VDPCOLOR(0x747474));
    VDP_clearText(12,8,6);
    inputDelay=inputDelayAmt;
    XGM_resumePlay();
    }

    if(state & BUTTON_START && gameoverdrawn==1){
    	SPR_releaseSprite(drmario_dead);
    	SPR_releaseSprite(virus1);SPR_releaseSprite(virus2);SPR_releaseSprite(virus3);
    	//SPR_releaseSprite(start_spr);
    	SPR_setVisibility(start_spr,HIDDEN);
    	donedrawing=0;
    	inputDelay=(inputDelayAmt*2);
    	//VDP_resetScreen();
    	SPR_reset();
    	gameState=1;//should be 1
    }
    else if(state & BUTTON_START && gameoverdrawn==2){
    	VDP_setTextPalette(PAL0);
    	SPR_releaseSprite(previewpill.sprite1);SPR_releaseSprite(previewpill.sprite2);//it gets re-initialized in gameState==2
    	//SPR_releaseSprite(start_spr);
    	SPR_setVisibility(start_spr,HIDDEN);
    	VDP_loadTileSet(blacktile.tileset,8,DMA);//replace orange
    	donedrawing=0;
    	level++;
    	inputDelay=inputDelayAmt;
    	//VDP_resetScreen();
    	SPR_reset();
    	gameState=2;
    }
	}
}
}

void handleInput()//dpad - this allows for continual press
{
    u16 value = JOY_readJoypad(JOY_1);

    if (value & BUTTON_LEFT && fallingpill.x>0 && playfield[fallingpill.x-1][fallingpill.y][0]==0 && fallingpill.rotated==0){fallingpill.x--;inputDelay=inputDelayAmt;XGM_startPlayPCM(65,1,SOUND_PCM_CH2);}
    else if(value & BUTTON_LEFT && fallingpill.x>0 && playfield[fallingpill.x-1][fallingpill.y][0]==0 && playfield[fallingpill.x-1][fallingpill.y-1][0]==0 && fallingpill.rotated==1){fallingpill.x--;inputDelay=inputDelayAmt;XGM_startPlayPCM(65,1,SOUND_PCM_CH2);}

    if (value & BUTTON_RIGHT && fallingpill.x<wellMaxX-2 && playfield[fallingpill.x+2][fallingpill.y][0]==0 && fallingpill.rotated==0){fallingpill.x++;inputDelay=inputDelayAmt;XGM_startPlayPCM(65,1,SOUND_PCM_CH2);}//when moving to the right while in hori, need to check 2 away from x
	else if(value & BUTTON_RIGHT && fallingpill.x<wellMaxX-1 && playfield[fallingpill.x+1][fallingpill.y][0]==0 && playfield[fallingpill.x+1][fallingpill.y-1][0]==0 && fallingpill.rotated==1){fallingpill.x++;inputDelay=inputDelayAmt;XGM_startPlayPCM(65,1,SOUND_PCM_CH2);}

    if (value & BUTTON_DOWN && fallingpill.y<wellMaxY-1 && playfield[fallingpill.x][fallingpill.y+1][0]==0 && playfield[fallingpill.x+1][fallingpill.y+1][0]==0 && fallingpill.rotated==0){fallingpill.y++;inputDelay=downInputDelayAmt;XGM_startPlayPCM(65,1,SOUND_PCM_CH2);}
	else if (value & BUTTON_DOWN && fallingpill.y<wellMaxY-1 && playfield[fallingpill.x][fallingpill.y+1][0]==0 && fallingpill.rotated==1){fallingpill.y++;inputDelay=downInputDelayAmt;XGM_startPlayPCM(65,1,SOUND_PCM_CH2);}

	if(value & BUTTON_B)displayDev++;
	else if(!(value & BUTTON_B))displayDev=0;
}

void drawScreenText()
{
if(level<10)sprintf(hud_string,"0%d",level);
if(level>9)sprintf(hud_string,"%d",level);
VDP_drawText(hud_string,27,18);

if(speed==0)sprintf(hud_string,"LOW");
else if(speed==1)sprintf(hud_string,"MED");
else if(speed==2)sprintf(hud_string," HI");
VDP_drawText(hud_string,26,21);

if(virus<10)sprintf(hud_string,"0%d",virus);
else if(virus>=10)sprintf(hud_string,"%d",virus);
VDP_drawText(hud_string,27,24);

if(topscore<=10000)sprintf(hud_string,"00%d",topscore);
else if(topscore>10000 && topscore<=100000)sprintf(hud_string,"0%d",topscore);
else if(topscore>100000)sprintf(hud_string,"%d",topscore);
VDP_drawText(hud_string,2,7);

if(score==0)VDP_drawText("0000000",2,10);
if(score>0 && score<1000){sprintf(hud_string,"%d",score);VDP_drawText(hud_string,6,10);}
else if(score>=1000 && score<10000){sprintf(hud_string,"%d",score);VDP_drawText(hud_string,5,10);}
else if(score>=10000 && score<100000){sprintf(hud_string,"%d",score);VDP_drawText(hud_string,4,10);}
else if(score>=100000){sprintf(hud_string,"%d",score);VDP_drawText(hud_string,3,10);}
//sprintf(hud_string,"%d",score); VDP_drawText(hud_string,3,5);//debug
}

void createVirus()//gameState==2 when this is called
{
virusred=0;virusyel=0;virusblu=0;//re-initialize
u8 create=virus,createX,createY,createColor;
while(create>0)
	{
		createX=(random() %  (wellMaxX-1+1));
		if(level>13)createY=(random() %  (wellMaxY-4-1+1))+4;
		else if(level>9 && level<=13)createY=(random() %  (wellMaxY-6-1+1))+6;
		else if(level<=9)createY=(random() %  (wellMaxY-8-1+1))+8;
		if(create>3)createColor=(random() %  (3-1+1))+1;
		else if(create==3)createColor=3;
		else if(create==2)createColor=2;
		else if(create==1)createColor=1;
		
		if(createColor==1)virusred++;
		else if(createColor==2)virusyel++;
		else if(createColor==3)virusblu++;

		if(playfield[createX][createY][0]==0){
		playfield[createX][createY][0]=createColor;
		playfield[createX][createY][1]=18+createColor;
		create--;drawBGpills();
		}
	}
	checkMatches();
}

u8 virusFrame,virusMoveIndex1,virusMoveIndex2=21,virusMoveIndex3=42;
void animateVirus()//for the dancing guys
{
if(virusFrame>0 && virusFrame<20){
SPR_setHFlip(virus1,TRUE);
SPR_setHFlip(virus2,TRUE);
SPR_setHFlip(virus3,TRUE);
}
else if(virusFrame>=20 && virusFrame<40){
SPR_setAnim(virus1,1);
SPR_setAnim(virus2,1);
SPR_setAnim(virus3,1);
}
else if(virusFrame>=40){
SPR_setAnim(virus1,0);SPR_setHFlip(virus1,FALSE);
SPR_setAnim(virus2,0);SPR_setHFlip(virus2,FALSE);
SPR_setAnim(virus3,0);SPR_setHFlip(virus3,FALSE);
}
virusFrame++;
if(virusFrame>60)virusFrame=1;

if(virusFrame%20==0){
virusMoveIndex1++;
virusMoveIndex2++;
virusMoveIndex3++;}
if(virusMoveIndex1>127)virusMoveIndex1=0;
else if(virusMoveIndex2>127)virusMoveIndex2=0;
else if(virusMoveIndex3>127)virusMoveIndex3=0;

SPR_setPosition(virus1,danceX[virusMoveIndex1],danceY[virusMoveIndex1]-8);
SPR_setPosition(virus2,danceX[virusMoveIndex2],danceY[virusMoveIndex2]-8);
SPR_setPosition(virus3,danceX[virusMoveIndex3],danceY[virusMoveIndex3]-8);
}

void checkMatches()
{
u8 iX,iY,conn=0,start=10,i,matched=0;

for(i=1;i<4;i++){//horizontal matches
	for(iY=wellMaxY-1;iY>0;iY--)
	{
		for(iX=0;iX<wellMaxX+1;iX++)
			{
				if(playfield[iX][iY][0]==i)conn++;
				if(playfield[iX][iY][0]==i && start==10)start=iX;

				if(playfield[iX][iY][0]!=i && conn<4){conn=0;start=10;}
				else if(playfield[iX][iY][0]!=i && conn>=4){
						while(conn>0)
						{
							
							if(i==1 && playfield[start][iY][1]>=19){virusred--;virus--;chain++;score+=((scoreMultiplier[chain-1]*100)*(speed+1));drawScreenText();}
							else if(i==2 && playfield[start][iY][1]>=19){virusyel--;virus--;chain++;score+=((scoreMultiplier[chain-1]*100)*(speed+1));drawScreenText();}
							else if(i==3 && playfield[start][iY][1]>=19){virusblu--;virus--;chain++;score+=((scoreMultiplier[chain-1]*100)*(speed+1));drawScreenText();}
							XGM_startPlayPCM(67,1,SOUND_PCM_CH2);
							playfield[start][iY][1]=15+playfield[start][iY][0];
							playfield[start][iY][0]=0;
							start++;
							conn--;
					
						}
						matched=1;
					}
}
}
}

start=20;

for(i=1;i<4;i++){//vertical matches
	for(iX=0;iX<wellMaxX;iX++)
	{
		for(iY=0;iY<wellMaxY+1;iY++)
			{
				if(playfield[iX][iY][0]==i)conn++;
				if(playfield[iX][iY][0]==i && start==20)start=iY;

				if(playfield[iX][iY][0]!=i && conn<4){conn=0;start=20;}
				else if(playfield[iX][iY][0]!=i && conn>=4){
						while(conn>0)
						{
				
							if(i==1 && playfield[iX][start][1]>=19){virusred--;virus--;chain++;score+=((scoreMultiplier[chain-1]*100)*(speed+1));drawScreenText();}
							else if(i==2 && playfield[iX][start][1]>=19){virusyel--;virus--;chain++;score+=((scoreMultiplier[chain-1]*100)*(speed+1));drawScreenText();}
							else if(i==3 && playfield[iX][start][1]>=19){virusblu--;virus--;chain++;score+=((scoreMultiplier[chain-1]*100)*(speed+1));drawScreenText();}
							XGM_startPlayPCM(67,1,SOUND_PCM_CH2);
							playfield[iX][start][1]=15+playfield[iX][start][0];
							playfield[iX][start][0]=0;
							start++;
							conn--;
						
						}
						matched=1;
					}
}
}
}

if(virusred==0){SPR_setVisibility(virus1,HIDDEN);virusred=-1;}//this needs to be updated to show an animation of the virus dying
if(virusyel==0){SPR_setVisibility(virus2,HIDDEN);virusyel=-1;}
if(virusblu==0){SPR_setVisibility(virus3,HIDDEN);virusblu=-1;}

if(virus==0)drawGameOver(2);

if(matched==1)adjustGraphics();
else if(matched==0)chain=0;
}

void adjustGraphics()//create the 'orphan' pieces if they've broken off of a partially destroyed pill
{
u8 iX,iY;
for(iX=0;iX<wellMaxX;iX++)
	{
	for(iY=0;iY<wellMaxY+1;iY++)
		{
		if(playfield[iX][iY][1]>=1 && playfield[iX][iY][1]<=3 && (playfield[iX][iY+1][1]>=7 || playfield[iX][iY+1][1]<=3))playfield[iX][iY][1]=12+playfield[iX][iY][1];
		if(playfield[iX][iY][1]>=4 && playfield[iX][iY][1]<=6 && playfield[iX][iY-1][1]>=4)playfield[iX][iY][1]=9+playfield[iX][iY][1];
		if(playfield[iX][iY][1]>=7 && playfield[iX][iY][1]<=9 && (playfield[iX+1][iY][1]<10 || playfield[iX+1][iY][1]>12))playfield[iX][iY][1]=6+playfield[iX][iY][1];
		if(playfield[iX][iY][1]>=10 && playfield[iX][iY][1]<=12 && (playfield[iX-1][iY][1]<7 || playfield[iX-1][iY][1]>9))playfield[iX][iY][1]=3+playfield[iX][iY][1];
		}
	}
SPR_setVisibility(fallingpill.sprite1,HIDDEN);//make the dropping pill sprite invisible during this process
SPR_setVisibility(fallingpill.sprite2,HIDDEN);
SPR_update();
drawBGpills();
waitMs(500);
for(iX=0;iX<wellMaxX;iX++)
	{
	for(iY=0;iY<wellMaxY+1;iY++)
		{
if(playfield[iX][iY][1]>=16 && playfield[iX][iY][1]<=18)playfield[iX][iY][1]=0;
}
}
doGravity();
}

void doGravity()
{
bool usedGravity=0;
u8 iX,iY;

do{
usedGravity=0;
for(iX=0;iX<wellMaxX;iX++)
	{
	for(iY=wellMaxY-2;iY>0;iY--)
		{
		if(playfield[iX][iY][1]>=4 && playfield[iX][iY][1]<=6 && playfield[iX][iY+1][0]==0){//whole vert pills
			playfield[iX][iY+1][0]=playfield[iX][iY][0];playfield[iX][iY+1][1]=playfield[iX][iY][1];playfield[iX][iY][0]=0;playfield[iX][iY][1]=0;
			playfield[iX][iY][0]=playfield[iX][iY-1][0];playfield[iX][iY][1]=playfield[iX][iY-1][1];playfield[iX][iY-1][0]=0;playfield[iX][iY-1][1]=0;
			usedGravity=1;}
		
		if(playfield[iX][iY][1]>=13 && playfield[iX][iY][1]<=15 && playfield[iX][iY+1][0]==0){//orphans
			playfield[iX][iY+1][0]=playfield[iX][iY][0];playfield[iX][iY+1][1]=playfield[iX][iY][1];playfield[iX][iY][0]=0;playfield[iX][iY][1]=0;
			usedGravity=1;}

		if(playfield[iX][iY][1]>=7 && playfield[iX][iY][1]<=8 && playfield[iX][iY+1][0]==0 && playfield[iX+1][iY+1][0]==0){//whole hori pills LEFT
		playfield[iX][iY+1][0]=playfield[iX][iY][0];playfield[iX][iY+1][1]=playfield[iX][iY][1];playfield[iX][iY][0]=0;playfield[iX][iY][1]=0;
		playfield[iX+1][iY+1][0]=playfield[iX+1][iY][0];playfield[iX+1][iY+1][1]=playfield[iX+1][iY][1];playfield[iX+1][iY][0]=0;playfield[iX+1][iY][1]=0;
		usedGravity=1;
		}

		if(playfield[iX][iY][1]>=10 && playfield[iX][iY][1]<=11 && playfield[iX][iY+1][0]==0 && playfield[iX-1][iY+1][0]==0){//whole hori pills RIGHT
		playfield[iX][iY+1][0]=playfield[iX][iY][0];playfield[iX][iY+1][1]=playfield[iX][iY][1];playfield[iX][iY][0]=0;playfield[iX][iY][1]=0;
		playfield[iX-1][iY+1][0]=playfield[iX-1][iY][0];playfield[iX-1][iY+1][1]=playfield[iX-1][iY][1];playfield[iX-1][iY][0]=0;playfield[iX-1][iY][1]=0;
		usedGravity=1;
		}

		}
	iY=0;
		if(playfield[iX][iY][1]>=13 && playfield[iX][iY][1]<=15 && playfield[iX][iY+1][0]==0){//orphans in top row
			playfield[iX][iY+1][0]=playfield[iX][iY][0];playfield[iX][iY+1][1]=playfield[iX][iY][1];playfield[iX][iY][0]=0;playfield[iX][iY][1]=0;
			usedGravity=1;}
	iY=0;
			if(playfield[iX][iY][1]>=4 && playfield[iX][iY][1]<=6 && playfield[iX][iY+1][0]==0){//whole vert pills
			playfield[iX][iY+1][0]=playfield[iX][iY][0];playfield[iX][iY+1][1]=playfield[iX][iY][1];playfield[iX][iY][0]=0;playfield[iX][iY][1]=0;
			playfield[iX][iY][0]=playfield[iX][iY-1][0];playfield[iX][iY][1]=playfield[iX][iY-1][1];playfield[iX][iY-1][0]=0;playfield[iX][iY-1][1]=0;
			usedGravity=1;}
	iY=0;
		if(playfield[iX][iY][1]>=7 && playfield[iX][iY][1]<=8 && playfield[iX][iY+1][0]==0 && playfield[iX+1][iY+1][0]==0){//whole hori pills
		playfield[iX][iY+1][0]=playfield[iX][iY][0];playfield[iX][iY+1][1]=playfield[iX][iY][1];playfield[iX][iY][0]=0;playfield[iX][iY][1]=0;
		playfield[iX+1][iY+1][0]=playfield[iX+1][iY][0];playfield[iX+1][iY+1][1]=playfield[iX+1][iY][1];playfield[iX+1][iY][0]=0;playfield[iX+1][iY][1]=0;
		usedGravity=1;
		}
		drawBGpills();
		waitMs(20);
	}


}while(usedGravity>0);

checkMatches();
}

u8 pulseStart;
void drawGameOver(u8 which)//1=lost,2=won
{
u8 iX,iY;
if(which==1){
			//SPR_releaseSprite(drmario);
			//SPR_releaseSprite(drmario_hand);
			SPR_setVisibility(drmario,HIDDEN);
			SPR_setVisibility(drmario_hand,HIDDEN);
			drmario_dead = SPR_addSprite(&mario_gameover,184,68,TILE_ATTR(PAL0,1,FALSE,FALSE));
			SPR_setVisibility(drmario_dead,VISIBLE);
			//SPR_releaseSprite(previewpill.sprite1);
			//SPR_releaseSprite(previewpill.sprite2);
			SPR_setVisibility(previewpill.sprite1,HIDDEN);
			SPR_setVisibility(previewpill.sprite2,HIDDEN);
			}
		//SPR_releaseSprite(fallingpill.sprite1);
		//SPR_releaseSprite(fallingpill.sprite2);
			SPR_setVisibility(fallingpill.sprite1,HIDDEN);
			SPR_setVisibility(fallingpill.sprite2,HIDDEN);
		SPR_update();


if(gameoverdrawn==0){

for(iY=wellMaxY;iY>0;iY--)
	{for(iX=0;iX<wellMaxX;iX++)
		{
			playfield[iX][iY][0]=0;
			playfield[iX][iY][1]=0;
			waitMs(4);drawBGpills();
		}
		}
for(iX=0;iX<wellMaxX;iX++)//the first row, [0]
		{
			playfield[iX][0][0]=0;
			playfield[iX][0][1]=0;
			waitMs(4);drawBGpills();
		}

drawBGpills();

if(which==1){
	VDP_drawText("GAME",14,13);
	VDP_drawText("OVER",14,15);
			}
else if(which==2){
	VDP_drawText("STAGE",13,13);
	VDP_drawText("CLEAR",14,14);
	VDP_drawText("TRY",13,16);
	VDP_drawText("NEXT",15,17);
			}
//VDP_setPaletteColor(7,RGB24_TO_VDPCOLOR(0xf0bc3c));
for(iY=12;iY<17;iY++){
for(iX=13;iX<19;iX++){
VDP_setTileMapXY(BG_B, 8, iX, iY);//orange
}
}
if(which==2){
for(iY=17;iY<19;iY++){
for(iX=13;iX<19;iX++){
VDP_setTileMapXY(BG_B, 8, iX, iY);//orange
}}}
VDP_loadTileSet(finishedtiles.tileset,1,DMA);//border
VDP_setTileMapXY(BG_A, 1, 12, 11);
for(iX=13;iX<19;iX++)VDP_setTileMapXY(BG_A, 2, iX, 11);
VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(0, 0, FALSE, TRUE, 1), 19, 11);

if(which==1){
VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(0, 0, TRUE, FALSE, 1), 12, 17);
VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(0, 0, TRUE, TRUE, 1), 19, 17);
for(iX=13;iX<19;iX++)VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(0, 0, TRUE, FALSE, 2), iX, 17);
for(iY=12;iY<17;iY++)VDP_setTileMapXY(BG_A, 3, 12, iY);
for(iY=12;iY<17;iY++)VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(0, 0, FALSE, TRUE, 3), 19, iY);
}
else if(which==2){
VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(0, 0, TRUE, FALSE, 1), 12, 19);
VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(0, 0, TRUE, TRUE, 1), 19, 19);
for(iX=13;iX<19;iX++)VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(0, 0, TRUE, FALSE, 2), iX, 19);
for(iY=12;iY<19;iY++)VDP_setTileMapXY(BG_A, 3, 12, iY);
for(iY=12;iY<19;iY++)VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(0, 0, FALSE, TRUE, 3), 19, iY);
}
gameoverdrawn=which;
}

while(gameState==3)
{
pulseStart++;
if(pulseStart>32){pulseStart=0;SPR_setVisibility(start_spr,HIDDEN);}
if(pulseStart==16)SPR_setVisibility(start_spr,VISIBLE);
animateVirus();
SPR_update();
SYS_doVBlankProcess();
}
}
