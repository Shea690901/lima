/* Do not remove the headers from this file! see /USAGE for more info. */

#include <mudlib.h>

inherit DAEMON;

nomask int main(string* argv)
{
  string fname;
  if(sizeof(argv))
    fname =argv[0];
    if ( !fname )
    {
        fname = this_body()->query_cwf();
        if ( !fname )
        {
            write("You have no cwf. Please specify a file.\n");
            return 1;
        }
    }
  else fname = evaluate_path(fname);

    this_body()->set_cwf(fname);

    clone_object(ED_SESSION)->begin_editing(fname);

    return 1;
}
