Ok what this does is allows the mud to attempt a copyover if you crash. I require no credit
for this because, this is based off ferric's original signal handler code. All I did was
make it so that it would work with copyover. If you have any problems let me know :). 


comm.c
******
with the other function prototypes right below:
void    bust_a_prompt           args( ( CHAR_DATA *ch ) );        

void    init_signals            args( ( void ) );
void    do_auto_shutdown        args( ( void ) );            


in function game_loop_unix

Above this:

 while ( !merc_down )     

put this init_signals();


at the end of comm.c put these functions:

void init_signals()
{
  signal(SIGBUS,sig_handler);
  signal(SIGTERM,sig_handler);
  signal(SIGABRT,sig_handler);
  signal(SIGSEGV,sig_handler);
}

void sig_handler(int sig)
{
  switch(sig)
  {
    case SIGBUS:
      bug("Sig handler SIGBUS.",0);
      do_auto_shutdown();
      break;
    case SIGTERM:
      bug("Sig handler SIGTERM.",0);
      do_auto_shutdown();
      break;
    case SIGABRT:
      bug("Sig handler SIGABRT",0);
      do_auto_shutdown();             
     case SIGSEGV:
      bug("Sig handler SIGSEGV",0);
      do_auto_shutdown();
      break;
  }
}
                      


Put the do_auto_shutdown function anywhere in act_wiz.c

void do_auto_shutdown()
{
   FILE *fp;
   DESCRIPTOR_DATA *d, *d_next;
   char buf [100], buf2[100];
   extern int port,control;
	
   fp = fopen (COPYOVER_FILE, "w");   

   if (!fp)
   {
       for (d = descriptor_list;d != NULL;d = d_next)
       {
           if(d->character)
           {
              do_function( d->character, &do_save, "" );
              send_to_char("Ok I tried but we're crashing anyway sorry!\n\r",d->character);
           }

           d_next=d->next;
           close_socket(d);
       }
         
       exit(1);
   }
	
   do_function( NULL, &do_asave, "changed" );

   sprintf(buf,"\n\rYour mud is crashing attempting a copyover now!\n\r");
   
   for (d = descriptor_list; d ; d = d_next)
   {
	CHAR_DATA * och = CH (d);
	d_next = d->next; /* We delete from the list , so need to save this */
		
	if (!d->character || d->connected > CON_PLAYING) 
	{
	    write_to_descriptor (d->descriptor, "\n\rSorry, we are rebooting. Come back in a few minutes.\n\r", 0);
	    close_socket (d); /* throw'em out */
	}
		
        else
	{
	    fprintf (fp, "%d %s %s\n", d->descriptor, och->name, d->host);
	    save_char_obj (och);
	    write_to_descriptor (d->descriptor, buf, 0);
	}
   }
	
   fprintf (fp, "-1\n");
   fclose(fp);
   fclose (fpReserve);
   sprintf (buf, "%d", port);
   sprintf (buf2, "%d", control);
   execl (EXE_FILE, "rom", buf, "copyover", buf2, (char *) NULL);   
   exit(1);
}



