
PUBLIC FUN CREATE_EFITRT_TEST (IN _shot)
{

   /* This routine is used to create non-btshot EFITRT trees on atlas
      from the PCS / Simserver: i.e. 700000+, 900000+ */    

   _cmd = ['set tree efitrt1/shot=-1'];
   _cmd = [_cmd,'create pulse '//ADJUSTL(_shot)];
   _cmd = [_cmd,'close'];
   _cmd = [_cmd,'set tree efitrt2/shot=-1'];
   _cmd = [_cmd,'create pulse '//ADJUSTL(_shot)];
   _cmd = [_cmd,'close'];

   write(*,"Creating EFITRT trees for shot: "//ADJUSTL(_shot));
   _status = TCL(_cmd);

   return(1);
}

