FUN  PUBLIC MITDEVICES() {
  _devices = MitCurrentDevices();
  if(getenv("ExcludeMitCamacDevices") == "") _devices = [_devices, MitCamacDevices()];  
  if(getenv("ExcludeMitObsoleteDevices") == "") _devices = [_devices, MitObsoleteDevices()];
  return(set_range(2, size(_devices)/2, _devices));
}
