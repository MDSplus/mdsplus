PUBLIC FUN FINDSIG_MULTI (IN _tags, OPTIONAL OUT _trees, OPTIONAL OUT _revert,
                   OPTIONAL IN _no_ptdata, OPTIONAL IN _closetree)
{
        /*
           This function is for using findsig with an array of signals.
           It will return an array of paths, as well as an array of 
           trees for the optional _trees output. 

           If one of the tags is not found, it's index in both the tag and
           tree arrays will be set to 'UNIDENTIFIED'.
        */ 
      
        /* Initialize input / output variables */ 
        if (not present(_no_ptdata)) _no_ptdata=0;
        if (not present(_closetree)) _closetree=0l;
        _revert=0l;
        _num_tags = esize(_tags);        
        _trees = '';
        _paths = '';

        /* Call findsig for each tag */
        for (_i=0; _i<_num_tags; _i++) { 
           _tag   = _tags[_i];
           _path  = if_error(findsig(_tag,_tree,_revert,_noptdata,_closetree),'UNIDENTIFIED'); 
           if ( EQ(_path,'UNIDENTIFIED') ) { _tree = 'UNIDENTIFIED'; } 
           _paths = [_paths,_path];
           _trees = [_trees,_tree];   
        }

        /* Adjust return variables to remove first (blank) element */
        _paths = _paths[1.._num_tags];
        _trees = _trees[1.._num_tags];
        return(_paths);
}
