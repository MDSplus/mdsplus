public fun VX4244__store(as_is _nid, optional _method)
{
   private _N_ADDRESS = 1;
   private _N_ID = 2;
   private _N_SAMPLE_PERIOD = 5;     
   private _N_SAMPLE_DELAY = 6;      
   private _N_SAMPLE_NUM = 8;        
   private _N_TRIG_T0 = 14;          
   private _N_INPUT_RANGE = 19;      
   private _N_FOO_0 = 21;            
   

/* Get id to communicate with a digitizer */
   private _id = if_error(data(DevNodeRef(_nid, _N_ID)),(DevLogErr(_nid, "Driver is not initialized!"); abort();));
    
/* Get interface type
 VI_INTF_GPIB (1)
 VI_INTF_VXI (2)
 VI_INTF_GPIB_VXI (3)
 VI_INTF_ASRL (4)
 VI_INTF_PXI (5)
 VI_INTF_TCPIP (6)
 VI_INTF_USB (7) */

   _interface = 0WU;
   IF_ERROR(Tkvx4244_32->tkvx4244_getInterfaceType(VAL(_id), REF(_interface)),(DevLogErr(_nid, "Error on tkvx4244_getInterfaceType()"); abort();));

/* Read data from a digitizer */
   private _port = 0w;
   private _msg= repeat(' ',128);

/* Raw data -> signal */
   IF_ERROR(Tkvx4244_32->tkvx4244_readInputVoltageSettings(VAL(_id), VAL(0),REF(_msg)),(DevLogErr(_nid, "Error while readInputVoltageSettings"); abort();));
   _range=compile("["//extract(0,112,_msg)//"]");
       
   IF_ERROR(Tkvx4244_32->tkvx4244_readActiveChannels(VAL(_id), VAL(1),REF(_msg)),(DevLogErr(_nid, "Error while readActiveChannels"); abort();));
   _ch=compile("["//extract(0,31,_msg)//"]");

   for(_i=0; _i<4; _i++)
   {
      _dt = if_error(data(DevNodeRef(_nid, _N_SAMPLE_PERIOD +_i*39)),(DevLogErr(_nid, 'Missing #, Sampling period in GP'//text(_i,1)); abort();));
      _dt = 1.0/_dt;
      _num = if_error(data(DevNodeRef(_nid, _N_SAMPLE_NUM +_i*39)),(DevLogErr(_nid, 'Missing #, Sample count in GP'//text(_i,1)); abort();));
      _t0 = if_error(data(DevNodeRef(_nid, _N_TRIG_T0 +_i*39)),(DevLogErr(_nid, 'Missing #, T0 in GP'//text(_i,1)), 0.));
      _delay = if_error(data(DevNodeRef(_nid, _N_SAMPLE_DELAY +_i*39)),(DevLogErr(_nid, 'Missing #, Delay in GP'//text(_i,1)), 0.));
      _t0 = _t0 + _delay;
      _dim = make_dim(make_window(0, _num - 1, _t0), make_range(*,*,_dt));
      IF (_interface==2)
/* For Embedded Controller or MXI2PCI interface */
      {
        _buf = '[';
        _nGp = 0;        
        FOR(_j=0; _j<4; _j++)
        {
           IF (_ch[4*_i+_j])
           {
              _buf=_buf//TEXT(_j,1)//',';
              _nGp++;
           }
        }
        _idx=COMPILE(EXTRACT(0,MAX(1,_nGp*2),_buf)//']');
        _nTotal = _nGp * _num;
        _response = array(_nTotal, 0w);

        IF (_nGp>0)
        {
          _status=IF_ERROR(Tkvx4244_32->tkvx4244_confFDCTransfer (VAL(_id), VAL(0), VAL(_i==0), VAL(_i==1), VAL(_i==2), VAL(_i==3), VAL(0), VAL(_nTotal), VAL(0)),
        	(DevLogErr(_nid, 'Error in confFDCTransfer() for GP'//text(_i,1)); abort();));
          IF (_status<0)
          {
            IF_ERROR(Tkvx4244_32->tkvx4244_error_message (VAL(_id), VAL(_status), REF(_msg)),
	             (DevLogErr(_nid, 'Error in tkvx4244_error_message() for GP'//text(_i,1)); abort();));
	    write(*, 'In confFDCTransfer()', _msg);
          }

          _status=IF_ERROR(Tkvx4244_32->tkvx4244_getFDCData (VAL(_id), VAL(_i), VAL(_nTotal), REF(_response)),
	             (DevLogErr(_nid, 'Error in tkvx4244_getFDCData() for GP'//text(_i,1)); abort();));
          IF (_status<0)
          {
            IF_ERROR(Tkvx4244_32->tkvx4244_error_message (VAL(_id), VAL(_status), REF(_msg)),
	             (DevLogErr(_nid, 'Error in tkvx4244_error_message() for GP'//text(_i,1)); abort();));
	    write(*, 'In getFDCData()', _msg);
          }
        }

        FOR(_j=0; _j<_nGp; _j++)
        {
          _k=_idx[_j];
          _signal = compile('build_signal(build_with_units((`_range[4*_i+_k]/32768)*$VALUE,"V"), (`_response[(_k):(_nTotal-1):(_nGp)]), (`_dim))');
          TreeShr->TreePutRecord(VAL(DevHead(_nid) + _N_FOO_0 + 39 *_i + 6 *_k), XD(_signal), VAL(0));
        }
      }
/* For GPIB interface */
      ELSE
      {
        _response = array(_num, 0w);
        for(_j=0; _j<4; _j++)
        {
           if (_ch[4*_i+_j])
           {
             IF_ERROR(Tkvx4244_32->tkvx4244_getBinaryData (VAL(_id), VAL(_num), VAL(0), VAL(_j+_i*4+1), REF(_response)),
	             (DevLogErr(_nid, 'Error in tkvx4244_getBinaryData() for AI'//text(_j+_i*4,2)); abort();));
             _signal = compile('build_signal(build_with_units((`_range[_j+_i*4]/32768)*$VALUE,"V"), (`_response), (`_dim))');
             TreeShr->TreePutRecord(VAL(DevHead(_nid) + _N_FOO_0 + 6 *_j + 39 *_i), XD(_signal), VAL(0));
           }
        }
      }
    }
    TreeClose();
    return (1);
}
