try:
  import gtk as _gtk
except:
  pass

if '_gtk' in dir():
  from mdspluswidget import MDSplusWidget
  from mdsplusonoff import MDSplusOnOffWidget
  from mdsplusexpr import MDSplusExprFieldWidget,MDSplusExprWidget
  from mdsplusokbuttons import MDSplusOkButtons
  from mdsplusxdbox import MDSplusXdBox
  from mdspluspathwidget import MDSplusPathWidget
  from mdsplusnidoptionwidget import MDSplusNidOptionWidget
  from mdsplusonoffxdbox import MDSplusOnOffXdBoxWidget
  from mdsplusxdboxbutton import MDSplusXdBoxButtonWidget
  from mdsplusdigchans import MDSplusDigChansWidget
