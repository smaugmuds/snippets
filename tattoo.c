From: Caustik Grip Productions <cgp@ICSI.Net>

Just read a message from someone wanting players to be able to keep items
even through death, simmilar to tattoos.  We have tattoos in our mud, and
they stay on the player even if the player dies.  Here is the code
segments used to accomplish this.

(This is assuming that you have the tattoo wear location defined.)

in fight.c in function raw_kill

add in a declaration for the tattoo..  ie OBJ_DATA *tattoo;

then right before the make_corpse call, pull the tattoo out...

  tattoo = get_eq_char(victim, WEAR_TATTOO);  /* keep tattoo */
  if (tattoo != NULL) 
    obj_from_char(tattoo); 


then after the extract_char call near the end of the function put the
tattoo back on the character...

  if (tattoo != NULL) 
    {
      obj_to_char(tattoo, victim); 
      equip_char(victim, tattoo, WEAR_TATTOO); 
    }

and that is about it.  works great.


Argawal, the lord of Darkness and Deceit
The Blasted Lands
anaconda.flinet.com 9000


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
