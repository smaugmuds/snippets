From arrasjid@netcom.com Wed Nov  9 03:45:00 1994
Received: from mail2.netcom.com by farside.atinc.com with SMTP (5.65/1.2-eef)
	id AA18862; Wed, 9 Nov 94 03:45:00 -0500
Return-Path: <arrasjid@netcom.com>
Received: from cs.tut.fi by mail2.netcom.com (8.6.9/Netcom)
	id AAA27752; Wed, 9 Nov 1994 00:27:13 -0800
Received: from kaarne.cs.tut.fi (turtle@kaarne.cs.tut.fi [130.230.3.2]) by cs.tut.fi (8.6.9/8.6.4) with ESMTP id KAA12048 for <merc-l@netcom.com>; Wed, 9 Nov 1994 10:29:01 +0200
From: Kilpikoski Mikko <turtle@cs.tut.fi>
Received: (turtle@localhost) by kaarne.cs.tut.fi (8.6.8/8.6.4) id KAA11444 for merc-l@netcom.com; Wed, 9 Nov 1994 10:26:22 +0200
Message-Id: <199411090826.KAA11444@kaarne.cs.tut.fi>
Subject: hunt.c (faster than graph.c)
To: merc-l@netcom.com
Date: Wed, 9 Nov 1994 10:26:21 +0200 (EET)
X-Mailer: ELM [version 2.4 PL23]
Mime-Version: 1.0
Content-Type: text/plain; charset=ISO-8859-1
Content-Transfer-Encoding: 8bit
Content-Length: 16082     
Status: RO

				HUNTING
				-------

