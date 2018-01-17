/*
  ptdata_error()
  Given a PTDATA error code, this function returns
  the corresponding error message.
*/
fun public ptdata_error(in _error) {

  switch(_error) {
    case (-1) return("error in DFI_DECODE");
    case (0) return("no errors");
    case (1) return("pointname does not exist for this shot");
    case (3) return("shot-source not found on disk");
    case (4) return("fewer data points returned than requested");
    case (5) return("unrecoverable file access error");
    case (7) return("invalid shot number");
    case (12) return("shotfile on optical disk");
    case (13) return("shotfile on tape");
    case (14) return("invalid call type");
    case (20) return("error allocating/deallocating buffer space for pointname");
    case (21) return("error getting node name or unknown node name");
    case (22) return("error opening network link connection");
    case (23) return("error writing shot number to remote node");
    case (24) return("error reading shot status from remote node");
    case (25) return("error reading data from remote node");
    case (30) return("digitizer or requested delta time less than or equal to zero");
    case (31) return("number of data points requested is less than one");
    case (32) return("invalid integer bits/word in fixed header");
    case (34) return("for call type 2,7,12, start or delta point less than 1");
    case (35) return("data format index is inconsistent with calling type");
    case (36) return("pointname requires special non ptdata timing handling");
    case (37) return("data format index is inconsistent with calling type");
    case (40) return("pointname size was too big to be processed");
    case (41) return("pointname size less than fixed header size");
    case (42) return("pointname size less than header word count");
    case (43) return("fixed+variable header greater than header word count");
    case (44) return("one or more variable header counts is negative");
    case (45) return("header word size from Modcomp not a multiple of 128");
    case (50) return("data file directory inconsistent with fixed header");
    case (53) return("invalid data type in fixed header");
    case (60) return("Modcomp error - fatal acquisition error. No data.");
    case (61) return("Modcomp error - I/O error in writing disk file.");
    case (62) return("Modcomp error - database error.");
    case (63) return("Modcomp error - processing inconsistencies.");
    case (64) return("Modcomp error - transfer error");
    case (70) return("Invalid PTDATA version on MULTIFLOW");
    case (71) return("MULTIFLOW error opening network link.");
    case (72) return("MULTIFLOW error writing to network link.");
    case (73) return("MULTIFLOW error reading from network link.");
    case (74) return("Allocation error on VAX");
    case default return("Unknown PTDATA error");
  }

  return("Unknown PTDATA error");
}
