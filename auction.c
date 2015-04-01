From 4u2@dec5102.aarhues.dk
Date: Wed, 30 Aug 1995 15:58:59 +0100 (WET DST)
From: "Erwin S. Andreasen" <4u2@dec5102.aarhues.dk>
To: MERC/Envy Mailing List <merc-l@webnexus.com>
Subject: CODE: an automated Auction


Withing this letter you will find my version of an automatic auction, as seen
on MUDs like Mystic Adventure (miniac.etu.gel.ulaval.ca 4000 :). I must
admit that it IS greatly inspired by exactly that great MUD :)


Changes were made in the following files:

act_obj.c: add the do_auction command.

update.c: add the update_auction function, and add this function to the end
 of the update_handler.

merc.h: declaration of the extern auction variable

db.c: auction declared here. Auction is also mallocated and item set to null
 at boot time (IMPORTANT!)

bet.h: contains advatoi, which converts an 'advanced' number contained within
 a string to a number. An advanced number can have the k and m letters to 
 signify 1000s and 1000000s. E.g.: 14k4 will translate to 14400, 200k to
 200 000 etc.
 also contains parsebet, which takes an existing number and an argument -
 this argument is parsed like an advanced number, but you can also pass it
 'x?' to multiply the existing number by ?, and +? which will add ? percent.

 I realize that those could be written better and more efficiently :)

act_comm.c: added a talk_auction, a raw function that sends an 
 "AUCTION: somethign" line to every player with Auction channel on.

 ALSO: (important) changed do_quit so if a character is byuing or selling
 something he will not quit.


Oh, and you might want to delete the original do_auction.

PLEASE NOTE:

- that you have to add the function prototypes in merc.h yourself for 
  do_auction and talk_auction and whatever else is used globally (dont 
  remember exactly what that was :) Try it out, and mdoify your files after 
  the warnings come up:)

- that you should not try this unless you have added some commands at your own
  before - this DOES *require* basic C knowledge!

- that I have based my code on standard MERC 2.2

- that I was too lazy to remove my colors codes, so you might want to remove
  all the &? thingies :) The codes are one & and exactly one character after 
  it. There's probably some mystical vi-command to do this <G>

- That it is important that the characters which are buyers or sellers are
  not extracted. Therefore they can't quit until the auction is over.
  For the same reasons, mobiles are not allowed to auction - as they can
  die (thus getting extracted) at any time.

- That an item IS returned to the character when the auction is over. You
  might want to remove that options, if someone starts to misuse this 
  to ID the items.

- That a character can bet on his own item. You might also want to remove
  that option, i.e. check if ch == auction->seller 

- that the types of items that can be auctioned are controlled by a switch
  in do_auction


There are NO guarantees. Please credit me if you use the code. I have had it
working on my home Linux system as well as my school's Ultrix.

Also tell me when (not if :) you find any bugs :) or have any suggestions :)

Enjoy! :)

PS: Syntax of the auction command is:

auction (no parameters) - will show the current item's stats
auction <item name>	- will auction an item, if no auction is going on
auction bet <bet>	- will bet on the item
auction stop		- will stop the auction, return the gold to the last
			  better and transfer the item to you (imm only).

Oh: and one thing more I thought of: consider making some changes to shutdown/
reboot, so the item being auctioned is not lost.

(hmm - some lines longer than 80 chars, hope it will transmit ok :)


/* FILE: act_obj.c - main procedure of the package */

