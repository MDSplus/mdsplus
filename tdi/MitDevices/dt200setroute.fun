public fun Dt200SetRoute(in _board, in _line, in _wire, in _bus )
{
  Dt200WriteMaster(_board, "setSyncRoute "//_line//" "//_wire//" "//_bus);
  return(1);
}
