# drmariomd
Dr. Mario ported from NES to Sega Mega Drive

project was done in SGDK v1.60

had some help from this disassembly: https://github.com/brianhuffman/drmario/blob/master/drmario.a65

thanks for Stef for making SGDK, Sik for always answering questions quickly on discord, and ohsat and danibus for making great examples.
I was also inspired by Mairtrus who did the SMB port to MD and Alekmaul who did the Puzzli port to MD.

note that this port is far from complete. here are some of the things it's missing:
* 2P VS
* ending screens
* title screen music (couldn't find a suitable MIDI to convert)
* many sound effects
* the monsters don't play their dying animation, they just disappear

and some of the bugs/problems:
* when generating a field of viruses, there are no checks done - this isn't the way the original game does it. sometimes there'll be matches from the very start, especially at higher levels
* the movement is not progressive - the pill moves the same amount regardless of how long you hold the dpad direction down. in the original game, the longer you hold down or left/right, the faster the pill will move.
* rotating the pill right before it lands can be buggy
* sometimes the music doesn't loop
* I used waitMs(500) instead of properly implementing timers, so the virus monsters stop dancing during the 'pause' of tiles being destroyed and gravity.
