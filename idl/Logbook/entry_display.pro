;+
; NAME: ENTRY_DISPLAY
;
; PURPOSE : Display/modify/create entries in the logbook database.
;
; CALLING SEQUENCE:  ENTRY_DISPLAY
;
; KEYWORD PARAMETERS: DEFAULT='save_file' to restore settings.
;
; About
;
;  - ENTRY_DISPLAY is a widget based IDL procedure for
;   looking at entries in the LOGBOOK database.  Its primary
;   purpose is to display an up to date set of logbook entries
;   during a run.
;
;   The user specified the record selection criteria
;   (Options->Select Options) and the display options
;   (Options->Display Options).  Whenever a new logbook
;   entry is made, the display is automatically updated to
;   include the new record(s) if they meet the criteria.
;
;   This tool allows the user to create new logbook entries.
;   Chose 'Make Entry' from the Entry menu.
;
;   In addition this tool can be used to void and edit
;   existing logbook entries.  Select part of an entry in
;   main display, and then choose 'Edit Entry' or
;   'Void Entry' from the Entry menu.
;   NOTE:  you may only void or edit entries which you have
;   entered into the database!
;
; MODIFICATION HISTORY:
;       Josh Stillerman 9/20/96  initial version
;       Josh Stillerman 2/07/97  added make_entry
;       Josh Stillerman 2/22/97  fix off by ones and make shot editable
;       Josh Stillerman 12/4/97  allow run is null queries
;       Josh Stillerman 1/9/98   fix for entries with crlf and lf in them
;       Josh Stillerman 2/12/98  Added custom query dialog
;       Josh Stillerman 2/17/98  Added find in entries
;       Josh Stillerman 2/18/98  added message line, update toggle, save of custom query
;       Josh Stillerman 2/19/98  added include from main toggles to custom query
;-

PRO	X_complain_Event, ev
	WIDGET_CONTROL, ev.top, /DESTROY
END ;=================================================================

PRO	X_complain, mess, title=title, group_leader=group_leader
common widget_common, base
        if keyword_set(title) then $
          ltitle = title $
        else $
          ltitle = "Warning"
        sz = size(group_leader)
        if (sz(1) eq 0) then group_leader=base
	  base0 = WIDGET_BASE(TITLE=ltitle, /COLUMN, modal=(group_leader ne 0), group_leader=group_leader)
	  dummy = WIDGET_LABEL(base0, VALUE='      ')
          sz = size(mess)
          if (sz(0) gt 0) then begin
            for i=0, n_elements(mess)-1 do begin
    	      dummy = WIDGET_LABEL(base0, VALUE=mess(i),/DYNAMIC_RESIZE)
            endfor
          endif else begin
  	    dummy = WIDGET_LABEL(base0, VALUE=mess)
	  endelse
	  dummy =	WIDGET_BUTTON(base0, VALUE='Acknowledge', /FRAME)
	  WIDGET_CONTROL, /REALIZE, base0
	  XMANAGER, 'Warning', base0, EVENT_HANDLER='X_complain_Event'
END ;=================================================================

function GetCurrentShot
;  set_database, 'logbook'
  dummy = dsql("select max(shot) from shots", shots)
;  sql_finish
  return, shots(0) mod 1000
end

function GetCurrentRun
;  set_database, 'logbook'
  dummy = dsql("select max(run) from runs", runs)
;  sql_finish
  return, runs(0)
end

pro write_options, file, display_options, select_options, print_options, make_options, custom_options
  catch, problem
  if (problem ne 0) then begin
    X_Complain, "Error saving options to "+file
    return
  endif
  save, file=file, display_options, select_options, print_options, make_options, custom_options
  return
end

function parse_file_path, name
  ans = ''
  idx = strpos(name, ']')
  if (idx ne -1) then begin
    ans = strmid(name, 0, idx+1)
  endif else begin
    idx = strpos(name, ':')
    if (idx ne -1) then begin
      ans = strmid(name, 0, idx+1)
    endif
  endelse
  return, ans
end

function read_options, file, status=status
  status = 1L
  catch, problem
  if (problem ne 0) then begin
    catch, /cancel
    x_complain, ["Could not read in defaults file "+file,"using default options"]
    display_options={display_options, order_by:'Entered',descend:0b,$
                     null_first:0b,auto_scroll:0b, auto_update:1b, scroll_bottom:0b, max_rows:80}
    select_options={select_options, voided:0, runs:'CURRENT', users:'ME', topics:'SESSION_LEADER,PHYSICS_OPERATOR'}
    print_options={print_options, queue:'NOPRINT', portrait:1, keep:0, file:'$HOME/entry_display.print'}
    make_options={make_options, after_action:0b,  template:''}
    custom_options={custom_options,query:'', inc_voided:1, inc_run:0, inc_user:0, inc_topic:0}
    status = 0L
    case !version.os of
    'Win32' : default_path = getenv('HOME')+'\'
    else: default_path=getenv('HOME')+'/'
    endcase

    return, {select_options:select_options, display_options:display_options, print_options:print_options, $
             make_options:make_options, custom_options:custom_options, default_path:default_path}
  endif
  select_options = ''
  display_options = ''
  custom_options={custom_options,query:'', inc_voided:1, inc_run:0, inc_user:0, inc_topic:0}
  restore, file
  return, {select_options:select_options, display_options:display_options, print_options:print_options, $
           make_options:make_options, custom_options:custom_options, default_path:parse_file_path(file)}
end
;
;  this function only works on NON NESTED structures!!
;
function struct_cmp, a, b
  asize = n_tags(a)
  bsize = n_tags(b)
  if (asize ne bsize) then return, -1
  for i = 0, asize-1 do begin
    if (a.(i) ne b.(i)) then return, -1
  endfor
  return, 0
end

function SET_SELECTION_OPTIONS, selection_options, leader
  base = widget_base(title='Specify record selection', /col, /modal, group_leader=leader)
  r = widget_base(base, /row)
  l = widget_label(r, value='Select RUN =')
  run_w = widget_TEXT(r, /editable, xsize=13)
  l = widget_label(r, value='and ENTRIES are ')
  vb = widget_base(r, /row, /exclusive)
  nv_w = widget_button(vb, value='Not Voided')
  v_w = widget_button(vb, value='Voided')
  dc_w = widget_button(vb, value="Both")
  c = widget_base(base, /column, /frame)
  r = widget_base(c, /row)
  l = widget_label(r, value='and USERNAME = ')
  user_w = WIDGET_TEXT(r, /editable, xsize=20)
  r = widget_base(c, /row)
  l = widget_label(r, value='or TOPIC = ')
;  set_database, 'logbook'
  count = dsql("select topic from topics order by topic", topics)
;  sql_finish
  tb_w = lonarr(count)
  tb = widget_base(c, col=4, /nonexclusive)
  for i = 0, count -1 do begin
    tb_w(i) = widget_button(tb, value = topics(i))
  endfor
  r = widget_base(base, /row)
  b = widget_button(r, value = 'Ok')
  b = widget_button(r, value = 'Reset')
  b = widget_button(r, value = 'Cancel')
  b = widget_button(r, value = 'Select All')
  b = widget_button(r, value = 'Clear All')
  b = widget_button(r, value = 'Help')
  handle = handle_create()
  handle_value, handle, selection_options, /set
  sel_ctx = {run_w:run_w, user_w:user_w, nv_w:nv_w, v_w:v_w, dc_w:dc_w, tb_w:tb_w, $
             topics:topics, hand:handle}
  widget_control, base, /realize, set_uvalue=sel_ctx
  RESET_SELECTION_OPTIONS, sel_ctx, selection_options
  XMANAGER, 'SELECT', base
  status = 0
  handle_value, handle, hand
  if (tag_names(hand, /structure_name) eq 'SELECT_OPTIONS') then $
    s_o = selection_options $
  else begin
    if (hand.ok) then $
      s_o={select_options, voided:hand.voided, runs:hand.runs, users:hand.users, topics:hand.topics} $
    else $
      s_o = selection_options
  endelse
  handle_free, handle
  return, s_o
end

function SET_MAKE_OPTIONS, make_options, default_path, leader
  base = widget_base(title='Make Entry Options', /column, /modal, group_leader=leader)

  r = widget_base(base, /exclusive, /row)
  e_b = widget_button(r, value='Erase text after')
  k_b = widget_button(r, value='Keep text after')
  t_b = widget_button(r, value='Use Template')
  r = widget_base(base, /row)
  l = widget_label(r, value='Template name:')
  t_txt = widget_text(r, xsize=40, /editable)
  b = widget_button(r, value='Pick File')
  r = widget_base(base, /row)
  b = widget_button(r, value = 'Ok')
  b = widget_button(r, value = 'Reset')
  b = widget_button(r, value = 'Cancel')
  b = widget_button(r, value = 'Help')

  handle = handle_create()
  ctx = {erase_b:e_b, keep_b:k_b, template_b:t_b, template_t:t_txt, $
         handle:handle, default_path:default_path}
  make_options_reset, ctx, make_options
  widget_control, base, /realize, set_uvalue=ctx
  XMANAGER, 'SET_MAKE', base
  handle_value, handle, hand
  if (hand.ok) then $
    m_o={make_options, after_action:hand.after_action, template:hand.template} $
  else $
    m_o = make_options
  return, m_o
end

function SET_PRINT_OPTIONS, print_options, leader
  base = widget_base(title='Print Options', /column, /modal, group_leader=leader)

  r = widget_base(base, /row)
  b = widget_label(r, value='Printer Queue')
  spawn, 'ScopePrinters', queues
  if (n_elements(queues) eq 1) then $
    if (strlen(queues) eq 0) then $
      queues = ['lp']
  queues = ['noprint', queues]

  queue_l = widget_droplist(r, /dynamic, value = queues)
  rr = widget_base(r, /row, /exclusive, /frame)
  keep_b = widget_button(rr, value = 'keep')
  delete_b = widget_button(rr, value = 'delete')

  r = widget_base(base, /row)
  l = widget_label(r,value='Page Orientation')
  rr = widget_base(r, /row, /exclusive, /frame)
  portrait_b = widget_button(rr, value = 'portrait')
  landscape_b = widget_button(rr, value = 'landscape')

  r = widget_base(base, /row)
  l = widget_label(r,value='Output file:')
  file_t = widget_text(r, value='', xsize=40, /editable)

  r = widget_base(base, /row)
  b = widget_button(r, value = 'Ok')
  b = widget_button(r, value = 'Reset')
  b = widget_button(r, value = 'Cancel')
  b = widget_button(r, value = 'Help')

  handle = handle_create()
  ctx = {queue_l:queue_l, landscape_b:landscape_b, portrait_b:portrait_b, $
         file_t:file_t, delete_b:delete_b, keep_b:keep_b, handle:handle, $
         queue_list:queues}
  print_options_reset, ctx, print_options
  widget_control, base, /realize, set_uvalue=ctx
  XMANAGER, 'SET_PRINT', base
  handle_value, handle, hand
  if (hand.ok) then $
    p_o={print_options, queue:hand.queue, portrait:hand.portrait, keep:hand.keep, file:hand.file} $
  else $
    p_o = print_options
  return, p_o
end

