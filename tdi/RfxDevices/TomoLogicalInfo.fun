public fun TomoLogicalInfo(in _id)
{
   write(*,"----------------------------------");

   write(*,"Vme Rack    ", TomoVMERack(_id) );

   write(*,"Vme Module  ", TomoVMEModule(_id) );

   write(*,"Vme Chan    ", TomoVMEChan(_id) );

   write(*,"Diode       ", TomoDiode(_id) );

   write(*,"Amp Type    ", TomoAmpType(_id) );

   write(*,"Amp Id      ", TomoAmpId(_id) );

   write(*,"GPIB Module ", TomoGPIBModule(_id) );

   write(*,"GPIB chan   ", TomoGPIBChan(_id) );

   write(*,"Amp Rack    ", TomoAmpRack(_id) );

   write(*,"Is Active   ", TomoChanIsActive(_id) );
  
   write(*,"----------------------------------");

}
