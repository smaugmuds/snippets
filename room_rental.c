/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/****************************************************************************
*	ROM 2.4 is copyright 1993-1998 Russ Taylor								*
*	ROM has been brought to you by the ROM consortium						*
*	    Russ Taylor (rtaylor@hypercube.org)									*
*	    Gabrielle Taylor (gtaylor@hypercube.org)							*
*	    Brian Moore (zump@rom.org)											*
*	By using this code, you have agreed to follow the terms of the			*
*	ROM license, in the file Rom24/doc/rom.license							*
*****************************************************************************/

/****************************************************************************
*	I've seen other innkeeper/room rental snippets out there, but they all	*
*	seemed rather generic and lacking on options.  Prior to writing this	*
*	code, all "inns" on my game consisted of an innkeeper mob programmed	*
*	to let people into a room for a certain amount of money, and the room	*
*	had a ridiculously high regen rate with a high regen bed.  This			*
*	solution wasn't very elegant, so I coded this.	What it allows is for	*
*	a player to go into a room and type rent and they will have the option	*
*	to rent the room out as a single (temporarily flags the room to			*
*	solitary) for a base amount of money, a double room (flags to private)	*
*	for a higher cost, or an "all" room which has no person limit and		*
*	charges an even higher cost.  In addition, a smaller amount of money is	*
*	charged every tick they are in the room and have it rented.  All of the	*
*	prices are also adjusted according to the player's level in order to	*
*	keep the prices scaled to level and a worthy money sink.  In order to	*
*	use this code, you must	abide by the Diku, Merc, and Rom licenses, and	*
*	may not be used in any MUD that may be considered commercial,			*
*	pay-to-play, or pay-for-perks.  No credit is required, but is certainly	*
*	appreciated if my name was added as credit in a snippet or rent			*
*	helpfile.  Code comments must remain in the do_rent and do_checkout		*
*	functions.  This code was originally released on Mudbytes at			*
*	http://www.mudbytes.net/												*
****************************************************************************/


/* Go into merc.h and look for the ROOM_ flags and add these two where ? represents
	any free letters you have: */

#define ROOM_RENT			(?)
#define ROOM_RENTED			(?)


/* In interp.c in the command table you need to add: */

	{"rent", do_rent, POS_STANDING, 0, LOG_NORMAL, 1},
	{"checkout", do_checkout, POS_STANDING, 0, LOG_NORMAL, 1},


/* In interp.h you'll need to add: */

DECLARE_DO_FUN(	do_checkout		);
DECLARE_DO_FUN(	do_rent			);



//The next few things are to make sure that the character renting the room will no longer
//be renting for any reason that might cause them to leave the room.  I think I caught them
//all, but it might be worth remembering this incase I missed something

/* In act_wiz.c go to do_transfer and add this just above or below the call to stop_fighting: */

if (IS_SET(victim->in_room->room_flags, ROOM_RENTED)
		&&  is_room_owner(victim,victim->in_room) )
	do_function(victim, &do_checkout, "");


/* In handler.c find extract_char and before the check and call for char_from_room add: */

/* clear out any rented room stuff when the character leaves the game */
if (IS_SET(ch->in_room->room_flags, ROOM_RENTED)
		&&  is_room_owner(ch,ch->in_room) )
{
	if (IS_SET(ch->in_room->room_flags, ROOM_SOLITARY))
		REMOVE_BIT(ch->in_room->room_flags, ROOM_SOLITARY);
	if (IS_SET(ch->in_room->room_flags, ROOM_PRIVATE))
		REMOVE_BIT(ch->in_room->room_flags, ROOM_PRIVATE);
	REMOVE_BIT(ch->in_room->room_flags, ROOM_RENTED);
	ch->in_room->heal_rate = 100;
	ch->in_room->mana_rate = 100;
	free_string( ch->in_room->owner );
	ch->in_room->owner = str_dup( "" );
}

/* If you have a word_of_recall spell (probably in magic.c) find it and add this near the top: */
//You might need to add a similar check in spells like gate, summon, etc.

