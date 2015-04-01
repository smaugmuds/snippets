From: Remmelt de Haan <remmelt@kosterix.icce.rug.nl>

In this day and age of OLC and whatnot, I believe there are still
a few dinosaurs out there who have an existing base of Merc 2.0,
2.1 or 2.2. It is for these people the following snippet is
intended. The command reloads a certain help-entry. I suggest you
check the code rougly to see what it does how it reads files (which
files, etc). The help-entry itself cannot be altered online, that must
still be done offline.
In the future, this snippet will probably be on an ftp site somewhere.
Comments to remmelt@kosterix.icce.rug.nl, pls. (Also, I'd like a mail
if you start using it.)

/*----------------------------------------------------------------
 * functions for RELOAD command.  --Zarniwoop@Dutch Mountains  (4-28-97)
 * Snippet intended for Merc 2.0, 2.1 and 2.2 
 *
 * ready to go, except for two things:
 * 1. make the usual modifications for do_reload (merc.h, interp.c)
 * 2. in merc.h, after the declaration of help_first, declare help_last
 *    so it is global.
 *
 * Bugs, comments: remmelt@kosterix.icce.rug.nl
 */
bool free_helps( CHAR_DATA *ch, char *arg )
{
    char buf[MAX_STRING_LENGTH];
    HELP_DATA *h, *h_next;
    HELP_DATA *prev = NULL;
    bool found = FALSE;

    prev = help_first;
    for ( h = help_first; h; h = h_next )
    {
	h_next = h->next;

        if ( !str_cmp( h->keyword, arg ) )
        {
            free_string( h->keyword );
            free_string( h->text );
            if ( h == help_first )
                help_first = h->next;
            else
            prev->next = h->next;
            if ( h == help_last )
               help_last = prev;
            free_mem( h, sizeof(*h) );
            found = TRUE;
        }
        prev = h;
    }

    if ( !found )
    {
        sprintf( buf, "Help entry %s not found.\n\r", arg );
        send_to_char( buf, ch );
        return FALSE;
    }

    return TRUE;
}


bool read_entry( CHAR_DATA *ch, FILE *fp, char *filename, char *arg )
{
    char buf[MAX_STRING_LENGTH];
    HELP_DATA *new;
    HELP_DATA *debug;
    char *test_keyword = 0;
    char *test_text = 0;
    int test_level = 0;

    if ( fread_letter( fp ) != '#' )
    {
        sprintf( buf, "read_entry: # not in %s.\n\r", filename );
        send_to_char( buf, ch );
        return FALSE;
    }

    fread_word( fp ); /* read the word HELPS */

    new          = alloc_mem( sizeof(*new) );
    new->next    = NULL;

    for ( ; ; )
    {
        test_level   = fread_number( fp );
        test_keyword = fread_string( fp );

        if ( !str_cmp( test_keyword, "$" )) /* end of file */
        {
            free_string( test_keyword );
            free_string( test_text );
            free_mem( new, sizeof(*new) );
            return FALSE;
        }

        test_text    = fread_string( fp );

        if ( !str_cmp( test_keyword, arg ))  /* match */
        {
            new->keyword = test_keyword;
            new->level   = test_level;
            new->text    = test_text;

            debug = help_last;
            if ( help_last ) 
                help_last->next = new;
            help_last = new;
            
            sprintf( buf, "entry %s updated.\n\r", new->keyword );
            send_to_char( buf, ch );
            return TRUE;
        }
        else
        {
            free_string( test_keyword );
            free_string( test_text );
        }
    }

    return FALSE;
}


void do_reload( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    FILE *fp;
    bool found = FALSE;
    int i;

    /* in order of 'importance' */
    char *helpfile[9] = {
        "../area/news.are", "../area/motd.are", "../area/wizlist.are",
        "../area/help.are", "../area/flags.are", "../area/rules.are",
        "../area/races.are", "../area/changes.are", "../area/greeting.are",
        };

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Usage: reload <help-keyword>\n\r", ch );
        return;
    }

    /* remove help entry from the list */
    if ( !free_helps( ch, arg ))
        return;

    /* here we go */
    fclose( fpReserve );
    for ( i = 0; i < 9; i++ )
    {
        if ( ( fp = fopen( helpfile[i], "r" ) ) == NULL )
        {
            sprintf( buf, "do_reload: couldn't open %s.\n\r", helpfile[i] );
            send_to_char( buf, ch );
        }
        else
        {
            if ( ( found = read_entry( ch, fp, helpfile[i], arg ) ) == TRUE )
            {
                fclose( fp );
                break;
            }
            fclose( fp );
        }
    }
    fpReserve = fopen( NULL_FILE, "r" );
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