function SET_DISPLAY_OPTIONS, display_options, leader
  base = widget_base(title='Display Options', /column, /modal, group_leader=leader)
  r = widget_base(base, /row)
  l = widget_label(r,value='Query sorted by ')
  rr = widget_base(r, /row, /exclusive, /frame)
  shot_b = widget_button(rr, value = 'shot')
  te_b = widget_button(rr, value = 'time entered')

  r = widget_base(base, /row)
  l = widget_label(r,value='In ')
  rr = widget_base(r, /row, /exclusive, /frame)
  asc_b = widget_button(rr, value = 'ascending')
  desc_b = widget_button(rr, value = 'descending')
  l = widget_label(r,value='order')

  r = widget_base(base, /row)
  b = widget_label(r, value='Null shots ')
  rr = widget_base(r, /row, /exclusive, /frame)
  null_begin_b = widget_button(rr, value = 'at begining')
  null_end_b = widget_button(rr, value = 'at end')

  r = widget_base(base, /row)
  b = widget_label(r, value='Automatic updates are')
  rr = widget_base(r, /row, /exclusive, /frame)
  auto_on_b = widget_button(rr, value = 'on')
  auto_off_b = widget_button(rr, value = 'off')

  r = widget_base(base, /row)
  b = widget_label(r, value='Automatic scroll is')
  rr = widget_base(r, /row, /exclusive, /frame)
  auto_scroll_on_b = widget_button(rr, value = 'on')
  auto_scroll_off_b = widget_button(rr, value = 'off')

  r = widget_base(base, /row)
  b = widget_label(r, value='Scroll to')
  rr = widget_base(r, /row, /exclusive, /frame)
  scroll_top_b = widget_button(rr, value = 'top')
  scroll_bottom_b = widget_button(rr, value = 'bottom')

  r = widget_base(base, /row)
  b = widget_label(r, value='Limit to')
  max_row_l = widget_droplist(r, /dynamic,value = ['10', '20', '40', '80', '160', '320'])
  b = widget_label(r, value='rows.')

  r = widget_base(base, /row)
  b = widget_button(r, value = 'Ok')
  b = widget_button(r, value = 'Reset')
  b = widget_button(r, value = 'Cancel')
  b = widget_button(r, value = 'Help')

  handle = handle_create()
  ctx = {shot_b:shot_b, te_b:te_b, asc_b:asc_b, desc_b:desc_b, $
         null_begin_b:null_begin_b, null_end_b:null_end_b, $
         auto_on_b:auto_on_b, auto_off_b:auto_off_b, $
         auto_scroll_on_b:auto_scroll_on_b, auto_scroll_off_b:auto_scroll_off_b, $
         scroll_top_b:scroll_top_b, scroll_bottom_b:scroll_bottom_b, $
         max_row_l:max_row_l, list:[10, 20, 40, 80, 160, 320], handle:handle}
  display_options_reset, ctx, display_options
  widget_control, base, /realize, set_uvalue=ctx
  XMANAGER, 'SET_DISPLAY', base
  handle_value, handle, hand
  if (hand.ok) then $
    d_o ={display_options, order_by:hand.order_by,descend:hand.descend,$
         null_first:hand.null_first,auto_scroll:hand.auto_scroll, $
         auto_update:hand.auto_update, $
         scroll_bottom:hand.scroll_bottom, max_rows:hand.max_rows} $
  else $
    d_o = display_options
  return, d_o
end

function get_query, ctx, voided=voided, user=user, run=run, topic=topic
  rq = ''
  runs = strtrim(ctx.select_options.runs, 2)
  if (strlen(runs) gt 0) then begin
    runs = str_sep(runs, ',')
    for i=0, n_elements(runs)-1 do begin
      if (strtrim(runs(i),2) eq 'CURRENT') then begin
;        set_database, 'logbook'
        n = dsql('select max(run) from runs', run)
;        sql_finish
        runs(i) = string(run(0), format='(I7)')
      endif
    endfor
    if (n_elements(runs) eq 1) then $
      if (strtrim(runs(0), 2) eq 'NULL') then $
        rq = 'run is null' $
      else $
        rq = 'run = '+runs(0) $
    else begin
      rq = 'run in ('
      for i=0, n_elements(runs)-1 do begin
        rq = rq + runs(i) + ','
      endfor
      rq = strmid(rq, 0, strlen(rq)-1)+')'
    endelse
  endif
  run = rq
  uq = ''
  users = strtrim(ctx.select_options.users, 2)
  if (strlen(users) gt 0) then begin
    users = str_sep(users, ',')
    for i = 0, n_elements(users)-1 do begin
      if users(i) eq 'ME' then begin
;        user="            "
;        k=call_external('LIBRTL','LIB$GETJPI',514,0,0,0,user,0,value=[0,1,1,1,0,1])
        users(i) = GETENV("user")
        if strlen(users(i)) eq 0 then begin
;          set_database, 'logbook'
          dummy = dsql('select suser_sname()', user)
          users(i)=user(0)
;          sql_finish
		endif
      endif
    endfor
    if (N_elements(users) eq 1) then $
      uq = "username = '"+users(0)+"'" $
    else begin
      uq = "username in ("
      for i=0, n_elements(users)-1 do begin
        uq = uq + "'"+strtrim(users(i),2)+"',"
      endfor
      uq = strmid(uq, 0, strlen(uq)-1) + ")"
    endelse
  endif
  user=uq

  tq = ''
  if (strlen(ctx.select_options.topics) gt 0) then begin
    topics = "'"+str_sep(ctx.select_options.topics, ',')+"'"
    if (n_elements(topics) eq 1) then $
      tq = 'topic = '+topics(0) $
    else begin
      tq = 'topic in ('
      for i=0, n_elements(topics)-1 do begin
        tq = tq + topics(i) + ','
      endfor
      tq = strmid(tq, 0, strlen(tq)-1) + ")"
    endelse
  endif
  topic = tq

  if (ctx.select_options.voided eq 0) then $
    vq = 'VOIDED IS NULL' $
  else  if (ctx.select_options.voided eq 1 ) then $
    vq = 'VOIDED IS NOT NULL' $
  else $
    vq = ''
  voided = vq


  if (strlen(uq) gt 0) and (strlen(tq) gt 0) then $
    utq = '(' + uq + ' or ' + tq +')' $
  else if (strlen(uq) gt 0) then $
    utq = uq $
  else if (strlen(tq) gt 0) then $
    utq = tq $
  else $
    utq = ''

  q = ''
  if (strlen(rq) gt 0) then q = rq

  if (strlen(utq) gt 0) then $
    if (strlen(q) gt 0) then $
      q = q + ' and ' + utq $
    else $
      q = utq

  if (strlen(vq) gt 0) then $
    if (strlen(q) gt 0) then $
      q = q + ' and ' + vq $
    else $
      q = vq

  return, q
end

function make_options_get, ctx
  widget_control, ctx.erase_b, get_uvalue = erase
  if erase then after_action = -1 else begin
    widget_control, ctx.keep_b, get_uvalue = keep
    if keep then after_action = 0 else after_action = 1
  endelse
  widget_control, ctx.template_t, get_value=template
  return, {ok:1, after_action:after_action, template:template(0)}
end

function print_options_get, ctx
  widget_control, ctx.portrait_b, get_uvalue = portrait
  widget_control, ctx.keep_b, get_uvalue = keep
  widget_control, ctx.queue_l, get_uvalue = queue
  widget_control, ctx.file_t, get_value = file
  file = file(0)
  return, {ok:1, portrait:portrait, keep:keep, queue:queue, file:file}
end

function display_options_get, ctx
  widget_control, ctx.desc_b, get_uvalue = descend
  widget_control, ctx.te_b, get_uvalue = te
  if (te) then $
    if (descend) then $
      order_by = 'Entered desc' $
    else $
      order_by = 'Entered asc' $
  else $
    if (descend) then $
      order_by = 'Shot desc, Entered desc' $
    else $
      order_by = 'Shot asc, Entered asc'
  widget_control, ctx.null_begin_b, get_uvalue = null_first
  widget_control, ctx.auto_scroll_on_b, get_uvalue = auto_scroll
  widget_control, ctx.auto_on_b, get_uvalue = auto_update
  widget_control, ctx.scroll_bottom_b, get_uvalue = scroll_bottom
  widget_control, ctx.max_row_l, get_uvalue = max_rows
  return, {ok:1, order_by:order_by,descend:descend,$
           null_first:null_first,auto_scroll:auto_scroll, $
           auto_update:auto_update, scroll_bottom:scroll_bottom, $
           max_rows:max_rows}
end

pro make_options_reset, ctx, make_options
  case (make_options.after_action) of
  -1 : begin
         widget_control, ctx.erase_b, set_button = 1, set_uvalue = 1
         widget_control, ctx.keep_b, set_button = 0, set_uvalue = 0
         widget_control, ctx.template_b, set_button = 0, set_uvalue = 0
       end
  0  : begin
         widget_control, ctx.erase_b, set_button = 0, set_uvalue = 0
         widget_control, ctx.keep_b, set_button = 1, set_uvalue = 1
         widget_control, ctx.template_b, set_button = 0, set_uvalue = 0
       end
  1  : begin
         widget_control, ctx.erase_b, set_button = 0, set_uvalue = 0
         widget_control, ctx.keep_b, set_button = 0, set_uvalue = 0
         widget_control, ctx.template_b, set_button = 1, set_uvalue = 1
       end
  end
  widget_control, ctx.template_t, set_value=make_options.template, set_uvalue=ctx.default_path
  handle_value, ctx.handle, make_options, /set
end

pro print_options_reset, ctx, print_options
  if (print_options.portrait) then begin
    WIDGET_CONTROL, ctx.portrait_b, set_button = 1, set_uvalue = 1
    WIDGET_CONTROL, ctx.landscape_b, set_button = 0, set_uvalue = 0
  endif else begin
    WIDGET_CONTROL, ctx.portrait_b, set_button = 0, set_uvalue = 0
    WIDGET_CONTROL, ctx.landscape_b, set_button = 1, set_uvalue = 1
  endelse

  if (print_options.keep) then begin
    WIDGET_CONTROL, ctx.keep_b, set_button = 1, set_uvalue = 1
    WIDGET_CONTROL, ctx.delete_b, set_button = 0, set_uvalue = 0
  endif else begin
    WIDGET_CONTROL, ctx.keep_b, set_button = 0, set_uvalue = 0
    WIDGET_CONTROL, ctx.delete_b, set_button = 1, set_uvalue = 1
  endelse

  WIDGET_CONTROL, ctx.file_t, set_value = print_options.file

  widget_control, ctx.queue_l, set_uvalue = ctx.queue_list(0), set_droplist_select=0
  for i=0, n_elements(ctx.queue_list)-1 do begin
    if (ctx.queue_list(i) eq print_options.queue) then $
      widget_control, ctx.queue_l, set_uvalue = ctx.queue_list(i), set_droplist_select=i
  endfor

  handle_value, ctx.handle, print_options, /set
end

