Written by esnible@goodnet.com

/*
   Wandering Sage Code

   Ok, you have probably seen MUDs that have a wandering banker mobile.
This is a little different.  This is a wandering sage who will IDENTIFY
objects for non-mage players.  (I've never been a God on a MUD that had it,
so I don't know if it has any wierd social dynamics.  My suggestion is to
make it hard to find this guy.)

   Anyway, players find the guy and type "IDENTIFY object" and the guy
tells them what it is, like the IDENTIFY spell does.  He also relieves
them of 100 gp * the level of the object.
*/

/* Add this line to the cmd_table in interp.c */
/*    { "identify", do_identify, POS_STANDING,  0,  LOG_NORMAL   }, */

/* Define this to be some mobile in your zone. */
#define MOB_VNUM_SAGE	   8911
void do_identify( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    CHAR_DATA *rch;

    if ( ( obj = get_obj_carry( ch, argument ) ) == NULL )
    {
       send_to_char( "You are not carrying that.\n\r", ch );
       return;
    }

    for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
       if (IS_NPC(rch) && rch->pIndexData->vnum == MOB_VNUM_SAGE)
          break;

    if (!rch)
    {
       send_to_char("No one here seems to know much about that.\n\r", ch);
       return;
    }

    if (IS_IMMORTAL(ch))
       act( "$n cackles and comps you!\n\r", rch, obj, ch, TO_VICT );
    else if (ch->gold < obj->level * 100 + 50)
       {
       act( "$n resumes tossing bones without looking at $p.",
             rch, obj, 0, TO_ROOM );
       return;
       }
    else
       {
       ch->gold -= obj->level * 100 + 50;
       send_to_char("Your purse feels lighter.\n\r", ch);
       }

    act( "$n fondles $p and tosses some chicken bones to the ground.",
       rch, obj, 0, TO_ROOM );
    spell_identify( 0, 0, ch, obj );
}

/* Example mobile to be the indentification guy.  I added a sage to the Holy
   Grove.

#8911
man cripple~
an old man~
A crippled old man is kneeling here, tossing chicken bones onto the ground.
~
When you look down at him, he raises his head to you and you can see that one
of his eyes is hideously enlarged and blue, while the other is a blackened
empty socket.  The old man beams a toothless grin at you.
~
64|128 1|8|128|8192 +1000 S
32 0 -5 20d40+150 1d8+12
50 60000
8 8 1
*/

/* 
   Falling objects

   Ok, what this does it causes objects in air sectors to fall.  This is
a real nice touch if you have lots of airy zones, like my Aarakocran city,
or the air above Midgaard.

   Everything falls.  This is pretty funny if you are standing in the
square in Midgaard below the air and corpses of flying monsters start
falling down.  It is also pretty funny if you have flying mobiles in air
zones who have treasure and as players loot the corpse the corpse is falling.

   Put this code in UPDATE.C in the function obj_update() where you see

	if ( obj->timer <= 0 || --obj->timer > 0 )
	    continue;

   Put it before the continue and put braces around the whole thing and
the continue.

            if (obj->in_room &&
                obj->in_room->sector_type == SECT_AIR &&
                (obj->wear_flags & ITEM_TAKE) &&
                obj->in_room->exit[5] &&
                obj->in_room->exit[5]->to_room)
               {
               ROOM_INDEX_DATA *new_room = obj->in_room->exit[5]->to_room;

               if (( rch = obj->in_room->people ) != NULL )
                  {
                  act( "$p falls away.", rch, obj, NULL, TO_ROOM );
                  act( "$p falls away.", rch, obj, NULL, TO_CHAR );
                  }

               obj_from_room(obj);
               obj_to_room(obj, new_room);

               if (( rch = obj->in_room->people ) != NULL )
                  {
                  act( "$p floats by.", rch, obj, NULL, TO_ROOM );
                  act( "$p floats by.", rch, obj, NULL, TO_CHAR );
                  }
               }
*/

