From: FRiTZ <fritz1@swbell.net>

O.k.. while I was on Arkriss Last night, then went to Dennys (yea, only
place open at 2am, with half-assed decent food) I thought about bounty
code, I liked that idea, and so heres my version of the bounty command. 
After all I used a few snippetts I found floating around, so here's my
contrib..
I changed this a little, and now have it set up to be in a certain
room(the flag ROOM_BOUNTY_OFFICE), but that's up to you, and easy to
add..


MERC.H
------
under struct pc_data
add:

int					bounty;


SAVE.C
------
Under void fwrite_char()

fprintf( fp, "Bounty %d\n",	ch->pcdata->bounty  );

I put this under the line :
fprintf( fp, "Titl %s~\n",	ch->pcdata->title	);
but it shouldn't matter anywhere in there should do nicely..

Under bool load_char_obj()
I added the following line just after the title stuff again:

ch->pcdata->bounty			= 0;

Under fread_char()
find case 'B':, and in there add:

KEY( "Bounty",  ch->pcdata->bounty,	fread_number( fp ) );



FIGHT.C
-------
Under Pay off for killing things in bool damage()
I put it just after the wiznet stuff..

if (!IS_NPC(ch)
&& !IS_NPC(victim)
&& victim->pcdata->bounty > 0)
{
    sprintf(buf,"You recive a %d gold bounty, for killing %s.\n\r",
    victim->pcdata->bounty, victim->name);
    send_to_char(buf, ch);
    ch->gold += victim->pcdata->bounty;
    victim->pcdata->bounty =0;
}


ANY FILE
--------

MAIN FUNCTION
-------------
/*
 *I put this in act_comm.c but you can stick it anywhere
 */

void do_bounty( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
	
    
	if ( arg1[0] == '\0' || arg2[0] == '\0' )
        {
	     send_to_char( "Place a bounty on who's head?\n\rSyntax: Bounty
<victim> <amount>\n\r", ch );
             return;
        }
	
        if ( ( victim = get_char_world( ch, arg1 ) ) == NULL)
	{
  	   send_to_char( "They are currently not logged in!", ch );
	   return;
        }
  
      if (IS_NPC(victim))
      {
	send_to_char( "You cannot put a bounty on NPCs!", ch );
	return;
      }

	if ( is_number( arg2 ) )
        {
	int amount;
	amount   = atoi(arg2);
        if (ch->gold < amount)
        {
		send_to_char( "You don't have that much gold!", ch );
		return;
        }
	ch->gold -= amount;
	victim->pcdata->bounty +=amount;
	sprintf( buf, "You have placed a %d gold bounty on %s{g.\n\r%s now has
a bounty of %d gold.",
	amount,victim->name,victim->name,victim->pcdata->bounty );
	send_to_char(buf,ch);
	return;
	}
}


And don't forget to add it to your interpet.c and interpet.h files.

FRiTZ
fritz1@swbell.net


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