pro display_options_reset, ctx, display_options
  if (display_options.order_by eq 'Entered') then begin
    WIDGET_CONTROL, ctx.te_b, set_button = 1, set_uvalue = 1
    WIDGET_CONTROL, ctx.shot_b, set_button = 0, set_uvalue = 0
  endif else begin
    WIDGET_CONTROL, ctx.te_b, set_button = 0, set_uvalue = 0
    WIDGET_CONTROL, ctx.shot_b, set_button = 1, set_uvalue = 1
  endelse

  if (display_options.descend) then begin
    WIDGET_CONTROL, ctx.asc_b, set_button = 0, set_uvalue = 0
    WIDGET_CONTROL, ctx.desc_b, set_button = 1, set_uvalue = 1
  endif else begin
    WIDGET_CONTROL, ctx.asc_b, set_button = 1, set_uvalue = 1
    WIDGET_CONTROL, ctx.desc_b, set_button = 0, set_uvalue = 0
  endelse

  if (display_options.null_first) then begin
    WIDGET_CONTROL, ctx.null_end_b, set_button = 0, set_uvalue = 0
    WIDGET_CONTROL, ctx.null_begin_b, set_button = 1, set_uvalue = 1
  endif else begin
    WIDGET_CONTROL, ctx.null_end_b, set_button = 1, set_uvalue = 1
    WIDGET_CONTROL, ctx.null_begin_b, set_button = 0, set_uvalue = 0
  endelse

  if (display_options.auto_scroll) then begin
    WIDGET_CONTROL, ctx.auto_scroll_off_b, set_button = 0, set_uvalue = 0
    WIDGET_CONTROL, ctx.auto_scroll_on_b, set_button = 1, set_uvalue = 1
  endif else begin
    WIDGET_CONTROL, ctx.auto_scroll_off_b, set_button = 1, set_uvalue = 1
    WIDGET_CONTROL, ctx.auto_scroll_on_b, set_button = 0, set_uvalue = 0
  endelse

  if (display_options.auto_update) then begin
    WIDGET_CONTROL, ctx.auto_off_b, set_button = 0, set_uvalue = 0
    WIDGET_CONTROL, ctx.auto_on_b, set_button = 1, set_uvalue = 1
  endif else begin
    WIDGET_CONTROL, ctx.auto_off_b, set_button = 1, set_uvalue = 1
    WIDGET_CONTROL, ctx.auto_on_b, set_button = 0, set_uvalue = 0
  endelse

  if (display_options.scroll_bottom) then begin
    widget_control, ctx.scroll_top_b, set_button = 0, set_uvalue = 0
    widget_control, ctx.scroll_bottom_b, set_button = 1, set_uvalue = 1
  endif else begin
    widget_control, ctx.scroll_top_b, set_button = 1, set_uvalue = 1
    widget_control, ctx.scroll_bottom_b, set_button = 0, set_uvalue = 0
  endelse

  widget_control, ctx.max_row_l, set_uvalue = ctx.list(0), set_droplist_select=0
  for i=0, n_elements(ctx.list)-1 do begin
    if (ctx.list(i) eq display_options.max_rows) then $
      widget_control, ctx.max_row_l, set_uvalue = ctx.list(i), set_droplist_select=i
  endfor

  handle_value, ctx.handle, display_options, /set
end

pro SET_MAKE_EVENT, ev
  widget_control, ev.top, get_uvalue = ctx, /no_copy, /hour
  case (tag_names(ev, /STRUCTURE_NAME)) of
  "WIDGET_BUTTON": begin
        WIDGET_CONTROL, ev.id, get_value=button
        case (button) of
        "Ok" : begin
           options = make_options_get(ctx)
           handle_value, ctx.handle, options, /set
           widget_control, ev.top, /destroy
         end
        "Cancel" : begin
           handle_value, ctx.handle, {ok:0}, /set
           WIDGET_CONTROL, ev.top, /DESTROY
         end
        "Reset" : begin
           handle_value, ctx.handle, make_options
           make_options_reset, ctx, make_options
         end
        "Pick File" : begin
           widget_control, ctx.template_t, get_value=str, get_uvalue=start_path
           name = DIALOG_PICKFILE(file=str(0), path=start_path, get_path=path, /read, /must,filter = "*.TEMPLATE")
           if (strlen(name) gt 0) then begin
             widget_control, ctx.template_t, set_value=name
           endif
         end
        "Help" : man_proc, 'entry-display o_make_options'
        else: begin
           widget_control, ev.id, get_uvalue=state
           state = state eq 0
           widget_control, ev.id, set_uvalue = state, set_button = state
         end
        endcase
       end
  else:
  endcase
  if (widget_info(ev.top, /Valid_id)) then widget_control, ev.top, set_uvalue = ctx, /no_copy
end

pro SET_PRINT_EVENT, ev
  widget_control, ev.top, get_uvalue = ctx, /no_copy, /hour
  case (tag_names(ev, /STRUCTURE_NAME)) of
  "WIDGET_BUTTON": begin
        WIDGET_CONTROL, ev.id, get_value=button
        case (button) of
        "Ok" : begin
           options = print_options_get(ctx)
           handle_value, ctx.handle, options, /set
           widget_control, ev.top, /destroy
         end
        "Cancel" : begin
           handle_value, ctx.handle, {ok:0}, /set
           WIDGET_CONTROL, ev.top, /DESTROY
         end
        "Reset" : begin
           handle_value, ctx.handle, print_options
           print_options_reset, ctx, print_options
         end
        "Help" : WIDGET_OLH, 'entry-display o_print_options'
        else: begin
           widget_control, ev.id, get_uvalue=state
           state = state eq 0
           widget_control, ev.id, set_uvalue = state, set_button = state
         end
        endcase
       end
  "WIDGET_DROPLIST": begin
        widget_control, ev.id, set_uvalue=ctx.queue_list(ev.index)
   end
  else:
  endcase
  if (widget_info(ev.top, /Valid_id)) then widget_control, ev.top, set_uvalue = ctx, /no_copy
end

pro SET_DISPLAY_EVENT, ev
  widget_control, ev.top, get_uvalue = ctx, /no_copy, /hour
  case (tag_names(ev, /STRUCTURE_NAME)) of
  "WIDGET_BUTTON": begin
        WIDGET_CONTROL, ev.id, get_value=button
        case (button) of
        "Ok" : begin
           options = display_options_get(ctx)
           handle_value, ctx.handle, options, /set
           widget_control, ev.top, /destroy
         end
        "Cancel" : begin
           handle_value, ctx.handle, {ok:0}, /set
           WIDGET_CONTROL, ev.top, /DESTROY
         end
        "Reset" : begin
           handle_value, ctx.handle, display_options
           display_options_reset, ctx, display_options
         end
        "Help" : man_proc, 'entry-display o_display_options'
        else: begin
           widget_control, ev.id, get_uvalue=state
           state = state eq 0
           widget_control, ev.id, set_uvalue = state, set_button = state
         end
        endcase
       end
  "WIDGET_DROPLIST": begin
        widget_control, ev.id, set_uvalue=ctx.list(ev.index)
   end
  else:
  endcase
  if (widget_info(ev.top, /Valid_id)) then widget_control, ev.top, set_uvalue = ctx, /no_copy
end

pro display_event, ev
  widget_control, ev.top, get_uvalue = ctx, /hour
  case (tag_names(ev, /STRUCTURE_NAME)) of
  "WIDGET_BASE": begin
        widget_control, ev.top, update=0
        new_width = (ev.x-ctx.x_off)/ctx.x_ch_sz
        new_height = (ev.y-ctx.y_off)/ctx.y_ch_sz
	    widget_control, ctx.txt_w, xsize = new_width, ysize=new_height
        txt_geo = widget_info(ctx.txt_w, /geometry)
        message_w = (ev.x-ctx.x_off-ctx.auto_x)/ctx.x_ch_sz
        widget_control, ctx.message_w, xsize=message_w
        widget_control, ev.top, update=1
      end
  "WIDGET_BUTTON": begin
        WIDGET_CONTROL, ev.id, get_value=button
        case (button) of
        "Print" : begin
          PRINT_OUTPUT, ctx
         end
        "Save Current Settings": begin
            write_options, ctx.defaults_file, ctx.display_options, $
                           ctx.select_options, ctx.print_options, ctx.make_options, $
                           ctx.custom_options
         end
        "Save Current Settings As ...": begin
           name = DIALOG_PICKFILE(file=ctx.defaults_file, path=ctx.default_path, get_path=path, /write, filter = "*.DAT")
           if (strlen(name) gt 0) then begin
             idx = strpos(name, ';')
             if (idx ne -1) then $
               name = strmid(name, 0, idx)
             write_options, name, ctx.display_options, $
                           ctx.select_options, ctx.print_options, ctx.make_options, $
                           ctx.custom_options
             ctx.defaults_file = name
           endif
         end
         "Restore Settings": begin
           options = read_options(ctx.defaults_file, status = status)
           if (status) then begin
             ctx.display_options = options.display_options
             widget_control, ctx.auto_w, set_button= (ctx.updates_disabled eq 0) and (ctx.display_options.auto_update)
             ctx.select_options = options.select_options
             query_txt = get_query(ctx)
             ctx.query_txt = query_txt
             do_query, ctx
           endif
         end
         "Restore Settings From ...": begin
           name = DIALOG_PICKFILE(file=ctx.defaults_file, path=ctx.default_path, get_path=path, /read, /must,filter = "*.DAT")
           if (strlen(name) gt 0) then begin
             options = read_options(name, status = status)
             if (status) then begin
               ctx.defaults_file = name
               ctx.display_options = options.display_options
               widget_control, ctx.auto_w, set_button= (ctx.updates_disabled eq 0) and (ctx.display_options.auto_update)
               ctx.select_options = options.select_options
               query_txt = get_query(ctx)
               ctx.query_txt = query_txt
               do_query, ctx
             endif
           endif
         end
        "Exit" : begin
            handle_free, ctx.handle
	    WIDGET_CONTROL, ev.top, /DESTROY
         end
         "Selection Options" : begin
            new_options = SET_SELECTION_OPTIONS(ctx.select_options, ctx.group_leader)
            if struct_cmp(new_options , ctx.select_options) ne 0 then begin
              ctx.select_options = new_options
              query_txt = get_query(ctx)
              ctx.query_txt = query_txt
              do_query, ctx
            endif
          end
         "Do Query" : begin
              query_txt = get_query(ctx)
              ctx.query_txt = query_txt
            DO_QUERY, ctx
          end
         "Show Query": begin
            q = get_query(ctx)
            qq = bust_string(q, 60)
            X_COMPLAIN, qq, title='Entry Display Query', group_leader=ev.top
          end
         "Custom Query": begin
            CUSTOM_QUERY, ctx
          end
         "Find in Entries" : begin
            FIND_IN_ENTRIES, ctx
         end
         "Make Entry" : begin
            MAKE_ENTRY, ctx
          end
         "Edit Entry" : begin
            EDIT_ENTRY, ctx
          end
         "Void Entry" : begin
            VOID_ENTRY, ctx
          end
         "Display Options" : begin
            new_options = SET_DISPLAY_OPTIONS(ctx.display_options, ctx.group_leader)
            if struct_cmp(new_options , ctx.display_options) ne 0 then begin
              widget_control, ctx.auto_w, set_button= (ctx.updates_disabled eq 0) and (ctx.display_options.auto_update)
              ctx.display_options = new_options
              query_txt = get_query(ctx)
              ctx.query_txt = query_txt
              do_query, ctx
            endif
          end
         "Print Options" : begin
            new_options = SET_PRINT_OPTIONS(ctx.print_options, ctx.group_leader)
            if struct_cmp(new_options , ctx.print_options) ne 0 then begin
              ctx.print_options = new_options
            endif
          end
         "Make Entry Options" : begin
            new_options = SET_MAKE_OPTIONS(ctx.make_options, ctx.default_path, ctx.group_leader)
            if struct_cmp(new_options , ctx.make_options) ne 0 then begin
              ctx.make_options = new_options
            endif
          end
         "Automatic Updates" : begin
            ctx.display_options.auto_update = ctx.display_options.auto_update eq 0
            widget_control, ctx.auto_w, set_button= (ctx.updates_disabled eq 0) and (ctx.display_options.auto_update)
          end
         "About" : man_proc, "entry-display about"
         else:
         endcase
       end
  "MDS_EVENT": begin
     if (ctx.display_options.auto_update and (ctx.updates_disabled eq 0)) then $
       DO_QUERY, ctx
     end
  else:
  endcase
  if (widget_info(ev.top, /Valid_id)) then begin
    widget_control, ev.top, set_uvalue = ctx
  endif