/* put an item on auction, or see the stats on the current item or bet */
void do_auction (CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj;
    char arg1[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];

    argument = one_argument (argument, arg1);

	if (IS_NPC(ch)) /* NPC can be extracted at any time and thus can't auction! */
		return;

    if (arg1[0] == '\0')
        if (auction->item != NULL)
        {
            /* show item data here */
            if (auction->bet > 0)
                sprintf (buf, "&gCurrent bet on this item is &y%d&g gold.&*\n\r",auction->bet);
            else
                sprintf (buf, "&rNo bets on this item have been received.&*\n\r");
            send_to_char (buf,ch);
            spell_identify (0, LEVEL_HERO - 1, ch, auction->item); /* uuuh! */
            return;
        }
        else
        {
            send_to_char ("Auction WHAT?\n\r",ch);
            return;
        }

    if (IS_IMMORTAL(ch) && !str_cmp(arg1,"stop"))
    if (auction->item == NULL)
    {
        send_to_char ("There is no auction going on you can stop.\n\r",ch);
        return;
    }
    else /* stop the auction */
    {
        sprintf (buf,"Sale of %s has been stopped by God. Item confiscated.",
                        auction->item->short_descr);
        talk_auction (buf);
        obj_to_char (auction->item, ch);
        auction->item = NULL;
        if (auction->buyer != NULL) /* return money to the buyer */
        {
            auction->buyer->gold += auction->bet;
            send_to_char ("Your money has been returned.\n\r",auction->buyer);
        }
        return;
    }

    if (!str_cmp(arg1,"bet") )
        if (auction->item != NULL)
        {
            int newbet;

            /* make - perhaps - a bet now */
            if (argument[0] == '\0')
            {
                send_to_char ("Bet how much?\n\r",ch);
                return;
            }

            newbet = parsebet (auction->bet, argument);
            printf ("Bet: %d\n\r",newbet);


	    /* to avoid slow auction, use a bigger amount than 100 if the bet
 	       is higher up
            */

            if (newbet < (auction->bet + 100))
            {
                send_to_char ("You must at least bid 100 coins over the current bet.\n\r",ch);
                return;
            }

            if (newbet > ch->gold)
            {
                send_to_char ("You don't have that much money!\n\r",ch);
                return;
            }

            /* the actual bet is OK! */

            /* return the gold to the last buyer, if one exists */
            if (auction->buyer != NULL)
                auction->buyer->gold += auction->bet;

            ch->gold -= newbet; /* substract the gold - important :) */
            auction->buyer = ch;
            auction->bet   = newbet;
            auction->going = 0;
            auction->pulse = PULSE_AUCTION; /* start the auction over again */

            sprintf (buf,"A bet of &y&@%d&*&g gold has been received on &l%s&*.\n\r",newbet,auction->item->short_descr);
            talk_auction (buf);
            return;


        }
        else
        {
            send_to_char ("There isn't anything being auctioned right now.\n\r",ch);
            return;
        }

/* finally... */

    obj = get_obj_list (ch, arg1, ch->carrying); /* does char have the item ? */

    if (obj == NULL)
    {
        send_to_char ("You aren't carrying that.\n\r",ch);
        return;
    }

    if (auction->item == NULL)
    switch (obj->item_type)
    {

    default:
        act ("You cannot auction $Ts.",ch, NULL, item_type_name (obj), TO_CHAR);
        return;

/* insert any more item types here... items with a timer MAY NOT BE 
   AUCTIONED! 
*/

    case ITEM_WEAPON:
    case ITEM_ARMOR:
    case ITEM_STAFF:
    case ITEM_WAND:
    case ITEM_SCROLL:
        obj_from_char (obj);
        auction->item = obj;
        auction->bet = 0;
        auction->buyer = NULL;
        auction->seller = ch;
        auction->pulse = PULSE_AUCTION;
        auction->going = 0;

        sprintf (buf, "A new item has been received: %s.", obj->short_descr);
        talk_auction (buf);

        return;

    } /* switch */
    else
    {
        act ("Try again later - $p is being auctioned right now!",ch,auction->item,NULL,TO_CHAR);
        return;
    }
}

/* FILE: update.c */

/* the auction update - another very important part*/

