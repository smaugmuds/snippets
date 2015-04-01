/* Explored Room Code
 * This snippet is intended for just plain old ROM, though it'll probably work on just about any codebase
 * that uses a unique number to identify the room.
 * Written By Davion Kalhen of MudBytes.net one boring Saturday morning!
 */

/***
 **In act_move.c at the top with the other #include's add
 */
#include <malloc.h>

/***
 **In act_move.c just toss these functions in at the bottom.
 */

//Check to see if a vnum has been explored
bool explored_vnum(CHAR_DATA *ch, int vnum)
{	int mask = vnum / 32; //Get which bucket the bit is in
	unsigned int bit = vnum % 32; //Get which bit in the bucket we're playing with
	EXPLORE_HOLDER *pExp; //The buckets bucket.

	if(bit == 0 ) // % 32 will return 0 if vnum == 32, instead make it the last bit of the previous mask
	{	mask--;
		bit = 32;
	}

	for(pExp = ch->pcdata->explored->bits ; pExp ; pExp = pExp->next ) //Iterate through the buckets
	{	if(pExp->mask != mask)
			continue;
		//Found the right bucket, might be explored.
		if(IS_SET(pExp->bits, ( 1 << bit ) ) ) //Convert bit to 2^(bit-1) and see if it's set.
			return TRUE;
		return FALSE; //Return immediately. This value wont be in any other bucket.
	}
	return FALSE;
}
//Explore a vnum. Assume it's not explored and just set it.
void explore_vnum(CHAR_DATA *ch, int vnum )
{	int mask = vnum / 32; //Get which bucket it will be in
	unsigned int bit = vnum % 32; // Get which bit to set
	EXPLORE_HOLDER *pExp; //The buckets bucket.

	if(bit == 0 ) // % 32 will return 0 if vnum is a multiple 32, instead make it the last bit of the previous mask
	{	mask--;
		bit = 32;
	}

	//Find the bucket.
	for(pExp = ch->pcdata->explored->bits ; pExp ; pExp = pExp->next )
		if(pExp->mask == mask)
			break;

	if(!pExp) //If it's null, bucket not found, we'll whip one up.
	{	pExp = (EXPLORE_HOLDER *)calloc(sizeof(*pExp), 1); //Alloc and zero
		pExp->mask = mask;
		pExp->next = ch->pcdata->explored->bits; //Add to
		ch->pcdata->explored->bits = pExp;       //the list
	}

	SET_BIT(pExp->bits, ( 1 << bit ) ); //Convert bit to 2^(bit-1) and set
	ch->pcdata->explored->set++; //Tell how many rooms we've explored
}
	

//Explore a vnum.
void check_explore( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoom )
{	if(IS_NPC(ch) ) return;

	
	if(explored_vnum(ch, pRoom->vnum) )
		return;

	explore_vnum(ch, pRoom->vnum);
}

void do_explored(CHAR_DATA *ch, char *argument )
{	char buf[MAX_STRING_LENGTH];
	sprintf(buf, "You have explored %d room%s!{x\r\n", ch->pcdata->explored->set, (ch->pcdata->explored->set == 1 ? "" : "s") );
	send_to_char(buf,ch);

//This shows all the rooms they've explored. Probably don't need mortals seeing this, and for immortals... it'd get really spammy. Mostly an example.
/*	for(pExp = ch->pcdata->explored->bits ; pExp ; pExp = pExp->next )
	{	for(bit = 1 ; bit <= 32 ; ++bit )
		{	if(IS_SET(pExp->bits, (1 << bit) ) )
			{	sprintf(buf, "[%-5d]", (pExp->mask * 32 + bit) );
				send_to_char(buf,ch);
			}
		}
		send_to_char("\r\n",ch);
	}
*/
	return;
}

/***
 **Now open up merc.h and add this above the struct pcdata definition.
 */

//Exploration holder.
//Basically just a linked list of int's where we only use the bits.
typedef struct explore_holder
{	struct explore_holder *next;
	unsigned int bits;
	int mask;
} EXPLORE_HOLDER;

typedef struct exploration_data
{	EXPLORE_HOLDER *bits;
	int set;
} EXPLORE_DATA;

/***
 **Add this to the bottom of the pcdata struct.
 */
	EXPLORE_DATA *explored;

/***
 **Lower down in merc.h look for the prototypes and add these under act_move.c
 */
void check_explore args( ( CHAR_DATA *, ROOM_INDEX_DATA * ) );
void explore_vnum args( (CHAR_DATA *, int ) );
bool explored_vnum args( (CHAR_DATA *, int ) );


/***
 **In recycle.c locate new_pcdata and add this right above the call to VALIDATE(pcdata);
 */

pcdata->explored = (EXPLORE_DATA *)calloc(1, sizeof(*(pcdata->explored) ) ); //Allocate explored data

/***
 **Right below in free_pcdata() add this before INVALIDATE(pcdata);
 */
	{	EXPLORE_HOLDER *pExp, *e_next;
		for(pExp = pcdata->explored->bits ; pExp ; pExp = e_next )
		{	e_next = pExp->next;
			free(pExp);
		}
	}


/***
 **Open up handler.c and find char_to_room. Add this right after a player is added to the list (pRoomIndex->people = ch;)
 */

    check_explore(ch, pRoomIndex); //Explore the room


/***
 **Open up save.c and under fwrite_char, and before it writes alias' add
 */
	if(ch->pcdata->explored->set > 0 )
	{	EXPLORE_HOLDER *pExp;

		fprintf(fp, "Explored %d\n", ch->pcdata->explored->set);
		for(pExp = ch->pcdata->explored->bits ; pExp ; pExp = pExp->next )
			fprintf(fp, "%d %d\n", pExp->mask, pExp->bits );
		fprintf(fp, "-1 -1\n" );
	}


/***
 **Look down farther for fread_char under case 'E' add this
 */
		if(!str_cmp(word, "Explored") )
		{	int mask, bit;
			EXPLORE_HOLDER *pExp;
			ch->pcdata->explored->set = fread_number(fp);
			while(1)
			{	mask = fread_number(fp);
				bit = fread_number(fp);
				if(mask == -1)
					break;
				for(pExp = ch->pcdata->explored->bits ; pExp ; pExp = pExp->next )
					if(pExp->mask == mask)
						break;
				if(!pExp)
				{	pExp = (EXPLORE_HOLDER *)calloc(1, sizeof(*pExp) );
					pExp->next = ch->pcdata->explored->bits;
					ch->pcdata->explored->bits = pExp;
					pExp->mask = mask;
				}
				pExp->bits = bit;
			}
			fMatch = TRUE;
		}

/***
 **Open up interp.c and throw this in the command table (I put mine near kill, cuz the functions are similar!)
 */
{ "explored",	do_explored,		POS_RESTING,	 0,  LOG_NORMAL, 1 },

/***
 **Add this in interp.h wherever
 */

DECLARE_DO_FUN( do_explored		);


/***
 **All you have to do now is do a clean make, and copyover! Everything should start working automatically!
 */