end

function get_query_txt, ctx
  widget_control, ctx.qt_w, get_value=txt
  ans = ' '
  for i=0, n_elements(txt)-1 do begin
    ans = ans+' '+txt(i)
  endfor
  ans = ans +' '
  if ((ctx.inc_voided ne 0) or (ctx.inc_topic ne 0) or (ctx.inc_run ne 0) or (ctx.inc_user ne 0)) then begin
    q = get_query(ctx.ctx, voided=voided, topic=topic, run=run, user=user)
    if (ctx.inc_voided ne 0) then if (strlen(voided) gt 0) then ans = voided + ' and '+ans
    if (ctx.inc_user ne 0) then if (strlen(user) gt 0) then ans = user + ' and '+ans
    if (ctx.inc_topic ne 0) then if (strlen(topic) gt 0) then ans = topic + ' and '+ans
    if (ctx.inc_run ne 0) then if (strlen(run) gt 0) then ans = run + ' and '+ans
  endif
  return, ans
end

function get_limit, ctx
  return, ' limit to '+string(ctx.display_options.max_rows)+' rows'
end

pro custom_event, ev
  widget_control, ev.top, get_uvalue = ctx, /no_copy, /hour
  case (tag_names(ev, /STRUCTURE_NAME)) of
  "WIDGET_BUTTON": begin
        WIDGET_CONTROL, ev.id, get_value=button
        case (button) of
        "Ok" : begin
           qry = get_query_txt(ctx)
           DO_QUERY, ctx.ctx, custom=qry
           ctx.ctx.updates_disabled = 0
           widget_control, ctx.ctx.auto_w, set_button= (ctx.ctx.updates_disabled eq 0) and $
                                                       (ctx.ctx.display_options.auto_update), sensitive=1
           widget_control, ctx.qt_w, get_value=qt
           ans = ' '
           for i=0, n_elements(qt)-1 do begin
             ans = ans+' '+qt(i)
           endfor
           ctx.ctx.custom_options.query=ans
           ctx.ctx.custom_options.inc_voided = ctx.inc_voided
           ctx.ctx.custom_options.inc_topic = ctx.inc_topic
           ctx.ctx.custom_options.inc_user = ctx.inc_user
           ctx.ctx.custom_options.inc_run = ctx.inc_run
           widget_control, ctx.ctx.group_leader, set_uvalue=ctx.ctx
           widget_control, ev.top, /destroy
         end
        "Apply" : begin
           qry = get_query_txt(ctx)
           DO_QUERY, ctx.ctx, custom=qry
           widget_control, ctx.qt_w, get_value=qt
           ans = ' '
           for i=0, n_elements(qt)-1 do begin
             ans = ans+' '+qt(i)
           endfor
           ctx.ctx.custom_options.query=ans
           ctx.ctx.custom_options.inc_voided = ctx.inc_voided
           ctx.ctx.custom_options.inc_topic = ctx.inc_topic
           ctx.ctx.custom_options.inc_user = ctx.inc_user
           ctx.ctx.custom_options.inc_run = ctx.inc_run
           widget_control, ctx.ctx.group_leader, set_uvalue=ctx.ctx
         end
        "Cancel" : begin
           ctx.ctx.updates_disabled = 0
           widget_control, ctx.ctx.auto_w, set_button= (ctx.ctx.updates_disabled eq 0) and $
                                                       (ctx.ctx.display_options.auto_update), sensitive=1
           widget_control, ctx.ctx.group_leader, set_uvalue=ctx.ctx
           widget_control, ev.top, /destroy
         end
        "Reset" : begin
           widget_control, ctx.qt_w, set_value=''
           widget_control, ctx.inc_voided_w, set_button=ctx.ctx.custom_options.inc_voided
           widget_control, ctx.inc_run_w, set_button=ctx.ctx.custom_options.inc_run
           widget_control, ctx.inc_topic_w, set_button=ctx.ctx.custom_options.inc_topic
           widget_control, ctx.inc_user_w, set_button=ctx.ctx.custom_options.inc_user
           ctx.inc_voided = ctx.ctx.custom_options.inc_voided
           ctx.inc_user = ctx.ctx.custom_options.inc_user
           ctx.inc_run = ctx.ctx.custom_options.inc_run
           ctx.inc_topic = ctx.ctx.custom_options.inc_topic
         end
         "Show Query": begin
            q = get_query_txt(ctx)
            qq = bust_string(q, 60)
            X_COMPLAIN, qq, title='Entry Display Query', group_leader=ev.top
          end
        "Help" : man_proc, 'entry-display q_custom_query'
        "Voided" : ctx.inc_voided = ctx.inc_voided eq 0
        "Run" :    ctx.inc_run = ctx.inc_run eq 0
        "User" :   ctx.inc_user = ctx.inc_user eq 0
        "Topic" :  ctx.inc_topic = ctx.inc_topic eq 0
        endcase
       end
  else:
  endcase
  if (widget_info(ev.top, /Valid_id)) then widget_control, ev.top, set_uvalue = ctx, /no_copy
end

pro custom_query, ctx
  base = widget_base(title='Custom SQL Query', /column, group_leader=ctx.group_leader)
  l = widget_label(base, value='Enter an sql predicate into this dialog to select the records to display.')
  l = widget_label(base, value='---------------------------------------------------------------------')
  l = widget_label(base, value='available fields - shot, run, topic, username, entered and text')
  l = widget_label(base, value='available operators - and, or, not, between, containing, like, <, >, =, <>')
  l = widget_label(base, value='---------- AUTOMATIC UPDATES ARE DISABLED ---------------------------')
  qt_w = widget_text(base, value=ctx.custom_options.query, xsize=60, ysize=10, /editable)
  r = widget_base(base, /row)
  b = widget_button(r, value='Ok')
  b = widget_button(r, value='Apply')
  b = widget_button(r, value='Cancel')
  b = widget_button(r, value='Show Query')
  b = widget_button(r, value='Help')
  l = widget_label(r, value='Include selection options')
  rr = widget_base(r, /row, /nonexclusive)
  inc_voided_w = widget_button(rr, value='Voided')
  widget_control, inc_voided_w, set_button=ctx.custom_options.inc_voided
  inc_run_w = widget_button(rr, value='Run')
  widget_control, inc_run_w, set_button=ctx.custom_options.inc_run
  inc_user_w = widget_button(rr, value='User')
  widget_control, inc_user_w, set_button=ctx.custom_options.inc_user
  inc_topic_w = widget_button(rr, value='Topic')
  widget_control, inc_topic_w, set_button=ctx.custom_options.inc_topic
  ctx.updates_disabled = 1
  widget_control, base, /realize, set_uvalue={ctx:ctx, qt_w:qt_w, $
                                              inc_voided_w:inc_voided_w, $
                                              inc_run_w:inc_run_w, $
                                              inc_user_w:inc_user_w, $
                                              inc_topic_w:inc_topic_w, $
                                              inc_voided:ctx.custom_options.inc_voided, $
                                              inc_run:ctx.custom_options.inc_run, $
                                              inc_user:ctx.custom_options.inc_user, $
                                              inc_topic:ctx.custom_options.inc_topic}
  widget_control, ctx.auto_w, set_button= (ctx.updates_disabled eq 0) and (ctx.display_options.auto_update), sensitive=0
  Xmanager, 'CUSTOM', base
end

pro print_output, ctx
  handle_value, ctx.handle, hand

  if (n_elements(hand) eq 0) then begin
    X_COMPLAIN, ['You must do a query', 'before choosing print.'], group_leader=ctx.group_leader
    return
  endif

  if (n_elements(hand.key) eq 0) then begin
    X_COMPLAIN, ['You must do a query', 'before choosing print.'], group_leader=ctx.group_leader
    return
  endif

  if (strlen(ctx.print_options.file) eq 0) then begin
    X_COMPLAIN, ['Please fill in a filename in "Print Options"', 'before choosing print.'], group_leader=ctx.group_leader
    return
  endif

  catch, open_err
  if (open_err ne 0) then begin
    X_Complain, ["Error opening output file", ctx.print_options.file, "Print aborted"], group_leader=ctx.group_leader
    return
  endif
  openw, lun, ctx.print_options.file, /get_lun

  catch, write_err
  if (write_err ne 0) then begin
    X_Complain, ["Error writing output file", ctx.print_options.file, "Print aborted"], group_leader=ctx.group_leader
    close, lun
    free_lun, lun
    return
  endif
  printf, lun, 'Entry display output on '+systime()
  printf, lun, ' '
  for i=0, n_elements(hand.text)-1 do begin
    printf, lun, hand.text(i)
  endfor

  catch, close_err
  if (write_err ne 0) then begin
    X_Complain, ["Error closing output file", ctx.print_options.file, "Print aborted"], group_leader=ctx.group_leader
    free_lun, lun
    return
  endif
  close, lun
  free_lun, lun
  catch , /cancel

  if (ctx.print_options.queue ne 'noprint') then begin
    print_cmd = 'lpr -P'+ctx.print_options.queue+' '
    if (not ctx.print_options.keep) then $
      print_cmd = print_cmd + ' -r '
    print_cmd = print_cmd +ctx.print_options.file
;    if (ctx.print_options.portrait) then $
;      print_cmd = print_cmd + '/PARAM=PAGE_ORIENTATION=PORTRAIT' $
;    else $
;      print_cmd = print_cmd + '/PARAM=PAGE_ORIENTATION=LANDSCAPE'
;    print, 'print command is -', print_cmd,'-'
    spawn, print_cmd
  endif
end

function selection_options_get, ctx
  widget_control, ctx.run_w, get_value=runs
  runs = strupcase(strtrim(runs(0),2))
  widget_control, ctx.user_w, get_value=users
  users = strupcase(strtrim(users(0),2))
  widget_control, ctx.nv_w, get_uvalue=nv
  if (nv) then $
    voided = 0 $
  else begin
    widget_control, ctx.v_w, get_uvalue=v
    if (v) then $
      voided = 1 $
    else $
      voided = -1
  endelse
  topics = ''
  for i=0, n_elements(ctx.tb_w)-1 do begin
    widget_control, ctx.tb_w(i), get_uvalue=t
    if (t) then $
      topics = topics+ctx.topics(i)+','
  endfor
  if (strlen(topics) gt 0) then $
    topics = strmid(topics, 0, strlen(topics)-1)
  return, {ok:1, runs:runs, topics:topics, users:users, voided:voided}
end

