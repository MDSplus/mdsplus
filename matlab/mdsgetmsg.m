% translates the tdi message id into the tdi message
% messages are cached in 'tdimsg.mat'
function msg = mdsgetmsg(id,throwerror,except)
narginchk(1,3)
if nargin<3, except     = [];
if nargin<2, throwerror = false;
end,end

if ischar(id)
    error(id);
else
    try
        msg =  mdsvalue('_msg = REPEAT(0,255);MdsShr->MdsGetMsgDsc(val($),descr(_msg));TRIM(_msg)',id);
    catch
        msg = sprintf('message not found: 0x%08x',id);
    end
    if xor(throwerror,ismember(id, except))
        error(msg)
    end
end