if (IS_SET(ch->in_room->room_flags, ROOM_RENTED)
		&&  is_room_owner(ch,ch->in_room) )
{
	send_to_char("You can't leave until you checkout!\r\n",ch);
	return;
}


/* In tables.c find
const struct flag_type room_flags[]
and add: */

    { "rent",		ROOM_RENT,			TRUE	},

/* Note that we don't add 'rented' as to keep that from being able to be set by a builder in OLC.
	Its also worth noting that you probably never want to set a room to rent that is also set to
	private or solitary since the code will remove those flags upon checkout as a way of clearing
	and modifications to the room as a result of renting */


/* In update.c find the char_update function.  Just after the part where the code deals with a
	a light flickering or burning out, add: */

/* handle all of the rent stuff */
if (IS_SET(ch->in_room->room_flags, ROOM_RENTED)
		&&  is_room_owner(ch,ch->in_room) )
{
	cost = ch->level * 6;
	if (cost < 100)
		cost = 100;

	cost /= 10;

	if ( ch->silver > cost )
		ch->silver -= cost;
	else
	{
		if (IS_SET(ch->in_room->room_flags, ROOM_SOLITARY))
			REMOVE_BIT(ch->in_room->room_flags, ROOM_SOLITARY);
		if (IS_SET(ch->in_room->room_flags, ROOM_PRIVATE))
			REMOVE_BIT(ch->in_room->room_flags, ROOM_PRIVATE);
		REMOVE_BIT(ch->in_room->room_flags, ROOM_RENTED);
		ch->in_room->heal_rate = 100;
		ch->in_room->mana_rate = 100;
		free_string( ch->in_room->owner );
		ch->in_room->owner = str_dup( "" );
		send_to_char("You are kicked out of the room for insufficient funds!\r\n",ch);
	}
}


/* Finally, we go into act_move.c (with some final additions to prevent people from leaving
	the room without checking out, and this is also where I chose to put my functions for
	renting and checking out)

/* In the move_char and do_recall functions, add near the top: */

    if (IS_SET(ch->in_room->room_flags, ROOM_RENTED)
            &&  is_room_owner(ch,ch->in_room) )
    {
        send_to_char("You can't leave until you checkout!\r\n",ch);
        return;
    }

/* Anywhere in the file, at the bottom if you prefer, these are the main functions that
	handle the code: */