pro SELECT_EVENT, ev
  widget_control, ev.top, get_uvalue = ctx, /no_copy, /hour
  case (tag_names(ev, /STRUCTURE_NAME)) of
  "WIDGET_BUTTON": begin
        WIDGET_CONTROL, ev.id, get_value=button
        case (button) of
        "Ok" : begin
           options  = selection_options_get(ctx)
           handle_value, ctx.hand, options, /set
           WIDGET_CONTROL, ev.top, /DESTROY
         end
        "Cancel" : begin
           handle_value, ctx.hand, {ok:0}, /set
           WIDGET_CONTROL, ev.top, /DESTROY
         end
        "Reset" : begin
           handle_value, ctx.hand, selection_options
           RESET_SELECTION_OPTIONS, ctx, selection_options
         end
        "Help" : man_proc, 'entry-display o_select_options'
        "Select All" : begin
           for i = 0, n_elements(ctx.tb_w) -1 do begin
             widget_control, ctx.tb_w(i), get_uvalue=state
             if state eq 0 then $
               widget_control, ctx.tb_w(i), set_uvalue = 1, set_button = 1
           endfor
         end
        "Clear All" : begin
           for i = 0, n_elements(ctx.tb_w) -1 do begin
             widget_control, ctx.tb_w(i), get_uvalue=state
             if state eq 1 then $
               widget_control, ctx.tb_w(i), set_uvalue = 0, set_button = 0
           endfor
         end
        else: begin
           widget_control, ev.id, get_uvalue=state
           state = state eq 0
           widget_control, ev.id, set_uvalue = state, set_button = state
         end
        endcase
       end
  else:
  endcase
  if (widget_info(ev.top, /Valid_id)) then widget_control, ev.top, set_uvalue = ctx, /no_copy
end

pro RESET_SELECTION_OPTIONS, ctx, selection_options
  widget_control, ctx.run_w, set_value = selection_options.runs
  widget_control, ctx.user_w, set_value = selection_options.users

  case (selection_options.voided) of
  -1 : begin
       widget_control, ctx.dc_w, set_button=1, set_uvalue=1
       widget_control, ctx.nv_w, set_button=0, set_uvalue=0
       widget_control, ctx.v_w, set_button=0, set_uvalue=0
       end
   0 : begin
       widget_control, ctx.dc_w, set_button=0, set_uvalue=0
       widget_control, ctx.nv_w, set_button=1, set_uvalue=1
       widget_control, ctx.v_w, set_button=0, set_uvalue=0
       end
   1 : begin
       widget_control, ctx.dc_w, set_button=0, set_uvalue=0
       widget_control, ctx.nv_w, set_button=0, set_uvalue=0
       widget_control, ctx.v_w, set_button=1, set_uvalue=1
       end
   else:
   endcase

   topics = str_sep(selection_options.topics, ',')
   for i=0, n_elements(ctx.topics)-1 do begin
     found = 0
     for j=0, n_elements(topics)-1 do begin
       if (strtrim(topics(j),2) eq strtrim(ctx.topics(i),2)) then found = 1
     endfor
     widget_control, ctx.tb_w(i), set_button = found, set_uvalue = found
   endfor
end

pro find_in_entries, ctx
  base = widget_base(title='Find Text in Entries', /column, group_leader=ctx.group_leader)
  l = widget_label(base, value='Look for:')
  t_w = widget_text(base, xsize=20, value = '', /editable, /all_events)
  b = widget_base(base, /row, /nonexclusive)
  case_w = widget_button(b, value='Case Sensitive')
  widget_control, case_w, set_button=0
  r = widget_base(base, /row)
  b = widget_button(r, value='Find First')
  b = widget_button(widget_base(r, /frame), value='Find Next')
  b = widget_button(r, value='Cancel')
  b = widget_button(r, value='Help')
  widget_control, base,/realize,set_uvalue={ctx:ctx, t_w:t_w, case_sens:0, start_pos:0}
  XMANAGER, 'FIND', base, group=ctx.group_leader
end

pro make_entry, ctx
  mbar = 0l
  base = widget_base(title="Make logbook entry", resource='makeEntry', /col, mbar=mbar)
  bb = widget_button(mbar, value='File', /menu)
  apply_b = widget_button(bb, value = 'Make Entry',/tracking, uvalue={key:'make entry', descr:'Make this entry.'})
  widget_control, apply_b, sensitive=0
  b = widget_button(bb, value='Dismiss', /tracking, uvalue = {key:'Cancel',descr:'Close this window without making an entry'})
  bb = widget_button(mbar, value='Run', /tracking, /menu, uvalue = {key:'',descr:'Control the behavior of the shot field'})
  b = widget_button(bb, value = 'Track Current', /tracking, uvalue ={key:'track current run', descr:'Run will always be the current run'})
  b = widget_button(bb, value = 'Current', /tracking, uvalue = {key:'current run', descr:'Run will be set to the current run'})
  b = widget_button(bb, value = 'Previous', /tracking, uvalue = {key:'previous run', descr:'Run will be set to the previous run from the current run'})
  b = widget_button(bb, value = 'Next', /tracking, uvalue = {key:'next run', descr:'Run will be set to the next run from the current run'})
  b = widget_button(bb, value = 'Increment', /tracking, uvalue = {key:'increment run', descr:'Run will be set to the current value plus 1'})
  b = widget_button(bb, value = 'Decrement', /tracking, uvalue = {key:'decrement run', descr:'Run will be set to the current value minus 1'})
  b = widget_button(bb, value = 'None', /tracking, uvalue = {key:'no run', descr:'Run will be cleared'})
  bb = widget_button(mbar, /tracking, value='Shot', /menu, uvalue = {key:'',descr:'Control the behavior of the shot field'})
  b = widget_button(bb, value = 'Track Current', /tracking, uvalue = {key:'track current shot', descr:'Shot will always be the current shot'})
  b = widget_button(bb, value = 'Current', /tracking, uvalue = {key:'current shot', descr:'Shot will be set to the current shot'})
  b = widget_button(bb, value = 'Previous', /tracking, uvalue = {key:'previous shot', descr:'Shot will be set to the previous shot from the current shot'})
  b = widget_button(bb, value = 'Next', /tracking, uvalue = {key:'next shot', descr:'Shot will be set to the next shot from the current shot'})
  b = widget_button(bb, value = 'Increment', /tracking, uvalue = {key:'increment shot', descr:'Shot will be set to the current value plus 1'})
  b = widget_button(bb, value = 'Decrement', /tracking, uvalue = {key:'decrement shot', descr:'Shot will be set to the current value minus 1'})
  b = widget_button(bb, value = 'None', /tracking, uvalue = {key:'no shot', descr:'Shot will be cleared'})
  bb = widget_button(mbar, value='Topic', /menu, /tracking, uvalue={key:'', descr:'Set the topic field from list of available topics'})
;  set_database, 'logbook'
  count = dsql("select topic, brief from topics order by topic", topics, briefs)
;  sql_finish
  for i=0, count-1 do begin
    b = widget_button(bb, value=topics(i), uvalue = {key:'topic', descr:briefs(i)}, /tracking)
  endfor
  bb = widget_button(mbar, value='Options')
  b = widget_button(bb, value = 'Set Preferences', uvalue = {key:'set preferences', descr:'Set default behavior'})
  r = widget_base(base, /row)
  l = widget_label(r, value = 'Run ')
  run_w = widget_text(r, xsize=7, value='', /editable)
  shot_w = widget_text(r, xsize=3, value='', /editable)
  l = widget_label(r, value = 'Shot ')
  r = widget_base(base, /row)
  user = GETENV("user")
  if strlen(user) eq 0 then begin
;    set_database, 'logbook'
    dummy = dsql('select suser_sname()', user)
    user=user(0)
;    sql_finish
  endif
  l = widget_label(r, value = 'User ')
  user_w = widget_label(r, value=user)
  l = widget_label(r, value = 'Topic ')
  topic_w = widget_text(r, value='               ')
  txt_w = widget_text(base, xsize=80, ysize=20, resource='entryText',/editable, /all_events, event_func = 'edit_txt_event' )
  r = widget_base(base, /row)
  l = widget_label(r, value='Run:')
  b = widget_button(r ,frame=1, value = ' T ', /tracking, uvalue ={key:'track current run', descr:'Run will always be the current run'})
  b = widget_button(r ,frame=1, value = ' C ', /tracking, uvalue = {key:'current run', descr:'Run will be set to the current run'})
  b = widget_button(r ,frame=1, value = ' P ', /tracking, uvalue = {key:'previous run', descr:'Run will be set to the previous run from the current run'})
  b = widget_button(r ,frame=1, value = ' N ', /tracking, uvalue = {key:'next run', descr:'Run will be set to the next run from the current run'})
  b = widget_button(r ,frame=1, value = ' I ', /tracking, uvalue = {key:'increment run', descr:'Run will be set to the current value plus 1'})
  b = widget_button(r ,frame=1, value = ' D ', /tracking, uvalue = {key:'decrement run', descr:'Run will be set to the current value minus 1'})
  b = widget_button(r ,frame=1, value = ' X ', /tracking, uvalue = {key:'no run', descr:'Run will be cleared'})

  l = widget_label(r, value='Shot:')
  b = widget_button(r ,frame=1, value = ' T ', /tracking, uvalue ={key:'track current shot', descr:'Shot will always be the current shot'})
  b = widget_button(r ,frame=1, value = ' C ', /tracking, uvalue = {key:'current shot', descr:'Shot will be set to the current shot'})
  b = widget_button(r ,frame=1, value = ' P ', /tracking, uvalue = {key:'previous shot', descr:'Shot will be set to the previous shot from the current shot'})
  b = widget_button(r ,frame=1, value = ' N ', /tracking, uvalue = {key:'next shot', descr:'Shot will be set to the next shot from the current shot'})
  b = widget_button(r ,frame=1, value = ' I ', /tracking, uvalue = {key:'increment shot', descr:'Shot will be set to the current value plus 1'})
  b = widget_button(r ,frame=1, value = ' D ', /tracking, uvalue = {key:'decrement shot', descr:'Shot will be set to the current value minus 1'})
  b = widget_button(r ,frame=1, value = ' X ', /tracking, uvalue = {key:'no shot', descr:'Shot will be cleared'})
  l = widget_label(r, value=' ')
  b = widget_button(r ,frame=1, value = 'Load Template', /tracking, uvalue = {key:'load template', descr:'Load a template file into the buffer'})
  r = widget_base(base, /row)
  ok_b = widget_button(r, /tracking, value = 'Make Entry', uvalue={key:'make entry', descr:'Make this entry.'})
  widget_control, ok_b, sensitive=0
  b = widget_button(r, value = 'Reset', /tracking, uvalue={key:'Reset', descr:'Clear window or load template'})
  b = widget_button(r, value = 'Dismiss', /tracking, uvalue={key:'Cancel', descr:'Close window WITHOUT making entry'})
  b = widget_button(r, value = 'Help', /tracking, uvalue={key:'Help', descr:'Display help information about this dialog'})

  status_w = widget_text(base, xsize=80)
  m_ctx = {status_w:status_w, ok_b:ok_b, apply_b:apply_b, dirty:0, $
         run_w:run_w, shot_w:shot_w, topic_w:topic_w, user_w:user_w, $
         txt_w:txt_w, status_txt:'', make_options:ctx.make_options, default_path:ctx.default_path}
  make_entry_reset, m_ctx
  widget_control, base, set_uvalue=m_ctx,/realize
  XMANAGER, 'MAKE', base, group=ctx.group_leader
