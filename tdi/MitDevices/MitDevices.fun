FUN  PUBLIC MITDEVICES() {
  _devices = MitCurrentDevices();
  if(getenv("ExcludeMitCamacDevices") == "") _devices = [_devices, MitCamacDevices()];  
  if(getenv("ExcludeMitObsoleteDevices") == "") _devices = [_devices, MitObsoleteDevices()];
  return(set_range(size(_devices)-1, _devices));
}
