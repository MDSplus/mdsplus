from MDSplus import Device

class RFXVICONTROL(Device):
  print('RFXVICONTROL')
  parts=[{'path':':COMMENT', 'type':'text'},
  {'path':':FILLING_TYPE', 'type':'numeric', 'value':3},
  {'path':':IMP_LEVEL', 'type':'numeric', 'value':100},
  {'path':':PRESSURE', 'type':'numeric', 'value':0},
  {'path':':CONTROL', 'type':'numeric', 'value':0},
  {'path':':CTRL_START', 'type':'numeric', 'value':0},
  {'path':':NE_SCALE', 'type':'numeric', 'value':0}]

  parts.append({'path':':FILL_WAVE','type':'signal', 'options':'compress_on_put'})
  parts.append({'path':':FILL_MIN_X','type':'numeric', 'value':0})
  parts.append({'path':':FILL_MAX_X','type':'numeric', 'value':1})
  parts.append({'path':':FILL_MIN_Y','type':'numeric', 'value':0})
  parts.append({'path':':FILL_MAX_Y','type':'numeric', 'value':10})

  parts.append({'path':':PUFF_WAVE','type':'signal', 'options':'compress_on_put'})
  parts.append({'path':':PUFF_MIN_X','type':'numeric', 'value':0})
  parts.append({'path':':PUFF_MAX_X','type':'numeric', 'value':1})
  parts.append({'path':':PUFF_MIN_Y','type':'numeric', 'value':0})
  parts.append({'path':':PUFF_MAX_Y','type':'numeric', 'value':10})

  parts.append({'path':':CTRL_WAVE','type':'signal', 'options':'compress_on_put'})
  parts.append({'path':':CTRL_MIN_X','type':'numeric', 'value':0})
  parts.append({'path':':CTRL_MAX_X','type':'numeric', 'value':1})
  parts.append({'path':':CTRL_MIN_Y','type':'numeric', 'value':0})
  parts.append({'path':':CTRL_MAX_Y','type':'numeric', 'value':10})