end

pro edit_entry, ctx
  handle_value, ctx.handle, hand
  if (hand.selected eq -1) then $
    X_Complain, "No entry selected to edit" $
  else begin
;  set_database, 'logbook'
    count = dsql('select run, shot, username, topic, text from entries where dbkey=?', hand.key(hand.selected), $
                 run, shot, user, topic, text)
;    SQL_FINISH
    if (count eq 1) then begin
      NULL = 2147483647L
      base = widget_base(title="Edit logbook entry", /col, /modal, group_leader=ctx.group_leader)
      r = widget_base(base, /row)
      l = widget_label(r, value = 'User '+ user(0))
      l = widget_label(r, value = 'Topic '+ topic(0))
      r = widget_base(base, /row)
      l = widget_label(r, value = 'Run ')
      if (run(0) ne NULL) then $
        run_str = strtrim(string(run(0)),2) $
      else $
        run_str = ' '
      runw = widget_text(r, xsize = 6, /editable, value=run_str, event_func = 'edit_txt_event', /all_events)
      l = widget_label(r, value = 'Shot ')
      if (shot(0) ne NULL) then $
        shot_str  = strtrim(string(shot(0) mod 1000),2) $
      else $
        shot_str = ' '
      shotw = widget_text(r, xsize=3, /editable, value = shot_str, event_func = 'edit_txt_event', /all_events)
      txt = widget_text(base, xsize=80, ysize=20, value=text(0), /editable, /all_events, event_func = 'edit_txt_event' )
      r = widget_base(base, /row)
      ok_b = widget_button(r, value = 'Ok')
      widget_control, ok_b, sensitive=0
      b = widget_button(r, value = 'Void')
      b = widget_button(r, value = 'Cancel')
      b = widget_button(r, value = 'Help')
      widget_control, base, /realize, set_uvalue = {txt:txt, runw:runw, shotw:shotw, ok_b:ok_b, apply_b:0L, $
                                                    key:hand.key(hand.selected), $
                                                    dirty:0, text:text(0), shot:shot(0), run:run(0), ctx:ctx}
      XMANAGER, 'EDIT', base
    endif else X_Complain, "Error selecting record for editing"
  endelse
end

pro void_entry, ctx
  handle_value, ctx.handle, hand
  if (hand.selected eq -1) then $
    X_Complain, "No entry selected to edit" $
  else begin
;    set_database, 'logbook'
    count = dsql('select username, topic, text from entries where dbkey=?', hand.key(hand.selected), user, topic, text)
;    SQL_FINISH
    if (count eq 1) then begin
      base = widget_base(title="Void logbook entry - are you sure?", /col, /modal, group_leader=ctx.group_leader)
      r = widget_base(base, /row)
      l = widget_label(r, value = 'User '+ user(0))
      l = widget_label(r, value = 'Topic '+ topic(0))
      txt = widget_text(base, xsize=80, ysize=20, value=text(0) )
      r = widget_base(base, /row)
      b = widget_button(r, value = 'Void')
      b = widget_button(r, value = 'Cancel')
      b = widget_button(r, value = 'Help')
      widget_control, base, /realize, set_uvalue = {txt:txt, $
                                                    key:hand.key(hand.selected), $
                                                    dirty:0, text:text(0), ctx:ctx}
      XMANAGER, 'EDIT', base
    endif else X_Complain, "Error selecting record for editing"
  endelse
end

function edit_txt_event, ev
  widget_control, ev.top, get_uvalue = ctx
  widget_control, ctx.ok_b, sensitive=1
  if (ctx.apply_b ne 0) then $
    widget_control, ctx.apply_b, sensitive=1
  ctx.dirty=1
  widget_control, ev.top, set_uvalue = ctx
end

pro make_entry_reset, ctx, template=template
  if (keyword_set(template)) then $
    load_template, ctx $
  else begin
    case ctx.make_options.after_action of
    -1 : widget_control, ctx.txt_w, set_value = ''
    0 :
    1 : load_template, ctx
    end
  endelse
end

pro load_template, ctx
  catch, problem
  if (problem ne 0) then begin
    X_Complain, "Error reading template file "+ctx.make_options.template
    return
  endif
  openr, lun, ctx.make_options.template, /get_lun
  line = ''
  widget_control, ctx.txt_w, set_value = ''
  while (not EOF(lun)) do begin
    readf, lun, line
    widget_control, ctx.txt_w, set_value = line , /append
  end
  catch, /cancel
  close, lun
  free_lun, lun
end

forward_function fix_quotes

function fix_quotes, text
  idx = strpos(text, '"')
  if (idx eq -1) then $
    return, text $
  else $
    return, strmid(text, 0, idx)+'""' + fix_quotes(strmid(text, idx+1, strlen(text)-idx-1))
;  return, text
end

function make_new_entry, ctx
  if (not ctx.dirty) then begin
    X_complain, 'Nothing typed - not entered'
    return, 0
  endif

  widget_control, ctx.shot_w, get_value=shot_txt
  shot_txt = strtrim(shot_txt(0), 2)
  widget_control, ctx.run_w, get_value=run_txt
  run_txt = strtrim(run_txt(0), 2)
  widget_control, ctx.topic_w, get_value=topic_txt
  topic_txt = strtrim(topic_txt(0), 2)
  widget_control, ctx.txt_w, get_value=new_text
  for I=0, n_elements(new_text)-1 do begin
    if (i eq 0) then $
      xx = new_text(i) $
    else $
      xx = xx + string([10b]) + new_text(i)
  endfor
  text_txt = xx
  widget_control, ctx.user_w, get_value = user_txt

  if (strlen(topic_txt) eq 0) then begin
    X_Complain, 'All logbook entries must have a topic'
    return, 0
  endif

  shot = -1
  run = -1
  if (strlen(shot_txt) gt 0) then begin
    if (shot_txt eq 'CURRENT') then begin
      shot = 0
    endif else begin
      shot = long(shot_txt)
    endelse
  endif
  if (strlen(run_txt) gt 0) then begin
    if (run_txt eq 'CURRENT') then begin
      run = 0
    endif else begin
      run = long(run_txt)
    endelse
  endif
  if (shot eq 0) and (run eq -1) then run = 0
  if (shot eq 0) then shot = GetCurrentShot()
  if (run eq 0) then run = GetCurrentRun()
  if (shot ne -1) and (run eq -1) then begin
    X_Complain, 'All entries associated with a shot must also be associated with a run'
    return, 0
  endif

;  dummy = dsql("set transaction read write reserving entries for shared write")
  text_txt = fix_quotes(text_txt)
  if (run eq -1) then begin
;    set_database, 'logbook'
    dummy = dsql("INSERT INTO ENTRIES (topic, text) values (?,?)", topic_txt, text_txt, status=status, /quiet)
;    SQL_FINISH
    goto, done
  endif

  if (shot eq -1) then begin
;    set_database, 'logbook'
    dummy = dsql("INSERT INTO ENTRIES (run, topic, text) values (?,?,?)", run, topic_txt, text_txt, status=status, /quiet)
;    SQL_FINISH
    goto, done
  endif

  lshot = 0l
  lshot = run*1000+shot
;  set_database, 'logbook'
  dummy = dsql("INSERT INTO ENTRIES (run, shot, topic, text) values (?,?,?,?)", run,  lshot, topic_txt, text_txt, status=status, /quiet)
;  SQL_FINISH

done:
  if (status) then begin
    if (not status) then goto, error_exit
    ctx.status_txt = 'Entry inserted at '+systime(0)
    ctx.dirty = 0
    widget_control, ctx.ok_b, sensitive=0
    widget_control, ctx.apply_b, sensitive=0
    widget_control, ctx.status_w, set_value = string(ctx.status_txt, format='(a132)')
    dummy = execute("mdssetevent, 'LOGBOOK_ENTRY")
    make_entry_reset, ctx
    return, 1
  endif else begin
error_exit:
    x_complain, 'Error inserting record'
    return, 0
  endelse
end
;
;
; procedure find - finds the next instance of a string in the
; entries buffer.
;
; basic algorithm
;   - do case conversion if ness
;   - find all instances of the string in the buffer
;   - if there were any
;   -   find the one which is after the last one found
;   -   if there were any
;   -     save this line as the last one
;   -     compute the text offset and select the text
;   -   else
;   -     ring the bell and reset start
;   - else
;   -   ring the bell and reset start
;
; NOTE - will only find the first instance of a string on any given
; line!!
;
pro find, ctx
  handle_value, ctx.ctx.handle, hand
  widget_control, ctx.t_w, get_value=str
  str = str(0)
  if (ctx.case_sens eq 0) then begin
    str = strlowcase(str)
    txt = strlowcase(hand.text)
  endif else txt = hand.text
  idxs = strpos(txt, str)
  found_idxs = where(idxs ne -1, count)
  if (count gt 0) then begin
    new_idxs = where (found_idxs gt ctx.start_pos, count)
    if (count gt 0) then begin
      ctx.start_pos = found_idxs(new_idxs(0))
      geo = widget_info(ctx.ctx.txt_w, /geometry)
      height = geo.ysize
      newtop = (ctx.start_pos-height/2) > 0
      offset = widget_info(ctx.ctx.txt_w, text_xy_to_offset=[idxs(ctx.start_pos), ctx.start_pos])
      widget_control, ctx.ctx.txt_w, set_text_top_line=newtop, set_text_select=[offset, strlen(str)]
    endif else begin
      ctx.start_pos = 0
      print, string(7b), format='(a,$)'
    endelse
  endif else begin
    ctx.start_pos = 0
    print, string(7b), format='(a,$)'
  endelse
end

pro find_event, ev
  widget_control, ev.top, get_uvalue = ctx
  case (tag_names(ev, /STRUCTURE_NAME)) of
  "WIDGET_BUTTON": begin
     WIDGET_CONTROL, ev.id, get_value=button
     case (button) of
     'Cancel': widget_control, ev.top, /destroy
     'Find First': begin
       ctx.start_pos = 0
       find, ctx
     end
     'Find Next': begin
       find, ctx
     end
     'Help':  man_proc, "entry-display q_find_in_entries"
     else:
     endcase
  end
  "WIDGET_TEXT_CH": begin
    if (ev.ch eq 10b) then find, ctx
  end
  else:
  endcase
  if (widget_info(ev.top, /Valid_id)) then widget_control, ev.top, set_uvalue = ctx
end