/*
   Ok, this is for making trophy bags.  Here is the idea.  Player kills some
major mobile, like Ares and can take the corpse and skin it and make a bag
out of it.  You can do this with player corpses as well, works great if you
have a PKILL mud.

   I originally set this up as a NPC only spell, like dragon breath.  The idea
was there would be a scroll of it, and also a taxidermist shopkeeper who would
do it for you.  On the MUD where it went in, it was made a player spell and
given to some kind of woodsman class.

   You can fiddle with the amount these bags will contain below.  Note that if
you decide to go with the taxidermist idea, and make him a shopkeeper, he will
buy corpses for 1-2gp and make 1000gp trophy bags.  Because of how Merc is set
up, you also need to change the "level" of corpses to level 1 instead of level
0, shopkeepers don't buy level 0 stuff.

   Add this to the spell table in CONST.C

        "make bag",	{ 37, 37, 37, 37 },
	spell_make_bag,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(50),	100,	24,
	"",			"!Make Bag!"*/

   Put this in MAGIC.C
*/

void spell_make_bag( int sn, int level, CHAR_DATA *ch, void *vo )
{
    static char *headers[] = { "corpse of the ", "corpse of The ",
                               "corpse of an ", "corpse of An ",
                               "corpse of a ", "corpse of A ",
                               "corpse of " }; // (This one must be last)
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    char buf[MAX_STRING_LENGTH];
    int i;

    if ( obj->item_type != ITEM_CORPSE_NPC && obj->item_type != ITEM_CORPSE_PC)
	return;

    for (i = 0; i < 7; i++)
       {
       int len = strlen(headers[i]);
       if ( memcmp(obj->short_descr, headers[i], len) == 0 )
          {
          sprintf( buf, "bag %s", obj->short_descr+len );
          free_string( obj->name );
          obj->name = str_dup(buf);

          sprintf( buf, "A bag of fine %s hide catches your eye.  ",
             obj->short_descr+len );
          free_string( obj->description );
          obj->description = str_dup( buf );

          sprintf( buf, "bag made from %s hide", obj->short_descr+len );
          free_string( obj->short_descr );
          obj->short_descr = str_dup( buf );

          break;
          }
       }

    obj->item_type = ITEM_CONTAINER;
    obj->wear_flags = ITEM_HOLD|ITEM_TAKE;
    obj->timer = 0;
    obj->weight = 5;
    obj->level = level/3;
    obj->cost = level * 50;
    obj->value[0] = level * 10;                 /* Weight capacity */
    obj->value[1] = 1;                          /* Closeable */
    obj->value[2] = -1;                         /* No key needed */
    obj->pIndexData = get_obj_index( 5660 );    /* So it's not a corpse */

    act( "Your new $p looks pretty snazzy.", ch, obj, NULL, TO_CHAR );
    act( "$n's new $p looks pretty snazzy.", ch, obj, NULL, TO_ROOM );

    send_to_char( "Ok.\n\r", ch );
    return;
}

/*

   Put this in a zone.  If you change the number change it in the code
as well.

#5660
bag~
a generic bag~
A generic bag generates the VNUM for the 'make purse' spell.  ~
~
15 0 1
50 0 0 0 
5 50 0
*/

/* Put this in SPECIAL.C.  Remember to add this to spec_lookup() as well. */

bool spec_taxidermist( CHAR_DATA *ch )
{
    OBJ_DATA *inv;
    int sn;

    if ( ch->position != POS_STANDING )
	return FALSE;

    if ( ch->pIndexData->pShop == 0 ||
         time_info.hour < ch->pIndexData->pShop->open_hour ||
         time_info.hour > ch->pIndexData->pShop->close_hour )
       return FALSE;

    for ( inv = ch->carrying; inv != NULL; inv = inv->next_content )
    {
    if (inv->item_type == ITEM_CORPSE_NPC)
       {
       if ( ( sn = skill_lookup( "make bag" ) ) >= 0 )
          (*skill_table[sn].spell_fun) ( sn, ch->level, ch, inv );
       return TRUE;
       }
    else if (inv->wear_loc == WEAR_NONE && number_percent() < 5)
       {
       act( "$n suggests you buy $p.", ch, inv, NULL, TO_ROOM );
       return TRUE;
       }
    }

    return FALSE;
}


/* Ok, here is a spec-fun example for a nasty mobile known as the snake
   charmer.  I rather like this guy, but I never could find a MUD to
   put this code in.

   Here is what happens.  Player attacks snake charmer mobile.  Snake
   charmer then casts CHARM spell on player, and forces player to give
   charmer weapons, sing, and he can even order the player to CHAT that
   the snake charmer is this really great mob to attack.

   I figure this should liven up any egypt or india zone.
*/

