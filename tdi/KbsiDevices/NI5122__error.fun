public fun NI5122__error(as_is _nid, optional _method)
{
    private _N_ID = 2;
    private _n=0;
    private _message= repeat(' ',128);
    
    _id = if_error(data(DevNodeRef(_nid, _N_ID)), (DevLogErr(_nid, "Driver is not initialized!"); abort();));
    for(_i=0; _i<10; _i++)
    {
       if_error(hpe1564_32->hpe1564_error_query (VAL(_id),REF(_n),REF(_message)), (DevLogErr(_nid, "Error in hpe1564_error_query()!");abort();));
       if(_n==0) break;
        write(*,getnci(DevHead(_nid),'NODE_NAME'), _n, _message);
    }
    TreeClose();
    return (1);
}
This function is included for compliance with the IviScope Class Specification.

Reads an error code and message from the error queue. 
National Instruments digitizers do not contain an error queue. 
Errors are reported as they occur. Therefore, this function does not detect errors; 


/*-------------------- Prototype ---------------------*/
ViStatus niScope_error_query (ViSession Instrument_Handle,
                              ViPInt32 Err_Code,
                              ViChar _VI_FAR Err_Message[]);
