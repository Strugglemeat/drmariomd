IMAGE background "image\bgpause_224.png" AUTO
IMAGE pausedpalette "image\paused_.png" AUTO

SPRITE pillsprite "sprite\pills_.png" 1 1 FAST 0
IMAGE pillbg "image\pills_and_virus.png" -1

SPRITE mario "sprite\mario_3pc_.png" 3 5 FAST 0
SPRITE mariohand "sprite\mario_throwhand.png" 1 1 FAST 0
SPRITE mario_gameover "sprite\mario_gameover.png" 5 5 FAST 0

TILESET tileset_Font_Namco "tileset\Font_Namco.png"

XGM fever "music\fever.vgm"

SPRITE virusguy1 "sprite\virusguy_red.png" 3 3 FAST 0
SPRITE virusguy2 "sprite\virusguy_yel.png" 3 3 FAST 0
SPRITE virusguy3 "sprite\virusguy_blu.png" 3 3 FAST 0
#SPRITE virusguypopspr "sprite\virusguy_disappear.png" 3 3 FAST 0

WAV rotatewav "wav\pill-rotate-32k-new-louder.wav" 0 16000
WAV pillmovewav "wav\pill-move-2x.wav" 0 16000
WAV menuchangewav "wav\menu-change-16k.wav" 0 16000
WAV matchwav "wav\match-louder-16k.wav" 0 16000
WAV killviruswav "wav\kill-virus-16k.wav" 0 16000

IMAGE title "image\title_.png" AUTO
SPRITE title_logoA "sprite\title_logo_A.png" 16 5 FAST 0
SPRITE title_logoB "sprite\title_logo_B.png" 7 5 FAST 0
SPRITE title_foot "sprite\title_foot.png" 2 1 FAST 0
SPRITE title_virus "sprite\title_virus_blk.png" 3 3 FAST 0

IMAGE options "image\options_2.png" AUTO

SPRITE optlevel "sprite\options_level.png" 1 1 FAST 0
SPRITE optdiff "sprite\options_diff.png" 3 1 FAST 0
SPRITE musicbox "sprite\options_musicbox.png" 7 3 FAST 0
SPRITE musicboxoff "sprite\options_musicbox_off.png" 5 3 FAST 0
SPRITE optionssel1spr "sprite\options_sel1.png" 13 3 FAST 0
SPRITE optionssel2spr "sprite\options_sel2.png" 7 3 FAST 0
SPRITE optionssel3spr "sprite\options_sel3.png" 12 3 FAST 0

XGM optionsmus "music\options.vgm"
XGM titlemus "music\title.vgm"

SPRITE startspr "sprite\start.png" 5 1 FAST 0

BIN danceX "data\virus_X.bin"
BIN danceY "data\virus_Y.bin"

IMAGE finishedtiles "image\finished_tiles_.png" NONE
IMAGE blacktile "image\blacktile.png" AUTO
IMAGE orangetile "image\orangetile_.png" AUTO

BIN throwX "data\throw_X.bin"
BIN throwY "data\throw_Y.bin"