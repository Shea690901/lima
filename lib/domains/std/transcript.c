/* Do not remove the headers from this file! see /USAGE for more info. */

// Written by Rust.
#include <mudlib.h>

inherit OBJ;
inherit M_READABLE;
inherit M_GETTABLE;


void create(string text)
{
  if(!strlen(text))
    {
      text = "*** Recorder heard nothing but silence.\n";
    }
  
  object::create();
  set_id("transcript");
  set_gettable(1);
  set_in_room_desc("A transcript of a recording lies here.");
  set_long("It's got a transcript of a conversation on it.\n");
  set_text(text);
}