
from MDSplus import Data
MC = __import__('MARTE2_COMPONENT', globals())


@MC.BUILDER('EPICS::EPICSCAClient', MC.MARTE2_COMPONENT.MODE_INTERFACE)
class MARTE2_EPICSCA(MC.MARTE2_COMPONENT):
    parameters = [
        {'name':'+PV1', 'type':'structure', 'value': [
            {'name':'Class', 'type':'string', 'value':'EPICS::EPICSPV'},
            {'name':'PVName', 'type':'string', 'value':'PV1'},
            {'name':'PVType', 'type':'string', 'value':'int32'},
            {'name':'NumberOfElements', 'type':'int32', 'value':1},
            {'name':'Event', 'type':'structure', 'value':[
                {'name':'Destination','type':'string', 'value':'GAMDest1'},
                {'name':'PVValue','type':'string', 'value':'Parameter'},
                {'name':'Function','type':'string', 'value':'UpdateSetPoint'},
            ]},
        ]},
        {'name':'+PV2', 'type':'structure', 'value': [
            {'name':'Class', 'type':'string', 'value':'EPICS::EPICSPV'},
            {'name':'PVName', 'type':'string', 'value':'PV2'},
            {'name':'PVType', 'type':'string', 'value':'int32'},
            {'name':'NumberOfElements', 'type':'int32', 'value':1},
            {'name':'Event', 'type':'structure', 'value':[
                {'name':'Destination','type':'string', 'value':'GAMDest1'},
                {'name':'PVValue','type':'string', 'value':'Parameter'},
                {'name':'Function','type':'string', 'value':'UpdateSetPoint'},
            ]},
        ]},
        {'name':'+PV3', 'type':'structure', 'value': [
            {'name':'Class', 'type':'string', 'value':'EPICS::EPICSPV'},
            {'name':'PVName', 'type':'string', 'value':'PV3'},
            {'name':'PVType', 'type':'string', 'value':'int32'},
            {'name':'NumberOfElements', 'type':'int32', 'value':1},
            {'name':'Event', 'type':'structure', 'value':[
                {'name':'Destination','type':'string', 'value':'GAMDest1'},
                {'name':'PVValue','type':'string', 'value':'Parameter'},
                {'name':'Function','type':'string', 'value':'UpdateSetPoint'},
            ]},
        ]},
        {'name':'+PV4', 'type':'structure', 'value': [
            {'name':'Class', 'type':'string', 'value':'EPICS::EPICSPV'},
            {'name':'PVName', 'type':'string', 'value':'PV4'},
            {'name':'PVType', 'type':'string', 'value':'int32'},
            {'name':'NumberOfElements', 'type':'int32', 'value':1},
            {'name':'Event', 'type':'structure', 'value':[
                {'name':'Destination','type':'string', 'value':'GAMDest1'},
                {'name':'PVValue','type':'string', 'value':'Parameter'},
                {'name':'Function','type':'string', 'value':'UpdateSetPoint'},
            ]},
        ]}
    ]
    parts = []