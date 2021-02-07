IMAGE background "bgpause_224.png" AUTO
IMAGE pausedpalette "paused_.png" AUTO

SPRITE pillsprite "pills_.png" 1 1 FAST 0
IMAGE pillbg "pills_and_virus.png" -1

SPRITE mario "mario_3pc_.png" 3 5 FAST 0
SPRITE mariohand "mario_throwhand.png" 1 1 FAST 0
SPRITE mario_gameover "mario_gameover.png" 5 5 FAST 0

TILESET tileset_Font_Namco "Font_Namco.png"

XGM fever "music\fever.vgm"

SPRITE virusguy1 "virusguy_red.png" 3 3 FAST 0
SPRITE virusguy2 "virusguy_yel.png" 3 3 FAST 0
SPRITE virusguy3 "virusguy_blu.png" 3 3 FAST 0
#SPRITE virusguypopspr "virusguy_disappear.png" 3 3 FAST 0

WAV rotatewav "wav\pill-rotate-32k-new-louder.wav" 0 16000
WAV pillmovewav "wav\pill-move-2x.wav" 0 16000
WAV menuchangewav "wav\menu-change-16k.wav" 0 16000
WAV matchwav "wav\match-louder-16k.wav" 0 16000
WAV killviruswav "wav\kill-virus-16k.wav" 0 16000

IMAGE title "title_.png" AUTO
SPRITE title_logoA "title_logo_A.png" 16 5 FAST 0
SPRITE title_logoB "title_logo_B.png" 7 5 FAST 0
SPRITE title_foot "title_foot.png" 2 1 FAST 0
SPRITE title_virus "title_virus_blk.png" 3 3 FAST 0

IMAGE options "options_2.png" AUTO

SPRITE optlevel "options_level.png" 1 1 FAST 0
SPRITE optdiff "options_diff.png" 3 1 FAST 0
SPRITE musicbox "options_musicbox.png" 7 3 FAST 0
SPRITE musicboxoff "options_musicbox_off.png" 5 3 FAST 0
SPRITE optionssel1spr "options_sel1.png" 13 3 FAST 0
SPRITE optionssel2spr "options_sel2.png" 7 3 FAST 0
SPRITE optionssel3spr "options_sel3.png" 12 3 FAST 0

XGM optionsmus "music\options.vgm"
XGM titlemus "music\title.vgm"

SPRITE startspr "start.png" 5 1 FAST 0

BIN danceX "virus_X.bin"
BIN danceY "virus_Y.bin"

IMAGE finishedtiles "finished_tiles_.png" NONE
IMAGE blacktile "blacktile.png" AUTO
IMAGE orangetile "orangetile_.png" AUTO

BIN throwX "throw_X.bin"
BIN throwY "throw_Y.bin"