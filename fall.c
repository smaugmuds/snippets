From: Phoenix <s080072@dutiwy.twi.tudelft.nl>

A while back someone send in some not working falling items code, they 
asked some help and I promised I would send them something. Bad thing is 
I don't remember who it was that send the original mail.
So here is the falling code and hopefully who needed it will get it :).

act_obj.c: do_drop


if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
    {

        ROOM_INDEX_DATA *pToRoom;       /* Line added! */
        int i = 0;                      /* Line added! */

        /* 'drop obj' */
        if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
        {
            send_to_char( "You do not have that item.\n\r", ch );
            return;
        }

        if ( !can_drop_obj( ch, obj ) )
        {
            send_to_char( "You can't let go of it.\n\r", ch );
            return;
        }

        obj_from_char( obj );
        obj_to_room( obj, ch->in_room );
        act( "$n drops $p.", ch, obj, NULL, TO_ROOM, POS_RESTING );
        act( "You drop $p.", ch, obj, NULL, TO_CHAR, POS_RESTING );
        if (IS_OBJ_STAT(obj,ITEM_MELT_DROP))
        {
            act("$p dissolves into smoke.",ch,obj,NULL,TO_ROOM, 
POS_RESTING);
            act("$p dissolves into smoke.",ch,obj,NULL,TO_CHAR, 
POS_RESTING);
            extract_obj(obj);
            return;             /* Line added! */
        }

        /* Phoenix: Start NEW BIT */
        for ( ; obj->in_room->exit[DIR_DOWN]
             && (pToRoom = obj->in_room->exit[DIR_DOWN]->u1.to_room)
             && obj->in_room->sector_type == SECT_AIR
             && i < 10; i++ ) {
           static ROOM_INDEX_DATA *visited_rooms[10];
           int j;
           
           if ( obj->in_room->people )
              act("$p falls away through the air below you.",
              obj->in_room->people, obj, NULL, TO_ALL, POS_RESTING);
           visited_rooms[i] = obj->in_room;
           for ( j = 0; j <= i; j++ )
              if ( visited_rooms[j] == pToRoom )
                 break;
           obj_from_room(obj);
           obj_to_room(obj, pToRoom);

           if ( obj->in_room->people && obj->in_room->exit[DIR_DOWN]
             && obj->in_room->exit[DIR_DOWN]->u1.to_room
             && obj->in_room->sector_type == SECT_AIR ) {
              if ( obj->in_room->people )
              act( "$p falls through the air.\n\r",
              obj->in_room->people, obj, NULL, TO_ALL, POS_RESTING);
           } else
              /* switch statement */
              act("$p falls from the sky and lands on the ground in "
              "front of you.", obj->in_room->people, obj, NULL, TO_ALL,
              POS_RESTING );
        }
        /* Phoenix: End NEW BIT */
  }
    else
    {
      [snipped rest]


You will see an object entering the room and falling through or ending up 
in front of your nose. If you wanted a special message for the 
underground that the object lands on you can put a switch statement at the 
/* switch statement */ line. Rooms that lead back to rooms where item 
already was has been taken into account. A max of 10 rooms that are in 
one straight line down is being assumed. Adjust the number 10 to the 
longest row of rooms you have in your mud that go all down.

Oh yeah, drop all doesn't work, but that is because the drop all doesn't 
call drop_object recursively. You might wanna change that.

PhoenX


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
