/***************************************************************************
 * If you ever looked at a lot of Rom MUDs, you'd notice that the lore     *
 * skill does nothing. This is a function I wrote using bits of the        *
 * identify spell.                                                         *
 *                                                                         *
 * If you choose to use this code, please retain my name in this file and  *
 * send me an email (dwa1844@rit.edu) saying you are using it. Suggestions *
 * for improvement are welcome                                             *
 ***************************************************************************/

/* 
 * Lore written by Rahl (Daniel Anderson).
 * Can use on object anywhere in the world, but you also get less info than 
 * identify 
 */
void do_lore( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char arg[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];

    one_argument( argument, arg );

    obj = get_obj_world( ch, arg );

    /* 
     * <blush> oops. Dunno how I forgot this the first time around
     * -Rahl
     */

    if ( obj == NULL )
    {
    sprintf( buf, "You've never heard of a %s.\n\r", arg );
    send_to_char( buf, ch );
    return;
    } 

    if ( get_skill( ch, gsn_lore ) == 0 )
    {
    send_to_char( "You don't know anything about it.\n\r", ch );
    return;
    }

    if ( arg[0] == '\0' )
    {
    send_to_char( "What do you want information on?\n\r", ch );
    return;
    }

    if ( number_percent( ) < get_skill( ch, gsn_lore ) )
    {
    sprintf( buf, "'%s' is type %s, extra flags %s.\n\rLevel %d.\n\r", 
        obj->name, item_type_name(obj), extra_bit_name( obj->extra_flags),
        obj->level );
    send_to_char( buf, ch ); 

    check_improve( ch, gsn_lore, TRUE, 1 );
    }
    else
    {
    send_to_char( "You can't remember a thing about it.\n\r", ch );
        check_improve( ch, gsn_lore, FALSE, 1 );
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
