function [shoto,status] = mdsopen(server, shot)
% function to open remote server server
% eg : mdsopen('tcv_shot',12345);
% defaults to mdsopen('eltca1::tcv_shot',shot);
% To open a remote server only, use mdsopen ( no arguments )
% If first parameter is not a string, will open 'tcv_shot' tree
%
% Basil P. DUVAL, Oct 1998

if(nargin < 1);server='';shot='none';
elseif(nargin< 2 & ~isstr(server));shot=server;server='tcv_shot';
elseif(nargin< 2 & isstr(server));shot='none';
elseif(nargin >2 | ~isstr(server) | isstr(shot));error('Incorrect arguments to mdsopen');end

% open the shot

	status = mdsipmex(1,server,shot);

%status  = mdsipmex('MDSLIB->MDS$OPEN($,int($))',server,shot);
% return the open shot number if successful
if(rem(status,2) == 1 & nargin > 1)
   shoto = mdsipmex(2,'$SHOT');
else
	if(nargout > 0);shoto=[];end
end
