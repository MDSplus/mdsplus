;
;+
; NAME:
;	MDSDECOMP_TREE
; PURPOSE:
;	Create a TCL command procedure to regenerate the tree
; CATEGORY:
;	MDSPLUS
; CALLING SEQUENCE:
;	MDSDECOMP_TREE,experiment[,shot]
;	experiment = name of the experiment
; OPTIONAL INPUT PARAMETERS:
;       shot       = shot number of the file.
;       If omitted, the model will be decompiled
; Keywords:
;       QUIET = prevents IDL error if TCL command fails
;       STATUS = return status, low bit set = success
; OUTPUTS:
;       istat = return status, low bit set = success
; OUTPUTS:
;	None.
; COMMON BLOCKS:
;	None.
; SIDE EFFECTS:
;	None.
; RESTRICTIONS:
;	None.
; PROCEDURE:
;	Creates a file called experiment_shot.TCL which
;       contains the TCL commands necessary to regenerate
;       the tree.
; MODIFICATION HISTORY:
;	 VERSION 1.0, CREATED BY T.W. Fredian, Sept 22,1992
;-
;
function device_part,nid
  if mdsvalue('GETNCI($,"CONGLOMERATE_ELT")',nid) gt 1 then return,1
  if nid then return,call_function('device_part',mdsvalue('GETNCI(GETNCI($,"PARENT"),"NID_NUMBER")',nid))
  return,0
end

pro report_done,numdone,total_nodes
numdone=numdone+1
part=total_nodes/10
if ((numdone ge part) and ((numdone mod part) eq 0)) or (part eq total_nodes) then $
  print,strtrim(string(numdone),2)," nodes completed."
return
end

pro setup_node,nid
  path = mdsvalue('GETNCI($,"FULLPATH")',nid)
;  ctx = 0l
;  while (call_external('treeshr','tree$find_node_tags',nid,ctx,tagname)) do begin
;    printf,1,'ADD TAG '+ path +' '+tagname
;  endwhile
  tagname = mdsvalue('TreeFindNodeTags($)',nid)
  while (tagname ne '') do begin
    printf,1,'ADD TAG '+ path +' '+tagname
    tagname = mdsvalue('TreeFindNodeTags($)',nid)
  endwhile
  cmd = 'SET NODE/MODEL_WRITE/SHOT_WRITE/NOWRITE_ONCE ' + path + '/'
  if not mdsvalue('getnci($,"COMPRESS_ON_PUT")',nid) then cmd=cmd+"NO"
  cmd = cmd + "COMPRESS_ON_PUT/"
  if not mdsvalue('getnci($,"DO_NOT_COMPRESS")',nid) then cmd=cmd+"NO"
  cmd = cmd + "DO_NOT_COMPRESS"
  printf,1,cmd
  quals = ''
  if mdsvalue('getnci($,"STATE")',nid)           then quals=quals+"/OFF" else quals=quals+"/ON"
  if mdsvalue('getnci($,"INCLUDE")',nid)         then quals=quals+"/INCLUDE" else quals=quals+"/NOINCLUDE"
  if mdsvalue('getnci($,"NO_WRITE_MODEL")',nid)  then quals=quals+"/NOMODEL_WRITE"
  if mdsvalue('getnci($,"NO_WRITE_SHOT")',nid)   then quals=quals+"/NOSHOT_WRITE"
  if mdsvalue('getnci($,"WRITE_ONCE")',nid)      then quals=quals+"/WRITE_ONCE"
  if mdsvalue('getnci($,"ESSENTIAL")',nid)       then quals=quals+"/ESSENTIAL"
  if (mdsvalue('getnci($1,"IS_MEMBER") && getnci($1,"SETUP_INFORMATION")',nid)) then begin
    if (mdsvalue('getnci($,"LENGTH")',nid) ne 0) then begin
      value = mdsvalue('DECOMPILE(`getnci($,"RECORD"))',nid)
      if strpos(value,"<no-node>") eq -1 then begin
        printf,1,'PUT/EXTEND/NOLF/EOF="****" ' + path
        strt = 0l
        len = strlen(value)
        while strt lt len do begin
          printf,1,strmid(value,strt,80)
          strt = strt + 80
        endwhile
        printf,1,"****"
      endif else begin
        printf,1,'TYPE ' + path + ' had references to non-existent nodes'
      endelse
    endif else begin
      printf,1,'PUT ' + path + ' ""'
    endelse
  endif
  if strlen(quals) gt 0 then begin
    printf,1,'SET NODE ' + quals + ' ' + path
  endif
  return
