function [reply,status] = mdsvalue(str,varargin)
% [reply,status] = mdsvalue(str,varargin)
% function to open server remotely
% eg : mdsvalue('2+3');

[reply,status] = mdsipmex(str,varargin{:});
