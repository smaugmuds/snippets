From: ferric@UWYO.EDU

Ok, here's a little piece of code I wrote up one night when I was feeling
bored.  The premise is simple.  Mud gods are really, really powerful, so
shouldn't they be able to rip people in half, then walk around wielding
the legs?  Sure they can!

Anyways, this code is freely distributable so long as the headers aren't
changed.  And I'd appreciate email from anybody who uses it, too :).
And last but not least, it's not perfect code; optimize where you see fit
:).


---------------------------------------------------------------------------

I called this sever.c....just add the sever.o to your makefile, or toss
this in one of your other files (remove the includes, in that case).

/*----------------------------------------------------*/
/* This is the Rip code, for Rom based muds. Rip a    */
/* player in half, keep their legs, and they die a    */
/* minute later, on tick.                             */
/* -Ferric@uwyo.edu				      */
/*----------------------------------------------------*/

/*----------------------------------------------------*/
/* All standard Diku/Merc/Rom copyrights, for the code*/
/* mentioned above, apply. This file freely           */
/* distributable so long as these headers  remain     */
/* intact.                                            */
/*----------------------------------------------------*/

#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include "merc.h"

void do_sever( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
       send_to_char("Who are you going to rip in half?\n\r",ch);
       return; 
    }
    
    if ( ( victim = get_char_room( ch, arg ) ) == NULL ) 
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if(victim == ch)
    {
	send_to_char("You reach your neck fine, but cant' get your ankles.\n\r",ch);
	return;
    }

    if(IS_SET(victim->aff2,AFF_SEVERED))
    {
	send_to_char("They have no legs to rip off.\n\r",ch);
	return;
    }

    SET_BIT(victim->aff2,AFF_SEVERED);
    if(!IS_NPC(victim))
    act("$n picks you up and rips you in half! Oh no!",ch,NULL,victim,TO_VICT);
    act("$n picks up $N and rips $S legs off!",ch,NULL,victim,TO_NOTVICT);
    send_to_char("You rip them in half!\n\r",ch);

    obj = create_object( get_obj_index(OBJ_VNUM_LEGS ), 0 );

    obj->timer = 5;

   
if(IS_NPC(victim))
    sprintf(buf,"A pair of %s's legs are here, twitching.",victim->short_descr);
else
    sprintf(buf,"A pair of %s's legs are here, twitching.",victim->name);	
    free_string(obj->description);
    obj->description = str_dup( buf );


if(IS_NPC(victim))
    sprintf(buf,"A pair of %s's legs",victim->short_descr);
else
    sprintf(buf,"A pair of %s's legs",victim->name);    
    free_string(obj->short_descr);
    obj->short_descr = str_dup( buf );

    obj_to_char(obj,ch);

}

/* End the independent file; modify other files as shown below.*/


Next, in merc.h you'll need to define:

#define AFF_SEVERED             (G) /*I used G, use anything you like*/
#define OBJ_VNUM_LEGS           46 /*change to mirror your legs obj vnum*/

Ok, next, in update.c (for the death on tick) add in 
void char_update(void ):


        if(IS_SET(ch->aff2,AFF_SEVERED))
        {       
            REMOVE_BIT(ch->aff2,AFF_SEVERED);
            act(
"With a last gasp of breath, $n dies due to massive\nlower body trauma.",
                ch,NULL,NULL,TO_ROOM);
            if(!IS_NPC(ch))
            send_to_char("Your injuries prove too much, and you
die.\n\r",ch);
            raw_kill(ch);
            return;
        }  /*Be sure to define raw_kill up top of update.c*/


Now, in act_move.c (in void move_char(yadda) ) add:

    if(IS_SET(ch->aff2,AFF_SEVERED))
    {
        send_to_char("Move without legs? How?\n\r",ch);
        return;
    }

Then, in act_info.c find:

    if ( victim->position == victim->start_pos 
       && victim->long_descr[0] != '\0' )
    {
        strcat( buf, victim->long_descr );
        send_to_char( buf, ch );
        return;
    }

and change it to:

    if ( victim->position == victim->start_pos 
       && !IS_SET(victim->aff2,AFF_SEVERED)
       && victim->long_descr[0] != '\0' )
    {
        strcat( buf, victim->long_descr );
        send_to_char( buf, ch );
        return;
    }

then, further down in act_info.c, find:

    strcat( buf, PERS( victim, ch ) );          
    if ( !IS_NPC(victim) && !IS_SET(ch->comm, COMM_BRIEF)
    &&   victim->position == POS_STANDING && ch->on == NULL )
        strcat( buf, victim->pcdata->title );

and directly under it add: /*I did 2 if's since I didn't care..feel free
				to neaten up. */

   if( (IS_SET(victim->aff2,AFF_SEVERED)) && IS_NPC(victim) )
   {
        sprintf(buf,"%s's upper torso is here,
twitching.\n\r",victim->short_descr);
        send_to_char(buf,ch);
        return;
    }

   if( (IS_SET(victim->aff2,AFF_SEVERED)) && !IS_NPC(victim) )
   { 
        sprintf(buf,"%s's upper torso is here,
twitching.\n\r",victim->short_descr);
        send_to_char(buf,ch);
        return;
    }


then, in interp.c add:

    
{ "rip",            do_sever,       POS_STANDING,   ML, LOG_ALWAYS, 0 },

and in interp.h add:

DECLARE_DO_FUN( do_sever        );



Next, in limbo.are (or wherever), add:

#46 /*be sure to change the vnum, and to remove this comment.*/
legs~
a pair of %s's legs~
A pair of %s's severed legs are here~
glass~
weapon P AN
sword 10 10 crush 0
35 15 0 P


And, that should wrap things up.  Hope you enjoy this novelty code.


Ferric
ferric@uwyo.edu
ferric@MelmothMUD


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
