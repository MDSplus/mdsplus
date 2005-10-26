PUBLIC FUN FINDSIG_CLEAN (IN _tag, OPTIONAL OUT _tree, OPTIONAL OUT _revert,
                   OPTIONAL IN _no_ptdata, OPTIONAL IN _closetree)
{

        /* This function is for helping users call FINDSIG.FUN when they want the
           function to close the D3D and EFIT trees.  Otherwise, users would have
           to call findsig with all the otional output arguments first, in order
           to specify the _closetree argument.  This just makes it simpler. i
        
           Behavior - All inputs / outputs work as they do in FINDSIG
                      Except, FINDSIG_CLEAN defaults to _closetree=1
                      Whereas FINDSIG defaults to _closetree=0        */


        IF (NOT PRESENT(_no_ptdata)) _no_ptdata=0;
        _revert=0l;
        _tree='';
        if (not present(_closetree)) _closetree=1l;

        return( findsig(_tag,_tree,_revert,_no_ptdata,_closetree) );

}
