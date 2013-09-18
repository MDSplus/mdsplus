public fun MarteAbortI2t()
{

/*
FAULT

 
BIT 0  EDA1
BIT 1  DEQU
BIT 2  EDA3
BIT 3  DFLU
BIT 4  MHD_AC    ora EDA3  : MHD superato il limite di I2t 
BIT 5  MHD_BC    ora EDA1  : Allarme sforzi su bobine F 
BIT 6  MHD_BR    ora SIGMA : Marte check fallito. MARTe thread in idle state
BIT 7  MHD_BR_I  
BIT 8  EDAV
BIT 9  Brad		 ora EDA3  : MHD Superato il limite di campo radiale 

WARNING

BIT 0  EDA1
BIT 1  DEQU
BIT 2  EDA3
BIT 3  DFLU
BIT 4  MHD_AC
BIT 5  MHD_BC
BIT 6  MHD_BR
BIT 7  MHD_BR_I
BIT 8  EDAV
BIT 9  Brad
BIT 10 MHD saturazione correnti
BIT 11 VIK1 pressure
*/

	return ( libOpcJniInterface->setWarningFaultBit("FAULT", "SET", val(4)) ); 
}