bool spec_snake_charm( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
    int sn;

    if ( ch->position != POS_FIGHTING )
       {
       switch ( number_bits( 3 ) ) {
       case 0:
          do_order( ch, "all sing charmer" ); /* a chance to get free here */
          break;
       case 1:
          do_order( ch,
             "all chat 'The snake charmer area is pretty cool.  "
             "I'm getting a lot of experience really fast!" );
          break;
       case 2:
          do_order( ch,
             "all chat 'YES!  I just got 327xp for killing the snake charmer!");
          break;
       case 3:
          do_order( ch, "all remove dagger" );
          do_order( ch, "all give dagger charmer" );
          break;
       case 4:
          do_order( ch, "all remove sword" );
          do_order( ch, "all give sword charmer" );
          break;
       case 5:
          do_order( ch, "all remove mace" );
          do_order( ch, "all give mace charmer" );
          break;
       case 6:
          do_order( ch, "all drop all" );
          break;
       case 7:
          do_order( ch, "all cast 'cure light' charmer" );
          break;
       };

       return TRUE;
       }

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( victim->fighting == ch && number_bits( 2 ) == 0 )
	    break;
    }

    if ( victim == NULL )
	return FALSE;

	act( "$n begins playing a new, beautiful song.", ch, NULL, NULL, TO_ROOM );
    spell_charm_person(gsn_charm_person, ch->level, ch, victim );
    if (IS_AFFECTED(victim, AFF_CHARM))
       stop_fighting( victim, TRUE );

    return TRUE;
}


/*
    This one is real nifty.  The idea comes from a Mr. Peckham whose
    address I have lost, this is my own reimplementation of it, as I
    never got to see his code.

    To get this to work you have to either modify your area file format
    to have resets for random objects and your DB.C loader.  I'll leave
    that as an exercise for you.

    If you want to see how this works without all the fuss, put something
    like this in your create_mobile() in DB.C.  This gives any mobile a
    random chance to have something special.

    if (number_percent() <= 10)
       wield_random_magic( mob );
    else if (number_percent() <= 10)
       wield_random_armor( mob );
*/


#define nelems(a) (sizeof (a)/sizeof (a)[0])

// Calculate a meaningful modifier and amount
void random_apply( OBJ_DATA *obj, CHAR_DATA *mob )
{
   static int attrib_types[] = { APPLY_STR, APPLY_DEX, APPLY_DEX, APPLY_INT,
      APPLY_INT, APPLY_WIS, APPLY_CON, APPLY_CON, APPLY_CON };
   static int power_types[] = { APPLY_MANA, APPLY_HIT, APPLY_MOVE, APPLY_AC };
   static int combat_types[] = { APPLY_HITROLL, APPLY_HITROLL, APPLY_DAMROLL,
      APPLY_SAVING_SPELL, APPLY_SAVING_SPELL, APPLY_SAVING_BREATH };

   AFFECT_DATA *paf = alloc_perm( sizeof(*paf) );
   paf->type	     = -1;
   paf->duration  = -1;
   paf->bitvector = 0;
   paf->next	     = obj->affected;
   obj->affected  = paf;
   switch (number_bits(2)) {
   case 0:
      paf->location  = attrib_types[number_range(0, nelems(attrib_types)-1)];
      paf->modifier  = 1;
      break;
   case 1:
      paf->location  = power_types[number_range(0, nelems(power_types)-1)];
      paf->modifier  = number_range(mob->level/2, mob->level);
      break;
   case 2:
   case 3:
      paf->location  = combat_types[number_range(0, nelems(combat_types)-1)];
      paf->modifier  = number_range(1, mob->level/6+1);
      break;
   }

   SET_BIT(obj->extra_flags, ITEM_MAGIC);

   // Is item cursed?
   if (number_percent() <= 5)
      {
      paf->modifier = -paf->modifier;
      SET_BIT(obj->extra_flags, ITEM_NODROP);
      if (number_percent() <= 15)
         SET_BIT(obj->extra_flags, ITEM_NOREMOVE);
      }
}

// Jewelry stuff
static char *adj1[] = { "splendid", "ancient", "dusty", "scratched",
   "flawed", "burnt", "heavy", "gilded", "spooky", "flaming", "plain",
   "ornate", "inscrutable", "obscene", "wrinkled" };
static char *adj2[] = { "diamond", "emerald", "topaz", "wooden", "jade",
   "white gold", "onyx", "tin", "glass", "marble", "black", "granite" };

