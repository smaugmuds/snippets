From 4u2@dec5102.aarhues.dk
Date: Tue, 5 Sep 1995 10:12:32 +0100 (WET DST)
From: "Erwin S. Andreasen" <4u2@dec5102.aarhues.dk>
To: Shalazar@ASU.Edu
Cc: merc-l@netcom.com
Subject: Re: ScanCode

On Sun, 3 Sep 1995 Shalazar@ASU.Edu wrote:

>     Well, i am sure you have read the title of this message and probably
> understand what i am writing about. I was referred to this address to write
> to by Michael E Quan. I am interested in obtaining the code for scan to 
> implement on my mud that i am running. We are fairly new and still in the 
> earlier beta stages. Alas, i am in school once again and grueling through
> the homework. My life isnt as free as it used to be and this would save me
> mucho mucho time in coding. If you can help at all, i would greatly (as 
> well as my staff) appreciate it.


I assume you know how to add a do_command to your MUD, i.e. add it to the 
table in interp.c, declare it in merc.h. If you call it scan, be sure to 
add it after score in interp.c - or else SC will get you SCAN not SCORE.

Note that some lines are longer than 80 chars and will probably become 
wrapped.

This version of the classic Scan command stops as soon as it meets a 
closed door, or the room that would be scanned is the same room as you 
one scans from. I find it a bit strange to SCAN and see yourself 2 rooms 
away :)

Note that the command does work even if you are blind and stuff.. you 
might want to fine tune it a bit yourself :)

/* SCAN command by 4u2@aarhues.dk - tested with MERC 2.2 */

/* returns everything the character can see at that location in buf*/
/* returns number of creatures seen */

int scan_room (CHAR_DATA *ch, const ROOM_INDEX_DATA *room,char *buf)
{
    CHAR_DATA *target = room->people;
    int number_found = 0;

    while (target != NULL) /* repeat as long more peple in the room */
    {
        if (can_see(target,ch)) /* show only if the character can see the target */
        {
            strcat (buf, " - ");
            strcat (buf, IS_NPC(target) ? target->short_descr : target->name);
            strcat (buf, "\n\r");
            number_found++;
        }
        target = target->next_in_room;
    }

    return number_found;
}

/* main procedure */
void do_scan (CHAR_DATA *ch, char *argument)
{
    EXIT_DATA * pexit;
    ROOM_INDEX_DATA * room;
    extern char * const dir_name[];
    char buf[MAX_STRING_LENGTH];
    int dir;
    int distance;

    sprintf (buf, "Right here you see:\n\r");
    if (scan_room(ch,ch->in_room,buf) == 0)
        strcat (buf, "Noone\n\r");
    send_to_char (buf,ch);

    for (dir = 0; dir < 6; dir++) /* look in every direction */
    {
        room = ch->in_room; /* starting point */

        for (distance = 1 ; distance < 4; distance++)
        {
            pexit = room->exit[dir]; /* find the door to the next room */
            if ((pexit == NULL) || (pexit->to_room == NULL) || 
                (IS_SET(pexit->exit_info, EX_CLOSED))
               )
                break; /* exit not there OR points to nothing OR is closed */

            /* char can see the room */
            sprintf (buf, "%d %s from here you see:\n\r", distance, 
                          dir_name[dir]);
            if (scan_room(ch,pexit->to_room,buf)) /* if there is something there */
                send_to_char (buf,ch);

            room = pexit->to_room; /* go to the next room */
        } /* for distance */
    } /* for dir */
}



> -Ghent, and the RiftsMud Staff.
> (Ghent, Hanson, Dreadstar, Warchild)

Hmm.. Ghent? :) The EXPENDABLE one ? :)

---------------
Erwin Andreasen
4u2@aarhues.dk
---------------



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

 This archive came from FTP.GAME.ORG, the ultimate source for MUD resources.

------------------------------------------------------------------------------