void auction_update (void)
{
    char buf[MAX_STRING_LENGTH];

    if (auction->item != NULL)
        if (--auction->pulse <= 0) /* decrease pulse */
        {
            auction->pulse = PULSE_AUCTION;
            switch (++auction->going) /* increase the going state */
            {
            case 1 : /* going once */
            case 2 : /* going twice */
            if (auction->bet > 0)
                sprintf (buf, "&l%s&*: going %s for &y%d&*.", auction->item->short_descr,
                     ((auction->going == 1) ? "once" : "twice"), auction->bet);
            else
                sprintf (buf, "&l%s&*: going %s (not bet received yet).", auction->item->short_descr,
                     ((auction->going == 1) ? "once" : "twice"));

            talk_auction (buf);
            break;

            case 3 : /* SOLD! */

            if (auction->bet > 0)
            {
                sprintf (buf, "&l%s&* sold to %s for &y%d&*.",
                    auction->item->short_descr,
                    IS_NPC(auction->buyer) ? auction->buyer->short_descr : auction->buyer->name,
                    auction->bet);
                talk_auction(buf);
                obj_to_char (auction->item,auction->buyer);
                act ("The auctioneer appears before you in a puff of smoke and hands you $p.",
                     auction->buyer,auction->item,NULL,TO_CHAR);
                act ("The auctioneer appears before $n, and hands $m $p",
                     auction->buyer,auction->item,NULL,TO_ROOM);

                auction->seller->gold += auction->bet; /* give him the money */

                auction->item = NULL; /* reset item */

            }
            else /* not sold */
            {
                sprintf (buf, "No bets received for &l%s&* - object has been removed.",auction->item->short_descr);
                talk_auction(buf);
                act ("The auctioneer appears before you to return $p to you.",
                      auction->seller,auction->item,NULL,TO_CHAR);
                act ("The auctioneer appears before $n to return $p to $m.",
                      auction->seller,auction->item,NULL,TO_ROOM);
                obj_to_char (auction->item,auction->seller);
                auction->item = NULL; /* clear auction */

            } /* else */

            } /* switch */
        } /* if */
} /* func */


/* FILE: bet.h. make an include file of those 2 functions and put them in
   somewhere
*/

/*
  This function allows the following kinds of bets to be made:

  Absolute bet
  ============

  bet 14k, bet 50m66, bet 100k

  Relative bet
  ============

  These bets are calculated relative to the current bet. The '+' symbol adds
  a certain number of percent to the current bet. The default is 25, so
  with a current bet of 1000, bet + gives 1250, bet +50 gives 1500 etc.
  Please note that the number must follow exactly after the +, without any
  spaces!

  The '*' or 'x' bet multiplies the current bet by the number specified,
  defaulting to 2. If the current bet is 1000, bet x  gives 2000, bet x10
  gives 10,000 etc.

*/

int advatoi (const char *s)
/*
  util function, converts an 'advanced' ASCII-number-string into a number.
  Used by parsebet() but could also be used by do_give or do_wimpy.

  Advanced strings can contain 'k' (or 'K') and 'm' ('M') in them, not just
  numbers. The letters multiply whatever is left of them by 1,000 and
  1,000,000 respectively. Example:

  14k = 14 * 1,000 = 14,000
  23m = 23 * 1,000,0000 = 23,000,000

  If any digits follow the 'k' or 'm', the are also added, but the number
  which they are multiplied is divided by ten, each time we get one left. This
  is best illustrated in an example :)

  14k42 = 14 * 1000 + 14 * 100 + 2 * 10 = 14420

  Of course, it only pays off to use that notation when you can skip many 0's.
  There is not much point in writing 66k666 instead of 66666, except maybe
  when you want to make sure that you get 66,666.

  More than 3 (in case of 'k') or 6 ('m') digits after 'k'/'m' are automatically
  disregarded. Example:

  14k1234 = 14,123

  If the number contains any other characters than digits, 'k' or 'm', the
  function returns 0. It also returns 0 if 'k' or 'm' appear more than
  once.

*/

