/* Do not remove the headers from this file! see /USAGE for more info. */

// Belboz

#include <daemons.h>
#include <mudlib.h>

inherit CMD;

private void main()
{
    int		pts,total;
    string	rank;

    if( wizardp(this_user()) )
    {
	write("You are an implementor.\n");
    }

    pts = this_body()->query_score();
    total = QUEST_D->total_points();
    if(total)
      switch( (100*pts)/total )
	{
	case 0:  rank = "complete and utter newbie";break;
	case 1..5: rank = "rank amateur";break;
	case 6..10: rank = "tyro";break;
	case 11..20: rank = "not quite rank, but still amateur";break;
	case 21..30: rank = "tenderfoot spelunker";break;
	case 31..40: rank = "second-class spelunker";break;
	case 41..50: rank = "first class spelunker";break;
	case 51..60: rank = "adventurer";break;
	case 61..70: rank = "expert adventurer";break;
	case 71..80: rank = "skilled player";break;
	case 81..90: rank = "complete and utter player";break;
	case 91..99: rank = "veteran player";break;
	default:
	  if(pts < 0 ) rank = "blundering buffon";
	  else rank = "Dungeon Master";
    }
    if(total)
      printf("You have earned %d out of the %d points on %s.\n"
	     "This gives you the rank of %s.\n", pts, total, mud_name(), rank );
    else
      printf("There are no points or anything on %s, so I guess you're doing Ok.\n", mud_name());
}