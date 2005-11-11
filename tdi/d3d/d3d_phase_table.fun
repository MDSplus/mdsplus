/*	 PHASE_TABLE.FUN
; PURPOSE:	Site customizable phase table
;               for use with MIT Dispatch/Serve
; CATEGORY:	CMOD (site customizable)
; CALLING SEQUENCE: phase_table = PHASE_TABLE()
; INPUTS:	 --
; OPTIONAL INPUT PARAMETERS: --
; KEYWORD PARAMETERS: --
; OUTPUTS:	--
; OPTIONAL OUTPUT PARAMETERS: --
; COMMON BLOCKS: --
; SIDE EFFECTS: --
; RESTRICTIONS: --
; PROCEDURE:	--
; MODIFICATION HISTORY:
;	TWF 3-AUG-1992 Initial coding.
;       CUSTOMIZED FOR DIII-D by Jeff Schachter
;        - initial date 2000.04.25


This function should return a phase table specific to a MDSplus site. The
phase table is a one dimensional string array containing a string pair for
each defined phase. The string pair consists of the phase name and value,
for example: 'PHASE_ONE','1'.

*/
FUN	PUBLIC PHASE_TABLE() {
          RETURN ( [
                     'OFF',                         '0',
                     'NEW',                         '1',
                     'MAG',                         '2',
                     'CO2',                         '3',
                     'TS_MDS',                      '4',
                     'EFIT01_MDS',                  '5',
                     'CERNEUR',                     '6',
                     'CERQUICK_MDS',                '7',
                     'NB',                          '8',
                     'EFIT',                        '9',
                     'IGS',                        '10',
                     'PHD',                        '11',
                     'PHDF',                       '12',
                     'SPR',                        '13',
                     'IMPDENS_MDS',                '14',
                     'HEL',                        '15',
                     'CERNEUR_MDS',                '16',
                     'EFIT02_MDS',                 '17',
                     'ZIPFITE',                    '18',
                     'GLOBAL',                     '19',
                     'CER_FIDUCIAL',               '20',
                     'GLOBAL_MDS',                 '21',
                     'CER',                        '22',
                     'CEU',                        '23',
                     'DB_UPDATE',                  '24',
                     'NB_MDS',                     '25',
                     'NEUTRONS_DONE',              '26',
                     'TANHFIT',                    '27',
                     'BDEF',                       '28',
                     'EFITRT',                     '29',
                     'EFITRT_DONE',                '30',
                     'PCS',                        '31',
                     'EFITRT2',                    '32',
                     'EFITRT2_DONE',               '33',
                     'PEDESTAL',                   '34',
                     'PWR',                        '35',
                     'ECH',                        '36',
                     'BESF',                       '37',
                     'BESS',                       '38',
                     'DSR',                        '39',
                     'ECES',                       '40',
                     'ECET',                       '41',
                     'ECEV',                       '42',
                     'ELAP',                       '43',
                     'FIR',                        '44',
                     'MAG0',                       '45',
                     'MAG2',                       '46',
                     'MAG3',                       '47',
                     'MAG4',                       '48',
                     'MAG5',                       '49',
                     'MAG7',                       '50',
                     'ICH',                        '51',
                     'ICOIL',                      '52',
                     'ICRF',                       '53',
                     'LIBM',                       '54',
                     'LPI',                        '55',
                     'ZEFF',                       '56',
                     'PHDM',                       '57',
                     'PWR0',                       '58',
                     'RFPA',                       '59',
                     'SXRF',                       '60',
                     'CERN_AND_NB',                '61',
                     'RFL',                        '62',
                     'RF2',                        '63',
                     'RF3',                        '64',
                     'RF4',                        '65',
                     'RF5',                        '66',
                     'MMS',                        '67',
                     'MS2',                        '68',
                     'SPR_AND_NB',                 '69',
                     'DBLK',                       '70',
                     'FIRE',                       '71',
                     'TM19',                       '72',
                     'ZERO',                       '73',
                     'PSYN',                       '74',
                     'ZIPFITE_AND_EFIT02',         '75',
                     'EFIT02_FILE',                '76',
                     'TSPROF',                     '77',
                     'ECE_MDS',                    '78',
                     'ECE_AND_EFIT01',             '79',
                     'PHDF_MDS',                   '80',
                     'PHDFMDS_AND_IGS',            '81',
                     'RHOMAPTS_MDS',               '82',
                     'EFIT01_AND_CERFID',          '83',
                     'SXRS',                       '84',
                     'TESTPHASE',                  '97',
                     'CPU',                        '98',
                     'FINISH_SHOT',                '99'
                  ]);
}
