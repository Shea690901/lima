/* Do not remove the headers from this file! see /USAGE for more info. */

//      /bin/system/_update.c
//      from the Nightmare Mudlib
//      destructs an old version of a file and loads a new one
//      created by Sulam 911219
//      security changes by Descartes of Borg 930810
//      mixed with the TMI update command by Beek 9301119
//     ported to ZorkMud, Beek 5/13/94
//     also added some features from the previous ZorkMUD update
//     ok, so I ended up rewriting alot of it.
//     Made it able to deal with 3.2's deep-only inherit_list
//     Rust made it so that it calls the doc daemon after successfully
//     loading an object.

#include <mudlib.h>
#include <daemons.h>

inherit DAEMON;


// Keep track of what we've done this round.  For technical reasons
// involving virtual inheritance this can't be done globally.
// Or maybe it can.  Who knows.  This is a kludge.

// this probably isn't needed any more and should be removed :)
mapping done;

int do_update(string file, int deep);

int main(mixed *arg, mapping flags) {
    object *obs;
    object ob;
    string temp;
    int n,deep_up;
    string file;

    done = ([]);

    if (flags["r"]) deep_up = 1;
    if (flags["R"]) deep_up = 2;
    file = arg[0];
    if (!file) file = this_body()->query_cwf();
    if (!file || file == "here") file = "/"+file_name(environment(this_body()));
    file = evaluate_path(file);
    this_body()->set_cwf( file );
    sscanf(file, "%s.c", file);
    if (file_size(file+".c")==-1) {
	printf("update: no such file.\n");
	return 0;
    }
    if(file_size(file) == -2 && file_size(file+".c") < 0 )
    {
	printf("update: file is a directory.\n");
	return 0;
    }
    if(file_size(file+".c"))  this_body()->set_cwf(file+".c");
    obs = 0;
    if (ob = find_object(file)) {
	obs = all_inventory(ob);
	for (n = 0; n < sizeof(obs); n++)
	{
	    if (obs[n]->query_link() && interactive(obs[n]->query_link()))
		obs[n]->move(VOID_ROOM);
	    else {
		obs[n]->remove();
		if (obs[n]) destruct(obs[n]);
	    }
	}
    }
    if (do_update(file,deep_up)) {
	for (n=0; n<sizeof(obs); n++) {
	    if (obs[n]) obs[n]->move(file);
	}
	return 1;
    }
    write("Error loading object.\n");
    return 0;
}

int do_update(string file, int deep) {
    object ob2;
    string *obs;
    int master_flag;
    int i,n;
    string res;

    ob2 = find_object(file);
    if(ob2) {
	if (deep) {
	    obs = inherit_list(ob2);
	    n = sizeof(obs);
	    for (i=0; i<n; i++) {
		if (done[obs[i]]) continue;
		done[obs[i]] = 1;
	    if (!do_update(obs[i], (deep==2?2:0))) return 0;
	}
    }
    ob2->remove();
    if(ob2) {
	destruct(ob2);
    }
}
res = catch(call_other(file,"???"));
if (res) printf(file+": "+res);
else{
    printf(file+": Updated and loaded.\n");
}
return 1;
}

int help()
{
    write(wrap(
	"Usage: update [-rR] <filename>\n"
	"The update commands loads or reloads the specefied file.  If no "
	"argument is given, the last file the wizard was working on is used "
	"or the environment if there is no such file.  The update command "
	"only affects the master copy, not any clones that were loaded "
	"before.\nThe optional flags -r and -R will also update files "
	"inherited by the file.  update -R will update this object, the "
	"files that it inherits, any files that _those_ inherit, and so "
	"on.  update -r just updates the files inherited by the object.\n"));
    return 1;
}