{

/* the pointer to buffer stuff is not really necessary, but originally I
   modified the buffer, so I had to make a copy of it. What the hell, it 
   works:) (read: it seems to work:)
*/

  char string[MAX_INPUT_LENGTH]; /* a buffer to hold a copy of the argument */
  char *stringptr = string; /* a pointer to the buffer so we can move around */
  char tempstring[2];       /* a small temp buffer to pass to atoi*/
  int number = 0;           /* number to be returned */
  int multiplier = 0;       /* multiplier used to get the extra digits right */


  strcpy (string,s);        /* working copy */

  while ( isdigit (*stringptr)) /* as long as the current character is a digit */
  {
      strncpy (tempstring,stringptr,1);           /* copy first digit */
      number = (number * 10) + atoi (tempstring); /* add to current number */
      stringptr++;                                /* advance */
  }

  switch (UPPER(*stringptr)) {
      case 'K'  : multiplier = 1000;    number *= multiplier; stringptr++; break;
      case 'M'  : multiplier = 1000000; number *= multiplier; stringptr++; break;
      case '\0' : break;
      default   : return 0; /* not k nor m nor NUL - return 0! */
  }

  while ( isdigit (*stringptr) && (multiplier > 1)) /* if any digits follow k/m, add those too */
  {
      strncpy (tempstring,stringptr,1);           /* copy first digit */
      multiplier = multiplier / 10;  /* the further we get to right, the less are the digit 'worth' */
      number = number + (atoi (tempstring) * multiplier);
      stringptr++;
  }

  if (*stringptr != '\0' && !isdigit(*stringptr)) /* a non-digit character was found, other than NUL */
    return 0; /* If a digit is found, it means the multiplier is 1 - i.e. extra
                 digits that just have to be ignore, liked 14k4443 -> 3 is ignored */


  return (number);
}


int parsebet (const int currentbet, const char *argument)
{

  int newbet = 0;                /* a variable to temporarily hold the new bet */
  char string[MAX_INPUT_LENGTH]; /* a buffer to modify the bet string */
  char *stringptr = string;      /* a pointer we can move around */

  strcpy (string,argument);      /* make a work copy of argument */


  if (*stringptr)               /* check for an empty string */
  {

    if (isdigit (*stringptr)) /* first char is a digit assume e.g. 433k */
      newbet = advatoi (stringptr); /* parse and set newbet to that value */

    else
      if (*stringptr == '+') /* add ?? percent */
      {
        if (strlen (stringptr) == 1) /* only + specified, assume default */
          newbet = (currentbet * 125) / 100; /* default: add 25% */
        else
          newbet = (currentbet * (100 + atoi (++stringptr))) / 100; /* cut off the first char */
      }
      else
        {
        printf ("considering: * x \n\r");
        if ((*stringptr == '*') || (*stringptr == 'x')) /* multiply */
          if (strlen (stringptr) == 1) /* only x specified, assume default */
            newbet = currentbet * 2 ; /* default: twice */
          else /* user specified a number */
            newbet = currentbet * atoi (++stringptr); /* cut off the first char */
        }
  }

  return newbet;        /* return the calculated bet */
}


/* FILE: act_comm.c */

/*
 * this function sends raw argument over the AUCTION: channel
 * I am not too sure if this method is right..
 */

void talk_auction (char *argument)
{
    DESCRIPTOR_DATA *d;
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *original;

    sprintf (buf,"&g&@AUCTION:&*&g %s%s", argument,"&*"); /* last %s to reset color */

    for (d = descriptor_list; d != NULL; d = d->next)
    {
        original = d->original ? d->original : d->character; /* if switched */
        if ((d->connected == CON_PLAYING) && !IS_SET(original->deaf,CHANNEL_AUCTION) )
            act (buf, original, NULL, NULL, TO_CHAR);

    }
}


/*
 * MODIFICATION to do_quit. set this in after e.g. check for stunned.
 */

    if ( auction->item != NULL && ((ch == auction->buyer) || (ch == auction->seller)) )
    {
        send_to_char ("Wait till you have sold/bought the item on auction.\n\r",ch);
        return;
    }



/* FILE: merc.h */


typedef struct  auction_data            AUCTION_DATA; /* auction data */

#define PULSE_AUCTION             (10 * PULSE_PER_SECOND)

extern          AUCTION_DATA      *     auction;

struct  auction_data
{
    OBJ_DATA  * item;   /* a pointer to the item */
    CHAR_DATA * seller; /* a pointer to the seller - which may NOT quit */
    CHAR_DATA * buyer;  /* a pointer to the buyer - which may NOT quit */
    int         bet;    /* last bet - or 0 if noone has bet anything */
    sh_int      going;  /* 1,2, sold */
    sh_int      pulse;  /* how many pulses (.25 sec) until another call-out ? */
};





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