pro make_event, ev
  widget_control, ev.top, get_uvalue = ctx
  case (tag_names(ev, /STRUCTURE_NAME)) of
  "WIDGET_TRACKING" : begin
     if (ev.enter) then begin
       widget_control, ev.id, get_uvalue = key
       widget_control, ctx.status_w, set_value = string(key.descr, format='(A132)')
     endif else widget_control, ctx.status_w, set_value = string(ctx.status_txt, format='(a132)')
   end

  "WIDGET_BASE": begin
	widget_control, ctx.txt_w, scr_xsize = ev.x-6, scr_ysize=ev.y-6
      end
  "WIDGET_BUTTON": begin
        widget_control, ev.top, /hour
        WIDGET_CONTROL, ev.id, get_value=button, get_uvalue=key
        case (key.key) of
          'track current run': begin
               widget_control, ctx.run_w, set_value = 'CURRENT'
             end
          'current run': begin
               widget_control, ctx.run_w, set_value = STRTRIM(STRING(GetCurrentRun(), format='(I7.7)'), 2)
               widget_control, ctx.shot_w, set_value = ''
             end
          'previous run': begin
               widget_control, ctx.run_w, set_value = STRTRIM(STRING(GetCurrentRun()-1, format='(I7.7)'), 2)
               widget_control, ctx.shot_w, set_value = ''
             end
          'next run': begin
               widget_control, ctx.run_w, set_value = strtrim(STRING(GetCurrentRun()+1, format='(I7.7)'),2)
               widget_control, ctx.shot_w, set_value = ''
             end
          'increment run': begin
               widget_control, ctx.run_w, get_value=txt
               if (STRTRIM(txt(0),2) eq 'CURRENT') then $
                 widget_control, ctx.run_w, set_value = strtrim(STRING(GetCurrentRun()+1, format='(I7.7)'), 2) $
               else begin
                 run = long(txt(0))
                 widget_control, ctx.run_w, set_value = strtrim(STRING(run+1, format='(I7.7)'),2)
               endelse
               widget_control, ctx.shot_w, set_value = ''
             end
          'decrement run': begin
               widget_control, ctx.run_w, get_value=txt
               if (STRTRIM(txt(0),2) eq 'CURRENT') then $
                 widget_control, ctx.run_w, set_value = strtrim(STRING(GetCurrentRun()-1, format='(I7.7)'),2) $
               else begin
                 run = long(txt(0))
                 widget_control, ctx.run_w, set_value = strtrim(STRING(run-1, format='(I7.7)'), 2)
               endelse
               widget_control, ctx.shot_w, set_value = ''
             end
          'no run': begin
               widget_control, ctx.run_w, set_value = ''
               widget_control, ctx.shot_w, set_value = ''
             end

          'track current shot': begin
               widget_control, ctx.run_w, set_value = 'CURRENT'
               widget_control, ctx.shot_w, set_value = 'CURRENT'
             end
          'current shot': begin
               widget_control, ctx.run_w, set_value = strtrim(STRING(GetCurrentRun(), format='(I7.7)'),2)
               widget_control, ctx.shot_w, set_value = strtrim(STRING(GetCurrentShot(), FORMAT='(I3.3)'),2)
             end
          'previous shot': begin
               shot = GetCurrentShot()-1
               if (shot gt 0) then begin
                 widget_control, ctx.run_w, set_value = strtrim(STRING(GetCurrentRun(), format='(I7.7)'),2)
                 widget_control, ctx.shot_w, set_value = strtrim(STRING(GetCurrentShot()-1, format='(I3.3)'),2)
               endif else begin
                 x_complain, 'No previous shot', group_leader=ev.top
               endelse
             end
          'next shot': begin
               widget_control, ctx.run_w, set_value = strtrim(STRING(GetCurrentRun(), format='(I7.7)'),2)
               widget_control, ctx.shot_w, set_value = strtrim(STRING(GetCurrentShot()+1, format='(I3.3)'),2)
             end
          'increment shot': begin
               widget_control, ctx.shot_w, get_value=txt
               if (STRTRIM(txt(0),2) eq 'CURRENT') then $
                 widget_control, ctx.shot_w, set_value = strtrim(STRING(GetCurrentShot()+1, format='(I3.3)'), 2) $
               else begin
                 shot = long(txt(0))
                 widget_control, ctx.shot_w, set_value = strtrim(STRING(shot+1, format='(I3.3)'), 2)
               endelse
             end
          'decrement shot': begin
               widget_control, ctx.shot_w, get_value=txt
               if (STRTRIM(txt(0),2) eq 'CURRENT') then begin
                 shot = GetCurrentShot()-1
                 if shot gt 0 then $
                   widget_control, ctx.shot_w, set_value = strtrim(STRING(GetCurrentShot()-1, format='(I3.3)'), 2) $
                 else $
                   x_complain, 'No previous shot', group_leader=ev.top
               endif else begin
                 shot = long(txt(0)) - 1
                 if shot gt 0 then $
                   widget_control, ctx.shot_w, set_value = strtrim(STRING(shot-1, format='(I3.3)'), 2) $
                 else $
                   x_complain, 'No previous shot', group_leader=ev.top
               endelse
             end
          'no shot': begin
               widget_control, ctx.shot_w, set_value = ''
             end
          'topic': begin
               widget_control, ctx.topic_w, set_value=button
             end
          'set preferences':  begin
              new_options = SET_MAKE_OPTIONS(ctx.make_options, ctx.default_path, ev.top)
              if struct_cmp(new_options , ctx.make_options) ne 0 then begin
                ctx.make_options = new_options
              endif
             end
          'make entry' : begin
               dummy = make_new_entry(ctx)
             end

          'Reset' :  make_entry_reset, ctx
          'Cancel' : begin
               widget_control, ev.top, /destroy
             end
          'Dismiss' : begin
               widget_control, ev.top, /destroy
             end
          'Help' : man_proc, "entry-display e_make_entry"
          'load template' : begin
             name = DIALOG_PICKFILE(file=ctx.make_options.template, path=ctx.default_path, $
                                     get_path=path, /read, /must,filter = "*.TEMPLATE")
             if (strlen(name) gt 0) then begin
               tmp = ctx.make_options.template
               ctx.make_options.template = name
               make_entry_reset, ctx, /template
               ctx.make_options.template = tmp
             endif
           end

         else:
         endcase
       end
  else:
  endcase
  if (widget_info(ev.top, /Valid_id)) then begin
    widget_control, ev.top, set_uvalue = ctx
  endif
end

pro edit_event, ev
  NULL = 2147483647L
  widget_control, ev.top, get_uvalue = ctx, /hour
  case (tag_names(ev, /STRUCTURE_NAME)) of
  "WIDGET_BASE": begin
	widget_control, ctx.txt_w, scr_xsize = ev.x-6, scr_ysize=ev.y-6
      end
  "WIDGET_BUTTON": begin
        WIDGET_CONTROL, ev.id, get_value=button
        case (button) of
        "Ok" : begin
;           set_database, 'logbook'
           widget_control, ev.top, get_uvalue = ctx
           if (ctx.dirty) then begin
             widget_control, ctx.txt, get_value=new_text
             for I=0, n_elements(new_text)-1 do begin
               if (i eq 0) then $
                 xx = new_text(i) $
               else $
                 xx = xx + string([10b]) + new_text(i)
             endfor
             new_text = xx
             widget_control, ctx.runw, get_value=run_text
             run_text = strtrim(run_text(0),2)
             if (strlen(run_text) eq 0) then new_run = null else new_run = long(run_text)
             widget_control, ctx.shotw, get_value=shot_text
             shot_text = strtrim(shot_text(0),2)
             if (strlen(shot_text) eq 0) then new_shot = null else new_shot = long(shot_text)
             if (new_shot ne null) and (new_run eq null) then begin
               x_complain,'You must fill in a run to fill in a shot', group_leader=ev.top
               return
             endif else begin
               if (new_shot ne null) then $
                 new_shot = new_run*1000+new_shot
             endelse
             if (new_text ne ctx.text) or (new_shot ne ctx.shot) or (new_run ne ctx.run) then begin
;               dummy = dsql("set transaction read write reserving entries for shared write")
               dummy = dsql("select run, shot, topic from entries where dbkey = ?", ctx.key, run, shot, topic)
               dummy = dsql("update entries set voided = getdate() where dbkey = ?", ctx.key, status=status)
               if (status) then begin
                 new_text = fix_quotes(new_text)
                 if (new_run eq null) then $
                   dummy = dsql("insert into entries (topic, text) values (?,?)", topic(0), new_text, /quiet, status=status) $
                 else if (new_shot eq null) then $
                   dummy = dsql("insert into entries (run, topic, text) values (?,?,?)", new_run, topic(0), new_text, /quiet, status=status) $
                 else $
                   dummy = dsql("insert into entries (run, shot, topic, text) values (?,?,?,?)", new_run, new_shot, topic(0), new_text, /quiet, status = status)
                if status then begin
                  if status then begin
      		    DUMMY = EXECUTE("mdssetevent, 'LOGBOOK_ENTRY")
                  endif else begin
                    X_Complain, "Could not commit new entry, changes not entered", group_leader=ev.top
                  endelse
                endif else begin
                 if (not status) then begin
                   X_Complain, "Could not insert new entry, changes not entered", group_leader=ev.top
                 endif
                endelse
               endif else begin
                 X_Complain, "Could not void entry, changes not entered", group_leader=ev.top
               endelse
             endif else begin
               print, "The text is the same"
             endelse
           endif
           widget_control, ev.top, /destroy
   ;       sql_finish
           end
        "Void" : begin
;           set_database, 'logbook'
           widget_control, ev.top, get_uvalue = ctx
           dummy = dsql("update entries set voided = getdate() where dbkey = ?", ctx.key, status = status, error =error)
           if (status) then begin
             if (status) then begin
      		dummy = EXECUTE("mdssetevent, 'LOGBOOK_ENTRY")
             endif else begin
               X_Complain, "Could not void entry, changes not commited", group_leader=ev.top
             endelse
           endif else begin
             X_Complain, ["Could not void entry, changes not entered",error], group_leader=ev.top
           endelse
           widget_control, ev.top, /destroy
 ;          sql_finish
           end
        "Cancel" : begin
           widget_control, ev.top, /destroy
           end
         "Help" : man_proc, "entry-display e_edit_entry"
         else:
         endcase
       end
  else:
  endcase
  if (widget_info(ev.top, /Valid_id)) then widget_control, ev.top, set_uvalue = ctx
end

function txt_event,ev
  if (tag_names(ev, /structure_name) eq 'WIDGET_TEXT_SEL') then begin
    widget_control, ev.top, get_uvalue = ctx
    handle_value, ctx.handle, hand
    if (ev.length gt 0) then begin
      off = ev.offset
      xy = widget_info(ev.id, text_offset_to_xy=off)
      if (!version.os eq 'Win32' ) then begin
        off = off-(xy(1)*2)
        xy = widget_info(ev.id, text_offset_to_xy=off)
      endif
      line = xy(1)
      start = total(hand.lines lt line)
      entry = FIX(start-1)
      if (hand.selected ne entry) then begin
        select_entry, ctx, hand, entry
        hand.selected = entry
      endif
    endif else hand.selected = -1
    handle_value, ctx.handle, hand, /set
  endif
  return, 0
end

pro select_entry, ctx, hand, idx
  end_pos =  widget_info(ctx.txt_w, text_xy_to_offset=[0,hand.lines(idx+1)+1])
  if (!version.os eq "Win32" ) then begin
    end_pos = end_pos+hand.lines(idx+1)*2
  endif
  if (end_pos le 0) then end_pos = widget_info(ctx.txt_w, /text_number)-1
  start_pos = widget_info(ctx.txt_w, text_xy_to_offset=[0,hand.lines(idx)+1])
  if (!version.os eq "Win32") then begin
    start_pos = start_pos+hand.lines(idx)*2
  endif
  len = end_pos-start_pos+1
  widget_control, ctx.txt_w, set_text_select=[start_pos, len]
end