#define MASK_IGNORE     (1<<TAR_IGNORE)
#define MASK_OFFENSIVE  (1<<TAR_CHAR_OFFENSIVE)
#define MASK_DEFENSIVE  (1<<TAR_CHAR_DEFENSIVE)
#define MASK_SELF       (1<<TAR_CHAR_SELF)
#define MASK_INV        (1<<TAR_OBJ_INV)

#define CLASS_MAGE       0
#define CLASS_CLERIC     1

// Returns a clerical or magical spell of the appropriate (masked) type
int random_spell( int level, int mask, sh_int *type )
{
   extern const	struct skill_type skill_table[MAX_SKILL];

   for ( ;; )
      {
      int skill_no = number_range(0, MAX_SKILL-1);
      if ((skill_table[skill_no].skill_level[CLASS_MAGE] <= level ||
           skill_table[skill_no].skill_level[CLASS_CLERIC] <= level) &&
          skill_table[skill_no].spell_fun &&         /* an actual spell? */
          mask & (1<<skill_table[skill_no].target))  /* appropriate? */
         {
         *type = skill_table[skill_no].target;
         return skill_no;
         }
      }
}

// Wands/Staves/Potions/Pills and bags
void wield_random_magic( CHAR_DATA *mob )
{
   int item_type = number_range(48, 53);  /* Get template obj from LIMBO.ARE */
   OBJ_INDEX_DATA *pObjIndex = get_obj_index( item_type );
   OBJ_DATA *obj = create_object( pObjIndex, number_fuzzy( mob->level ) );
   sh_int type;
   int    n_adj1 = number_range(0, nelems(adj1)-1);
   int    n_adj2 = number_range(0, nelems(adj2)-1);

   // Potion stuff
   static char *potions[] = { "bottle", "snifter", "jar", "bulb", "potion" };

   // Pill stuff
   static char *pills[] = { "bag of dust", "honeycomb", "root", "leaf",
      "crystal", "powder packet", "seed" };

   // Bag types (no wearable belts/backpacks... yet)
   static char *skins[] = { "lamia skin", "cloth", "leather", "tough leather",
      "zebra-skin", "gilded" };
   static char *bags[] = { "bag", "valise", "pail", "basket", "jar",
      "box", "sack" };

   char buffer[256];
   char buf[MAX_STRING_LENGTH];
   char name[MAX_STRING_LENGTH];

   switch (item_type) {
   case 48: /* scroll */
      sprintf(buffer, "%s %s scroll", adj1[n_adj1], adj2[n_adj2]);
      obj->value[0] = number_range(obj->level / 2+1, obj->level * 3 / 2+1); /* level */
      obj->value[0] = URANGE(1, obj->value[0], 36);
      obj->value[1] = random_spell(obj->value[0],
         MASK_IGNORE|MASK_OFFENSIVE|MASK_DEFENSIVE|MASK_SELF, &type);
      if (number_percent() < 50)
         obj->value[2] = random_spell(obj->value[0], 1<<type, &type);
      break;
   case 49: /* wand */
      sprintf(buffer, "%s %s wand", adj1[n_adj1], adj2[n_adj2]);
      obj->value[0] = number_range(obj->level / 3 + 1, obj->level * 3 / 2);  /* level */
      obj->value[0] = URANGE(1, obj->value[0], 36);
      obj->value[1] = number_fuzzy(obj->level / 2 + 3); /* max charges */
      obj->value[2] = number_range(1, obj->value[1]);   /* charges left */
      obj->value[3] = random_spell(obj->value[0], MASK_OFFENSIVE, &type);
      break;
   case 50: /* staff */
      sprintf(buffer, "%s %s staff", adj1[n_adj1], adj2[n_adj2]);
      obj->value[0] = number_range(obj->level / 4 + 1, obj->level * 3 / 2);  /* level */
      obj->value[0] = URANGE(1, obj->value[0], 36);
      obj->value[1] = number_fuzzy(obj->level / 2 + 3); /* max charges */
      obj->value[2] = number_range(1, obj->value[1]);   /* charges left */
      obj->value[3] = random_spell(obj->value[0],
         MASK_IGNORE|MASK_OFFENSIVE|MASK_DEFENSIVE|MASK_SELF, &type);
      break;
   case 51: /* potion */
      sprintf(buffer, "%s %s %s", adj1[n_adj1], adj2[n_adj2],
         potions[number_range(0, nelems(potions)-1)]);
      obj->value[0] = number_range(obj->level / 2+1, obj->level * 3 / 2+1); /* level */
      obj->value[0] = URANGE(1, obj->value[0], 36);
      obj->value[1] = random_spell(obj->value[0], MASK_DEFENSIVE, &type);
      if (number_percent() < 50)
         obj->value[2] = random_spell(obj->value[0], MASK_DEFENSIVE, &type);
      break;
   case 52: /* pill */
      sprintf(buffer, "%s %s %s", adj1[n_adj1], adj2[n_adj2],
         pills[number_range(0, nelems(pills)-1)]);
      obj->value[0] = number_range(obj->level / 2+1, obj->level * 3 / 2+1); /* level */
      obj->value[0] = URANGE(1, obj->value[0], 36);
      obj->value[1] = random_spell(obj->value[0], MASK_DEFENSIVE, &type);
      if (number_percent() < 50)
         obj->value[2] = random_spell(obj->value[0], MASK_DEFENSIVE, &type);
      break;
   case 53: /* bag */
      sprintf(buffer, "%s %s %s", adj1[n_adj1],
         skins[number_range(0, nelems(skins)-1)],
         bags[number_range(0, nelems(bags)-1)]);
      obj->value[0] = number_range(mob->level, mob->level * 25); /* weight */
      obj->value[1] = number_range(0, 1);
      obj->value[2] = -1;
      break;
   };

   // Generate the description strings
   free_string( obj->name );
   obj->name = str_dup( buffer );

   sprintf( buf, "a%s %s",
      (buffer[0] == 'a' || buffer[0] == 'e' || buffer[0] == 'i' ||
       buffer[0] == 'o' || buffer[0] == 'u') ? "n" : "", buffer );
   free_string( obj->short_descr );
   obj->short_descr = str_dup( buf );

   free_string( obj->description );
   sprintf( buf, "%s lies here.", obj->short_descr );
   obj->description = str_dup( buf );

   // Put the item in the mob's inventory
   obj_to_char( obj, mob );
}

