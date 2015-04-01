From: Robert Schultz <bert@ncinter.net>

I whipped up this code in about 5 minutes so its nothing special. Its for
immortals on any ROM based mud running with Ivans OLC code. (my setup is:
rom 2.4b4 with ivans olc 1.5a) Just add the following code in act_wiz.c and
add a declare_do_fun in interp.h along with a line for this in the interp.c
No need to mention me if you don't want to (since this code is so simple.)
but feel free to if you want to.


void do_secset(CHAR_DATA *ch, char *argument)      /* By sembiance -
bert@ncinter.net */
{
	CHAR_DATA *victim;
	char arg1[MAX_STRING_LENGTH];
	char arg2[MAX_STRING_LENGTH];
	char buf[MAX_STRING_LENGTH];
	int seclevel;

	argument = one_argument(argument, arg1);

	if (arg1[0] == '\0')
	{
		send_to_char("Set who's Security?\n\r", ch);
		return;
	}

	if ((victim = get_char_world(ch, arg1)) == NULL)
	{
		send_to_char("They're not here.\n\r", ch);
		return;
	}

	if ( IS_NPC( victim ) )
    {
		send_to_char( "You may change an NPC's security.\n\r", ch );
		return;
    }

    if ( victim->level > ch->level )
    {
    	send_to_char("You may not alter someones who is a higher level than
you.\n\r", ch);
		return;
	}

	argument = one_argument(argument, arg2);

	if (arg2[0] == '\0')
	{
		send_to_char("Set their security to what level?\n\r", ch);
		return;
	}

	seclevel = atoi(arg2);

	if( (seclevel <= 0) || (seclevel > 9) )
	{
		send_to_char("Security range is from 1 to 9.\n\r", ch);
		return;
	}

	victim->pcdata->security = seclevel;
	send_to_char("Changed players security level.\n\r", ch);
	sprintf(buf, "%s just raised your security to level %d", ch->name,
seclevel);
	send_to_char(buf, victim);

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
