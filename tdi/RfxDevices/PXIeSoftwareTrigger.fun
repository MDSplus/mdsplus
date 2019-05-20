public fun PXIeSoftwareTrigger( in _devIDs, in _numIDs )
{
     return( libNiInterface->PXIeTriggerRTSI1( _devIDs, val(_numIDs) ) );
} 
