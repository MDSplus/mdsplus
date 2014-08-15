public fun RFXDiagTimes__add(in _path, out _nidout)
{
    DevAddStart(_path, 'RFXDiagTimesV1', 11, _nidout);


    _diag = _path//'.DTSR';
    DevAddNode(_diag , 'STRUCTURE',  *,        *, _nid);

    DevAddNode(_diag // ':TRIGGER', 'NUMERIC', -0.025, *, _nid);
    DevAddNode(_diag // ':TRIG_MODE',	'TEXT', 'EXT_DT', *, _nid);
    DevAddNode(_diag // ':RT_TRIG_MODE','TEXT', 'N7_AMP', *, _nid);
    DevAddNode(_diag // ':DELAY_PULSE', 'NUMERIC', 0, *, _nid);
    DevAddNode(_diag // ':AMP_MAX', 'NUMERIC', 0, *, _nid);
    DevAddNode(_diag // ':AMP_MIN', 'NUMERIC', 0, *, _nid);
    DevAddNode(_diag // ':PHASE_MAX', 'NUMERIC', 0, *, _nid);
    DevAddNode(_diag // ':PHASE_MIN', 'NUMERIC', 0, *, _nid);

    DevAddAction(_path//':DTSR_INIT',  'INIT',  'DTSR_INIT', 20,'THOMSON_SERVER', getnci(_path, 'fullpath'), _nid);


    DevAddEnd();
}

       

