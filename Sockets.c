From: John Strange <gambit@communique.net>

Here is a newer version of the new do_socket command, it was orginally
written for Envy by Stimpy, I modified it to Rom2.4 and added the extra
COMM_ cases.

/* Written by Stimpy, ported to rom2.4 by Silverhand 3/12
 *
 *	Added the other COMM_ stuff that wasn't defined before 4/16 -Silverhand
 */
void do_sockets( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *vch;
    DESCRIPTOR_DATA *d;
    char            buf  [ MAX_STRING_LENGTH ];
    char            buf2 [ MAX_STRING_LENGTH ];
    int             count;
    char *          st;
    char            s[100];
    char            idle[10];


    count       = 0;
    buf[0]      = '\0';
    buf2[0]     = '\0';

    strcat( buf2, "\n\r[Num Connected_State Login@ Idl] Player Name
Host\n\r" );
    strcat( buf2,
"--------------------------------------------------------------------------\
n\r");  
    for ( d = descriptor_list; d; d = d->next )
    {
        if ( d->character && can_see( ch, d->character ) )
        {
           /* NB: You may need to edit the CON_ values */
           switch( d->connected )
           {
              case CON_PLAYING:              st = "    PLAYING    ";    break;
              case CON_GET_NAME:             st = "   Get Name    ";    break;
              case CON_GET_OLD_PASSWORD:     st = "Get Old Passwd ";    break;
              case CON_CONFIRM_NEW_NAME:     st = " Confirm Name  ";    break;
              case CON_GET_NEW_PASSWORD:     st = "Get New Passwd ";    break;
              case CON_CONFIRM_NEW_PASSWORD: st = "Confirm Passwd ";    break;
              case CON_GET_NEW_RACE:         st = "  Get New Race ";    break;
              case CON_GET_NEW_SEX:          st = "  Get New Sex  ";    break;
              case CON_GET_NEW_CLASS:        st = " Get New Class ";    break;
              case CON_GET_ALIGNMENT:   	 st = " Get New Align ";	break;
              case CON_DEFAULT_CHOICE:		 st = " Choosing Cust ";	break;
              case CON_GEN_GROUPS:			 st = " Customization ";	break;
              case CON_PICK_WEAPON:			 st = " Picking Weapon";	break;
			  case CON_READ_IMOTD:			 st = " Reading IMOTD "; 	break;
			  case CON_BREAK_CONNECT:		 st = "   LINKDEAD    ";	break;
              case CON_READ_MOTD:            st = "  Reading MOTD ";    break;
              default:                       st = "   !UNKNOWN!   ";    break;
           }
           count++;
           
           /* Format "login" value... */
           vch = d->original ? d->original : d->character;
           strftime( s, 100, "%I:%M%p", localtime( &vch->logon ) );
           
           if ( vch->timer > 0 )
              sprintf( idle, "%-2d", vch->timer );
           else
              sprintf( idle, "  " );
           
           sprintf( buf, "[%3d %s %7s %2s] %-12s %-32.32s\n\r",
              d->descriptor,
              st,
              s,
              idle,
              ( d->original ) ? d->original->name
                              : ( d->character )  ? d->character->name
                                                  : "(None!)",
              d->host );
              
           strcat( buf2, buf );

        }
    }

    sprintf( buf, "\n\r%d user%s\n\r", count, count == 1 ? "" : "s" );
    strcat( buf2, buf );
    send_to_char( buf2, ch );
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
