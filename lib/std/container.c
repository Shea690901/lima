/* Do not remove the headers from this file! see /USAGE for more info. */
// container.c
// possibly the grossest piece of code in the mudlib.

#include <mudlib.h>
#include <move.h>
#include <setbit.h>
#include <driver/origin.h>

inherit OBJ;


private int max_capacity = 10;
private static int capacity;
private mapping objects;
string main_prep = "in";

void reset();

//:FUNCTION container
//Returns 1 if an object is a container
int
container()  { return 1; }

int query_capacity();

int stat_me() {
    write("Container capacity: "+query_capacity()+"/"+max_capacity+"\n");
    write("main_prep: " + main_prep + "\n");
    write("It contains: "+
      inv_list(this_object(), 0, 1)+"\n");
    return ::stat_me();
}

//:FUNCTION ob_state
//Determine whether an object should be grouped with other objects of the
//same kind as it.  -1 is unique, otherwise if objects will be grouped
//according to the return value of the function.
int ob_state() {
/* if we have an inventory we should be unique */
    if (first_inventory(this_object())) return -1;
    return 0;
}

//:FUNCTION query_capacity
//Returns the amount of mass currently in a container
int query_capacity()
{
  return capacity;
}

int query_mass()
{
  return capacity + ::query_mass();
}

//:FUNCTION set_capacity
//Sets the amount of mass inside a container; this function should probably
//never be called as the value is kept internally.
static void
set_capacity( int c )
{
//### what calls this?  Is it needed?
  capacity = c;
}

//:FUNCTION query_max_capacity
//Find out the maximum capacity of an object
int query_max_capacity()
{
    return max_capacity;
}

//:FUNCTION receive_object
//Determine whether we will accept having an object moved into us;
//returns a value from <move.h> if there is an error
int receive_object( object target, string relation )
{
    int x,m;
    object ob;

    if( target == this_object() )
	  return MOVE_NOT_ALLOWED;

    if( origin() != ORIGIN_LOCAL )
    {
        /* allow a matching relation or newly cloned objects */
        if ( relation && relation != main_prep && relation != "#CLONE#" )
            return MOVE_NOT_ALLOWED;
	if (!this_object()->container())   return MOVE_NOT_CONTAINER;
	else if (this_object()->hold_container() ||
	  this_object()->this_function_should_return_non_0_if_and_only_if_this_container_is_not_hypothetically_capable_of_containing_another_container())
	    return MOVE_NOT_ALLOWED;
    }
    x=target->query_mass();
//    printf("cap=%O  max=%O  x=%O\n",query_capacity(),max_capacity,x);
    if ((m=(query_capacity())+x)>max_capacity)
    { return MOVE_NO_ROOM; }

    set_capacity( m );

    return 0;
}

//:FUNCTION release_object
//Prepare for an object to be moved out of us; the object isn't allowed
//to leave if we return nonzero
int release_object( object target )
{
    set_capacity( query_capacity() - (target->query_mass()) );
    return 0;
}

//:FUNCTION set_max_capacity
//Change the maximum capacity of an object.
void
set_max_capacity(int x) {

/* You can mask this if you want it to be changed externally in
   your room, but this is to prevent pranks
 */
//### why not just static it?
    if( origin() != ORIGIN_LOCAL)
	return 0;
    max_capacity = x;
}

//:FUNCTION look_in
//returns a string containing the result of looking inside (or optionally
//a different relation) of the object
string look_in( string relation )
{
    object* obs;
    string inv;

    if (relation && relation != main_prep)
        return "There is nothing there.\n";

    inv = inv_list(this_object(), 0, 1);
    if ( !inv )
	inv = "  nothing";
    
    return (sprintf( "%s %s you see: \n%s\n",
	capitalize(main_prep), short(),
	inv ));
}

void create() {
    objects = ([]);
    set_capacity( 0 );
    ::create();
}

//:FUNCTION set_objects
//Provide a list of objects to be loaded now and at every reset.  The key
//should be the filename of the object, and the value should be the number
//of objects to clone.  Note:  the number already present is determined
//by counting the number of objects with the same first id, and objects
//are only cloned to bring the count up to that number
void set_objects(mapping m) {
    objects = m;
    reset();
}

//:FUNCTION inventory_header
//Returns a string header to put before inventory lists
string inventory_header() {
    return "It contains:";
}

int inventory_visible();
int inventory_accessible();

