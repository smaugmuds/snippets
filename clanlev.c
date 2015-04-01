From: Dennis Reichel <dennis@starlifter.reichel.net>

> 
> I'm working on adding clan levels into the code.  Does anyone have an easy
> way to add them to the current system, or am I going to have to rip the
> old stuff out and rewrite it? :)  Seems to me the way the structure is
> currently setup there's no way to do it that isn't "hackish" in nature.
> If anyone has any suggestions, please let me know before I begin redoing
> it =)
> 

Hackish?  Here it is :-)  Provides titles by gender...  Some stuff
will not be generally applicable - have a PLR_LEADER bit and a
room_leader flag, etc..

Immortals are given the rank <patron> if they are guilded.

Anyways, I think that's most of what I've got set up..  enjoy.

act_info.c

in do_who(...)

/* be sure to test for !IS_NPC( ch ) when accessing ch->pcdata :-) */

      is_clan(wch) ?
          clan_rank_table[wch->pcdata->clan_rank].title_of_rank[wch->sex]
          : "",

merc.h

in pc_data
    sh_int	clan_rank;/* 0 for unguilded and loner 1-10 otherwise */


tables.h

extern  const   struct  clan_titles     clan_rank_table[];

struct clan_titles
{
    char *title_of_rank[3];
};

tables.c

const struct clan_titles clan_rank_table[] =
{   
    {
         {"<#OWildthing#n>",   "<#OFreeman#n>",    "<#OFreewoman#n>"   }
    },
    {
         {"<#ORecruit#n>"    ,"<#ORecruit#n>"    ,"<#ORecruit#n>"      }
    },
    {
         {"<#OAcolyte#n>"     ,"<#OAcolyte#n>"    ,"<#OAcolyte#n>"     }
    },
    {
         {"<#OAdept#n>"       ,"<#OAdept#n>"      ,"<#OAdept#n>"       }
    },
    {
         {"<#OOfficer#n>"     ,"<#OOfficer#n>"    ,"<#OOfficer#n>"     }
    },
    {
         {"<#OLieutenant#n>"  ,"<#OLieutenant#n>" ,"<#OLieutenant#n>"  }
    },
    {
         {"<#OVassal#n>"      ,"<#OVassal#n>"     ,"<#OVassal#n>"      }
    },
    {
         {"<#OAmbassador#n>"  ,"<#OAmbassador#n>" ,"<#OAmbassadress#n>"}
    },
    {
         {"<#OMagistrate#n>"  ,"<#OMagistrate#n>" ,"<#OMagistrate#n>"  }
    },
    {
         {"<#OLord#n>"        ,"<#OLord#n>"       ,"<#OLady#n>"        }
    },
    {
         {"<#OLeader#n>"      ,"<#OLeader#n>"     ,"<#OLeader#n>"      }
    },
    {
         {"<#OPatron#n>"      ,"<#OPatron#n>"     , "<#OPatron#n>"     }
    },
    {
         { NULL, NULL, NULL }                
    }         
};

act_wiz.c

void do_promote( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( IS_NPC(ch) )
    {
        send_to_char( "NPC's can not promote someone.\n\r",ch);
        return;
    }
        
    if ( ( ch->pcdata->clan_rank < 9 ) && ( !IS_IMMORTAL(ch) ) )
    {
        send_to_char( "You must be a clan Leader or Magistrate to promote someone.\n\r",ch);
        return;
    }
    if ( arg1[0] == '\0' || arg2[0] == '\0' 
      || atoi(arg2) < 1 || atoi(arg2) > 10 )
    {
        send_to_char( "Syntax: promote <char> <rank 1-10>\n\r",ch);
        return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
        send_to_char( "They must be present to be promoted.\n\r", ch );
        return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char("You must be mad.\n\r",ch);
	return;
    }
    
    if ( (victim->clan != ch->clan ) && (!IS_IMMORTAL(ch) ) )
    {
	send_to_char("You can not promote a player who is not in your clan.\n\r",ch);
	return;
    }
    
    if ( !IS_SET(victim->act, PLR_LEADER) && atoi(arg2)==10 )
    {
	send_to_char("This player is not qualified to lead.\n\r",ch);
	return;
    }

    victim->pcdata->clan_rank = atoi(arg2);
    
    send_to_char( "Rank ceremony complete\n\r", ch);
    send_to_char( "Rank ceremony complete\n\r", victim);
    
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
