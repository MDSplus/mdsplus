% make_mdsipmex.m
% Make the mdsipmex routine (not shared version)
% make sure ../mdsplus points to current version of mdsplus distribution
if(exist('debug','var'))
mex -DDEBUG mdsipmex.c ../mdsplus/mdstcpip/mdsipshr.c ../mdsplus/mdstcpip/mdsiputil.c -I../mdsplus/include -I../mdsplus/mdstcpip
else
mex mdsipmex.c ../mdsplus/mdstcpip/mdsipshr.c ../mdsplus/mdstcpip/mdsiputil.c -I../mdsplus/include -I../mdsplus/mdstcpip
end
% to make the shared version, first install the Mds distribution
% then lines like:
% mex -v mdsipmex.c -I/$MDSROOT/include -I/$MDSROOT/mdstcpip -L/$MDSROOT/lib -lMdsIpShr 
% mex -v -DDEBUG mdsipmex.c -I/$MDSROOT/include -I/$MDSROOT/mdstcpip -L/$MDSROOT/lib -lMdsIpShr 