end

pro mdsdecomp_tree,tree,shot,quiet=quiet,status=status
mdstcl,'close',/quiet
tmp=size(quiet)
if tmp(1) eq 0 then quiet=0
case n_params() of
  1: mdstcl,'edit '+tree,quiet=quiet,status=status
  2: mdstcl,'edit '+tree+'/shot='+string(shot),quiet=quiet,status=status
  else: status=0
endcase
if not status then return
tree=mdsvalue('$EXPT')
shot=mdsvalue('$SHOTNAME')
close,1
openw,1,tree+'_'+shot+'.TCL'
printf,1,'EDIT/NEW '+tree+'/SHOT='+string(mdsvalue('$SHOT'))
all_nids=MDSVALUE('GETNCI("\\TOP***","NID_NUMBER")')
defered_nids=0
usage=['ANY','STRUCTURE','ACTION','DEVICE','DISPATCH','NUMERIC','SIGNAL','TASK','TEXT','WINDOW','AXIS','SUBTREE','COMPOUND_DATA']
punct=[':','.']
;
; Add all the nodes which are do not have ancestors which are part of a device
;
total_nodes=n_elements(all_nids)
print,'Decompiling ',strtrim(string(total_nodes),2),' nodes.'
nodes_done=0
for i=0,n_elements(all_nids)-1 do begin
  nid = all_nids(i)
  if not device_part(nid) then begin
    path = mdsvalue('GETNCI($,"FULLPATH")',nid)
    elt=mdsvalue('GETNCI($,"CONGLOMERATE_ELT")',nid)
    if elt eq 0 then begin
      usagenum = MDSVALUE('GETNCI($,"USAGE")',nid)
      case usagenum of
        1:  begin
            printf,1,'ADD NODE '+ path
            end
        11: begin
            printf,1,'ADD NODE '+ path
            printf,1,'SET NODE/SUBTREE ' + path
            end
        else: begin
            printf,1,'ADD NODE/USAGE=' + USAGE(usagenum) + ' ' + path
            end
      endcase
    endif else begin
      printf,1,'ADD NODE/MODEL=' + MDSVALUE('MODEL_OF(GETNCI($,"RECORD"))',nid) + ' ' + path
    endelse
  endif else begin
    defered_nids=[defered_nids,nid]
  endelse
endfor
renamed_nids = [0]
renamed_names = ['']
;
; Add remaining nodes
;
while n_elements(defered_nids) gt 1 do begin
  nid = defered_nids(1)
  parent = mdsvalue('GETNCI(GETNCI($,"PARENT"),"NID_NUMBER")',nid)
  path = mdsvalue('GETNCI($,"FULLPATH")',nid)
  tmp = where(parent eq defered_nids,count)
  if (count) then begin
    defered_nids = [0,defered_nids(2:*),nid]
  endif else begin
    elt=mdsvalue('GETNCI($,"CONGLOMERATE_ELT")',nid)
    if elt eq 0 then begin
        printf,1,'ADD NODE/USAGE=' + USAGE(MDSVALUE('GETNCI($,"USAGE")',nid)) + path
    endif else begin
      original = mdsvalue('GETNCI($,"FULLPATH")',nid-elt+1) + mdsvalue('GETNCI($,"ORIGINAL_PART_NAME")',nid)
      if original ne path then begin
        idx = where(parent eq renamed_nids,count)
        if (count) then begin
          printf,1,'RENAME '+renamed_names(idx)+punct(mdsvalue('GETNCI($,"IS_CHILD")',nid))+$
               MDSVALUE('GETNCI($,"NODE_NAME")',nid)+ ' ' + path
        endif else begin
          printf,1,'RENAME '+original + ' ' + path
        endelse
        renamed_nids = [renamed_nids,nid]
        renamed_names = [renamed_names,path]
      endif
    endelse
    if n_elements(defered_nids) gt 2 then defered_nids = [0,defered_nids(2:*)] else defered_nids=[0]
  endelse
endwhile
;
; Assign tags, load data, and set node characteristics of all nodes
;
for i=0,n_elements(all_nids)-1 do begin
  setup_node,all_nids(i)
  report_done,nodes_done,total_nodes
endfor
printf,1,'WRITE'
close,1
end
