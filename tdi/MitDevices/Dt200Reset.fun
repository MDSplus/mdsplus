public fun Dt200Reset(in _board)
{
  Dt200WriteMaster(_board, "setAbort");
  Dt200WriteMaster(_board, "fireEvent AI E1");
  Dt200WriteMaster(_board, "fireEvent AI E2");
  Dt200WriteMaster(_board, "fireEvent AI E3");
  Dt200WriteMaster(_board, "fireEvent AO E1");
  Dt200WriteMaster(_board, "fireEvent AO E2");
  Dt200WriteMaster(_board, "fireEvent AO E3");
  Dt200WriteMaster(_board, "fireEvent DO E1");
  Dt200WriteMaster(_board, "fireEvent DO E2");
  Dt200WriteMaster(_board, "fireEvent DO E3");
  Dt200WriteMaster(_board, "resetGUT");
}
