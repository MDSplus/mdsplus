FUN PUBLIC NIMROD_DB(IN _shot, IN _runtype, IN _runcomment, IN _runby, IN _experiment)
{

   _cmd = "/mdstrees2/nimrod_temp/nimrod_db "//_shot//" "//_runtype//" '"//_runcomment//"' "//_runby//" "//_experiment;
   SPAWN(_cmd);
   return(1);
}
