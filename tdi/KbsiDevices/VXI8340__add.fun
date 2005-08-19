public fun VXI8340__add(in _path, out _nidout)
{   

    DevAddStart(_path, 'VXI8340', 9, _nidout);
    DevAddNode(_path// ':ADDRESS', 'TEXT', *, *, _nid);
    DevAddNode(_path// ':ID', 'NUMERIC', *, *, _nid);
    DevAddNode(_path// '.TRIG','STRUCTURE', *, *, _nid);
    DevAddNode(_path// '.TRIG:PROTOCAL','NUMERIC', *, *, _nid); 
    DevAddNode(_path// '.TRIG:SOURCE','NUMERIC', *, *, _nid);
    DevAddNode(_path// '.TRIG:DESTINA','NUMERIC', *, *, _nid); 

    DevAddAction(_path// ':OPEN','OPEN','OPEN',50,'LOCALHOST:8001',getnci(_path, 'fullpath'), _nid);
/*    DevAddAction(_path// ':INIT','INIT','INIT',50,'LOCALHOST:8001',getnci(_path, 'fullpath'), _nid); */
    DevAddAction(_path// ':SETUP','SETUP','SETUP',50,'LOCALHOST:8001',getnci(_path, 'fullpath'), _nid);
/*    DevAddAction(_path// ':ARM','ARM','ARM',50,'LOCALHOST:8001',getnci(_path, 'fullpath'), _nid);         */
/*    DevAddAction(_path// ':STORE','STORE','STORE',50,'LOCALHOST:8001',getnci(_path, 'fullpath'), _nid);   */
/*    DevAddAction(_path// ':ERROR','ERROR','ERROR',50,'LOCALHOST:8001',getnci(_path, 'fullpath'), _nid);   */
/*    DevAddAction(_path// ':STATUS','STATUS','STATUS',50,'LOCALHOST:8001',getnci(_path, 'fullpath'), _nid); */
    DevAddAction(_path// ':CLOSE','CLOSE','CLOSE',50,'LOCALHOST:8001',getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}
