public fun CamImage()
{
  if (len(getenv("CTS_DB_DIR")) == 0)
    _image="RemCamShr";
  else
    _image="CamShr";
  return(_image);
}