function supersep, text
  cr = string(13b)
  lf = string(10b)
  crlf = string([13b, 10b])
  if (strpos(text, crlf) ne -1) then $
    e_t = str_sep(text, crlf) $
  else $
    e_t = [text]
  ans = e_t
  idx = 0
  for i=0, n_elements(e_t)-1 do begin
    if (strpos(e_t(i), lf) ne -1) then $
      this = str_sep(e_t(i), lf) $
    else $
      this = e_t(i)
    if (i eq 0) then $
      ans = this $
    else $
      ans = [ans, this]
  endfor
  e_t = ans
  for i=0, n_elements(e_t)-1 do begin
    if (strpos(e_t(i), cr) ne -1) then $
      this = str_sep(e_t(i), cr) $
    else $
      this = e_t(i)
    if (i eq 0) then $
      ans = this $
    else $
      ans = [ans, this]
  endfor
  return, ans
end

function get_entry_text, run, shot, entered, topic, text, user
  NULL = 2147483647L
  e_t = supersep(text)
  txt = ['.   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   ',e_t]
  if shot ne NULL then $
    txt = ['----------------------------------------------------------------------------',+ $
           string(shot)+'	'+strtrim(user,2)+'	'+topic+'	'+entered, txt] $
  else if run ne NULL then $
    txt = ['----------------------------------------------------------------------------',+ $
           strtrim(string(run), 2)+'xxx'+'	'+strtrim(user,2)+'	'+topic+'	'+entered, txt] $
  else $
    txt = ['----------------------------------------------------------------------------',+ $
           '         '+'	'+strtrim(user,2)+'	'+topic+'	'+entered, txt]
;  if (!version.os eq 'Win32') then begin
;    crlf = string([13b,10b])
;    tmp = txt+(crlf)
;    txt = ''
;    for i=0, n_elements(tmp)-1 do begin
;      txt = txt + tmp(i)
;    endfor
;  endif
  return, txt
end

pro do_query, ctx, custom=custom
  sz = size(custom)
  if (sz(1) eq 7) then begin
      widget_control, ctx.message_w, set_value='Custom Query...'
      query=custom
  endif else begin
    widget_control, ctx.message_w, set_value='Auto Query...'
    query = ctx.query_txt
  endelse
  if (strlen(query) GT 0) then begin
;    query=query+ ' order by '+ctx.display_options.order_by+' limit to '+string(ctx.display_options.max_rows)+' rows'
    query=query+ ' order by '+ctx.display_options.order_by
;  set_database, 'logbook'
    dummy = dsql('set rowcount '+string(ctx.display_options.max_rows))
    count = dsql('select dbkey, username, run, shot, topic, text, entered from entries where '+query,  $
		   dbk2, user2, run2, shot2, topic2, text2, entered2, /QUIET, STATUS=STATUS, error=error)
;    print, "GOT BACK ", count
    dummy = dsql('set rowcount 0')
;    sql_finish
    if(not status) then begin
      X_Complain, ["SQL Error",string(error),"No records selected..." ]
      return
    endif
    if (count gt 0) then begin
      NULL = 2147483647L
      dbk = strarr(count)
      top_line = widget_info(ctx.txt_w, /text_top_line)
      widget_control, ctx.txt_w, set_value = ''
      widget_control, ctx.num_w, set_value = 'Num = '+string(count, format='(I3)')

      null_txt = [' ']
      null_lines = [0]
      null_line = 0
      null_idx = where (shot2 eq NULL, null_count)
      if (null_count gt 0) then begin
        null_dbk = strarr(n_elements(null_idx))
        for i=0, n_elements(null_idx)-1 do begin
          txt = get_entry_text(run2(null_idx(i)), shot2(null_idx(i)), entered2(null_idx(i)), $
                               topic2(null_idx(i)), text2(null_idx(i)), user2(null_idx(i)))
          null_txt = [null_txt, txt]
          null_line = null_line + n_elements(txt)
          null_lines = [null_lines, null_line]
          null_dbk(i) = dbk2(null_idx(i))
        endfor
      endif

      not_null_txt = [' ']
      not_null_lines = [0]
      not_null_line = 0
      not_null_idx = where (shot2 ne NULL, not_null_count)
      if (not_null_count gt 0) then begin
        not_null_dbk = strarr(n_elements(not_null_idx))
        for i=0, n_elements(not_null_idx)-1 do begin
          txt = get_entry_text(run2(not_null_idx(i)), shot2(not_null_idx(i)), entered2(not_null_idx(i)),$
                               topic2(not_null_idx(i)), text2(not_null_idx(i)), user2(not_null_idx(i)))
          not_null_txt = [not_null_txt, txt]
          not_null_line = not_null_line + n_elements(txt)
          not_null_lines = [not_null_lines, not_null_line]
          not_null_dbk(i) = dbk2(not_null_idx(i))
        endfor
      endif

      if (not_null_count gt 0) and (null_count gt 0) then begin
        if (ctx.display_options.null_first) then begin
          text = [null_txt, not_null_txt(1:*)]
          not_null_lines = not_null_lines + null_lines(n_elements(null_lines)-1)
          lines = [null_lines, not_null_lines]
          dbk = [null_dbk,'', not_null_dbk]
        endif else begin
          text = [not_null_txt, null_txt(1:*)]
          null_lines = null_lines + not_null_lines(n_elements(not_null_lines)-1)
          lines = [not_null_lines, null_lines]
          dbk = [not_null_dbk,'', null_dbk]
        endelse
      endif else if (not_null_count gt 0) then begin
        text = not_null_txt
        lines = not_null_lines
        dbk = not_null_dbk
      endif else begin
        text = null_txt
        lines = null_lines
        dbk = null_dbk
      endelse
      widget_control, ctx.txt_w, set_value =text
      if (count eq ctx.display_options.max_rows) then $
        widget_control, ctx.message_w, set_value="Max entries selected - To select more use Options->Display Options"
      handle_value, ctx.handle, {text:text, $
                                 lines:[lines, lines(n_elements(lines)-1)+1], key:LONG(dbk), selected : -1}, /set
      if ctx.display_options.auto_scroll then begin
        if ctx.display_options.scroll_bottom then begin
          geo = widget_info(ctx.txt_w, /geometry)
          chars = widget_info(ctx.txt_w, /text_number)
          gg = widget_info(ctx.txt_w,text_offset_to_xy=chars)
;          print, 'Auto to bottom'
;          help, /structure, geo
;          help, chars, gg(1)
          if gg(1) gt geo.ysize then begin
;            print, 'setting top line to ', gg(1)-geo.ysize
            widget_control, ctx.txt_w, set_text_top_line=gg(1)-geo.ysize+2
          endif; else begin
;            print, gg(1), ' is not > ', geo.ysize,' so scroll is nooop'
;          endelse
        endif else begin
;          print, 'Scroll top case'
          widget_control, ctx.txt_w, set_text_top_line=0
        endelse
      endif else begin
        widget_control, ctx.txt_w, set_text_top_line=top_line
      endelse
    endif else begin
      X_Complain, "No records selected..."
      widget_control, ctx.txt_w, set_value=''
    endelse
  endif else X_Complain, "You must specify a query before choosing 'Do Query'."
end

pro entry_display, default=default
common widget_common, base
  base=0L
  window,color=2
  wdelete
  if (not keyword_set(default)) then begin
    case !version.os of
    'Win32' : defaults_file = getenv('HOME')+'\EntryDisplay.dat'
    else: defaults_file=getenv('HOME')+'/EntryDisplay.dat'
    endcase
  endif else $
    defaults_file = default
  options = READ_OPTIONS(defaults_file)
  if (getenv("SYBASE_HOST") EQ "") then $
    X_COMPLAIN, "Use the environment varable SYBASE_HOST to specify your SQLSERVER database server before using ENTRY_DISPLAY" $
  else begin
  set_database, 'logbook'
;  sql_finish
  mbar = 0l
  base = widget_base(title='logbook entries', RESOURCE_NAME='entryDisplay', /column, /tlb_size_events, mbar=mbar)
  bb = widget_button(mbar, value='File',/menu)
  b = widget_button(bb, value='Print')
  b = widget_button(BB, value='Save Current Settings', /sep)
  b = widget_button(BB, value='Save Current Settings As ...')
  b = widget_button(BB, value='Restore Settings')
  b = widget_button(BB, value='Restore Settings From ...')
  b = widget_button(BB, value='Exit', /sep)
  b = widget_button(mbar, value='Query', /menu)
  bb = widget_button(b, value='Do Query')
  bb = widget_button(b, value='Show Query')
  bb = widget_button(b, value='Custom Query')
  bb = widget_button(b, value='Find in Entries')
  b = widget_button(mbar, value='Entry', /menu)
  bb = widget_button(b, value='Make Entry')
  bb = widget_button(b, value='Edit Entry')
  bb = widget_button(b, value='Void Entry')
  b = widget_button(mbar, value='Options', /menu)
  bb = widget_button(b, value='Selection Options')
  bb = widget_button(b, value='Display Options')
  bb = widget_button(b, value='Print Options')
  bb = widget_button(b, value='Make Entry Options')
  num_w = widget_button(mbar, value = 'Num =  0')
  bb = widget_button(num_w, value='')
;  widget_control, num_w, sensitive=0
  b = widget_button(mbar, value='Help', /help)
  bb = widget_button(b, value='About')
  bb = widget_button(b, value='About LOGBOOK')
  txt = widget_text(base, /scroll, xsize=80, ysize=30, event_func='txt_event', resource_name='mainText',/all_events)
  r = widget_base(base, /row)
  message_w = widget_text(r, value='', xsize=70)
  auto_w = widget_button(widget_base(r, /nonexclusive), value='Automatic Updates')
  widget_control,auto_w, set_button=options.display_options.auto_update
  if (!version.os eq "Win32" or !version.os eq "linux") then begin
    host = getenv("MDS_HOST")
    if (host ne "") then $
      mdsconnect, host
    dummy = execute("id = mdsevent(base, 'LOGBOOK_ENTRY')")
  endif else $
     dummy = execute("mdsevent, base, 'LOGBOOK_ENTRY'")
  ctx = {query_txt:'', txt_w:txt, handle:handle_create(), $
         num_w:num_w, auto_w:auto_w, message_w:message_w, $
         defaults_file:defaults_file,$
         updates_disabled:0, $
         display_options:options.display_options, $
         select_options:options.select_options, $
         print_options:options.print_options, $
         make_options:options.make_options, $
         custom_options:options.custom_options, $
         default_path:options.default_path, $
         x_off:0l, y_off:0l, x_ch_sz:0l, y_ch_sz:0l, auto_x:0l, $
         group_leader:base}
  handle_value, ctx.handle, {selected:-1}, /set
  ctx.query_txt = get_query(ctx)
  widget_control, base, /realize
  whole_geo=widget_info(base, /geometry)
  txt_geo = widget_info(ctx.txt_w, /geometry)
  auto_geo = widget_info(auto_w, /geometry)
  ctx.x_ch_sz = long(txt_geo.scr_xsize/txt_geo.xsize)
  ctx.y_ch_sz = long(txt_geo.scr_ysize/txt_geo.ysize)
  ctx.x_off = whole_geo.scr_xsize - txt_geo.scr_xsize +32
  ctx.y_off = whole_geo.scr_ysize - txt_geo.scr_ysize +32 + ctx.y_ch_sz + 20
  ctx.auto_x = auto_geo.scr_xsize
  widget_control, base, set_uvalue=ctx

  Xmanager, 'display', base, group=base
;  if (!version.os eq 'Win32') then $
;    if (getenv("MDS_HOST") ne "") then $
;      mdsdisconnect
  endelse
end
