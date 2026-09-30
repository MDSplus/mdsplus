
from dataclasses import dataclass, field
from enum import Enum
from typing import Optional, List
from ctypes import LittleEndianStructure, Union, c_int32, c_uint32

class GlobalDefaultsBits(LittleEndianStructure):
    _fields_ = [
        ("update",        c_int32, 1),
        ("x_grid_labels", c_int32, 1),
        ("y_grid_labels", c_int32, 1),
        ("show_mode",     c_int32, 1),
        ("step_plot",     c_int32, 1),
        ("x_grid_lines",  c_int32, 1),
        ("y_grid_lines",  c_int32, 1),
        ("experiment",    c_int32, 1),
        ("shot",          c_int32, 1),
        ("default_node",  c_int32, 1),
        ("x_label",       c_int32, 1),
        ("y_label",       c_int32, 1),
        ("xmin",          c_int32, 1),
        ("xmax",          c_int32, 1),
        ("ymin",          c_int32, 1),
        ("ymax",          c_int32, 1),
        ("title",         c_int32, 1),
        ("event",         c_int32, 1),
        ("print_title",   c_int32, 1),
        ("pad_label",     c_int32, 1),
    ]

class GlobalDefaults(Union):
    _fields_ = [
        ("flags", GlobalDefaultsBits),
        ("raw",   c_int32),
    ]

    def __repr__(self):
        return repr({
            "update":        self.flags.update,
            "x_grid_labels": self.flags.x_grid_labels,
            "y_grid_labels": self.flags.y_grid_labels,
            "show_mode":     self.flags.show_mode,
            "step_plot":     self.flags.step_plot,
            "x_grid_lines":  self.flags.x_grid_lines,
            "y_grid_lines":  self.flags.y_grid_lines,
            "experiment":    self.flags.experiment,
            "shot":          self.flags.shot,
            "default_node":  self.flags.default_node,
            "x_label":       self.flags.x_label,
            "y_label":       self.flags.y_label,
            "xmin":          self.flags.xmin,
            "xmax":          self.flags.xmax,
            "ymin":          self.flags.ymin,
            "ymax":          self.flags.ymax,
            "title":         self.flags.title,
            "event":         self.flags.event,
            "print_title":   self.flags.print_title,
            "pad_label":     self.flags.pad_label,
        })

class ScopeSignalMode1D(Enum):
    LINE    = 'Line'
    NO_LINE = 'Noline'
    STEP    = 'Step'

class ScopeSignalMode2D(Enum):
    XZ_Y    = 'xz(y)'
    YZ_X    = 'yz(x)'
    Contour = 'Contour'
    Image   = 'Image'

class ScopeSignalColor(Enum):
    BLACK   = 0
    BLUE    = 1
    CYAN    = 2
    GREEN   = 3
    MAGENTA = 4
    ORANGE  = 5
    PINK    = 6
    RED     = 7
    YELLOW  = 8

class ScopeSignalMarker(Enum):
    NONE     = 0
    SQUARE   = 1
    CIRCLE   = 2
    CROSS    = 3
    TRIANGLE = 4
    POINT    = 5

@dataclass
class ScopeSignal:
    # jScope + dwscope
    x: Optional[str] = None
    y: Optional[str] = None
    label: Optional[str] = None

    # jScope
    mode: ScopeSignalMode1D = ScopeSignalMode1D.LINE
    mode2D: ScopeSignalMode2D = ScopeSignalMode2D.XZ_Y
    color: ScopeSignalColor = ScopeSignalColor.BLACK
    marker: ScopeSignalMarker = ScopeSignalMarker.NONE
    # step_marker ?

    def write(self, file, prefix, index=1, dwscope=False):
        if dwscope:
            if self.x is not None:
                file.write(f'{prefix}.x: {self.x}\n')

            if self.y is not None:
                file.write(f'{prefix}.y: {self.y}\n')

            if self.label is not None:
                file.write(f'{prefix}.label: {self.label}\n')

        else:
            if self.x is not None:
                file.write(f'{prefix}.x_expr_{index}: {self.x}\n')

            if self.y is not None:
                file.write(f'{prefix}.y_expr_{index}: {self.y}\n')

            if self.label is not None:
                file.write(f'{prefix}.label_{index}: {self.label}\n')

            if self.mode is not None:
                file.write(f'{prefix}.mode_1D_{index}_1: {self.mode.value}\n')

            if self.mode2D is not None:
                file.write(f'{prefix}.mode_2D_{index}_1: {self.mode2D.value}\n')

            if self.color is not None:
                file.write(f'{prefix}.color_{index}_1: {self.color.value}\n')

            if self.marker is not None:
                file.write(f'{prefix}.marker_{index}_1: {self.marker.value}\n')

