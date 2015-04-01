From: Joshua Schmidlkofer <menion@cyberhighway.net>

	This is a simple loop, gives you a svsp (dam/2) every loop, and sends 1
message at
	dam (X) for every hit.  You simply adjust the const table it you want
to.


This is not quite the way I would imp it, but here is a quick version:
(forgive the crudity of this code, as I did not check the syntax or
anything.)

[magic.c]
void spell_magic_missile( int sn, int level, CHAR_DATA *ch,void *vo,int
target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    static const sh_int dam_each[] = 
    {
	 0,
	 3,  3,  4,  4,  5,	 6,  6,  6,  6,  6,
	 7,  7,  7,  7,  7,	 8,  8,  8,  8,  8,
	 9,  9,  9,  9,  9,	10, 10, 10, 10, 10,
	11, 11, 11, 11, 11,	12, 12, 12, 12, 12,
	13, 13, 13, 13, 13,	14, 14, 14, 14, 14
    };
    int dam,i;

    level	= UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
    level	= UMAX(0, level);
/* 1 Missile for every ten levels of the caster */

    if(ch->level<10) i=1 else i=ch->level/10;
    
    for(i; i>0; i--) {
     dam= number_range( dam_each[level] / 2, dam_each[level] * 2 );
      if ( saves_spell( level, victim,DAM_ENERGY) )
	dam /= 2;
       damage_old( ch, victim, dam, sn, DAM_ENERGY ,TRUE);
   }
   return;
}


/*
 =============================================================================
/   ______ _______ ____   _____   ___ __    _ ______    ____  ____   _____   /
\  |  ____|__   __|  _ \ / ____\ / _ \| \  / |  ____|  / __ \|  _ \ / ____\  \
/  | |__     | |  | |_| | |     | |_| | |\/| | |___   | |  | | |_| | |       /
/  | ___|    | |  | ___/| |   __|  _  | |  | | ____|  | |  | |  __/| |   ___ \
\  | |       | |  | |   | |___| | | | | |  | | |____  | |__| | |\ \| |___| | /
/  |_|       |_|  |_|  o \_____/|_| |_|_|  |_|______|o \____/|_| \_|\_____/  \
\                                                                            /
 ============================================================================

------------------------------------------------------------------------------
ftp://ftp.game.org/pub/mud      FTP.GAME.ORG      http://www.game.org/ftpsite/
------------------------------------------------------------------------------

  This file came from FTP.GAME.ORG, the ultimate source for MUD resources.

------------------------------------------------------------------------------

*/
