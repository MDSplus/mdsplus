public fun GPIBReadShorts(in _ud, in _size)
{

write(*, 'Sono la GPIBReadShorts ', _size);
  return(gpiblib->GPIBReadShorts:dsc(_ud, _size));
}
