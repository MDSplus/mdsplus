public fun dc1394__load_image(as_is _nid, optional in _method)
{
  _status = libdc1394_support->dc1394LoadImage();

  return(_status);
} 
