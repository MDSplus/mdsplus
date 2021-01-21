from MDSplus import mdsExceptions, Device


class SPIDER_PARAMS(Device):
    """SPIDER Experiment Parameters"""
    parts = [{'path': ':COMMENT', 'type': 'text'},
             {'path': ':R_ISBP', 'type': 'NUMERIC',
                 'valueExpr': 'Data.compile("build_param(build_with_units(0.420,\'Ohm\'),\'Bias Plate parallel resistance\',*)")'},
             {'path': ':R_ISBI', 'type': 'NUMERIC',
                 'valueExpr': 'Data.compile("build_param(build_with_units(0.420,\'Ohm\'),\'Bias  parallel resistance\',*)")'},
             {'path': ':CS1',    'type': 'NUMERIC',
              'valueExpr': 'Data.compile("build_param(build_with_units(10.e-9,\'F\'),\'Series capacitor DR1 and DR2\',*)")'},
             {'path': ':CP1',    'type': 'NUMERIC',
              'valueExpr': 'Data.compile("build_param(build_with_units(10.e-9,\'F\'),\'Parallel capacitor DR1 and DR2\',*)")'},
             {'path': ':CS2',    'type': 'NUMERIC',
              'valueExpr': 'Data.compile("build_param(build_with_units(10.e-9,\'F\'),\'Series capacitor DR7 and DR7\',*)")'},
             {'path': ':CP2',    'type': 'NUMERIC',
              'valueExpr': 'Data.compile("build_param(build_with_units(10.e-9,\'F\'),\'Parallel capacitor DR7 and DR8\',*)")'},
             {'path': ':CS3',    'type': 'NUMERIC',
              'valueExpr': 'Data.compile("build_param(build_with_units(10.e-9,\'F\'),\'Series capacitor DR3 and DR4\',*)")'},
             {'path': ':CP3',    'type': 'NUMERIC',
              'valueExpr': 'Data.compile("build_param(build_with_units(10.e-9,\'F\'),\'Parallel capacitor DR3 and DR4\',*)")'},
             {'path': ':CS4',    'type': 'NUMERIC',
              'valueExpr': 'Data.compile("build_param(build_with_units(10.e-9,\'F\'),\'Series capacitor DR5 and DR5\',*)")'},
             {'path': ':CP4',    'type': 'NUMERIC', 'valueExpr': 'Data.compile("build_param(build_with_units(10.e-9,\'F\'),\'Parallel capacitor DR5 and DR6\',*)")'}]