string long() {
    string res;
    string contents;

    res = ::long();
    contents = inv_list(this_object(), 1, 1);
  if (inventory_visible() && contents)
    res += inventory_header() + "\n" + contents;
    return res;
}

//:FUNCTION simple_long
//Return the long description without the inventory list.
string simple_long() {
    return ::long();
}

void reset(){
    int j, tally, value;
    string file, name;
    string *oids;
    object *inv;
    int ret;
    
    if(!mapp(objects))
	return;

    inv = all_inventory();
    foreach (file, value in objects) {
	if (intp(value))
	{
	    oids = file->query_id();
	    if( pointerp(oids) && sizeof(oids) )
		name = oids[0];
	    else if (!stringp(oids))
		name = "";

	    tally = sizeof(filter_array(inv, (: $1->id($(name)) :) ));
	    value -= tally;
	    for (j = 0; j < value; j++) {
		if (ret = clone_object(file)->move(this_object(), "#CLONE#"))
		    error("Initial clone failed for '" + file +"' with error " + ret + "\n");
	    }
	}
    }
}

//:FUNCTION inventory_visible
//Return 1 if the contents of this object can be seen, zero otherwise
int inventory_visible() {
  if (test_flag(INVIS)) return 0;
  if (!short()) return 0;
  if (test_flag(TRANSPARENT)) return 1;
  return !this_object()->query_closed();
}

//:FUNCTION inventory_accessible
//Return 1 if the contents of this object can be touched, manipulated, etc
int inventory_accessible() {
  if (test_flag(INVIS)) return 0;
  if (!short()) return 0;
  return !this_object()->query_closed();
}

//:FUNCTION can_take_from
//This is compat, I think

//### is this called?
int can_take_from() { return inventory_accessible(); }

//:FUNCTION can_put_in
//can objects put put into us?

//### this is outdated; should be done in indirect_put_obj_in_obj()
int can_put_in() {
    if (this_object()->query_closed()) {
        write(capitalize(the_short())+" is closed.\n");
        return -1;
    }
    return inventory_accessible();
}

//:FUNCTION introduce_contents
//returns a string appropriate for introduction the contents of an object
//in room descriptions.
string
introduce_contents() {
    switch (main_prep) {
    case "in": return capitalize(the_short()) + " contains:\n";
    case "on": return "Sitting on "+the_short()+" you see:\n";
    default: return capitalize(main_prep)+" "+the_short()+" you see:\n";
    }
}

string inventory_recurse(int depth) {
    string res;
    object *obs;
    int i;
    string str;
    string tmp;

    str = "";
    obs = all_inventory(this_object());
    for (i=0; i<sizeof(obs); i++) {
        if (obs[i]->test_flag(INVIS)) continue;
        if (!obs[i]->test_flag(TOUCHED) && obs[i]->untouched_long()) {
	    str += obs[i]->untouched_long()+"\n";
             if (obs[i]->inventory_visible())
                 str += obs[i]->inventory_recurse(depth+1);
        }
    }
    res = introduce_contents();
    if (tmp = inv_list(this_object(), 1, depth + 1)) {
      for (i=0; i<depth-1; i++) str += "  ";
      str += res + tmp;
    }
    return str;
}

//:FUNCTION set_preposition
//Sets what relation the contents of this object are to it.  I.e. are they
//in it, on it, etc ...
void set_preposition( string new_prep )
{
    if( origin() != ORIGIN_LOCAL ) return;

    main_prep = new_prep;
}

// Recognize this as a shitty scope hack?  I do.

//### obsolete?
string query_prep(){ return main_prep; }

// Right now this is called only if mass is set in an object that is in
//the env, or if release_object() has been called and recieve_object() bails
void update_capacity()
{
  mixed	masses;
  int m,i;

  masses = all_inventory()->query_mass();
  i = sizeof( masses );

  while( i-- )
    m += masses[i];

  set_capacity(m);
}

/* verb interaction */
mixed direct_get_obj(object ob, string name) {
    if (this_object() == environment(this_body())) {
	if (this_object()->get_item_desc(name))
	    return "That doesn't seem possible.\n";
        return "You're in it!\n";
    }
    return ::direct_get_obj(ob);
}

mixed indirect_put_obj_word_obj(object ob1, string prep, object ob2) {
    return prep == main_prep;
}

mixed direct_look_str_obj(string prep, object ob) {
    if (prep == main_prep)
	return 1;
    return "There is nothing " + prep + " the backpack.\n";
}