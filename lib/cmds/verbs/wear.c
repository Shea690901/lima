#include <mudlib.h>

inherit VERB_OB;

mixed can_wear_obj(object ob)
{
    return 1;
}

void do_wear_obj(object ob)
{
    if (!try_to_acquire(ob))
	return;

    ob->wear();
    this_body()->simple_action("$N $vwear $o.\n", ob);
}