@dataclass
class ScopePlot:
    # dwscope
    # The signal data is stored in signals[0]

    # jScope + dwscope
    title: Optional[str] = None
    title_event: Optional[str] = None
    experiment: Optional[str] = None
    shot: Optional[str] = None
    xmin: Optional[str] = None
    xmax: Optional[str] = None
    ymin: Optional[str] = None
    ymax: Optional[str] = None
    height: Optional[int] = None
    event: Optional[str] = None
    default_node: Optional[str] = None
    global_defaults: GlobalDefaults = field(default_factory=GlobalDefaults)
    # vertical_offset

    # jScope
    signals: list[ScopeSignal] = field(default_factory=lambda: [ ScopeSignal() ])

    def set(self, key, value):

        # dwscope
        if key == 'x':
            self.signals[0].x = value

        if key == 'y':
            self.signals[0].y = value

        if key == 'label':
            self.signals[0].label = value

        # jScope + dwscope
        elif key == 'height':
            self.height = int(value)

        elif key == 'experiment':
            self.experiment = value

        elif key == 'shot':
            self.shot = value

        elif key == 'default_node':
            self.default_node = value

        elif key == 'xmin':
            self.xmin = value

        elif key == 'xmax':
            self.xmax = value

        elif key == 'ymin':
            self.ymin = value

        elif key == 'ymax':
            self.ymax = value

        elif key == 'event':
            self.event = value

        elif key == 'title':
            self.title = value

        elif key == 'title_event':
            self.title_event = value

        elif key == 'global_defaults':
            self.global_defaults.raw = int(value)

        # jScope
        elif key.startswith('num_expr'):
            num_expr = int(value)
            for i in range(num_expr):
                if i <= len(self.signals):
                    self.signals.append(ScopeSignal())

        elif key.startswith('x_expr_'):
            index = int(key.removeprefix('x_expr_')) - 1
            self.signals[index].x = value

        elif key.startswith('y_expr_'):
            index = int(key.removeprefix('y_expr_')) - 1
            self.signals[index].y = value

        elif key.startswith('label_'):
            index = int(key.removeprefix('label_')) - 1
            self.signals[index].label = value

        elif key.startswith('mode_1D_'):
            index = int(key.removeprefix('mode_1D_').removesuffix('_1')) - 1
            self.signals[index].mode = ScopeSignalMode1D(value)

        elif key.startswith('mode_2D_'):
            index = int(key.removeprefix('mode_2D_').removesuffix('_1')) - 1
            self.signals[index].mode2D = ScopeSignalMode2D(value)

        elif key.startswith('color_'):
            index = int(key.removeprefix('color_').removesuffix('_1')) - 1
            self.signals[index].color = ScopeSignalColor(int(value))

        elif key.startswith('marker_'):
            index = int(key.removeprefix('marker_').removesuffix('_1')) - 1
            self.signals[index].marker = ScopeSignalMarker(int(value))

    def write(self, file, prefix, dwscope=False):
        if self.height is not None:
            file.write(f'{prefix}.height: {self.height}\n')

        if self.experiment is not None:
            file.write(f'{prefix}.experiment: {self.experiment}\n')

        if self.shot is not None:
            file.write(f'{prefix}.shot: {self.shot}\n')

        if self.default_node is not None:
            file.write(f'{prefix}.default_node: {self.default_node}\n')

        if self.xmin is not None:
            file.write(f'{prefix}.xmin: {self.xmin}\n')

        if self.xmax is not None:
            file.write(f'{prefix}.xmax: {self.xmax}\n')

        if self.ymin is not None:
            file.write(f'{prefix}.ymin: {self.ymin}\n')

        if self.ymax is not None:
            file.write(f'{prefix}.ymax: {self.ymax}\n')

        if self.event is not None:
            file.write(f'{prefix}.event: {self.event}\n')

        if self.title is not None:
            file.write(f'{prefix}.title: {self.title}\n')

        self.global_defaults.experiment = (self.experiment is None)
        self.global_defaults.shot = (self.shot is None)
        self.global_defaults.default_node = (self.default_node is None)
        self.global_defaults.xmin = (self.xmin is None)
        self.global_defaults.xmax = (self.xmax is None)
        self.global_defaults.ymin = (self.ymin is None)
        self.global_defaults.ymax = (self.ymax is None)
        self.global_defaults.title = (self.title is None)
        self.global_defaults.event = (self.event is None)

        if dwscope:
            self.global_defaults.pad_label = (self.signals[0].label is None)
            file.write(f'{prefix}.global_defaults: {c_int32(self.global_defaults.raw).value}\n')
        else:
            file.write(f'{prefix}.global_defaults: {self.global_defaults.raw}\n')

        if not dwscope:
            file.write(f'{prefix}.num_shot: 1\n') # TODO: ?
            file.write(f'{prefix}.num_expr: {len(self.signals)}\n')

        for i, signal in enumerate(self.signals):
            signal.write(file, prefix, i + 1, dwscope)

