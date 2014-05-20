/*---------------------------------------------------------------------------------------------------
  SHOW_TREE_NAMES
  This funciton will spawn an env command to figure out the names of all the MDSplus
  trees on the server.  Output is written to a file since mdsip routinely captures
  stdout & stderr into the MDSplus logfiles.  The output file is written to /tmp so
  that Linux can clean it up automatically.

  20121002 SMF
---------------------------------------------------------------------------------------------------*/

PUBLIC FUN SHOW_TREE_NAMES()
{

	/* Redirect tree names to output file */
	SPAWN("env | grep _path | awk -F'_path' '{print $1}' | uniq | sort >& /tmp/mdstrees.txt");
 
	/* Open file and read all of the tree names */ 
	_u=FOPEN("/tmp/mdstrees.txt","r");
	_tree_names = '';
	while (sizeof(_u) != 0 && (if_error((_l=Read(_u),1),0) != 0))
    	{
		_tree_names = [_tree_names,_l];
	}
	_tree_names = _tree_names[1:*];

	/* Close file and return */
	FCLOSE(_u);
	return(_tree_names);

}	
