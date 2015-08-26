function y = mdscvt(x,t)

% MDSCVT local conversion for MDSIP
%   MDSCVT(X,T) prepares array X to be locally converted to type T when used as
%   a $ argument in MDSDATA or MDSPUT. Supported types are 'd'ouble, 'f'loat,
%   'q'uadword, 'l'ong, 's'hort, signed 'c'har, unsigned 'Q'uadword, u'L'ong,
%   u'S'hort, u'C'har and 't'ext. Ex.: MDSDATA('$',MDSCVT(X,'l')) uses less
%   network bandwidth than MDSDATA('LONG($)',X).

if nargin == 0
    y = mdscvt([]);
elseif isa(x,'mdscvt')
    y = x;
else
    if nargin == 1
        switch class(x)
            case 'double', t = 'd';
            case 'char',   t = 't';
            otherwise, error('Unsupported class.')
        end
    end
    y.x = x;
    y.t = t;
    y = class(y,'mdscvt');
end
