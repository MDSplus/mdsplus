"""MDSplus acquisition device for a Hyperion SI255 interrogator.

This version keeps the interrogator peak arrays as fixed-width raw records and
also routes individual wavelengths to DIRECT, HYPERION_MUX or HYPERION_FOS
outputs.  MUX and FOS devices are passive MDSplus models: all acquisition,
timestamping and mechanical-switch coordination is performed here.
"""

import math
import socket
import threading
import time
import traceback
from numbers import Integral, Real

from MDSplus import Data, Device, Float32, Int64, Tree, mdsExceptions


class HYPERION_SI255(Device):
    """Hyperion SI255 peak acquisition with wavelength-based sensor routing."""

    CHANNEL_COUNT = 8
    MUX_INPUT_COUNT = 10
    FOS_INPUT_COUNT = 32
    # Fixed width of optional PEAK_RAW records; extra detected peaks are still
    # used for matching and reported in the diagnostic log.
    RAW_PEAK_CAPACITY = 16

    parts = [
        {'path': ':NAME', 'type': 'text'},
        {'path': ':COMMENT', 'type': 'text'},
        {'path': ':IP_ADDR', 'type': 'text', 'value': '192.168.111.54'},
        {'path': ':ACQ_MODE', 'type': 'text', 'value': 'PEAK'},
        {'path': ':ACQ_FREQ', 'type': 'numeric', 'value': 1.0},
        {'path': ':WL_TOLERANCE', 'type': 'numeric', 'value': 1.0},
        {'path': ':TIME0', 'type': 'numeric', 'value': 0},
    ]

    for i in range(1, CHANNEL_COUNT + 1):
        parts.extend([
            {'path': '.CHANNEL_%02d' % i, 'type': 'structure'},
            {'path': '.CHANNEL_%02d:INPUT_MODE' % i, 'type': 'text', 'value': 'DIRECT'},
            {'path': '.CHANNEL_%02d:INPUT_PATH' % i, 'type': 'any'},
            {'path': '.CHANNEL_%02d:NOMINAL_WL' % i, 'type': 'numeric', 'value': 0.0},
            {'path': '.CHANNEL_%02d:PEAK' % i, 'type': 'signal'},
            {'path': '.CHANNEL_%02d:PEAK_RTIME' % i, 'type': 'numeric', 'valueExpr': ('Data.compile("pvResample($1,,,,$2)", head.channel_%02d_peak, head.time0)' % i)},
            {'path': '.CHANNEL_%02d:PEAK_RAW' % i, 'type': 'signal'},
        ])
    del i
    parts.append({'path': ':INIT_ACT', 'type': 'action',
                  'valueExpr': "Action(Dispatch('FEDE_SERVER','PULSE_PREPARATION',50,None),Method(None,'init',head))",
                  'options': ('no_write_shot',)})
    parts.append({'path': ':START_ACT', 'type': 'action',
                  'valueExpr': "Action(Dispatch('FEDE_SERVER','INIT',50,None),Method(None,'startAcquisition',head))",
                  'options': ('no_write_shot',)})
    parts.append({'path': ':STOP_ACT', 'type': 'action',
                  'valueExpr': "Action(Dispatch('FEDE_SERVER','STORE',50,None),Method(None,'stopAcquisition',head))",
                  'options': ('no_write_shot',)})

    handles = {}
    initialized_contexts = {}
    workers = {}

    def debugPrint(self, message='', detail=''):
        print('------ DEBUG %s: %s %s' % (self.name, message, detail))

    class AsynchStore(threading.Thread):
        POSIX_TIME_AT_EPICS_EPOCH = 631152000000000000
        SEGMENT_SIZE = 1000
        INTEGER_SCALAR_TYPES = {
            'Int8', 'Int16', 'Int32', 'Int64',
            'Uint8', 'Uint16', 'Uint32', 'Uint64',
        }
        FLOAT_SCALAR_TYPES = {'Float32', 'Float64'}

        def configure(self, tree_name, shot, device_path, handle):
            self.device = None
            self.tree_name = tree_name
            self.shot = shot
            self.device_path = device_path
            self.handle = handle
            self.stop_event = threading.Event()
            self.setup_done = threading.Event()
            self.setup_error = None
            self.fos_connections = []

        @staticmethod
        def _as_text(value):
            if isinstance(value, bytes):
                return value.decode('utf-8')
            return str(value)

        @classmethod
        def _reference_path(cls, value):
            for method_name in ('getFullPath', 'getPath'):
                method = getattr(value, method_name, None)
                if method is not None:
                    return cls._as_text(method())
            return cls._as_text(value)

        @staticmethod
        def _read_scalar(node, description):
            try:
                return float(node.getData())
            except Exception as exc:
                raise ValueError('Missing or invalid %s: %s' % (description, exc))

        @classmethod
        def _literal_time0(cls, record, description):
            """Return an integer for a literal scalar, or None for expressions.

            TreeNode.getData() returns the stored record without evaluating it.
            A literal zero therefore selects the first-sample fallback, while a
            path or any other expression is preserved even when it currently
            evaluates to zero.
            """
            class_name = record.__class__.__name__
            if (isinstance(record, Integral)
                    or class_name in cls.INTEGER_SCALAR_TYPES):
                try:
                    return int(record)
                except Exception as exc:
                    raise ValueError(
                        'Invalid literal %s: %s' % (description, exc))

            if (isinstance(record, Real)
                    or class_name in cls.FLOAT_SCALAR_TYPES):
                try:
                    value = float(record)
                except Exception as exc:
                    raise ValueError(
                        'Invalid literal %s: %s' % (description, exc))
                if not math.isfinite(value) or value != int(value):
                    raise ValueError(
                        '%s must contain an integer nanosecond value'
                        % description)
                return int(value)

            return None

        @staticmethod
        def match_peaks(detected_peaks, nominal_wavelengths, tolerance):
            """Return one value per nominal wavelength using one-to-one matching.

            Candidate pairs are processed from the smallest wavelength error to
            the largest.  Configuration validation prevents overlapping MUX
            windows, so the result is deterministic for simultaneous sensors.
            """
            detected = [float(value) for value in detected_peaks]
            nominal = [float(value) for value in nominal_wavelengths]
            # Zero is not a valid wavelength and is handled correctly by
            # jScope, unlike NaN.
            matched = [0.0] * len(nominal)
            assigned_sensors = set()
            assigned_peaks = set()
            candidates = []

            for sensor_index, nominal_value in enumerate(nominal):
                for peak_index, measured_value in enumerate(detected):
                    distance = abs(measured_value - nominal_value)
                    if distance <= tolerance:
                        candidates.append((distance, sensor_index, peak_index))

            for _, sensor_index, peak_index in sorted(candidates):
                if sensor_index in assigned_sensors or peak_index in assigned_peaks:
                    continue
                matched[sensor_index] = detected[peak_index]
                assigned_sensors.add(sensor_index)
                assigned_peaks.add(peak_index)

            unmatched_peaks = [
                detected[index]
                for index in range(len(detected))
                if index not in assigned_peaks
            ]
            return matched, unmatched_peaks

        def _node(self, tree, suffix):
            return tree.getNode(self.device_path + suffix)

        def _external_node(self, tree, device_path, suffix):
            return tree.getNode(device_path + suffix)

        def _read_input_path(self, tree, channel):
            path_node = self._node(
                tree, '.CHANNEL_%02d:INPUT_PATH' % channel)
            try:
                referenced = path_node.getData()
            except Exception as exc:
                raise ValueError(
                    'Missing INPUT_PATH on channel %02d: %s' % (channel, exc))

            path = self._reference_path(referenced)
            try:
                target = tree.getNode(path)
            except Exception as exc:
                raise ValueError(
                    'Invalid INPUT_PATH on channel %02d (%s): %s'
                    % (channel, path, exc))
            return self._reference_path(target)

        def _read_external_inputs(self, tree, target_path, input_count):
            endpoints = []
            for input_index in range(1, input_count + 1):
                input_node = self._external_node(
                    tree, target_path,
                    '.INPUT_%02d' % input_index)
                if not input_node.isOn():
                    continue
                peak_node = self._external_node(
                    tree, target_path,
                    '.INPUT_%02d:PEAK' % input_index)
                nominal_node = self._external_node(
                    tree, target_path,
                    '.INPUT_%02d:NOMINAL_WL' % input_index)
                try:
                    nominal = self._read_scalar(
                        nominal_node,
                        '%s.INPUT_%02d:NOMINAL_WL'
                        % (target_path, input_index))
                except ValueError as exc:
                    self.device.debugPrint(
                        'Skipping input without nominal wavelength',
                        '%s INPUT_%02d: %s'
                        % (target_path, input_index, exc))
                    continue
                if nominal == 0.0:
                    continue
                if nominal < 0.0:
                    self.device.debugPrint(
                        'Skipping input with invalid nominal wavelength',
                        '%s INPUT_%02d: %.6f nm'
                        % (target_path, input_index, nominal))
                    continue
                endpoints.append({
                    'input_id': input_index,
                    'nominal': nominal,
                    'peak_node': peak_node,
                })
            return endpoints

        def _validate_mux_windows(self, channel, endpoints):
            ordered = sorted(
                (endpoint['nominal'], endpoint['input_id'])
                for endpoint in endpoints)
            for previous, current in zip(ordered, ordered[1:]):
                if current[0] - previous[0] <= 2.0 * self.tolerance:
                    raise ValueError(
                        'Overlapping wavelength windows on Hyperion channel %02d: '
                        'MUX inputs %02d (%.6f nm) and %02d (%.6f nm), '
                        'tolerance %.6f nm'
                        % (
                            channel,
                            previous[1], previous[0],
                            current[1], current[0],
                            self.tolerance,
                        ))

        def _connect_fos(self, tree, target_path):
            ip_addr = self._as_text(
                self._external_node(tree, target_path, ':IP_ADDR').getData())
            port = int(self._external_node(
                tree, target_path, ':PORT').getData())
            settle_time = self._read_scalar(
                self._external_node(tree, target_path, ':SETTLE_TIME'),
                target_path + ':SETTLE_TIME')
            read_timeout = self._read_scalar(
                self._external_node(tree, target_path, ':READ_TIMEOUT'),
                target_path + ':READ_TIMEOUT')

            if settle_time < 0:
                raise ValueError('SETTLE_TIME must be >= 0 on %s' % target_path)
            if read_timeout <= 0:
                raise ValueError('READ_TIMEOUT must be > 0 on %s' % target_path)

            connection = socket.create_connection(
                (ip_addr, port), timeout=read_timeout)
            connection.settimeout(read_timeout)
            self.fos_connections.append(connection)
            self.device.debugPrint(
                'Connected to FOS', '%s:%d' % (ip_addr, port))
            return connection, settle_time

        def _select_fos_input(self, config, input_id):
            command = b'*SW' + str(input_id).zfill(3).encode('ascii') + b'\n'
            config['connection'].sendall(command)
            self.device.debugPrint(
                'FOS input selected',
                '%s INPUT_%02d' % (config['target_path'], input_id))
            if config['settle_time'] > 0:
                self.stop_event.wait(config['settle_time'])

        def _load_configuration(self, tree):
            configurations = []
            fos_paths = set()

            for channel in range(1, self.device.CHANNEL_COUNT + 1):
                channel_node = self._node(
                    tree, '.CHANNEL_%02d' % channel)
                if not channel_node.isOn():
                    continue

                raw_node = self._node(
                    tree, '.CHANNEL_%02d:PEAK_RAW' % channel)

                mode_value = self._node(
                    tree, '.CHANNEL_%02d:INPUT_MODE' % channel).getData()
                mode = self._as_text(mode_value).strip().upper()
                if mode not in ('DIRECT', 'MUX', 'FOS'):
                    raise ValueError(
                        'Invalid INPUT_MODE %r on channel %02d'
                        % (mode, channel))

                config = {
                    'channel': channel,
                    'mode': mode,
                    'raw_node': raw_node,
                    'last_diagnostic': None,
                }

                if mode == 'DIRECT':
                    peak_node = self._node(
                        tree, '.CHANNEL_%02d:PEAK' % channel)
                    try:
                        nominal = self._read_scalar(
                            self._node(
                                tree,
                                '.CHANNEL_%02d:NOMINAL_WL' % channel),
                            'CHANNEL_%02d:NOMINAL_WL' % channel)
                    except ValueError as exc:
                        self.device.debugPrint(
                            'Skipping DIRECT channel without nominal wavelength',
                            'channel=%02d: %s' % (channel, exc))
                        continue
                    if nominal == 0.0:
                        continue
                    if nominal < 0.0:
                        self.device.debugPrint(
                            'Skipping DIRECT channel with invalid nominal wavelength',
                            'channel=%02d nominal=%.6f nm'
                            % (channel, nominal))
                        continue
                    config['endpoints'] = [{
                        'input_id': 0,
                        'nominal': nominal,
                        'peak_node': peak_node,
                    }]

                elif mode == 'MUX':
                    target_path = self._read_input_path(tree, channel)
                    endpoints = self._read_external_inputs(
                        tree, target_path, self.device.MUX_INPUT_COUNT)
                    if not endpoints:
                        self.device.debugPrint(
                            'Skipping unconfigured MUX channel',
                            'channel=%02d path=%s'
                            % (channel, target_path))
                        continue
                    self._validate_mux_windows(channel, endpoints)
                    config['target_path'] = target_path
                    config['endpoints'] = endpoints

                else:
                    target_path = self._read_input_path(tree, channel)
                    if target_path in fos_paths:
                        raise ValueError(
                            'FOS %s is referenced by more than one Hyperion channel'
                            % target_path)
                    endpoints = self._read_external_inputs(
                        tree, target_path, self.device.FOS_INPUT_COUNT)
                    if not endpoints:
                        self.device.debugPrint(
                            'Skipping unconfigured FOS channel',
                            'channel=%02d path=%s'
                            % (channel, target_path))
                        continue
                    fos_paths.add(target_path)
                    connection, settle_time = self._connect_fos(
                        tree, target_path)
                    try:
                        config.update({
                            'target_path': target_path,
                            'endpoints': endpoints,
                            'connection': connection,
                            'settle_time': settle_time,
                            'current_index': 0,
                        })
                        self._select_fos_input(
                            config, endpoints[0]['input_id'])
                    except Exception:
                        raise

                configurations.append(config)

            if not configurations:
                raise ValueError('No configured Hyperion acquisition channels')
            return configurations

        def _initialize_time0(self, tree, configurations, timestamp):
            """Configure the time origin without overwriting expressions.

            pvResample expects TIME0 in the same units as the absolute signal
            dimension and performs the conversion to relative seconds itself.
            A literal zero in the master selects the first acquired timestamp;
            any non-zero literal, path or expression is preserved. Child
            devices left at literal zero are linked dynamically to master
            TIME0, while an existing child value or expression is preserved.
            """
            master_path = self.device_path + ':TIME0'
            master_node = self._node(tree, ':TIME0')
            try:
                master_record = master_node.getData()
            except Exception as exc:
                raise ValueError(
                    'Cannot read TIME0 %s: %s' % (master_path, exc))

            master_literal = self._literal_time0(
                master_record, 'TIME0 %s' % master_path)
            if master_literal is not None and master_literal < 0:
                raise ValueError(
                    'TIME0 %s cannot be negative' % master_path)

            if master_literal == 0:
                try:
                    master_node.putData(Int64(timestamp))
                except Exception as exc:
                    raise ValueError(
                        'Cannot write TIME0 %s: %s' % (master_path, exc))
                self.device.debugPrint(
                    'TIME0 first-sample fallback',
                    '%d ns' % int(timestamp))
            elif master_literal is not None:
                self.device.debugPrint(
                    'TIME0 configured literal',
                    '%d ns' % master_literal)
            else:
                self.device.debugPrint(
                    'TIME0 external expression preserved',
                    '%s' % master_record)

            seen_paths = set()
            linked_children = 0
            preserved_children = 0

            for config in configurations:
                target_path = config.get('target_path')
                if target_path is None:
                    continue
                time0_path = target_path + ':TIME0'
                if time0_path in seen_paths:
                    continue
                seen_paths.add(time0_path)
                child_node = self._external_node(
                    tree, target_path, ':TIME0')
                try:
                    child_record = child_node.getData()
                except Exception as exc:
                    raise ValueError(
                        'Cannot read TIME0 %s: %s' % (time0_path, exc))
                child_literal = self._literal_time0(
                    child_record, 'TIME0 %s' % time0_path)
                if child_literal is not None and child_literal < 0:
                    raise ValueError(
                        'TIME0 %s cannot be negative' % time0_path)

                if child_literal == 0:
                    try:
                        reference = tree.tdiCompile(
                            'build_path($)', master_path)
                        child_node.putData(reference)
                    except Exception as exc:
                        raise ValueError(
                            'Cannot link TIME0 %s to %s: %s'
                            % (time0_path, master_path, exc))
                    linked_children += 1
                else:
                    preserved_children += 1
                    self.device.debugPrint(
                        'TIME0 child value/expression preserved',
                        '%s=%s' % (time0_path, child_record))

            self.device.debugPrint(
                'TIME0 child references',
                'linked=%d preserved=%d'
                % (linked_children, preserved_children))

        def _store_raw(self, config, detected, timestamp):
            if not config['raw_node'].isOn():
                return False

            detected_count = len(detected)
            raw_peak_capacity = self.device.RAW_PEAK_CAPACITY
            overflow = detected_count > raw_peak_capacity
            stored = list(detected[:raw_peak_capacity])
            stored.extend(
                [0.0] * (raw_peak_capacity - len(stored)))

            config['raw_node'].putRow(
                self.SEGMENT_SIZE, Float32(stored), Int64(timestamp))
            return overflow

        def _store_matched(self, config, detected, timestamp):
            if config['mode'] == 'FOS':
                endpoint = config['endpoints'][config['current_index']]
                active_endpoints = [endpoint]
            else:
                active_endpoints = config['endpoints']

            nominal = [endpoint['nominal'] for endpoint in active_endpoints]
            matched, unmatched = self.match_peaks(
                detected, nominal, self.tolerance)
            overflow = self._store_raw(config, detected, timestamp)

            matched_count = 0
            for endpoint, matched_value in zip(active_endpoints, matched):
                endpoint['peak_node'].putRow(
                    self.SEGMENT_SIZE,
                    Float32(matched_value),
                    Int64(timestamp))
                if matched_value != 0.0:
                    matched_count += 1

            missing = [
                endpoint
                for endpoint, matched_value in zip(active_endpoints, matched)
                if matched_value == 0.0
            ]

            diagnostic = (
                tuple(endpoint['input_id'] for endpoint in missing),
                len(unmatched), overflow)
            if diagnostic != config['last_diagnostic']:
                if missing or unmatched or overflow:
                    missing_text = ','.join(
                        '%02d(%.6f)'
                        % (endpoint['input_id'], endpoint['nominal'])
                        for endpoint in missing)
                    unmatched_text = ','.join(
                        '%.6f' % value for value in unmatched)
                    self.device.debugPrint(
                        'Peak matching warning',
                        'channel=%02d mode=%s detected=%d expected=%d '
                        'matched=%d unmatched=%d overflow=%s '
                        'missing=[%s] unmatched_wavelengths=[%s]'
                        % (
                            config['channel'], config['mode'],
                            len(detected), len(active_endpoints),
                            matched_count, len(unmatched), overflow,
                            missing_text, unmatched_text,
                        ))
                config['last_diagnostic'] = diagnostic

        def _advance_fos(self, config):
            config['current_index'] = (
                config['current_index'] + 1) % len(config['endpoints'])
            next_input = config['endpoints'][
                config['current_index']]['input_id']
            self._select_fos_input(config, next_input)

        def run(self):
            configurations = []
            tree = None
            loop = None
            try:
                tree = Tree(self.tree_name, self.shot)
                self.device = HYPERION_SI255(
                    tree.getNode(self.device_path))

                try:
                    import asyncio
                except Exception as exc:
                    Data.execute(
                        'DevLogErr($1,$2)', self.device.nid,
                        'Cannot import asyncio library: ' + str(exc))
                    raise mdsExceptions.TclFAILED_ESSENTIAL

                # The synchronous Hyperion API uses asyncio.get_event_loop()
                # internally.  Since this code runs in a worker thread, the
                # thread must own an event loop for the duration of acquisition.
                loop = asyncio.new_event_loop()
                asyncio.set_event_loop(loop)

                # Recover acquisition settings from the device in this
                # thread's tree context instead of copying them from the
                # action instance.
                (
                    self.acq_freq,
                    self.tolerance,
                ) = self.device._configuration_values(self.handle)
                self.device.debugPrint(
                    'Acquisition thread opened tree',
                    '%s %s' % (self.tree_name, self.shot))
                configurations = self._load_configuration(tree)
                time0_initialized = False
                period = 1.0 / self.acq_freq
                self.setup_done.set()
                while not self.stop_event.is_set():
                    cycle_start = time.monotonic()
                    try:
                        peaks = self.handle.peaks
                        timestamp = (
                            int(peaks.header.timestamp_int) * 1000000000
                            + int(peaks.header.timestamp_frac)
                            - self.POSIX_TIME_AT_EPICS_EPOCH)
                    except Exception as exc:
                        self.device.debugPrint(
                            'ERROR reading peaks', str(exc))
                        self.stop_event.wait(period)
                        continue

                    if not time0_initialized:
                        self._initialize_time0(
                            tree, configurations, timestamp)
                        time0_initialized = True

                    for config in configurations:
                        try:
                            detected = [
                                float(value)
                                for value in peaks[config['channel']]
                            ]
                            self._store_matched(
                                config, detected, timestamp)
                            if config['mode'] == 'FOS':
                                self._advance_fos(config)
                        except Exception as exc:
                            self.device.debugPrint(
                                'ERROR storing channel %02d'
                                % config['channel'],
                                '%s\n%s' % (exc, traceback.format_exc()))

                    remaining = period - (time.monotonic() - cycle_start)
                    if remaining > 0:
                        self.stop_event.wait(remaining)

            except Exception as exc:
                self.setup_error = '%s\n%s' % (exc, traceback.format_exc())
                if self.device is not None:
                    try:
                        self.device.debugPrint(
                            'Acquisition thread failed', self.setup_error)
                    except Exception:
                        pass
                self.setup_done.set()
            finally:
                for connection in self.fos_connections:
                    try:
                        connection.shutdown(socket.SHUT_RDWR)
                    except Exception:
                        pass
                    try:
                        connection.close()
                    except Exception:
                        pass

                if loop is not None:
                    try:
                        asyncio.set_event_loop(None)
                    except Exception:
                        pass
                    try:
                        loop.close()
                    except Exception:
                        pass

                self.setup_done.set()
                if self.device is not None:
                    try:
                        self.device.debugPrint('End acquisition thread')
                    except Exception:
                        pass

                if self.device is not None:
                    nid = self.device.nid
                    HYPERION_SI255.handles.pop(
                        nid, None)
                    HYPERION_SI255.initialized_contexts.pop(
                        nid, None)
                    current_worker = HYPERION_SI255.workers.get(
                        nid)
                    if current_worker is self:
                        HYPERION_SI255.workers.pop(
                            nid, None)

                if tree is not None:
                    try:
                        tree.close()
                    except Exception:
                        pass

                self.fos_connections = []
                self.handle = None
                self.device = None

        def stop(self):
            self.stop_event.set()

    def saveWorker(self):
        HYPERION_SI255.workers[self.nid] = self.worker

    def removeWorker(self, worker=None):
        current_worker = HYPERION_SI255.workers.get(self.nid)
        if worker is None or current_worker is worker:
            HYPERION_SI255.workers.pop(self.nid, None)

    def restoreWorker(self):
        if self.nid not in HYPERION_SI255.workers:
            Data.execute(
                'DevLogErr($1,$2)', self.nid, 'Cannot restore worker')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        self.worker = HYPERION_SI255.workers[self.nid]

    def saveInfo(self):
        HYPERION_SI255.handles[self.nid] = self.handle
        HYPERION_SI255.initialized_contexts[self.nid] = (
            self._device_context())

    def _device_context(self):
        """Identify the tree occurrence validated by the latest INIT."""
        tree = self.getTree()
        return (
            str(tree.name).upper(),
            int(tree.shot),
            str(self.getFullPath()).upper(),
        )

    def restoreInfo(self):
        self.debugPrint('restoreInfo')
        try:
            import hyperion
        except Exception as exc:
            Data.execute(
                'DevLogErr($1,$2)', self.nid,
                'Cannot import hyperion library: ' + str(exc))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        if self.nid in HYPERION_SI255.handles:
            self.handle = HYPERION_SI255.handles[self.nid]
            return

        try:
            ip_addr = str(self.ip_addr.data())
            self.handle = hyperion.Hyperion(ip_addr)
            if not self.handle.is_ready:
                raise RuntimeError('instrument is not ready')
        except Exception as exc:
            self.debugPrint(
                'ERROR opening Hyperion',
                '%s\n%s' % (exc, traceback.format_exc()))
            Data.execute(
                'DevLogErr($1,$2)', self.nid,
                'Cannot open Hyperion: ' + str(exc))
            raise mdsExceptions.TclFAILED_ESSENTIAL

    def removeInfo(self):
        HYPERION_SI255.handles.pop(self.nid, None)
        HYPERION_SI255.initialized_contexts.pop(self.nid, None)

    def _configuration_values(self, handle=None):
        acq_mode = str(self.acq_mode.data()).strip().upper()
        if acq_mode != 'PEAK':
            raise ValueError('only PEAK acquisition mode is supported')

        acq_freq = float(self.acq_freq.data())
        if acq_freq <= 0 or acq_freq > 50.0:
            raise ValueError('ACQ_FREQ must be > 0 and <= 50 Hz')

        tolerance = float(self.wl_tolerance.data())
        if tolerance <= 0:
            raise ValueError('WL_TOLERANCE must be > 0 nm')

        if handle is None:
            handle = self.handle

        if int(handle.channel_count) != self.CHANNEL_COUNT:
            raise ValueError(
                'expected %d Hyperion channels, instrument reports %d'
                % (self.CHANNEL_COUNT, int(handle.channel_count)))

        return acq_freq, tolerance

    def init(self):
        worker = HYPERION_SI255.workers.get(self.nid)
        if worker is not None and worker.is_alive():
            Data.execute(
                'DevLogErr($1,$2)', self.nid,
                'Cannot initialize HYPERION_SI255 while acquisition is running')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        loop = None
        created_loop = False
        try:
            import asyncio
            try:
                loop = asyncio.get_event_loop()
                if loop.is_closed():
                    raise RuntimeError('current event loop is closed')
            except RuntimeError:
                # Redis/web action servers execute device methods in worker
                # threads, which do not receive an asyncio loop automatically.
                loop = asyncio.new_event_loop()
                asyncio.set_event_loop(loop)
                created_loop = True

            # INIT is authoritative: always discard a cached handle and verify
            # the instrument and the current shot configuration again.
            self.removeInfo()
            self.restoreInfo()
            acq_freq, tolerance = (
                self._configuration_values())
            self.saveInfo()
        except Exception as exc:
            self.removeInfo()
            if isinstance(exc, mdsExceptions.TclFAILED_ESSENTIAL):
                raise
            Data.execute(
                'DevLogErr($1,$2)', self.nid,
                'Invalid HYPERION_SI255 configuration: ' + str(exc))
            raise mdsExceptions.TclFAILED_ESSENTIAL
        finally:
            if created_loop:
                try:
                    # Let transports scheduled by writer.close() finish before
                    # destroying the temporary action-thread event loop.
                    loop.run_until_complete(asyncio.sleep(0))
                except Exception:
                    pass
                try:
                    asyncio.set_event_loop(None)
                except Exception:
                    pass
                try:
                    loop.close()
                except Exception:
                    pass

        self.debugPrint(
            'Initialized',
            'frequency=%.3f Hz tolerance=%.6f nm raw_capacity=%d'
            % (acq_freq, tolerance, self.RAW_PEAK_CAPACITY))

    def startAcquisition(self):
        expected_context = self._device_context()
        initialized_context = HYPERION_SI255.initialized_contexts.get(
            self.nid)
        if (self.nid not in HYPERION_SI255.handles
                or initialized_context != expected_context):
            Data.execute(
                'DevLogErr($1,$2)', self.nid,
                'HYPERION_SI255 is not initialized for this tree and shot; '
                'run INIT successfully before START')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        self.handle = HYPERION_SI255.handles[self.nid]

        previous_worker = HYPERION_SI255.workers.get(self.nid)
        if previous_worker is not None and previous_worker.is_alive():
            self.debugPrint('Stopping previous acquisition thread')
            previous_worker.stop()
            previous_worker.join(10.0)
            if previous_worker.is_alive():
                Data.execute(
                    'DevLogErr($1,$2)', self.nid,
                    'Previous acquisition thread did not stop')
                raise mdsExceptions.TclFAILED_ESSENTIAL

        HYPERION_SI255.workers.pop(self.nid, None)
        # A terminating worker removes the shared handle entry.  Re-publish
        # the handle before registering the replacement worker.
        self.saveInfo()

        self.worker = self.AsynchStore()
        self.worker.daemon = True
        self.worker.configure(self.getTree().name, self.getTree().shot, self.getFullPath(), self.handle)
        self.saveWorker()
        self.worker.start()

        if not self.worker.setup_done.wait(15.0):
            Data.execute(
                'DevLogErr($1,$2)', self.nid,
                'Acquisition thread setup timed out')
            self.worker.stop()
            self.worker.join(10.0)
            if self.worker.is_alive():
                Data.execute(
                    'DevLogErr($1,$2)', self.nid,
                    'Acquisition thread did not stop after setup timeout')
            else:
                HYPERION_SI255.workers.pop(self.nid, None)
                self.removeInfo()
            raise mdsExceptions.TclFAILED_ESSENTIAL
        if self.worker.setup_error is not None:
            self.worker.join(10.0)
            if not self.worker.is_alive():
                HYPERION_SI255.workers.pop(self.nid, None)
                self.removeInfo()
            Data.execute(
                'DevLogErr($1,$2)', self.nid,
                'Acquisition thread setup failed: ' + self.worker.setup_error)
            raise mdsExceptions.TclFAILED_ESSENTIAL

        self.debugPrint('Acquisition started')

    def stopAcquisition(self):
        self.worker = HYPERION_SI255.workers.get(self.nid)
        if self.worker is None:
            self.debugPrint('Acquisition thread already stopped')
            return

        if not self.worker.is_alive():
            self.debugPrint('Acquisition thread already stopped')
            HYPERION_SI255.workers.pop(self.nid, None)
            self.worker = None
            return

        self.debugPrint('Stopping acquisition thread')
        self.worker.stop()
        self.worker.join(10.0)
        if self.worker.is_alive():
            Data.execute(
                'DevLogErr($1,$2)', self.nid,
                'Acquisition thread did not stop within 10 seconds')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        HYPERION_SI255.workers.pop(self.nid, None)
        self.worker = None
        self.debugPrint('Acquisition stopped')
