/* Do not remove the headers from this file! see /USAGE for more info. */

// product du Belboz/Rust

#include <mudlib.h>

string* MSGS;


create()
{
 MSGS =	({
"a statue of Dimwit Flathead",
"a Frobozz magical broom",
"supermodel Elle McFlathead",
"a fearesome hellhound",
"a crowd of Elvis Flathead fans",
"the boot patrol",
"a giant brogmoid",
"the Wings of Icarus",
"the mighty dragon Thermofax",
"laughing eldrich vapors",
});
}



int main( string arg )
{
  object	start_room;
  string	name;

  start_room = environment( this_body() );
  name	= this_body()->query_name();

  if(!arg)
    arg = MSGS[random(sizeof(MSGS))];

  if( this_body()->move( WIZ_ROOM ) )
  {
    write( "You remain where you are.\n" );
    return 1;
  }
  force_look();
  printf("You are carried to the wizard lounge on %s!\n", arg );
  if( name == "Someone" )
    return 1;

  tell_room( start_room, sprintf( "%s is whisked away on %s!\n",
	name, arg ) );
  say( sprintf( "%s arrives on %s!\n", name, arg ) );
  return 1;
}

int help()
{

write(wrap(
"Usage: wizz [message]\n"
"Takes you to the wizard lounge from anywhere on the mud, and optionally "
"allows you to specify what you arrive on.\n" ));

return 1;
}
