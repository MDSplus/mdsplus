public fun TR10SetPostSamples(in _handle, in _post_trig)
{
  TR10Error(TR10->TR10_Trg_SetPostSamples(val(_handle), val(long(_post_trig))));
}
