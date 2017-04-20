public fun RTSM_stop()
{
   wait(5);
   tcl('do/method \\ISEPS::RTSM post_req');
   wait(5);
   tcl('do/method \\ISEPS::RTSM collection_complete');
   wait(5);
   return (1);
}
