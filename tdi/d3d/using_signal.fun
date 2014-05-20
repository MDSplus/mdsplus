/*	USING_SIGNAL.FUN

	USAGE EXAMPLE: using_signal("\\angrot","aot01")
	where the first parameter is a node reference and the second parameter is the
        tree that contains the node

	RETURN: The data/signal from the specified node in the specified tree.

	This function was designed for the AOT tree.  Note that the parameter tree does
        not have to be open in order for this function to work.
*/

public fun using_signal(_path,_tree,optional in _shot) {

   if (!present(_shot)) {   
      _thesig=using(build_path(_path),"\\TOP",*,_tree);
      _units=using(data(units(_thesig)),"\\TOP",*,_tree);
      _data=using(data(_thesig),"\\TOP",*,_tree);
			_error=using(error_of(_thesig),"\\TOP",*,_tree);
   } else {
      _thesig=using(build_path(_path),"\\TOP",_shot,_tree);
      _units=using(data(units(_thesig)),"\\TOP",_shot,_tree);
      _data=using(data(_thesig),"\\TOP",_shot,_tree);
			_error=using(error_of(_thesig),"\\TOP",_shot,_tree);
   }

   _rank=rank(_data);
   if (_rank == 0)
   {
      return(make_with_units(make_with_error(_data,_error),_units));
   }
   else
   {

      if (!present(_shot)) { 
         _dim1=using(data(dim_of(_thesig)),"\\TOP",*,_tree);
         _dim1_units=using(data(units(dim_of(_thesig))),"\\TOP",*,_tree);
      } else {
         _dim1=using(data(dim_of(_thesig)),"\\TOP",_shot,_tree);
         _dim1_units=using(data(units(dim_of(_thesig))),"\\TOP",_shot,_tree);
      }

      if (_rank == 1)
      {
         return(make_signal(make_with_units(make_with_error(_data,_error),_units),*,make_with_units(_dim1,_dim1_units)));
      }
      else if (_rank == 2)
      {
         if (!present(_shot)) {
            _dim2=using(data(dim_of(_thesig,1)),"\\TOP",*,_tree);
            _dim2_units=using(data(units(dim_of(_thesig,1))),"\\TOP",*,_tree);
         } else {
            _dim2=using(data(dim_of(_thesig,1)),"\\TOP",_shot,_tree);
            _dim2_units=using(data(units(dim_of(_thesig,1))),"\\TOP",_shot,_tree);
         }
         return(make_signal(make_with_units(make_with_error(_data,_error),_units),*,make_with_units(_dim1,_dim1_units),make_with_units(_dim2,_dim2_units)));
      }
      else
      {
         abort();
      }
   }
}
