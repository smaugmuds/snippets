/*
 * Baxter the Overlord of Deadland.
 * deadland.ada.com.tr 9000 (195.142.130.3)
 * Date: 02/20/97
 */
vodi spell_aid( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
    {
	if ( victim == ch )
	    send_to_char("You are already aided.\n\r", ch);
	else
	    act("$N is already aided.",ch,NULL,victim,TO_CHAR);
	return;
    }

    af.where	 = TO_AFFECTS;
    af.type	 = sn;
    af.level	 = level;
    af.duration	 = level/2;
    af.location	 = APPLY_HITROLL;
    af.modifier	 = 1;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    send_to_char("You feel better!\n\r", victim);
    if ( ch != victim )
	act("$N looks aided.",ch,NULL,victim,TO_CHAR);
    return;
}

/* const.c (skill_table)

    {
	"aid",			{ 53,  8, 53, 53 },	{ 0, 1, 0, 0 },
	spell_aid,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(xxx),	15,	12,
	"",			"You don't feel so aided anymore.",
	""
    }
 */



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
