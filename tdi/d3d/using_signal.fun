/*	USING_SIGNAL.FUN

	USAGE EXAMPLE: using_signal("\\angrot","aot01")
	where the first parameter is a node reference and the second parameter is the
        tree that contains the node

	RETURN: The data/signal from the specified node in the specified tree.

*/

public fun using_signal(_path,_tree,optional in _shot) {

   IF (NOT PRESENT(_shot)) _shot=$SHOT;

   _stat=TreeShr->TreeOpen(ref(_tree),val(_shot));
   IF (NOT(_stat)) { return([0]); }

   _thesig=build_path(_path);
   _units=data(units(_thesig));
   _data=data(_thesig);
   _error=error_of(_thesig);

   _rank=rank(_data);
   if (_rank == 0)
   {
      _dummy = TreeShr->TreeClose(ref(_tree),val(_shot));
      return(make_with_units(make_with_error(_data,_error),_units));
   }
   else
   {

      _dim1=data(dim_of(_thesig));
      _dim1_units=data(units(dim_of(_thesig)));
      if (_rank == 1)
      {
         _dummy = TreeShr->TreeClose(ref(_tree),val(_shot));
         return(make_signal(make_with_units(make_with_error(_data,_error),_units),*,make_with_units(_dim1,_dim1_units)));
      }
      else if (_rank == 2)
      {
         _dim2=data(dim_of(_thesig,1));
         _dim2_units=data(units(dim_of(_thesig,1)));
         _dummy = TreeShr->TreeClose(ref(_tree),val(_shot));
         return(make_signal(make_with_units(make_with_error(_data,_error),_units),*,make_with_units(_dim1,_dim1_units),make_with_units(_dim2,_dim2_units)));
      }
      else
      {
         abort();
      }
   }
}
