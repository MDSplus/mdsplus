function mdstcl_open_tree_edit,tree,shot
  shot=long(shot)
  x=mds$value('treeshr->tree$open_tree_edit($,$)',tree,shot,status=stat)
  print,'Status on open for edit: ',x,stat
  return,x
end


function mdstcl_open_new_model,tree
  x=mds$value('treeshr->tree$open_tree_new($,$)',tree,-1l,status=stat)
  print,'Status on new tree: ',x,stat
  stat=mdstcl_open_tree_edit(tree,-1l)
  return,stat
end

pro mdstcl_add_node,node,usage

  ;this procedure sets the Ken variable _newNID, that can be used in subsequent
  ;TREESHR calls to this node

  x=mds$value('_newNID=0L, treeshr->tree$add_node($,ref(_newNID),$)', $
	node,usage,status=stat) 
  if (not(stat)) then print,'Status adding node: ',node,x,stat

end

pro mdstcl_add_signal,signal,label,tag=tag
  forward_function mdstcl_usage
  usage=mdstcl_usage()
  mdstcl_add_node,signal,usage.signal
  if (keyword_set(tag)) then begin
    x=mds$value('treeshr->tree$add_tag(_newNID,$)',tag)
  endif
  mdstcl_add_node,signal+':LABEL',usage.text
  mds$put,signal+':LABEL','$',label
end

function mdstcl_add_structure,node
  usage_structure=1 ; defined in MDS$ROOT:[SYSLIB]USAGEDEF.H
  newnode='.'+node
  x=mds$value('_newNID=0L, treeshr->tree$add_node($,ref(_newNID),$)',newnode,usage_structure,status=stat)
  return,stat
end
  
pro mdstcl_add_subtree,tree,path,subtree,shot
  stat=mdstcl_open_tree_edit(tree,shot)
  mds$set_def,path,status=stat
  stat=mdstcl_add_structure(subtree)
  x=mds$value('treeshr->tree$set_subtree(_newNID)',status=stat)
  print,'STAT Set subtree: ',stat
  mdstcl_write_tree
  mdstcl_close_tree,tree,shot  
end

pro mdstcl_write_tree
  x=mds$value('treeshr->tree$write_tree()',status=stat)
  print,'STAT Write tree: ',stat
end

pro mdstcl_close_tree,tree,shot
  x=mds$value('treeshr->tree$close_tree($,$)',tree,shot)
end

pro mdstcl_auto_load,shot
  cd,current=curdir
  print,'Loading shot: ',shot 
  cd,'$MDSROOT/tree/neutrals/gasflow'
  load_gasflow,shot
  cd,'$MDSROOT/tree/neutrals/pressure'
  load_pressure,shot
  cd,'$MDSROOT/tree/nb'
  load_nb,shot
  cd,'$MDSROOT/tree/spectroscopy/phd'
  load_phd,shot
  cd,curdir
end

function mdstcl_check_pulse,tree,shot,ask=ask

  forward_function mdstcl_create_pulse
  common mdstcl_noload,shots_noload

  if (not(keyword_set(shots_noload))) then shots_noload=[0.]

  inoload=where(shots_noload eq shot,nnoload)

  if (nnoload eq 0) then begin

    msg=["The signals stored in MDSplus have not yet been loaded for this shot.", $
	 "You have the option of loading them now (this could take up to 5 minutes).", $
	 "Note that any PTDATA shot files needed to load the analyzed data for this", $
	 "shot *MUST* be already restored to disk.", $
         "", $
 	 "Load the MDSplus data for this shot now?"]

    mds$open,tree,shot,status=stat

    if (not(stat)) then begin
      ;***
      spawn,'whoami',user
if (user(0) eq 'schacht') then begin
        if (keyword_set(ask)) then answer=widget_message(msg,/question) else answer='Yes'
        if (answer eq 'Yes') then begin
          stat=mdstcl_create_pulse(tree,shot) 
	endif else shots_noload=[shots_noload,shot]
endif
    endif 
    mds$close,tree,shot,/quiet

  endif else begin
    stat=0
    print,'MDSTCL_CHECK_PULSE: requested to not load shot: ',shot
  endelse

  return,stat
end

function mdstcl_set_current_shot,tree,shot
  stat=mds$value('mdsshr->mds$set_current_shotid($,$)',tree,long(shot))
  return,stat
end

function mdstcl_get_current_shot,tree
  shot=mds$value('_shot=0, mdsshr->mds$get_current_shotid($,ref(_shot)),_shot',tree)
  return,shot
end

function mdstcl_create_pulse,tree,shot,auto=auto
  mds$open,tree,-1,status=stat
  if (stat) then begin
    print,'Creating pulse file: ',tree,' ',shot
    stat=mds$value('treeshr->tree$create_pulse_file($)',long(shot))
    mds$close,tree,-1
  endif
  return,stat
end


function mdstcl_shot_list

  ;This function calls the TDI function FINDFILE to return a list of
  ;D3D tree shot files.  It assumes that mdsinit has already been run.

  filespec='D3D$SHOTS:[000000]D3D_*.TREE'
  p=strpos(filespec,'*')

  files=mds$value('FINDFILE($)',filespec)

  l=strlen(files(0))-p   
  shots=long(strmid(files,p,l))

  return,shots

end

  

function mdstcl_usage
  return,{structure:1, signal:6, text:8, numeric:5}
  ;usage values from MDS$ROOT:[SYSLIB]USAGEDEF.H
end

pro mdstcl_write_close,tree,shot
  
  x=mds$value('treeshr->tree$write_tree()',status=stat)
  print,'STAT Write tree: ',stat

  x=mds$value('treeshr->tree$close_tree($,$)',tree,shot)

end

pro mdstcl_use_ptdata,node,tag
  cmd='USING(\'+tag+',\PTDATA::TOP,,"PTDATA")'
  print,cmd
  mds$put,node,cmd
end

pro mdstcl_put_1Dsignal,node,signal,sigunits,time,timeunits,timeexp=timeexp
  cmd='BUILD_SIGNAL(BUILD_WITH_UNITS($,$),,'
  if (keyword_set(timeexp)) then begin
    cmd=cmd+timeexp+')'
    mds$put,node,cmd,signal,sigunits
  endif else begin
    cmd=cmd+'BUILD_WITH_UNITS($,$))'
    mds$put,node,cmd,signal,sigunits,time,timeunits
  endelse
end








