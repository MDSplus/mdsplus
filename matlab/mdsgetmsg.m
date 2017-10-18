%% msg = mdsgetmsg(id, [throwerror, except])
% id         : message id
% throwerror : throw an error by default flag      : false
% except     : exception list of ids               : []
%
% translates the tdi message ID into the tdi message MSG
% - If THROWERROR is true, an error with the tdi message is thrown,
%   unless the ID is a member of the EXCEPT list.
% - If THROWERROR is false, no error will be thrown,
%   unless the ID is a member of the EXCEPT list.

function msg = mdsgetmsg(id,throwerror,except)
narginchk(1,3)
if nargin<3
  except     = [];
  if nargin<2
    throwerror = false;
  end
end

if ischar(id)
    error(id);
else
    try
        msg = mdsvalue('_msg = REPEAT(0,255);MdsShr->MdsGetMsgDsc(val($),descr(_msg));TRIM(_msg)',id);
    catch
        msg = sprintf('message not found: 0x%08x',id);
    end
    if xor(throwerror,ismember(id, except))
        error(msg)
    end
end