/*	 EVENT_TABLE.FUN
; PURPOSE:	Return event table
;               for use with encoders and decoders.
; CATEGORY:	CMOD Specific
; CALLING SEQUENCE: event_table = EVENT_TABLE()
; INPUTS:	none
; OPTIONAL INPUT PARAMETERS: --
; KEYWORD PARAMETERS: --
; OUTPUTS:	--
; OPTIONAL OUTPUT PARAMETERS: --
; COMMON BLOCKS: --
; SIDE EFFECTS: --
; RESTRICTIONS: --
; PROCEDURE:	--
; MODIFICATION HISTORY:
;	TWF 23-SEP-1991 Initial coding.
;	TWF 13-AUG-1992 Removed from EVENT_LOOKUP.FUN

This function should return a event table specific to an MDSplus site. The
event table is a one dimensional string array containing a string pair for
each defined event. The string pair consists of the event name and a value,
for example: 'START','1'.

*/
FUN	PUBLIC EVENT_TABLE() {
        RETURN ( [
                   'START',                 '127',
		   'DISRUPTION',	      '1',
                   'MHD_START',               '2',
                   'ELECTRONS_START',         '3',
                   'PELLET_START',            '4',   
                   'RF_START',                '5',
                   'YAG_START',               '101',
                   'HYBRID_START',            '7',
                   'TCI_START',               '8',
                   'SPECTROSCOPY_START',      '9',
                   'MAGNETICS_START',        '10',
                   'XTOMO_START',            '11',
                   'RF_LIMITER_START',       '12',
                   'PHA_START',              '13',
                   'HALPHA_START',           '14',
                   'BOLOMETER_START',        '15',
		   'PROBES_START',	     '16',
		   'RADIATION_START',	     '17',
		   'ECE_START',		     '18',
		   'UVIS_START',	     '19',
		   'IMP_START',              '20',
		   'TCX_START',              '21',
                   'REFLECTOMETER_START',    '22',
                   'VUV_START',		     '23',
                   'GAS_START',		     '24',
		   'LPI_START',		     '25',
                   'CFP_START',		     '26',
                   'SI_START',		     '27',
		   'PCX_START',		     '28',
		   'GAMMA_START',	     '29',
		   'RF_ANTENNA_START',	     '30',
		   'RF_TRANSMITTER_START',   '31',
		   'VIDEO_START',	     '32',
		   'SA_DEHNG',		     '33',
		   'OMEGA_START',	     '34',
		   'PRISMA_START',	     '35',
		   'DIV_THOM_START',	     '36',
		   'TTCI_START',	     '37',
		   'FSPGAS_START',	     '38',
		   'CHROMEX_START',	     '39',
		   'CRECE_START',	     '40',
		   'RFPROBE_START',	     '41',
		   'SPEC_START',	     '42',
		   'BES_START', 	     '43',
		   'DNB_START', 	     '44',
		   'GPC_2_START',	     '45',
		   'PCI_START',		     '46',
		   'FSP2_START',	     '47',
		   'DNB_COND_CHECK',	     '48',
		   'FRCECE_START',	     '49',
                   'PEI_START',              '50',
                   'ETS_START',              '51',
                   'YAG_ETS_START',          '52',
		   'LONGPULSE_START',	     '53',
		   'OECE_START',	     '54',
		   'RF_ARCLIGHT_START',	     '55',
		   'LH_START',		     '56',
		   'HXR_START',		     '57',
                   'MSE_START',		    '123',
		   'DNB_CHECK', 	    '100',
                   'MLP_START',	            '124',
                   'START_SCALERS',         '125',
                   'STOP_SCALERS',          '126'
                  ]);

}
