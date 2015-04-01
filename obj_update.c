From mn54196@uta.fi
Date: Fri, 1 Sep 1995 10:19:25 +0300 (EET DST)
From: Markku Nylander <mn54196@uta.fi>
To: merc-l@webnexus.com
Subject: obj_update()

    [The following text is in the "ISO-8859-1" character set]
    [Your display is set for the "US-ASCII" character set]
    [Some characters may be displayed incorrectly]




	I've been thinking about the object update system in Merc.
	In the stock code, the obj_update() function traverses the
	entire obj_list, every object in the game world. This may mean
	thousands of checks even though only maybe a dozen objects actually
	need updating. Checking every single object is obviously
	unnecessary overhead even though obj_update() is only called
	once every 60 seconds or so (the comment in the Merc code states
	that obj_update() is "performance sensitive").
	Objects that need updating can be in two possible places: in a
	room or in a character's inventory. My idea to modify the obj_update
	is as follows:

	Create a simple singly linked list with pointers to ROOM_INDEX_DATA:
	struct obj_update_list {
		ROOM_INDEX_DATA 	*objs_to_update;
		struct obj_update_list 	*next;
	};
	Now, every time an object is put into a room via obj_to_room(),
	the object is checked whether it needs updating. If it does need
	updating, pointer to the room where the object will be placed is
	added to the obj_update_list.
	To update objects, you simply:
	For every room in obj_update_list
		For every object in the room
			Check if the object needs updating
			and update if necessary
		If no objects that need updating were found in the room
			Remove the room from obj_update_list.
	You can move and extract objects without any effect to the
	obj_update_list; if no objects to be updated are found, the list
	node is simply removed. Yet the number of checks is greatly reduced.
	Updating objects in characters' inventory can be added to
	char_update().

	I haven't tried this one myself since machine lag is not (yet) a
	problem in my MUD, but does it seem feasible?

		-- MNy

--
"The real prophets of doom are not those who criticize technological
 civilization but those who accept domination of the machine." - Rene Dubois
----------------------------------------------------------------------------
E-mail: mn54196@uta.fi | URL: http://www.uta.fi/~mn54196/info.html



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