// Anything wearable, and trinkets
void wield_random_armor( CHAR_DATA *mob )
{
   int item_type = number_range(0, MAX_WEAR - 1); /* template from LIMBO.ARE */
   OBJ_INDEX_DATA *pObjIndex = get_obj_index( item_type + 30 );
   OBJ_DATA *obj = create_object( pObjIndex, number_fuzzy( mob->level ) );
   int    n_adj1 = number_range(0, nelems(adj1)-1);
   int    n_adj2 = number_range(0, nelems(adj2)-1);
   char *name = "[random]";

   // Armor stuff
   static char *armor_types[] = { "leather", "studded leather", "bronze",
      "chain", "plate", "mithral" };
   static int armor_mul[] = { 1, 3, 2, 5, 10, 10 };
   static int armor_div[] = { 1, 2, 1, 1,  1,  3 };

   // Weapon stuff
   static char *weapon_types[] = { "sword", "sword", "sword", "sword", "sword",
      "short sword", "dagger", "dagger", "hammer", "mace", "mace", "whip" };
   static int weapon_dam[] = { 3, 3, 3, 3, 3, 11, 11, 11, 0, 7, 7, 4 };

   // Trinket stuff
   static char *noun[] = { "pebble", "bauble", "stone", "charm", "fetish",
      "bone", "trinket" };
   char buffer[64];

   char buf[MAX_STRING_LENGTH];

   if (obj->item_type == ITEM_ARMOR)
      {
      int ac_type = URANGE(0, mob->level/5, nelems(armor_types)-1);
      name = armor_types[ac_type];
      obj->weight *= armor_mul[ac_type];
      obj->weight /= armor_div[ac_type];

      if (number_percent() < mob->level / 3)
         random_apply(obj, mob);
      }
   else if (obj->item_type == ITEM_WEAPON)
      {
      int wea_type = number_range(0, nelems(weapon_types)-1);
      name = weapon_types[wea_type];
      obj->value[3] = weapon_dam[wea_type];
      }
   else if (obj->item_type == ITEM_TREASURE)
      {
      if (number_percent() < mob->level)
         {
         random_apply(obj, mob);

         if (number_percent() < mob->level / 3)
            random_apply(obj, mob);
         }

      if (obj->wear_flags & ITEM_HOLD) /* trinket? */
         sprintf(buffer, "%s %s %s", adj1[n_adj1],
            adj2[n_adj2],
            noun[number_range(0, nelems(noun)-1)]);
      else /* no, necklace or something */
         sprintf(buffer, "%s %s", adj1[n_adj1],
            adj2[n_adj2]);
      name = buffer;
      }

   sprintf( buf, obj->short_descr, name );
   free_string( obj->short_descr );
   obj->short_descr = str_dup( buf );

   obj_to_char( obj, mob );
   equip_char( mob, obj, item_type );
}
</pre>
You also need to add the following to LIMBO.ARE
<pre>
#30
light~
a%s %s light source~
A random light source makes the 'VNUM' for a random light.~
~
1 0 1
0 0 48 0
1 0 0
#31
ring~
a%s %s ring~
A random ring makes the 'VNUM' for a random ring.~
~
8 0 1|2
0 0 0 0
1 0 0
#32
ring~
a%s %s ring~
A random ring makes the 'VNUM' for a random ring.~
~
8 0 1|2
0 0 0 0
1 0 0
#33
necklace~
a%s %s necklace~
A random necklace makes the 'VNUM' for a random necklace.~
~
8 0 1|4
0 0 0 0
1 0 0
#34
cape~
a%s  %s cape~
A random cape makes the 'VNUM' for a random cape.~
~
9 0 1|4
0 0 0 0
5 0 0
#35
body armor~
%s body armor~
Random body armor makes the 'VNUM' for random body armor.~
~
9 0 1|8
0 0 0 0
10 0 0
#36
helmet~
a%s %s helmet~
A random helmet makes the 'VNUM' for a random helmet.~
~
9 0 1|16
0 0 0 0
5 0 0
#37
leggings~
%s leggings~
Random leggings make the 'VNUM' for random leggings.~
~
9 0 1|32
0 0 0 0
5 0 0
#38
boots~
%s boots~
Random boots make the 'VNUM' for random boots.~
~
9 0 1|64
0 0 0 0
5 0 0
#39
gauntlets~
%s gauntlets~
Random gauntlets make the 'VNUM' for random gauntlets.~
~
9 0 1|128
0 0 0 0
5 0 0
#40
sleeves~
%s sleeves~
Random sleeves make the 'VNUM' for random sleeves.~
~
9 0 1|256
0 0 0 0
4 0 0
#41
shield~
a %s shield~
A random shield makes the 'VNUM' for a random shield.~
~
9 0 1|512
0 0 0 0
12 0 0
#42
robe~
a %s robe~
A random robe makes the 'VNUM' for a random robe.~
~
9 0 1|1024
0 0 0 0
2 0 0
#43
belt~
a%s %s belt~
A random belt makes the 'VNUM' for a random belt.~
~
9 0 1|2048
0 0 0 0
3 0 0
#44
bracelet~
a%s %s bracelet~
A random bracelet makes the 'VNUM' for a random bracelet.~
~
8 0 1|4096
0 0 0 0
1 0 0
#45
bracer~
a%s %s bracer~
A random bracer makes the 'VNUM' for a random bracer.~
~
9 0 1|4096
0 0 0 0
5 0 0
#46
weapon~
a%s %s~
A random weapon makes the 'VNUM' for a random weapon.~
~
5 0 1|8192
0 0 0 0
5 0 0
#47
trinket~
a%s %s~
A random trinket makes the 'VNUM' for a random trinket.~
~
8 0 1|16384
0 0 0 0
4 0 0
#48
scroll~
a%s %s scroll~
A random scroll makes the 'VNUM' for a random scroll.~
~
2 0 1|16384
0 0 0 0
1 0 0
#49
wand~
a%s wand~
A random wand makes the 'VNUM' for a random wand.~
~
3 0 1|16384
0 0 0 0
1 0 0
#50
staff~
a%s %s staff~
A random staff makes the 'VNUM' for a random staff.~
~
4 0 1|16384
0 0 0 0
5 0 0
#51
potion~
a%s~
A random potion makes the 'VNUM' for a random potion.~
~
10 0 1|16384
0 0 0 0
1 0 0
#52
pill~
a%s~
A random pill makes the 'VNUM' for a random pill.~
~
26 0 1
0 0 0 0
1 0 0
#53
bag~
a%s~
A random bag makes the 'VNUM' for a random bag.~
~
15 0 1|16384
0 0 0 0
1 0 0


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
