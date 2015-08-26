function response = mdstcl(command)

% This function provides Matlab with the same MDSTCL interface as IDL.
% Written by R. Granetz on 2014/12/23

% If a TCL command is passed to this routine, then execute it, output the
% response, if any, to the terminal, and return to the Matlab prompt.  If
% this routine is called without a TCL command, then go into a loop,
% prompting for, and executing TCL commands, and outputting the responses,
% until 'exit' is entered.

if nargin>0
    mdsvalue('tcl($, _response)', command);
    response = char(mdsvalue('_response'));
    if ~(nargout || isempty(response))
        fprintf(1, '%s\n', response);
    end
else
    while true
        tclcmd = input('TCL> ','s');
        if (isempty(tclcmd))
            continue
        elseif (any(strcmpi(tclcmd,{'ex','exi','exit'})))
            return
        else
            mdstcl(tclcmd);
        end
    end
end