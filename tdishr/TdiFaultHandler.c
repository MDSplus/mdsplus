int TdiFault=0;
int TdiFaultHandler(){return 1;}
int TdiFaultHandlerNoFixup(){return 1;}
int TdiFaultClear(){TdiFault=0; return 1;}