Some time ago someone posted graph.c on the list, and it was pretty nice.
The code was clean and well commented, but I found it too slow. A longer
while ago there was another hunt code posted here, and it was a lot faster.
(Though it contained a whole bunch of files and the code didn't look as nice.)
Well, here I post it again, in one file and re-indended to look a bit
cleaner.

The code was obtained from ftp.atinc.com:/pub/mud/outgoing/track.merc21.tar.gz.
It seems to originate from SillyMUD.

			Installation (Merc22)
			---------------------

The code should work as well with envies and older mercs, though some changes
may be needed. The following files will be modified:
Makefile, act_wiz.c, const.c, db.c, fight.c, interp.c, and merc.h

	Makefile
	--------

- add hunt.o to O_FILES list

	act_wiz.c
	---------

- add to do_mstat right after showing the mob spec:
    if ( IS_NPC(victim) && victim->hunting != NULL )
    {
        sprintf(buf, "Hunting victim: %s (%s)\n\r",
                IS_NPC(victim->hunting) ? victim->hunting->short_descr
                                        : victim->hunting->name,
                IS_NPC(victim->hunting) ? "MOB" : "PLAYER" );
        strcat(buf1, buf);
    }

- change the line
        send_to_char( "  thirst drunk full",                            ch );
  in do_mset to:
        send_to_char( "  thirst drunk full hunt",                       ch );

- add to do_mset:
    /* Change their hunting status. */
    else if (!str_cmp(arg2, "hunt"))
    {
        CHAR_DATA *hunted = 0;

        if ( !IS_NPC(victim) )
        {
            send_to_char( "Not on PC's.\n\r", ch );
            return;
        }

        if ( str_cmp( arg3, "." ) )
          if ( (hunted = get_char_area(victim, arg3)) == NULL )
            {
              send_to_char("Mob couldn't locate the victim to hunt.\n\r", ch);
              return;
            }

        victim->hunting = hunted;
        return;
    }

	const.c
	-------

- add skill for hunting in skill table:
    {
        "hunt",                 { 37, 37, 30, 20 },
        spell_null,             TAR_IGNORE,             POS_RESTING,
        &gsn_hunt,              SLOT( 0),        0,     12,
        "",                     "!Hunt!"
    },

	db.c
	----
- add gsn declaration:
sh_int                  gsn_hunt;

	fight.c
	-------

- add the violence_update before lines
        if ( ( victim = ch->fighting ) == NULL || ch->in_room == NULL )
            continue;
  right after lines
    for ( ch = char_list; ch != NULL; ch = ch->next )
    {
        ch_next = ch->next;
  the following:
        /*
         * Hunting mobs.
         */
        if ( IS_NPC(ch)
            && ch->fighting == NULL
            && IS_AWAKE(ch)
            && ch->hunting != NULL )
          {
            hunt_victim(ch);
            continue;
          }

	interp.c
	--------

- add command for hunting:
    { "hunt",           do_hunt,        POS_STANDING,    0,  LOG_NORMAL },

	merc.h
	------
- add hunting data to char_data structure:
    CHAR_DATA *         hunting;                /* Used by hunting routine */

- add declaration for do_hunt:
DECLARE_DO_FUN( do_hunt         );

- add hunt_victim definition:
/* hunt.c */
void    hunt_victim     args( ( CHAR_DATA *ch ) );

I hope I didn't leave anything vital out, but if anything fails to work,
feel free to consult me. I myself have the code installed in my mud, and
it works fine. The times needed to hunt down someone are really a _lot_
quicker that with graph.c.

				hunt.c
				------

And here is the hunt.c file itself:

/*
  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement
  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD

  Modifications by Rip in attempt to port to merc 2.1
*/

/*
  Modified by Turtle for Merc22 (07-Nov-94)

  I got this one from ftp.atinc.com:/pub/mud/outgoing/track.merc21.tar.gz.
  It cointained 5 files: README, hash.c, hash.h, skills.c, and skills.h.
  I combined the *.c and *.h files in this hunt.c, which should compile
  without any warnings or errors.
*/



#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"

void bcopy(char *s1,char* s2,int len);
void bzero(char *sp,int len);

extern const char * dir_name[];



struct hash_link
{
  int			key;
  struct hash_link	*next;
  void			*data;
};

struct hash_header
{
  int			rec_size;
  int			table_size;
  int			*keylist, klistsize, klistlen; /* this is really lame,
							  AMAZINGLY lame */
  struct hash_link	**buckets;
};

#define WORLD_SIZE	30000
#define	HASH_KEY(ht,key)((((unsigned int)(key))*17)%(ht)->table_size)



struct hunting_data
{
  char			*name;
  struct char_data	**victim;
};

struct room_q
{
  int		room_nr;
  struct room_q	*next_q;
};

struct nodes
{
  int	visited;
  int	ancestor;
};

#define IS_DIR		(get_room_index(q_head->room_nr)->exit[i])
#define GO_OK		(!IS_SET( IS_DIR->exit_info, EX_CLOSED ))
#define GO_OK_SMARTER	1



void init_hash_table(struct hash_header	*ht,int rec_size,int table_size)
{
  ht->rec_size	= rec_size;
  ht->table_size= table_size;
  ht->buckets	= (void*)calloc(sizeof(struct hash_link**),table_size);
  ht->keylist	= (void*)malloc(sizeof(ht->keylist)*(ht->klistsize=128));
  ht->klistlen	= 0;
}

void init_world(ROOM_INDEX_DATA *room_db[])
{
  /* zero out the world */
  bzero((char *)room_db,sizeof(ROOM_INDEX_DATA *)*WORLD_SIZE);
}

void destroy_hash_table(struct hash_header *ht,void (*gman)())
{
  int			i;
  struct hash_link	*scan,*temp;

  for(i=0;i<ht->table_size;i++)
    for(scan=ht->buckets[i];scan;)
      {
	temp = scan->next;
	(*gman)(scan->data);
	free(scan);
	scan = temp;
      }
  free(ht->buckets);
  free(ht->keylist);
}

void _hash_enter(struct hash_header *ht,int key,void *data)
{
  /* precondition: there is no entry for <key> yet */
  struct hash_link	*temp;
  int			i;

  temp		= (struct hash_link *)malloc(sizeof(struct hash_link));
  temp->key	= key;
  temp->next	= ht->buckets[HASH_KEY(ht,key)];
  temp->data	= data;
  ht->buckets[HASH_KEY(ht,key)] = temp;
  if(ht->klistlen>=ht->klistsize)
    {
      ht->keylist = (void*)realloc(ht->keylist,sizeof(*ht->keylist)*
				   (ht->klistsize*=2));
    }
  for(i=ht->klistlen;i>=0;i--)
    {
      if(ht->keylist[i-1]<key)
	{
	  ht->keylist[i] = key;
	  break;
	}
      ht->keylist[i] = ht->keylist[i-1];
    }
  ht->klistlen++;
}

ROOM_INDEX_DATA *room_find(ROOM_INDEX_DATA *room_db[],int key)
{
  return((key<WORLD_SIZE&&key>-1)?room_db[key]:0);
}

void *hash_find(struct hash_header *ht,int key)
{
  struct hash_link *scan;

  scan = ht->buckets[HASH_KEY(ht,key)];

  while(scan && scan->key!=key)
    scan = scan->next;

  return scan ? scan->data : NULL;
}

int room_enter(ROOM_INDEX_DATA *rb[],int key,ROOM_INDEX_DATA *rm)
{
  ROOM_INDEX_DATA *temp;
   
  temp = room_find(rb,key);
  if(temp) return(0);

  rb[key] = rm;
  return(1);
}

int hash_enter(struct hash_header *ht,int key,void *data)
{
  void *temp;

  temp = hash_find(ht,key);
  if(temp) return 0;

  _hash_enter(ht,key,data);
  return 1;
}

ROOM_INDEX_DATA *room_find_or_create(ROOM_INDEX_DATA *rb[],int key)
{
  ROOM_INDEX_DATA *rv;

  rv = room_find(rb,key);
  if(rv) return rv;

  rv = (ROOM_INDEX_DATA *)malloc(sizeof(ROOM_INDEX_DATA));
  rb[key] = rv;
    
  return rv;
}

void *hash_find_or_create(struct hash_header *ht,int key)
{
  void *rval;

  rval = hash_find(ht, key);
  if(rval) return rval;

  rval = (void*)malloc(ht->rec_size);
  _hash_enter(ht,key,rval);

  return rval;
}

int room_remove(ROOM_INDEX_DATA *rb[],int key)
{
  ROOM_INDEX_DATA *tmp;

  tmp = room_find(rb,key);
  if(tmp)
    {
      rb[key] = 0;
      free(tmp);
    }
  return(0);
}

void *hash_remove(struct hash_header *ht,int key)
{
  struct hash_link **scan;

  scan = ht->buckets+HASH_KEY(ht,key);

  while(*scan && (*scan)->key!=key)
    scan = &(*scan)->next;

  if(*scan)
    {
      int		i;
      struct hash_link	*temp, *aux;

      temp	= (*scan)->data;
      aux	= *scan;
      *scan	= aux->next;
      free(aux);

      for(i=0;i<ht->klistlen;i++)
	if(ht->keylist[i]==key)
	  break;

      if(i<ht->klistlen)
	{
	  bcopy((char *)ht->keylist+i+1,(char *)ht->keylist+i,(ht->klistlen-i)
		*sizeof(*ht->keylist));
	  ht->klistlen--;
	}

      return temp;
    }

  return NULL;
}

void room_iterate(ROOM_INDEX_DATA *rb[],void (*func)(),void *cdata)
{
  register int i;

  for(i=0;i<WORLD_SIZE;i++)
    {
      ROOM_INDEX_DATA *temp;
  
      temp = room_find(rb,i);
      if(temp) (*func)(i,temp,cdata);
    }
}

void hash_iterate(struct hash_header *ht,void (*func)(),void *cdata)
{
  int i;

  for(i=0;i<ht->klistlen;i++)
    {
      void		*temp;
      register int	key;

      key = ht->keylist[i];
      temp = hash_find(ht,key);
      (*func)(key,temp,cdata);
      if(ht->keylist[i]!=key) /* They must have deleted this room */
	i--;		      /* Hit this slot again. */
    }
}



int exit_ok( EXIT_DATA *pexit )
{
  ROOM_INDEX_DATA *to_room;

  if ( ( pexit == NULL )
  ||   ( to_room = pexit->to_room ) == NULL )
    return 0;

  return 1;
}

void donothing()
{
  return;
}

int find_path( int in_room_vnum, int out_room_vnum, CHAR_DATA *ch, 
	       int depth, int in_zone )
{
  struct room_q		*tmp_q, *q_head, *q_tail;
  struct hash_header	x_room;
  int			i, tmp_room, count=0, thru_doors;
  ROOM_INDEX_DATA	*herep;
  ROOM_INDEX_DATA	*startp;
  EXIT_DATA		*exitp;

  if ( depth <0 )
    {
      thru_doors = TRUE;
      depth = -depth;
    }
  else
    {
      thru_doors = FALSE;
    }

  startp = get_room_index( in_room_vnum );

  init_hash_table( &x_room, sizeof(int), 2048 );
  hash_enter( &x_room, in_room_vnum, (void *) - 1 );

  /* initialize queue */
  q_head = (struct room_q *) malloc(sizeof(struct room_q));
  q_tail = q_head;
  q_tail->room_nr = in_room_vnum;
  q_tail->next_q = 0;

  while(q_head)
    {
      herep = get_room_index( q_head->room_nr );
      /* for each room test all directions */
      if( herep->area == startp->area || !in_zone )
	{
	  /* only look in this zone...
	     saves cpu time and  makes world safer for players  */
	  for( i = 0; i <= 5; i++ )
	    {
	      exitp = herep->exit[i];
	      if( exit_ok(exitp) && ( thru_doors ? GO_OK_SMARTER : GO_OK ) )
		{
		  /* next room */
		  tmp_room = herep->exit[i]->to_room->vnum;
		  if( tmp_room != out_room_vnum )
		    {
		      /* shall we add room to queue ?
			 count determines total breadth and depth */
		      if( !hash_find( &x_room, tmp_room )
			 && ( count < depth ) )
			/* && !IS_SET( RM_FLAGS(tmp_room), DEATH ) ) */
			{
			  count++;
			  /* mark room as visted and put on queue */
			  
			  tmp_q = (struct room_q *)
			    malloc(sizeof(struct room_q));
			  tmp_q->room_nr = tmp_room;
			  tmp_q->next_q = 0;
			  q_tail->next_q = tmp_q;
			  q_tail = tmp_q;
	      
			  /* ancestor for first layer is the direction */
			  hash_enter( &x_room, tmp_room,
				     ((int)hash_find(&x_room,q_head->room_nr)
				      == -1) ? (void*)(i+1)
				     : hash_find(&x_room,q_head->room_nr));
			}
		    }
		  else
		    {
		      /* have reached our goal so free queue */
		      tmp_room = q_head->room_nr;
		      for(;q_head;q_head = tmp_q)
			{
			  tmp_q = q_head->next_q;
			  free(q_head);
			}
		      /* return direction if first layer */
		      if ((int)hash_find(&x_room,tmp_room)==-1)
			{
			  if (x_room.buckets)
			    {
			      /* junk left over from a previous track */
			      destroy_hash_table(&x_room, donothing);
			    }
			  return(i);
			}
		      else
			{
			  /* else return the ancestor */
			  int i;
			  
			  i = (int)hash_find(&x_room,tmp_room);
			  if (x_room.buckets)
			    {
			      /* junk left over from a previous track */
			      destroy_hash_table(&x_room, donothing);
			    }
			  return( -1+i);
			}
		    }
		}
	    }
	}
      
      /* free queue head and point to next entry */
      tmp_q = q_head->next_q;
      free(q_head);
      q_head = tmp_q;
    }

  /* couldn't find path */
  if( x_room.buckets )
    {
      /* junk left over from a previous track */
      destroy_hash_table( &x_room, donothing );
    }
  return -1;
}



void do_hunt( CHAR_DATA *ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  int direction;
  bool fArea;

  one_argument( argument, arg );

  if( arg[0] == '\0' )
    {
      send_to_char( "Whom are you trying to hunt?\n\r", ch );
      return;
    }

  /* only imps can hunt to different areas */
  fArea = ( get_trust(ch) < MAX_LEVEL );

  if( fArea )
    victim = get_char_area( ch, arg );
  else
    victim = get_char_world( ch, arg );

  if( victim == NULL )
    {
      send_to_char("No-one around by that name.\n\r", ch );
      return;
    }

  if( ch->in_room == victim->in_room )
    {
      act( "$N is here!", ch, NULL, victim, TO_CHAR );
      return;
    }

  /*
   * Deduct some movement.
   */
  if( ch->move > 2 )
    ch->move -= 3;
  else
    {
      send_to_char( "You're too exhausted to hunt anyone!\n\r", ch );
      return;
    }

  act( "$n carefully sniffs the air.", ch, NULL, NULL, TO_ROOM );
  WAIT_STATE( ch, skill_table[gsn_hunt].beats );
  direction = find_path( ch->in_room->vnum, victim->in_room->vnum,
			ch, -40000, fArea );

  if( direction == -1 )
    {
      act( "You couldn't find a path to $N from here.",
	  ch, NULL, victim, TO_CHAR );
      return;
    }

  if( direction < 0 || direction > 5 )
    {
      send_to_char( "Hmm... Something seems to be wrong.\n\r", ch );
      return;
    }

  /*
   * Give a random direction if the player misses the die roll.
   */
  if( ( IS_NPC (ch) && number_percent () > 75)        /* NPC @ 25% */
     || (!IS_NPC (ch) && number_percent () >          /* PC @ norm */
	 ch->pcdata->learned[gsn_hunt] ) )
    {
      do
	{
	  direction = number_door();
	}
      while( ( ch->in_room->exit[direction] == NULL )
	    || ( ch->in_room->exit[direction]->to_room == NULL) );
    }

  /*
   * Display the results of the search.
   */
  sprintf( buf, "$N is %s from here.", dir_name[direction] );
  act( buf, ch, NULL, victim, TO_CHAR );

  return;
}



void hunt_victim( CHAR_DATA *ch )
{
  int		dir;
  bool		found;
  CHAR_DATA	*tmp;

  if( ch == NULL || ch->hunting == NULL || !IS_NPC(ch) )
    return;

  /*
   * Make sure the victim still exists.
   */
  for( found = 0, tmp = char_list; tmp && !found; tmp = tmp->next )
    if( ch->hunting == tmp )
      found = 1;

  if( !found || !can_see( ch, ch->hunting ) )
    {
      do_say( ch, "Damn!  My prey is gone!!" );
      ch->hunting = NULL;
      return;
    }

  if( ch->in_room == ch->hunting->in_room )
    {
      act( "$n glares at $N and says, 'Ye shall DIE!'",
	  ch, NULL, ch->hunting, TO_NOTVICT );
      act( "$n glares at you and says, 'Ye shall DIE!'",
	  ch, NULL, ch->hunting, TO_VICT );
      act( "You glare at $N and say, 'Ye shall DIE!",
	  ch, NULL, ch->hunting, TO_CHAR);
      multi_hit( ch, ch->hunting, TYPE_UNDEFINED );
      ch->hunting = NULL;
      return;
    }

  WAIT_STATE( ch, skill_table[gsn_hunt].beats );
  dir = find_path( ch->in_room->vnum, ch->hunting->in_room->vnum,
		  ch, -40000, TRUE );

  if( dir < 0 || dir > 5 )
    {
      act( "$n says 'Damn!  Lost $M!'", ch, NULL, ch->hunting, TO_ROOM );
      ch->hunting = NULL;
      return;
    }

  /*
   * Give a random direction if the mob misses the die roll.
   */
  if( number_percent () > 75 )        /* @ 25% */
    {
      do
        {
	  dir = number_door();
        }
      while( ( ch->in_room->exit[dir] == NULL )
	    || ( ch->in_room->exit[dir]->to_room == NULL ) );
    }


  if( IS_SET( ch->in_room->exit[dir]->exit_info, EX_CLOSED ) )
    {
      do_open( ch, (char *) dir_name[dir] );
      return;
    }

  move_char( ch, dir );
  return;
}

That's all there is to hunt... Read you later :)
-- 
Mikko Kilpikoski                                __     Sig?  Naaah...
    E-Mail: turtle@modeemi.cs.tut.fi           /__\@     I feel too relaxed
       WWW: http://modeemi.cs.tut.fi/~turtle   L  L        for that...
       MUD: vichy.modeemi.cs.tut.fi (130.230.11.124) 2011



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
