pro CAMASSIGN,MODULE,KEY
;+
;CAM$ASSIGN assigns a key to a module's logical name.
;No descriptors, so we just use string.
;Normally: key points to physical module address and is not changed by CTS.
;Caution: assignment does not imply exclusive use of logical name or physical address.
;-
KEY=MODULE
end
