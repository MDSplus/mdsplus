
MC = __import__('MARTE2_COMPONENT', globals())


@MC.BUILDER('OPCUA::OPCUAServer', MC.MARTE2_COMPONENT.MODE_INTERFACE)
class MARTE2_OPCUA_SERVER(MC.MARTE2_COMPONENT):
    parameters = [
        {'name': 'Port', 'type':'int32', 'value': 4840},
        {'name': 'CPUMask', 'type':'int32', 'value': 255},
        {'name': 'Authentication', 'type':'string', 'value': 'None'},
        {'name':'AddressSpace', 'type':'structure', 'value': [
            {'name': 'ciccio', 'type': 'structure', 'value': [
                {'name': 'Type', 'type': 'string'}
            ]
            },
            {'name': 'bombo', 'type': 'structure', 'value': [
                {'name': 'Type', 'type': 'string'}
            ]
            },
            {'name': '', 'type': 'structure', 'value': [
                {'name': 'Type', 'type': 'string'}
            ]
            },
           {'name': '', 'type': 'structure', 'value': [
                {'name': 'Type', 'type': 'string'}
            ]
            },
           {'name': '', 'type': 'structure', 'value': [
                {'name': 'Type', 'type': 'string'}
            ]
            },
           {'name': '', 'type': 'structure', 'value': [
                {'name': 'Type', 'type': 'string'}
            ]
            },
           {'name': '', 'type': 'structure', 'value': [
                {'name': 'Type', 'type': 'string'}
            ]
            },
           {'name': '', 'type': 'structure', 'value': [
                {'name': 'Type', 'type': 'string'}
            ]
            },
           {'name': '', 'type': 'structure', 'value': [
                {'name': 'Type', 'type': 'string'}
            ]
            },
           {'name': '', 'type': 'structure', 'value': [
                {'name': 'Type', 'type': 'string'}
            ]
            },
           {'name': '', 'type': 'structure', 'value': [
                {'name': 'Type', 'type': 'string'}
            ]
            },
           {'name': '', 'type': 'structure', 'value': [
                {'name': 'Type', 'type': 'string'}
            ]
            },
           {'name': '', 'type': 'structure', 'value': [
                {'name': 'Type', 'type': 'string'}
            ]
            },
           {'name': '', 'type': 'structure', 'value': [
                {'name': 'Type', 'type': 'string'}
            ]
            },
           {'name': '', 'type': 'structure', 'value': [
                {'name': 'Type', 'type': 'string'}
            ]
            },
           {'name': '', 'type': 'structure', 'value': [
                {'name': 'Type', 'type': 'string'}
            ]
            },
         ]
        }
    ]
    parts = []

