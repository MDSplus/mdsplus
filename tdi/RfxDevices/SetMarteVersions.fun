/*
Function call by RFX Marte RT frameworks in answer to the
GET_VERSIONS event, sent by jScheduler in PAS state.
*/

public fun SetMarteVersions(in _verDEQU, in _verDFLU, in _verEDA1, in _verEDA3, in _verMHD, in _verMHD_BR)
{

write(*, "TEST");


	tcl('set tree rfx');

    _nid = getnci("\\RFX::VERSIONS:VME_DEQU", "NID_NUMBER");
    _status = TreeShr->TreePutRecord(val(_nid),xd(_verDEQU),val(0));
	if( ! _status & 1 )
		write(*, 'Error on save DFLU version in VERSION device' );

    _nid = getnci("\\RFX::VERSIONS:VME_DFLU", "NID_NUMBER");
    _status = TreeShr->TreePutRecord(val(_nid),xd(_verDFLU),val(0));
	if( ! _status & 1 )
		write(*, 'Error on save DFLU version in VERSION device' );

    _nid = getnci("\\RFX::VERSIONS:VME_EDA3", "NID_NUMBER");
    _status = TreeShr->TreePutRecord(val(_nid),xd(_verEDA3),val(0));
	if( ! _status & 1 )
		write(*, 'Error on save EDA3 version in VERSION device' );

    _nid = getnci("\\RFX::VERSIONS:VME_MHD_AC", "NID_NUMBER");
    _status = TreeShr->TreePutRecord(val(_nid),xd(_verMHD),val(0));
	if( ! _status & 1 )
		write(*, 'Error on save MHD_AC version in VERSION device' );

    _nid = getnci("\\RFX::VERSIONS:VME_MHD_BC", "NID_NUMBER");
    _status = TreeShr->TreePutRecord(val(_nid),xd(_verMHD),val(0));
	if( ! _status & 1 )
		write(*, 'Error on save MHD_BC version in VERSION device' );

    _nid = getnci("\\RFX::VERSIONS:VME_MHD_BR", "NID_NUMBER");
    _status = TreeShr->TreePutRecord(val(_nid),xd(_verMHD_BR),val(0));
	if( ! _status & 1 )
		write(*, 'Error on save MHD_BR version in VERSION device' );

    _nid = getnci("\\RFX::VERSIONS:VME_EDA1", "NID_NUMBER");
    _status = TreeShr->TreePutRecord(val(_nid),xd(_verEDA1),val(0));
	if( ! _status & 1 )
		write(*, 'Error on save EDA1 version in VERSION device' );

	tcl('close');

}
