#!/bin/sh
. /cgi-bin/header $0\?$QUERY_STRING

eval $QUERY_STRING
if [ "$SLOT" != "" ]; then
	KNOBDIR=/dev/ao32cpci/ctrl/ao32cpci.$SLOT/
else
	echo please specify \?SLOT=N
	footer
	exit 1
fi

header "AO32CPCI Setup Slot:$SLOT" "AO32CPCI Setup Slot:$SLOT" nopre

AOKNOBS=$(cd $KNOBDIR;FORMAT="{k:'%s',v:'%s'}," list_knobs A*)
DOKNOBS=$(cd $KNOBDIR;FORMAT="{k:'%s',v:'%s'}," list_knobs DO*)
SGKNOBS=$(cd $KNOBDIR;FORMAT="{k:'%s',v:'%s'}," list_knobs SIG_*)
DO12KNB=$(cd $KNOBDIR;FORMAT="{k:'%s',v:'%s'}," list_knobs DO[01])

tr99
cat - <<EOF
<SCRIPT LANGUAGE=javascript>

var knobs = new Array(2);
knobs[0] = new Array(2);
knobs[1] = new Array(2);

knobs[0][0] = [ 
	$AOKNOBS 
];
knobs[1][0] = [ 
	$DOKNOBS 
];
knobs[0][1] = [ 
	$SGKNOBS 
];
knobs[1][1] = [ 
	'' 
];

var slot=$SLOT;

function buildForm () {
	document.write(
		'<form name="ao32cpci_setup slot '+slot+'" method=POST '+
		'action="/cgi-bin/submitargs">'
	);


	for (var row = 0; row <= 1; ++row){
		document.write('<tr>');		
		for (var col = 0; col <= 1; ++col){			
			document.write(col==0?'<td>': '<td rowspan="2">');
			for (var k in knobs[row][col]){
				var kx = knobs[row][col][k];
				if (kx.k == undefined) break;
	
				document.write(
					'<table>'+
					'<tr>'+
						'<td><input type=TEXT align="left" width="35" value="'+kx.k+'"></td>'+
						'<td><input type=TEXT name="'+kx.k+'" '+
						'width='+(col==0? '"40" ': ' "20"') + 'value="'+kx.v+'"></td>'+
					'</tr>'+
					'</table>'
				);
			}
			document.write('</td>');
		}
		document.write('</tr>');
	} 

	document.write(
    /*
	'<tr><td>press REFRESH to cancel, Press OK to submit (change) data</td>'+
	'<td><input type=SUBMIT value="OK"></td></tr>'+
    */
	'</form>');

}
buildForm();
</SCRIPT>
EOF

#tab99

#jsfun addCommitHandler
footer