void do_rent( CHAR_DATA *ch, char *argument )
{
	/* Rent/Checkout commands written by Joseph, aka Diablos, of End of Time	eotmud.com:4000	*/
    char arg[MIL];
    char buf[MIL];
    CHAR_DATA *rch;
    int count = 0, cost = 0;

    argument = one_argument (argument, arg);

	//Count how many people are in the room
    for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
        if (!IS_NPC(rch))
		count++;

    if (IS_NPC(ch))
    {
        send_to_char("NPCs may not rent rooms!\r\n",ch);
        return;
    }

	//Room must be set to rent in order to be rentable
    if (!IS_SET(ch->in_room->room_flags, ROOM_RENT) )
    {
        send_to_char("This room is not for rent.\r\n",ch);
        return;
    }

    if (IS_SET(ch->in_room->room_flags, ROOM_RENTED) )
    {
        send_to_char("This room is already rented out.\r\n",ch);
        return;
    }

	//This is your base cost, adjusted by level.  If you modify this, you need to also be sure
	//to modfiy the same line in update.c as well
    cost = ch->level * 6;
    if (cost < 100)
        cost = 100;

	//Where is says "gold" you may need to change to silver, or otherwise recalculate exactly how
	//it is handled as my game handles money in a different way
    if ( arg[0] == '\0' )
    {
        sprintf(buf, "Syntax: rent <single/double/all>\r\nRental Rates: Single - %d  Gold\r\n"
                "            : Double - %d Gold\r\n            : All    - %d Gold\r\n"
                "An additional %d Gold will be charged per hour.\r\n",
                cost - (cost / 4),
                cost,
                cost + (cost / 4),
                cost / 10);
        send_to_char(buf,ch);
        return;
    }
    else if (!str_prefix (arg, "single"))
    {
        if ( ch->silver < cost * 0.75 )
        {
            send_to_char( "You can't afford it.\r\n", ch );
            return;
        }

        if (count > 1)
        {
            send_to_char( "You can't rent a single room with more than one person in the room.\r\n", ch );
            return;
        }

        ch->silver -= cost * 0.75;
        SET_BIT(ch->in_room->room_flags, ROOM_SOLITARY);
        SET_BIT(ch->in_room->room_flags, ROOM_RENTED);
        ch->in_room->heal_rate = 2000;
        ch->in_room->mana_rate = 2000;
        send_to_char( "You have rented this room.\r\n", ch );
        act("$n has rented this room as a single.",ch, NULL, NULL, TO_ROOM);
        free_string( ch->in_room->owner );
        ch->in_room->owner = str_dup( ch->name );
    }
    else if (!str_prefix (arg, "double"))
    {
        if ( ch->silver < cost )
        {
            send_to_char( "You can't afford it.\r\n", ch );
            return;
        }

        if (count > 2)
        {
            send_to_char( "You can't rent a double room with more than two people in the room.\r\n", ch );
            return;
        }

        ch->silver -= cost;
        SET_BIT(ch->in_room->room_flags, ROOM_PRIVATE);
        SET_BIT(ch->in_room->room_flags, ROOM_RENTED);
        ch->in_room->heal_rate = 2000;
        ch->in_room->mana_rate = 2000;
        send_to_char( "You have rented this room.\r\n", ch );
        act("$n has rented this room as a double.",ch, NULL, NULL, TO_ROOM);
        free_string( ch->in_room->owner );
        ch->in_room->owner = str_dup( ch->name );
    }
    else if (!str_prefix (arg, "all"))
    {
        if ( ch->silver < cost * 1.25 )
        {
            send_to_char( "You can't afford it.\r\n", ch );
            return;
        }

        ch->silver -= cost * 1.25;
        SET_BIT(ch->in_room->room_flags, ROOM_RENTED);
        ch->in_room->heal_rate = 2000;
        ch->in_room->mana_rate = 2000;
        send_to_char( "You have rented this room.\r\n", ch );
        act("$n has rented this room.",ch, NULL, NULL, TO_ROOM);
        free_string( ch->in_room->owner );
        ch->in_room->owner = str_dup( ch->name );
    }
    return;
};

void do_checkout( CHAR_DATA *ch, char *argument )
{
	/* Rent/Checkout commands written by Joseph, aka Diablos, of End of Time	eotmud.com:4000	*/
    if (IS_NPC(ch))
    {
        send_to_char("NPCs may not rent rooms!\r\n",ch);
        return;
    }

    if (!IS_SET(ch->in_room->room_flags, ROOM_RENT) )
    {
        send_to_char("This room is not for rent.\r\n",ch);
        return;
    }

    if (!IS_SET(ch->in_room->room_flags, ROOM_RENTED) )
    {
        send_to_char("This room is not rented out.\r\n",ch);
        return;
    }

    if (!is_room_owner(ch,ch->in_room))
    {
        send_to_char("You didn't rent this room out.\r\n",ch);
        return;
    }

    if (IS_SET(ch->in_room->room_flags, ROOM_SOLITARY))
        REMOVE_BIT(ch->in_room->room_flags, ROOM_SOLITARY);
    if (IS_SET(ch->in_room->room_flags, ROOM_PRIVATE))
        REMOVE_BIT(ch->in_room->room_flags, ROOM_PRIVATE);
    REMOVE_BIT(ch->in_room->room_flags, ROOM_RENTED);
    ch->in_room->heal_rate = 100;
    ch->in_room->mana_rate = 100;
    free_string( ch->in_room->owner );
    ch->in_room->owner = str_dup( "" );
    send_to_char( "You have checked out from this room.\r\n", ch );
    act("$n has checked out of this room.",ch, NULL, NULL, TO_ROOM);
    return;
};

