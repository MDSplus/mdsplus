function [ status ] = mdsput( node, expression, varargin)
% MDSPUT  put data into MDSplus tree node
%   This routine uses the java or python interface
info = mdsInfo();
n = nargin - 2;
if info.usePython
    args = cell(n, 1);
else
    if info.isConnected
        extra = 0;
    else
        extra = 1;
    end
    args = javaArray('MDSplus.Data', max(1, n + extra));
end
for k = 1 : n
    argin = varargin(k);
    if iscell(argin{1})
        argout = mdsFromMatlab(argin{1});
    else
        argout = mdsFromMatlab(cell2mat(argin));
    end
    if info.usePython
        args{k} = argout;
    else
        args(k) = argout;
    end
end

try
    if info.isConnected
        if n > 0
            if info.usePython
                info.connection.put(node, expression, args{:});
            else
                info.connection.put(node, expression, args);
            end
        else
            info.connection.put(node, expression);
        end
    else
        [mdsres, mdsok] = mdsvalue(sprintf('treeput($, $%s)', repmat(', $', 1, nargin - 2)), node, expression, varargin{:});
        if mdsok==0 || ~isnumeric(mdsres) || rem(mdsres,2)==0
            throw(MException('MDSplus:treeput', 'treeput error %d: %s', mdsok, string(mdsres)));
        end
    end
    status = 1;
catch err
    status = 0;
    error(err.message);
end