@dataclass
class ScopeGeometry:
    x: int
    y: int
    width: int
    height: int

@dataclass
class Scope:
    # jScope + dwscope
    title: str = '"MDSplus Scope"'
    title_event: Optional[str] = None
    geometry: ScopeGeometry = field(default_factory=lambda: ScopeGeometry(600, 500, 200, 200))
    defaults: ScopePlot = field(default_factory=ScopePlot)
    plots: list[list[ScopePlot]] = field(default_factory=list)
    vpanes: list[int] = field(default_factory=list)

    # jScope
    data_server_name: str = 'Local'
    data_server_class: str = 'MdsDataProviderLocal'
    reversed: bool = False

    @staticmethod
    def load(filename):
        try:
            # If we open a .dat file that isn't a scope file,
            content = open(filename, 'rt').read()

        except UnicodeDecodeError:
            # The file is not a text file, so definitely not a scope definition
            print(f'Unable to load {filename}. it is not a scope definition.')
            return None

        except FileNotFoundError:
            print(f'Unable to open {filename}, file not found.')
            return None

        # TODO: Improve
        if not 'Scope' in content:
            print(f'Unable to load {filename}. it is not a scope definition.')
            return None

        scope = Scope()

        lines = content.splitlines()
        lines = [ line.strip() for line in lines if line != '' ]

        for line in lines:
            parts = line.split(': ', maxsplit=1)
            if len(parts) < 2:
                continue

            key, value = parts
            key = key.removeprefix('Scope.').split('.')

            if key[0] == 'geometry':
                size, x, y = value.split('+')
                width, height = size.split('x')
                scope.geometry = ScopeGeometry(int(x), int(y), int(width), int(height))

            elif key[0] == 'title':
                scope.title = value

            elif key[0] == 'title_event':
                scope.title_event = value

            elif key[0] == 'global_1_1':
                scope.defaults.set(key[1], value)

            elif key[0] == 'columns':
                num_cols = int(value)
                for i in range(num_cols):
                    if i <= len(scope.plots):
                        scope.plots.append([])

            elif key[0].startswith('rows_in_column_'):
                col = int(key[0].removeprefix('rows_in_column_')) - 1
                num_rows = int(value)
                for i in range(num_rows):
                    if i <= len(scope.plots[col]):
                        scope.plots[col].append(ScopePlot())

            elif key[0].startswith('plot_'):
                row, col = key[0].removeprefix('plot_').split('_')
                row = int(row) - 1
                col = int(col) - 1
                scope.plots[col][row].set(key[1], value)

            elif key[0].startswith('vpane_'):
                i = int(key[0].removeprefix('vpane_'))
                if i != len(scope.vpanes) + 1:
                    print('Invalid vpane definition order')
                scope.vpanes.append(int(value))

            else:
                print('Unknown', line)

        return scope

    def save(self, filename):
        if filename.endswith('.dat'):
            return self.save_dwscope(filename)
        elif filename.endswith('.jscp'):
            return self.save_jScope(filename)
        
        raise ValueError(f'"{filename}" is not a dwscope (.dat) or jScope (.jscp) file')

    def save_dwscope(self, filename):
        file = open(filename, 'wt')

        file.write(f'Scope.geometry: {self.geometry.width}x{self.geometry.height}+{self.geometry.x}+{self.geometry.y}\n')
        file.write(f'Scope.title: {self.title}\n')

        if self.title_event is not None:
            file.write(f'Scope.title_event: {self.title_event}\n')

        file.write(f'Scope.columns: {len(self.plots)}\n')

        self.defaults.write(file, f'Scope.global_1_1', True)

        for col, column in enumerate(self.plots):
            file.write(f'Scope.rows_in_column_{col + 1}: {len(column)}\n')
            for row, plot in enumerate(column):
                file.write('\n')
                plot.write(file, f'Scope.plot_{row + 1}_{col + 1}', True)

        for i, vpane in enumerate(self.vpanes):
            file.write(f'Scope.vpane_{i + 1}: {vpane}\n')

        file.close()

    def save_jScope(self, filename):
        file = open(filename, 'wt')

        file.write(f'Scope.geometry: {self.geometry.width}x{self.geometry.height}+{self.geometry.x}+{self.geometry.y}\n')
        file.write(f'Scope.title: {self.title}\n')

        if self.title_event is not None:
            file.write(f'Scope.title_event: {self.title_event}\n')

        file.write('\n')

        COLORS = [
            'Black,java.awt.Color[r=0,g=0,b=0]',
            'Blue,java.awt.Color[r=0,g=0,b=255]',
            'Cyan,java.awt.Color[r=0,g=255,b=255]',
            'Green,java.awt.Color[r=0,g=255,b=0]',
            'Magenta,java.awt.Color[r=255,g=0,b=255]',
            'Orange,java.awt.Color[r=255,g=200,b=0]',
            'Pink,java.awt.Color[r=255,g=175,b=175]',
            'Red,java.awt.Color[r=255,g=0,b=0]',
            'Yellow,java.awt.Color[r=255,g=255,b=0]',
        ]

        for i, color in enumerate(COLORS):
            file.write(f'Scope.color_{i}: {color}\n')

        file.write('\n')

        file.write(f'Scope.data_server_name: {self.data_server_name}\n')
        file.write(f'Scope.data_server_class: {self.data_server_class}\n')
        # file.write(f'Scope.reversed: {}\n')

        file.write('\n')

        self.defaults.write(file, f'Scope.global_1_1', False)

        file.write('\n')
        
        file.write(f'Scope.columns: {len(self.plots)}\n')

        for col, column in enumerate(self.plots):
            file.write(f'Scope.rows_in_column_{col + 1}: {len(column)}\n')
            for row, plot in enumerate(column):
                file.write('\n')
                plot.write(file, f'Scope.plot_{row + 1}_{col + 1}', False)

        for i, vpane in enumerate(self.vpanes):
            file.write(f'Scope.vpane_{i + 1}: {vpane}\n')

        file.close()