function status = mdsput(node,expr,varargin)

% MDSPUT   puts data in MDS data base
% mdsput(node,d1)
%           node='target::node',d1=data
% mdsput(node,expr,d1,d2,d3...)
%           node='target::node',expr='sin($1)',d1,d2,d3...=data
% if d1 is expressed as class mdscvt, data is translated before sending
% d1 = mdscvt(data,'CONVERSION')
% CONVERSION 'd'='G_FLOAT';'f'='F_FLOAT';'q'='QUADWORD';'l'='LONG';'s'='WORD';'c'='BYTE';
% 'Q'='QUADWORD_UNSIGNED';'L'='LONG_UNSIGNED';'S'='WORD_UNSIGNED';'C'='BYTE_UNSIGNED';'t'='TEXT';
%
% Adapted for mdsvalue Basil P. DUVAL, May 2000

% defaults
if nargin < 2, error('Specify the node and the content.'), end
if nargin < 3 , varargin{1} = expr; expr = '$1'; end

 cmd = ['TreePut($1' sprintf(',$%d',2:length(varargin)+2) ')'];

stat = mdsipmex(cmd,node,expr,varargin{1:end});

if nargout, status = stat; end
