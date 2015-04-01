From: Christian Boynton <ferris@FootPrints.net>

Here is a function that will add applies to your object in question. As
far as spells go, I haven't ever needed to do that, I'm sure that would be
easy though... If you need it, send me back an e-mail...


void do_addapply(CHAR_DATA *ch, char *argument)
{
  OBJ_DATA *obj;
  AFFECT_DATA *paf,*af_new;
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char arg3[MAX_INPUT_LENGTH];
  int affect_modify = 1, enchant_type;

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  argument = one_argument( argument, arg3 );

  if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
  {
    send_to_char("Syntax: addapply <object> <apply> <value>\n\r", ch);
    return;
  }

  obj = get_obj_world(ch,arg1);

  if (obj == NULL)
  {
    send_to_char("No such object exists!\n\r",ch);
    return;
  }

       if (!str_cmp(arg2,"hp"))
	enchant_type=APPLY_HIT;
  else if (!str_cmp(arg2,"str"))
	enchant_type=APPLY_STR;
  else if (!str_cmp(arg2,"dex"))
	enchant_type=APPLY_DEX;
  else if (!str_cmp(arg2,"int"))
	enchant_type=APPLY_INT;
  else if (!str_cmp(arg2,"wis"))
	enchant_type=APPLY_WIS;
  else if (!str_cmp(arg2,"con"))
	enchant_type=APPLY_CON;
/*  else if (!str_cmp(arg2,"sex"))
	enchant_type=APPLY_SEX;
*/
  else if (!str_cmp(arg2,"mana"))
	enchant_type=APPLY_MANA;
  else if (!str_cmp(arg2,"move"))
	enchant_type=APPLY_MOVE;
  else if (!str_cmp(arg2,"ac"))
	enchant_type=APPLY_AC;
  else if (!str_cmp(arg2,"hitroll"))
	enchant_type=APPLY_HITROLL;
  else if (!str_cmp(arg2,"damroll"))
	enchant_type=APPLY_DAMROLL;
  else if (!str_cmp(arg2,"saves"))
	enchant_type=APPLY_SAVING_SPELL;
  else
  {
    send_to_char("That apply is not possible!\n\r",ch);
    return;
  }

  if ( !is_number(arg3) )
  {
    send_to_char("Applies require a value.\n\r", ch);
    return;
  }

  if (enchant_type < 25)
	affect_modify=atoi(arg3);	

    if (!obj->enchanted)
    {
      obj->enchanted = TRUE;

      for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
      {
	if (affect_free == NULL)
	af_new = alloc_perm(sizeof(*af_new));
	else
        {
	  af_new = affect_free;
	  affect_free = affect_free->next;
        }

      af_new->next = obj->affected;
      obj->affected = af_new;
      af_new->type        = UMAX(0,paf->type);
      af_new->level       = paf->level;
      af_new->duration    = paf->duration;
      af_new->location    = paf->location;
      af_new->modifier    = paf->modifier;
      af_new->bitvector   = paf->bitvector;
      }
    }

  if (affect_free == NULL)
    paf = alloc_perm(sizeof(*paf));
  else
  {
    paf = affect_free;
    affect_free = affect_free->next;
  }

    if (enchant_type > 24)	
	paf->type       = skill_lookup(arg2);
    else
	paf->type 	= 0;
        paf->level      = 100;
        paf->duration   = -1;
        paf->location   = enchant_type;
	paf->modifier   = affect_modify;
        paf->bitvector  = 0;
	paf->next       = obj->affected;
	obj->affected   = paf;

	send_to_char("Ok.\n\r", ch);
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
