/* $Id$ */

public fun DevPutValue(in _nid, in _value, optional in _units )

/*
 *  TDI function to store a value (with optional units) into a node
 *
 *  _nid must be the node id to hold the value
 *	(use DevHead() to get value)
 */

{

  if ( not present(_units) ) 
    _val_build = _value ;
  else
    _val_build = compile( 'build_with_units( `_value, `_units )' );
  
  return( TreeShr->TreePutRecord(val(_nid),xd(_val_build),val(0)) );

}
