public fun RfxControlNameToIdx(in _idx)
{
   
	_names = [ "OFFSET CORRECTION", 
			   "WAVE GENERATION", 
			   "IND. PERTURBATION ", 
			   "ROT.PERTURBATION", 
			   "SIMULINK", 
			   "LOCK CONTROL", 
			   "MODE CONTROL",
			   "MODE CONTROL + ROT.PERT. (FEEDBACK)",			   
			   "VIRTUAL SHELL", 
			   "VS+ROT.PERT.(FEEDBACK)", 
			   "VS+ROT.PERT.(FEEDFORW)", 
			   "Bt CONTROL", 
			   "F CONTROL", 
			   "Closer VS", 
			   "Closer VS + Rot.Pert.", 
			   "MODE CONTROL + ROT.PERT. (FEEDFORW)",
			   "Mode control + Shell comp.",
			   "Open Loop F Control",
			   "Open Loop Q Control",
			   "Flux Control",			   
			   "CMC + Modal Decoupling",			   
			   "Unknow" ];
			   
			   
    _code  = [ 1, 6, 5, 3, 7, 8, 4, 9, 10, 11, 12, 13, 14, 15, 16, 20, 17, 18, 19, 20, 23];

	for(_i = 0; _i < size(_code)  ; _i++)
		if( _code[_i] == _idx )
			break;
		
	return ( trim(adjustl(_names[_i])) );

}