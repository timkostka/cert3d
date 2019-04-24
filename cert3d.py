"""
Cert3D.py is a GUI for showing and interpreting results.

"""

import statistics
import time
from threading import Thread
import random
import struct
import os

import dpi
from dpi import asize

import serial
import serial.tools.list_ports
import wx
import wx.richtext

from AnalysisWindowBase import AnalysisWindowBase, TestWindowBase
from Data import *
from ScopePanel import *

vid = 0x0483
pid = 0x5740

# signals as processed from file
signal_names = [
    "X_STEP",
    "X_DIR",
    "Y_STEP",
    "Y_DIR",
    "Z_STEP",
    "Z_DIR",
    "E_STEP",
    "E_DIR",
]

verbose = True

# text added to a test when it's modified
modified_suffix = " (modified)"

# exit_children = False

# flag to clear the log file
# clear_log_file = False

# flag to log to file
# log_to_file = True

# flag for open log file (not thread safe but probably fine)
# log_file_open = False

# thread for monitoring the port
# port_thread = None

# c3d_port = None

# data rate in Mbps
# c3d_data_rate = 0.0

# bytes received since last data rate check
# c3d_data_rate_bytes = 0

# time of last data rate check
# c3d_data_rate_time = 0

# frequency of data rate calculations
# c3d_data_rate_frequency = 4.0

# main window
# c3d_gui_window = None

# default file name
c3d_log_filename = "c3d_data.bin"


class PrinterGeometry:
    """A PrinterGeometry object holds information about printer geometry."""

    def __init__(self, output=""):
        # steps per mm
        self.steps_per_mm = {c: 1 for c in "XYZE"}
        # steps per mm
        self.steps_per_mm = {c: 1 for c in "XYZE"}

    def parse_output(self, output):
        pass


def derivate_data_triangle_pulses(
    data: PlotData,
    idle_corrections=False,
    max_pulse_duration=0.001,
    min_pulse_duration=0.001,
):
    """Return the derivative of the given data."""
    """
    points = [(data.points[0][0], 0)]
    for p1, p2 in zip(data.points[:-1], data.points[1:]):
        dx = p2[0] - p1[0]
        dy = p2[1] - p1[1]
        if dx == 0:
            dx = 1
            dy = 0
        points.append((p2[0], dy / dx))
    """
    # find defintion for each pulse in the form of
    # (peak_time, half_duration)
    # not the peak amplitude is equal to 1 / half_duration by definition
    # for example (5, 2) is a triangular pulse with the points
    # (5 - 2, 0), (5, 1 / 2), (6 + 2, 0)
    pulses = []
    points = data.points
    # add another point at the end thg
    points.append(points[-1])
    for p1, p2, p3 in zip(points[:-2], points[1:-1], points[2:]):
        if p2[1] == p1[1]:
            continue
        half_duration = max_pulse_duration / data.seconds_per_tick
        pulses.append((p2[0], (p2[1] - p1[1]) / half_duration, half_duration))
    # create data points
    x_points = set()
    for (center_time, peak, half_duration) in pulses:
        x_points.add(center_time - half_duration)
        x_points.add(center_time)
        x_points.add(center_time + half_duration)
    # add first and last point
    x_points.add(points[0][0])
    x_points.add(points[-1][0])
    # sort points and keep without bounds
    x_points = sorted(x_points)
    x_points = [
        x for x in x_points if x >= points[0][0] and x <= points[-1][0]
    ]
    # sort pulses by end time
    pulses = sorted(pulses, key=lambda p: p[0] + p[2])
    # now create velocity for each point
    last_index = 0
    first_index = 0
    new_points = []
    for x in x_points:
        # increment first index if necessary
        while (
            first_index < len(pulses)
            and pulses[first_index][0] + pulses[first_index][2] <= x
        ):
            first_index += 1
        # increment end index if necessary
        while (
            last_index < len(pulses)
            and pulses[last_index][0] - pulses[last_index][2] < x
        ):
            last_index += 1
        # now add up pulses to get this time
        y = 0.0
        for index in range(first_index, last_index):
            center_time, peak, half_duration = pulses[index]
            alpha = abs(x - center_time) / half_duration
            assert alpha <= 1.0
            y += peak * (1.0 - alpha)
        new_points.append((x, y))
    new_data = PlotData()
    new_data.start_time = data.start_time
    new_data.seconds_per_tick = data.seconds_per_tick
    new_data.points = new_points
    return new_data


def derivate_data_squared_signal(data: PlotData):
    """Return the exact derivative of the given data."""
    points = []
    for p1, p2 in zip(data.points[:-1], data.points[1:]):
        dx = (p2[0] - p1[0]) * data.seconds_per_tick
        dy = p2[1] - p1[1]
        if dx == 0:
            dx = 1
            dy = 0
        points.append((p1[0], dy / dx))
        points.append((p2[0], dy / dx))
    new_data = PlotData()
    new_data.start_time = data.start_time
    new_data.seconds_per_tick = data.seconds_per_tick
    new_data.points = points
    return new_data


def derivate_data(data: PlotData, idle_corrections=False):
    """Return the derivative of the given data."""
    points = [(data.points[0][0], 0)]
    for p1, p2 in zip(data.points[:-1], data.points[1:]):
        dx = p2[0] - p1[0]
        dy = p2[1] - p1[1]
        if dx == 0:
            dx = 1
            dy = 0
        points.append((p2[0], dy / dx))
    # Note: the data above is taken by averaging over the distance between
    # active edges.  When this is a large value, such as when the stepper is at
    # rest and begins to move, it can produce misleading data.  To avoid this,
    # we reduce the duration in cases where the stepper motion is very slow.
    if idle_corrections:
        durations = [y[0] - x[0] for x, y in zip(points[:-1], points[1:])]
        durations.sort()
        cutoff = durations[len(durations) // 2] * 5
        # cutoff = max(x for x in durations if x <= cutoff)
        i = 0
        while i < len(points) - 1:
            i += 1
            duration = points[i][0] - points[i - 1][0]
            if duration <= 2 * cutoff:
                continue
            if points[i][1] == points[i - 1][1]:
                continue
            print(i, duration, cutoff, duration / cutoff)
            # add ramp down
            if points[i - 1][1] == 0:
                points.insert(i, (points[i][0] - cutoff, 0.0))
                i += 1
            # add ramp up
            if points[i][1] == 0:
                points.insert(i, (points[i - 1][0] + cutoff, 0.0))
                i += 1
    new_data = PlotData()
    new_data.start_time = data.start_time
    new_data.seconds_per_tick = data.seconds_per_tick
    new_data.points = points
    return new_data


def decode_stepper(
    step_data: BilevelData,
    dir_data: BilevelData,
    correct_idle_time=False,
    idle_time=0.020,
):
    """Given the STEP and DIR channels, return step position as a PlotData."""
    # ensure the channels match in start and duration
    assert step_data.start_time == dir_data.start_time
    assert step_data.get_length() == dir_data.get_length()
    assert step_data.seconds_per_tick == dir_data.seconds_per_tick
    # hold step vs position
    points = []
    points.append((0, 0))
    steps = 0
    # loop through until we reach the end of either stream
    try:
        # read first dir tick
        dir_it = iter(dir_data.edges)
        dir_tick = next(dir_it)
        # value of the DIR channel before dir_tick
        dir_is_low = dir_data.start_high
        # value of STEP channel
        step_is_low = not step_data.start_high
        for step_tick in step_data.edges[1:-1]:
            step_is_low = not step_is_low
            # advance dir until we're past the step tick
            while dir_tick < step_tick:
                dir_tick = next(dir_it)
                dir_is_low = not dir_is_low
            # if STEP transitioned from low to high, increase or decrease pos
            if not step_is_low:
                if dir_is_low:
                    steps -= 1
                else:
                    steps += 1
                points.append((step_tick, steps))
    except StopIteration:
        pass
    # add last value
    points.append((step_data.edges[-1], steps))
    if correct_idle_time:
        # Note: the data above is taken by averaging over the distance between
        # active edges.  When this is a large value, such as when the stepper
        # is at rest and begins to move, it can produce misleading data.  To
        # avoid this, we reduce the duration in cases where the stepper motion
        # is very slow.
        durations = [y[0] - x[0] for x, y in zip(points[:-1], points[1:])]
        durations.sort()
        cutoff = idle_time / step_data.seconds_per_tick
        # cutoff = min(x for x in durations if x <= cutoff)
        i = 0
        while i < len(points) - 1:
            i += 1
            duration = points[i][0] - points[i - 1][0]
            if duration <= cutoff:
                continue
            points.insert(i, (points[i][0] - cutoff, points[i - 1][1]))
            i += 1
    # create PlotData object
    pos = PlotData()
    pos.seconds_per_tick = step_data.seconds_per_tick
    pos.points = points
    return pos


def find_c3d_ports():
    """Find and return possible COM ports attached to a Cert3D board."""
    c3d_ports = []
    description = "USB Serial Device (COM"
    for port in serial.tools.list_ports.comports():
        if port.description.startswith(
            description
        ) and port.serial_number.startswith("C3DR1"):
            c3d_ports.append(port)
    return c3d_ports


all_colors = [wx.RED, wx.GREEN, wx.YELLOW, wx.Colour(255, 0, 255), wx.CYAN]


class AnalysisWindow(AnalysisWindowBase):
    def __init__(self, parent):
        super(AnalysisWindow, self).__init__(parent)
        # set icon
        self.SetIcon(wx.Icon("c3d_icon.ico"))
        # ratio of the entire display to take up for the initial window
        ratio = 0.6
        # reduce window size to maintain 16:9 ratio
        rect = wx.Display().GetClientArea()
        scaler = min(x / y * ratio for x, y in zip(rect[2:], [16, 9]))
        window_size = [round(16 * scaler), round(9 * scaler)]
        self.SetSize(window_size)
        self.Centre()
        # create slave thread to monitor c3d port
        self.c3d_port_thread = C3DPortMonitor()
        # adjust signal name width
        self.scope_panel.adjust_channel_name_size()
        # zoom to all
        self.scope_panel.zoom_to_all()
        # set scaling of status bar
        self.status_bar.SetStatusWidths(
            [-1] * self.status_bar.GetFieldsCount()
        )
        # active test window, or None
        self.test_window = None

    def event_on_vertical_scroll(self, _event):
        # get new scroll offset
        new_offset = self.scroll_bar_vertical.GetThumbPosition()
        if self.scope_panel.y_offset != new_offset:
            self.scope_panel.y_offset = new_offset
            self.scope_panel.Refresh()

    def event_close(self, event):
        print("Handling EVT_CLOSE event")
        # hide window immediately
        print("Hiding window")
        self.Hide()
        # close children
        for child in self.GetChildren():
            child.Close()
        # join child thread
        print("Joining child thread")
        self.c3d_port_thread.exit_and_join()
        print("Closing window")

        # process this event
        event.Skip()

    def event_menu_file_exit(self, _event):
        self.Close()

    def event_menu_file_open(self, _event):
        with wx.FileDialog(
            self,
            "Open data file",
            defaultDir=os.path.curdir,
            wildcard="BIN files (*.bin)|*.bin|All files (*.*)|*.*",
            style=wx.FD_OPEN | wx.FD_FILE_MUST_EXIST,
        ) as fileDialog:
            if fileDialog.ShowModal() == wx.ID_CANCEL:
                return
            # Proceed loading the file chosen by the user
            self.interpret_data_file(fileDialog.GetPath())

    def event_button_start_stream_click(self, _event):
        self.c3d_port_thread.log_to_file = True
        self.c3d_port_thread.discard_data = False
        self.c3d_port_thread.open_port_automatically = True
        self.c3d_port_thread.send_command(b"start")

    def event_button_stop_stream_click(self, _event):
        self.c3d_port_thread.log_to_file = False
        self.c3d_port_thread.discard_data = True
        self.c3d_port_thread.open_port_automatically = True
        self.c3d_port_thread.send_command(b"stop")

    def event_button_zoom_all_click(self, event):
        self.scope_panel.zoom_to_all()

    def event_button_debug_click(self, event):
        self.c3d_port_thread.send_command(b"debug")

    def event_button_reset_click(self, event):
        self.c3d_port_thread.send_command(b"reset")

    def event_button_clear_log_click(self, event):
        global clear_log_file
        clear_log_file = True

    def interpret_data_file(self, filename):
        data = interpret_data(filename)
        if data is None:
            print("ERROR: no data in file")
            return
        print([x.get_length() for x in data])
        # replace signal data with data from file
        self.scope_panel.clear()
        for index, this_data in enumerate(data):
            channel = ScopeChannel(
                signal=Signal(name=signal_names[index], data=this_data)
            )
            self.scope_panel.add_channel(channel)
        # create step channels if possible
        postprocess_signals(self.scope_panel)
        # find simplified representations
        for channel in self.scope_panel.channels:
            for signal in channel.signals:
                signal.create_simplified_data_sets()
        self.scope_panel.zoom_to_all()
        self.scope_panel.Refresh()

    def event_button_debug_2_click(self, event):
        self.interpret_data_file("c3d_data - Copy.bin")

    def event_button_interpret_click(self, _event):
        # stop streaming and close file
        self.event_button_stop_stream_click(_event)
        # wait until file is closed
        while self.c3d_port_thread.log_file:
            time.sleep(0.010)
        self.interpret_data_file(self.c3d_port_thread.log_filename)
        return
        signals = interpret_data()
        if signals is None:
            print("ERROR: could not interpret log file")
            return
        print([x.get_length() for x in signals])
        # replace signal data with data from file
        for index, this_data in enumerate(signals):
            channel = c3d_gui_window.scope_panel.channels[index]
            name = channel.signals[0].name
            channel.signals = []
            channel.add_signal(Signal(name=name, data=this_data))
        del c3d_gui_window.scope_panel.channels[8:]
        postprocess_signals(self.scope_panel)
        self.scope_panel.zoom_to_all()
        self.scope_panel.Refresh()

    def event_timer_update_ui(self, _event):
        # noinspection PyUnusedLocal
        port_name = "Disconnected"
        # noinspection PyUnusedLocal
        data_rate = "n/a"
        try:
            port_name = "C3D on %s" % self.c3d_port_thread.serial_port.port
            data_rate = "%.3f Mbps" % self.c3d_port_thread.get_data_rate_mbps()
        except AttributeError:
            # this is triggered when the serial port is closed
            pass
        if port_name != self.static_text_usb_port_status.GetLabel():
            self.static_text_usb_port_status.SetLabel(port_name)
        if data_rate != self.static_text_data_rate.GetLabel():
            self.static_text_data_rate.SetLabel(data_rate)

    def event_button_trim_click(self, _event):
        self.scope_panel.trim_signals()
        self.scope_panel.zoom_to_all()

    def event_button_open_test_window_click(self, event):
        if not self.test_window:
            self.test_window = TestWindow(self, self.c3d_port_thread)
        self.test_window.Show()
        self.test_window.SetFocus()


class InfoHeader:
    """An InfoHeader contains information about sets of Packets."""

    def __init__(self, file):
        # True if the information is complete
        self.valid = False
        # frequency of the system processor
        self.system_clock = None
        # number of signal channels
        self.signal_count = None
        # frequency for each signal channel
        self.signal_frequencies = []
        # timer period in ticks for each signal channel
        self.signal_overflow_ticks = []
        # system ticks per ADC reading (1 sample on all channels)
        self.ticks_per_adc_reading = None
        # number of ADC channels
        self.adc_count = None
        # low/high value for ADC channels
        self.adc_ranges = None
        # try to parse from reading file
        self.read_from_file(file)

    def read_from_file(self, file):
        """Populate information from the given file object."""
        try:
            print("Reading header")
            # read info block
            start = file.read(9).decode("utf-8")
            assert start == "InfoStart"
            # read clock speed
            self.system_clock = struct.unpack("L", file.read(4))[0]
            print("- Clock speed: %d" % self.system_clock)
            self.signal_count = struct.unpack("B", file.read(1))[0]
            print("- Signal count: %d" % self.signal_count)
            self.adc_count = struct.unpack("B", file.read(1))[0]
            print("- ADC count: %d" % self.adc_count)
            self.ticks_per_adc_reading = struct.unpack("L", file.read(4))[0]
            print("- Ticks per ADC reading: %d" % self.ticks_per_adc_reading)
            self.signal_frequencies = []
            self.signal_overflow_ticks = []
            for i in range(self.signal_count):
                clock = struct.unpack("L", file.read(4))[0]
                overflow = struct.unpack("L", file.read(4))[0]
                self.signal_frequencies.append(clock)
                self.signal_overflow_ticks.append(overflow)
                print(
                    "  - Signal channel %d: clock=%d Hz, overflow=%d ticks"
                    % (i + 1, clock, overflow)
                )
            self.adc_ranges = [
                (
                    struct.unpack("f", file.read(4))[0],
                    struct.unpack("f", file.read(4))[0],
                )
                for _ in range(self.adc_count)
            ]
            stop = file.read(8).decode("utf-8")
            assert stop == "InfoStop"
            print(" - Success!")
            self.valid = True
        except AssertionError:
            print("ERROR: file is empty or has invalid header")
            self.valid = False

    def is_valid(self):
        """Return True if the info is valid."""
        return self.valid


class Packet:
    """A Packet is an output from a single process."""

    def __init__(self, file):
        self.number = struct.unpack("B", file.read(1))[0]
        channel_mask = struct.unpack("B", file.read(1))[0]
        channel_edges = []
        for index in range(8):
            if channel_mask & (1 << index):
                count = struct.unpack("B", file.read(1))[0]
                edges = struct.unpack("%dH" % count, file.read(2 * count))
                channel_edges.append(edges)
            else:
                channel_edges.append(tuple())
        self.channel_edges = channel_edges
        # read adc values
        adc_count = struct.unpack("B", file.read(1))[0]
        # print(adc_count)
        assert adc_count == 0


def packets_to_signals(packets, header: InfoHeader):
    """Process packets and return signals."""
    # get system ticks for each timer overflow
    ticks_per_overflow = [
        1.0 * header.signal_frequencies[i] / header.signal_overflow_ticks[i]
        for i in range(header.signal_count)
    ]
    assert len(set(ticks_per_overflow)) == 1
    # get system ticks per packet
    system_ticks_per_packet = (
        header.system_clock
        * header.signal_overflow_ticks[0]
        / header.signal_frequencies[0]
        / 2
    )
    assert system_ticks_per_packet == int(system_ticks_per_packet)
    system_ticks_per_packet = int(system_ticks_per_packet)
    print("- Converting readings to per-signal lists")
    edges = [[0] for _ in range(header.signal_count)]
    # convert edges for each channel to another format
    # signal_ticks[0] = [(cycle1, value1), (cycle2, value2), etc...]
    signal_ticks = [[] for _ in range(header.signal_count)]
    for packet_index, packet in enumerate(packets):
        # process each channel
        for channel_index, cycle in enumerate(packet.channel_edges):
            if cycle:
                signal_ticks[channel_index].extend(
                    [(packet_index, delta) for delta in cycle]
                )
    # now process each channel
    print("- Processing channels")
    for channel_index, ticks in enumerate(signal_ticks):
        # store range of deltas
        delta_values = []
        # get timer ticks per packet
        ticks_per_packet = (
            system_ticks_per_packet
            * header.signal_frequencies[channel_index]
            // header.system_clock
        )
        expected_offset = -ticks_per_packet // 8 - ticks_per_packet
        # get overflow
        overflow = 2 * ticks_per_packet
        assert overflow == header.signal_overflow_ticks[channel_index]
        for (cycle, delta) in signal_ticks[channel_index]:
            expected = cycle * ticks_per_packet + expected_offset
            delta_from_expected = (delta - expected) % overflow
            delta_values.append(delta_from_expected)
            this_tick = expected + delta_from_expected
            edges[channel_index].append(this_tick)
        if delta_values:
            print(
                "expected=%d, overflow=%d, delta min=%d, max=%d"
                % (
                    expected_offset,
                    overflow,
                    min(delta_values) + expected_offset,
                    max(delta_values) + expected_offset,
                )
            )
        # adjust edges so no edge is negative:
        for i in range(1, len(edges[channel_index])):
            while edges[channel_index][i] < edges[channel_index][i - 1]:
                print("WARNING: adjusted edge to make it non-negative")
                print("         possibly something is out of sync")
                print("         restarting may fix")
                edges[channel_index][i] += overflow
        # add data to finish signals
        edges[channel_index].append(len(packets) * ticks_per_packet)
    # DEBUG
    print("- Done!")
    print("- Edges in each channel:", [len(x) - 2 for x in edges])
    # print([x[:10] for x in edges])
    #    print("Edges in each channel:", [len(x) - 2 for x in edges])
    # process each signal into a BilevelData object
    signals = []
    for i in range(header.signal_count):
        data = BilevelData()
        data.start_time = 0.0
        data.start_high = False
        if edges[i][:2] == [0, 0]:
            del edges[i][0]
            data.start_high = True
        data.edges = edges[i]
        signals.append(data)
    return signals

def create_xy_vel(scope_panel: ScopePanel):
    """Calculate and return the XY_VEL signal from X_VEL and Y_VEL."""
    x_vel = None
    y_vel = None
    for channel in scope_panel.channels:
        for signal in channel.signals:
            if signal.name == 'X_VEL':
                x_vel = signal.get_master_data()
            if signal.name == 'Y_VEL':
                y_vel = signal.get_master_data()
    assert x_vel and y_vel
    # get all x points
    x_values = [x[0] for x in x_vel.points]
    x_values.extend(x[0] for x in y_vel.points)
    x_values = sorted(set(x_values))
    # go through all points and find y values
    x_vel_it = iter(x_vel.points)
    y_vel_it = iter(y_vel.points)
    x_vel_point = next(x_vel_it)
    y_vel_point = next(y_vel_it)
    # hold new points
    points = []
    for x in x_values:
        # increase x_vel point until we're at/past this point
        while x_vel_point[0] < x:
            x_vel_point = next(x_vel_it)
        while y_vel_point[0] < x:
            y_vel_point = next(y_vel_it)
        vel = math.sqrt(x_vel_point[1] ** 2 + y_vel_point[1] ** 2)
        if points:
            points.append((points[-1][0], vel))
        points.append((x, vel))
    data = PlotData()
    data.name = 'XY_VEL'
    data.start_time = x_vel.start_time
    data.seconds_per_tick = x_vel.seconds_per_tick
    data.points = points
    signal = Signal(name="XY_VEL", color=wx.WHITE, data=data)
    return signal


def postprocess_signals(scope_panel: ScopePanel):
    """Postprocess step position from the DIR and STEP channels."""
    all_signals = dict()
    for channel in scope_panel.channels:
        for signal in channel.signals:
            all_signals[signal.name] = signal.get_master_data()
    # find all channels with both a *_STEP and *_DIR signal
    names = [x[:-5] for x in all_signals.keys() if x.endswith("_STEP")]
    names = [x for x in names if x + "_DIR" in all_signals.keys()]
    # postprocess each one
    new_signals = []
    for name in names:
        step_data = all_signals[name + "_STEP"]
        dir_data = all_signals[name + "_DIR"]
        pos_data = decode_stepper(step_data, dir_data)
        # scale from counts to units
        # pos_data.points = [(x, y / 80.0) for (x, y) in pos_data.points]
        vel_data = derivate_data_squared_signal(pos_data)
        # vel_data = derivate_data_triangle_pulses(pos_data)
        # acc_data = derivate_data_squared_signal(vel_data)
        # vel_data = derivate_data(pos_data)
        acc_data = derivate_data(vel_data)
        new_signals.append(Signal(name + "_POS", wx.CYAN, 1, pos_data))
        new_signals.append(Signal(name + "_VEL", wx.CYAN, 1, vel_data))
        # for now, don't calculate acceleration
        # new_signals.append(Signal(name + "_ACC", wx.CYAN, 1, acc_data))
    # add new channels for the new signals
    for signal in new_signals:
        scope_panel.add_channel(ScopeChannel(height=240, signal=signal))
    # combine POS/VEL/ACC channels
    colors = {
        "X": wx.GREEN,
        "Y": wx.CYAN,
        "Z": wx.Colour(255, 0, 255),
        "E": wx.WHITE,
    }
    name_to_index = {
        scope_panel.channels[i].signals[0].name: i
        for i in range(len(scope_panel.channels))
    }
    # set colors
    for channel in scope_panel.channels:
        channel.signals[0].color = colors[channel.signals[0].name[0]]
    for c in "YZ":
        for suffix in ["_POS", "_VEL"]:
            if c == 'E' and suffix == '_POS':
                continue
            scope_panel.channels[name_to_index["X" + suffix]].add_signal(
                scope_panel.channels[name_to_index[c + suffix]].signals[0]
            )
    # delete channels that don't start with X
    indices_to_delete = [
        y for x, y in name_to_index.items() if x[0] != "X" and y >= 8 and x != 'E_VEL'
    ]
    # delete channels
    indices_to_delete.sort(reverse=True)
    for i in indices_to_delete:
        del scope_panel.channels[i]
    # create XY_VEL signal and add it to the X_VEL channel
    xy_vel_signal = create_xy_vel(scope_panel)
    for channel in scope_panel.channels:
        if channel.signals[0].name == 'X_VEL':
            channel.add_signal(xy_vel_signal)
            break


def interpret_data(filename):
    """Interpret streamed data within the file and return signals."""
    with open(filename, "rb") as f:
        # read header
        header = InfoHeader(f)
        if not header.is_valid():
            return None
        # now read each packet
        index = 0
        packets = []
        while True:
            # read sync byte
            if index % 8 == 0:
                data = f.read(1)
                if not data:
                    break
                sync = struct.unpack("B", data)[0]
                assert sync == 0x77
            # read packets
            try:
                this_packet = Packet(f)
                packets.append(this_packet)
            except struct.error:
                break
            index += 1
        # process data
        print("Found %d packets" % len(packets))
    # convert packets to BilevelData
    signals = packets_to_signals(packets, header)
    # set clock for each signal
    for i in range(header.signal_count):
        signals[i].seconds_per_tick = 1.0 / header.signal_frequencies[i]
    return signals


class C3DPortMonitor:
    """Asynchronously control and communicate the Cert3D port."""

    def __init__(self):
        """Create a new slave thread."""
        # if True, open the C3D port
        self.open_port_automatically = True
        # if True, log to file
        self.log_to_file = False
        # if True, read data and clear buffer, but don't write it to a file
        # the log_to_file setting overrides this
        self.discard_data = True
        # if True, close all ports and files and exit
        self.exit_thread = False
        # USB serial port open with Cert3D board
        self.serial_port = None
        # log filename
        self.log_filename = c3d_log_filename
        # log file objec
        self.log_file = None
        # number of bytes read from the serial port
        self.bytes_read = 0
        # if True, overwrite log file instead of appending
        # self.overwrite_log_file = True
        # create and start the Thread object
        self.thread = Thread(target=self.entry_point)
        self.thread.start()
        # history for the data rate
        self.data_rate_history = [(time.time(), 0)]

    def get_data_rate_mbps(self):
        """Return the estimated data rate in Mbps."""
        # number of seconds to average over
        averaging_duration = 1.0
        # add a data point
        this_time = time.time()
        history = self.data_rate_history
        history.append((this_time, self.bytes_read))
        # remove obsolete data points
        while (
            len(history) > 2 and history[0][0] < this_time - averaging_duration
        ):
            del history[0]
        # estimate rate
        assert len(history) >= 2
        delta_time = min(averaging_duration, history[-1][0] - history[0][0])
        assert delta_time > 0
        byte_count = history[-1][1] - history[0][1]
        assert byte_count >= 0
        mbps = byte_count * 8e-6 / delta_time
        return mbps

    def read_and_ignore_data(self):
        """Read and ignore data on the port until a command changes."""
        while self.serial_port and not self.exit_thread:
            time.sleep(0.010)
            if not self.open_port_automatically:
                break
            if self.log_to_file:
                break
            if not self.discard_data:
                break
            # read and ignore data
            try:
                data = self.serial_port.read(self.serial_port.in_waiting)
                self.bytes_read += len(data)
            except (
                serial.serialutil.SerialTimeoutException,
                serial.serialutil.SerialException,
            ):
                print("Cert3D board serial port disappeared!")
                self.serial_port.close()
                self.serial_port = None

    def log_data_to_file(self):
        """Read and log data on the port until a command changes."""
        # open log file if it's not already open
        if self.serial_port and not self.log_file:
            self.log_file = open(self.log_filename, "bw")
        while not self.exit_thread:
            time.sleep(0.001)
            if not self.open_port_automatically:
                break
            # if self.discard_data:
            #    break
            if not self.log_to_file:
                break
            # read data and log to file
            try:
                data = self.serial_port.read(self.serial_port.in_waiting)
                self.bytes_read += len(data)
            except (
                serial.serialutil.SerialTimeoutException,
                serial.serialutil.SerialException,
            ):
                print("Cert3D board serial port disappeared!")
                self.serial_port.close()
                self.serial_port = None
                break
            # write data to file
            self.log_file.write(data)
        # close the log file
        self.log_file.close()
        self.log_file = None

    def open_port(self):
        """Try to find and open the cert3d board."""
        # open port if it's not already open
        if not self.serial_port:
            for port in find_c3d_ports():
                try:
                    serial_port = serial.Serial(
                        port=port.device,
                        baudrate=115200,
                        timeout=0,
                        parity=serial.PARITY_EVEN,
                    )
                    self.serial_port = serial_port
                    print("Connected to C3D board on %s." % port.device)
                except serial.serialutil.SerialException as e:
                    # There is a bug somewhere (win10 usbser?) that causes a
                    # call to win32.SetCommState from serialwin32.py:220 to
                    # fail and return with:
                    #   OSError(22, 'The parameter is incorrect.', None, 87)
                    # No solution to this has been identified.  Many users have
                    # encountered it.  If we simply ignore this error, the
                    # program works fine, so that is exactly what we do.
                    #
                    # Except we can't, since the object never gets saved to
                    # serial_port.  I have to edit the PySerial library to get
                    # this to work correctly.
                    #
                    # After later consideration, I believe this issue is caused
                    # by a non-graceful shutdown of the COM port.  It seems to
                    # be self-correcting, in that if you wait 5-10 minutes, the
                    # error doesn't come up.  So it should not be a large issue
                    # for users, since abrupt board resets should not happen.
                    if (
                        "OSError(22, 'The parameter is incorrect.', None, 87)"
                        in str(e)
                    ):
                        raise
                    print("Unable to connect to %s" % port.device)

    def entry_point(self):
        """Entry point of the slave thread."""
        print("Slave thread is born!")
        while not self.exit_thread:
            time.sleep(0.050)
            # if we're not connecting to anything, just idle
            if not self.open_port_automatically:
                # close port if it's open
                if self.serial_port:
                    self.serial_port.close()
                    self.serial_port = None
                # close log file if it's open
                if self.log_file:
                    self.log_file.close()
                    self.log_file = None
                continue
            # try to open the port
            self.open_port()
            # if port isn't open, just continue
            if not self.serial_port:
                continue
            # at this point, we need to do
            if self.log_to_file:
                self.log_data_to_file()
            elif self.discard_data:
                self.read_and_ignore_data()
            # log file shouldn't be open
            assert not self.log_file
        print("Slave thread is dying!")

    def is_alive(self):
        """Return True if thread is alive."""
        return self.thread.is_alive()

    def exit_and_join(self):
        """Exit this thread and join it."""
        self.exit_thread = True
        start = time.time()
        while self.thread.is_alive():
            time.sleep(0.010)
            if time.time() - start > 5:
                print("ERROR: slave thread not exiting")
                exit(1)
        self.thread.join()

    def send_command(self, command):
        """Send a command over the Cert3D board port."""
        if self.serial_port:
            self.serial_port.write(command)
        else:
            print("WARNING: port not open")


class PrinterPortMonitor:
    """Asynchronously control and communicate the printer serial port."""

    def __init__(
        self,
        control: wx.richtext.RichTextCtrl,
        gauge: wx.Gauge,
        c3d_thread: C3DPortMonitor,
    ):
        # if True, close all ports and files and exit
        self.exit_thread = False
        # USB serial port open with the 3d printer board
        self.serial_port = None
        # text control to update with received messages
        self.text_control = control
        # set to True when an "ok" is received
        self.ok_received = False
        # if True, running commands
        self.test_mode = False
        # commands to send
        self.commands_to_send = []
        # progress gauge control
        self.progress_gauge = gauge
        # create and start the Thread object
        self.thread = Thread(target=self.entry_point)
        self.thread.start()
        # hold the c3d board thread object
        self.c3d_thread = c3d_thread

    def entry_point(self):
        """Entry point for the slave thread."""
        print("Printer port slave thread is born!")
        while not self.exit_thread:
            # pause a bit
            if self.test_mode:
                time.sleep(0.001)
            else:
                time.sleep(0.050)
            # look for ports if one isn't already open
            if not self.serial_port:
                self.open_port()
            if not self.serial_port:
                continue
            # send command if it's ready
            if self.test_mode and self.ok_received:
                if not self.commands_to_send:
                    self.test_mode = False
                    self.report_info("Test complete!")
                    print("We logged %d bytes." % self.c3d_thread.bytes_read)
                    # stop logging
                    time.sleep(0.25)
                    self.c3d_thread.send_command(b"stop")
                    self.c3d_thread.log_to_file = False
                    self.progress_gauge.SetValue(
                        self.progress_gauge.GetRange()
                    )
                else:
                    self.send_command(self.commands_to_send[0])
                    value = self.progress_gauge.GetRange() - len(
                        self.commands_to_send
                    )
                    self.progress_gauge.SetValue(value)
                    del self.commands_to_send[0]
            # read and echo received commands
            try:
                data = self.serial_port.read(self.serial_port.in_waiting)
                text = data.decode("utf-8")
                if text:
                    if "ok\n" in text:
                        self.ok_received = True
                    elif (
                        text.startswith("\n")
                        and self.text_control.GetValue()[-2:] == "ok"
                    ):
                        self.ok_received = True
                    elif (
                        text.startswith("k\n")
                        and self.text_control.GetValue()[-1:] == "o"
                    ):
                        self.ok_received = True
                    self.text_control.SetDefaultStyle(
                        wx.TextAttr(wx.Colour(wx.BLACK))
                    )
                    self.text_control.AppendText(text)
            except (
                serial.serialutil.SerialTimeoutException,
                serial.serialutil.SerialException,
            ):
                print("Printer board serial port disappeared!")
                self.serial_port.close()
                self.serial_port = None
            pass
        # close the port
        if self.serial_port:
            print("Closing printer port.")
            self.serial_port.close()
            self.serial_port = None
        print("Printer port slave thread is dying!")

    def report_info(self, text):
        self.text_control.SetDefaultStyle(wx.TextAttr(wx.Colour(wx.BLUE)))
        self.text_control.AppendText(text)
        if not text.endswith("\n"):
            self.text_control.AppendText("\n")

    def run_test(self, commands):
        """Run the given commands as a test."""
        # reset C3D board
        if not self.serial_port:
            return
        # begin logging
        self.c3d_thread.log_to_file = True
        self.c3d_thread.send_command(b"start")
        # parse g code commands
        commands = commands.split("\n")
        # remove comments
        commands = [x[: (x + ";").index(";")].strip() for x in commands]
        # remove blank lines
        commands = [x for x in commands if x]
        self.progress_gauge.SetRange(len(commands))
        self.progress_gauge.SetValue(0)
        # set slave thread into test mode
        self.report_info("Beginning test")
        # start test
        self.test_mode = False
        self.ok_received = True
        self.commands_to_send = commands
        self.test_mode = True

    def find_printer_ports(self):
        """Return potential ports"""
        ports = []
        # look for Arduino mega boards
        description = "Arduino Mega 2560 (COM"
        for port in serial.tools.list_ports.comports():
            if port.description.startswith(description):
                ports.append(port)
        # look for duet board
        description = "USB Serial Device (COM"
        for port in serial.tools.list_ports.comports():
            if (
                port.description.startswith(description)
                and port.vid == 0x1D50
                and port.pid == 0x60EC
            ):
                ports.append(port)
        return ports

    def open_port(self):
        """Try to find and open the cert3d board."""
        # open port if it's not already open
        if not self.serial_port:
            for port in self.find_printer_ports():
                try:
                    serial_port = serial.Serial(
                        port=port.device,
                        baudrate=250000,
                        timeout=0,
                        parity=serial.PARITY_NONE,
                    )
                    self.serial_port = serial_port
                    print("Connected to printer board on %s." % port.device)
                except serial.serialutil.SerialException as e:
                    # There is a bug somewhere (win10 usbser?) that causes a
                    # call to win32.SetCommState from serialwin32.py:220 to
                    # fail and return with:
                    #   OSError(22, 'The parameter is incorrect.', None, 87)
                    # No solution to this has been identified.  Many users have
                    # encountered it.  If we simply ignore this error, the
                    # program works fine, so that is exactly what we do.
                    #
                    # Except we can't, since the object never gets saved to
                    # serial_port.  I have to edit the PySerial library to get
                    # this to work correctly.
                    #
                    # After later consideration, I believe this issue is caused
                    # by a non-graceful shutdown of the COM port.  It seems to
                    # be self-correcting, in that if you wait 5-10 minutes, the
                    # error doesn't come up.  So it should not be a large issue
                    # for users, since abrupt board resets should not happen.
                    if (
                        "OSError(22, 'The parameter is incorrect.', None, 87)"
                        in str(e)
                    ):
                        raise
                    print("Unable to connect to printer on %s" % port.device)
                    # sleep so this doesn't trigger a ton
                    time.sleep(1.0)

    def send_command(self, command):
        """Send a command to the printer."""
        self.ok_received = False
        if not command:
            return
        # add newline if necessary
        if command[-1] != "\n":
            command += "\n"
        if self.serial_port:
            self.serial_port.write(command.encode("utf-8"))
            self.text_control.SetDefaultStyle(wx.TextAttr(wx.Colour(wx.RED)))
            self.text_control.AppendText(command)
        else:
            print("WARNING: port not open.  command ignored")

    def exit_and_join(self):
        """Exit this thread and join it."""
        self.exit_thread = True
        start = time.time()
        while self.thread.is_alive():
            time.sleep(0.010)
            if time.time() - start > 5:
                print("ERROR: slave thread not exiting")
                exit(1)
        self.thread.join()


class TestWindow(TestWindowBase):
    """The TestWindow is for running g-code tests."""

    def __init__(self, parent, c3d_thread):
        super(TestWindow, self).__init__(parent)
        # set size
        self.SetSize(asize(wx.Size(1200, 600)))
        self.Center()
        # set icon
        self.SetIcon(wx.Icon("c3d_icon.ico"))
        # printer port monitor
        self.port_thread = PrinterPortMonitor(
            self.rich_text_serial_log, self.gauge_test_progress, c3d_thread
        )
        # set font for text control
        self.rich_text_serial_log.SetFont(self.text_ctrl_test_code.GetFont())
        # set C3d port thread
        self.c3d_port_thread = c3d_thread
        # hold list of tests
        self.tests = {}
        # load tests
        self.load_tests()
        self.update_test_combo_box()
        # if test is selected, update it
        self.event_combo_selection_made(None)

    def save_tests(self):
        """Save tests to disk."""
        filename = os.path.join(os.environ["LOCALAPPDATA"], "cert3d")
        os.makedirs(filename, exist_ok=True)
        old_filename = os.path.join(filename, "tests_old.py")
        filename = os.path.join(filename, "tests.py")
        # rename file if it exists
        if os.path.isfile(filename):
            if os.path.isfile(old_filename):
                os.remove(old_filename)
            os.rename(filename, old_filename)
        with open(filename, "w") as f:
            for name in sorted(self.tests.keys()):
                f.write(
                    "tests['%s'] = '%s'\n"
                    % (name, self.tests[name].replace("\n", "\\n"))
                )
        print("Saved %d tests to %s." % (len(self.tests), filename))

    def load_tests(self):
        """Load tests from disk and populate the combo."""
        # erase tests
        self.tests = {}
        filename = os.path.join(os.environ["LOCALAPPDATA"], "cert3d")
        os.makedirs(filename, exist_ok=True)
        filename = os.path.join(filename, "tests.py")
        if not os.path.isfile(filename):
            print("Could not find test definition file.")
            return
        # populate tests
        test_lines = open(filename, "r").readlines()
        try:
            for line in test_lines:
                if not line:
                    continue
                exec(line, {"tests": self.tests})
        except (SyntaxError, NameError):
            print("ERROR: could not read tests")
            self.tests = {}
        print("We found %d tests." % len(self.tests))

    def update_test_combo_box(self):
        """Update combobox with current tests."""
        # alias to get shorter name
        combo = self.combo_box_test_choice
        # clear all entries
        combo.Dismiss()
        combo.Clear()
        # add entries
        combo.Append(sorted(self.tests.keys()))
        # if nothing is selected, select the first test
        if combo.GetCount():
            combo.SetSelection(0)

    def event_button_create_update_click(self, event):
        name = self.combo_box_test_choice.GetValue()
        # if it's modified, erase the *
        if name.endswith(modified_suffix):
            name = name[: -len(modified_suffix)]
            self.combo_box_test_choice.SetValue(name)
        # name cannot be empty
        if not name:
            return
        creating = name not in self.tests
        if name in self.tests:
            print("Overwriting test %s" % name)
        else:
            print("Creating new test %s" % name)
        self.tests[name] = self.text_ctrl_test_code.GetValue()
        if creating:
            self.update_test_combo_box()
            combo = self.combo_box_test_choice
            combo.SetSelection(combo.GetItems().index(name))
        # save changes
        self.save_tests()

    def event_button_delete_click(self, event):
        name = self.combo_box_test_choice.GetValue()
        if name.endswith(modified_suffix):
            name = name[: -len(modified_suffix)]
        if name in self.tests:
            print("Deleting test %s" % name)
            matching_test = (
                self.text_ctrl_test_code.GetValue() == self.tests[name]
            )
            del self.tests[name]
            self.update_test_combo_box()
            if matching_test:
                self.event_combo_selection_made(event)

    def event_combo_on_text_enter(self, event):
        self.event_button_create_update_click(event)

    def event_combo_selection_made(self, event):
        index = self.combo_box_test_choice.GetSelection()
        if index == -1:
            self.text_ctrl_test_code.SetValue("")
            return
        name = self.combo_box_test_choice.GetItems()[index]
        assert name in self.tests
        self.text_ctrl_test_code.SetValue(self.tests[name])
        self.text_ctrl_test_code.SetInsertionPointEnd()
        print("Test %s selected" % name)

    def event_static_text_gcode_reference_click(self, _event):
        wx.BeginBusyCursor()
        import webbrowser

        webbrowser.open(self.static_text_gcode_reference.GetLabel())
        wx.EndBusyCursor()

    def event_static_text_reprap_reference_click(self, _event):
        wx.BeginBusyCursor()
        import webbrowser

        webbrowser.open(self.static_text_reprap_link.GetLabel())
        wx.EndBusyCursor()

    def event_button_run_test_click(self, event):
        self.run_test()

    def event_button_send_click(self, _event):
        if not self.text_ctrl_message.GetValue():
            return
        if not self.port_thread.serial_port:
            self.rich_text_serial_log.SetDefaultStyle(
                wx.TextAttr(wx.Colour(wx.RED))
            )
            self.rich_text_serial_log.AppendText("Printer not connected\n")
            return
        command = self.text_ctrl_message.GetValue() + "\n"
        self.port_thread.send_command(command)
        self.text_ctrl_message.Clear()

    def event_text_ctrl_message_enter(self, event):
        self.event_button_send_click(event)

    def event_close(self, event):
        print("Handling EVT_CLOSE event")
        # hide window immediately
        print("Hiding window")
        self.Hide()
        # save tests
        self.save_tests()
        # signal child thread to exit
        # join child thread
        print("Joining child thread")
        self.port_thread.exit_and_join()
        print("Closing window")
        # destory this window
        self.Destroy()
        # process this event
        event.Skip()

    def run_test(self):
        self.port_thread.run_test(self.text_ctrl_test_code.GetValue())

    def event_text_ctrl_test_code_char(self, event):
        # Ctrl+A pressed
        if event.GetUnicodeKey() == 1:
            self.text_ctrl_test_code.SetSelection(0, -1)
            return
        event.Skip()

    def event_text_ctrl_test_code_on_text(self, event):
        value = self.combo_box_test_choice.GetValue()
        if value in self.tests:
            if self.text_ctrl_test_code.GetValue() != self.tests[value]:
                self.combo_box_test_choice.SetValue(value + modified_suffix)

    def event_button_view_results_click(self, _event):
        viewer = self.GetParent()
        # get steps per mm for each channel
        steps_per_mm = self.rich_text_serial_log.GetValue()
        steps_per_mm = steps_per_mm.split("\n")
        # get lines with M92
        steps_per_mm = [x for x in steps_per_mm if "M92" in x]
        # get text after M92
        steps_per_mm = steps_per_mm[-1]
        steps_per_mm = steps_per_mm[steps_per_mm.index("M92") + 3 :]
        # remove comments
        steps_per_mm = steps_per_mm[: (steps_per_mm + ";").index(";")]
        steps_per_mm = [x for x in steps_per_mm.split(" ") if x]
        steps_per_mm = {
            x[0]: float(x[1:]) for x in steps_per_mm if x[0] in "XYZE"
        }
        viewer.event_button_interpret_click(None)
        viewer.event_button_trim_click(None)
        # scale POS/VEL/ACC channels by steps/mm
        for channel in viewer.scope_panel.channels:
            for signal in channel.signals:
                data = signal.get_master_data()
                if not isinstance(data, PlotData):
                    continue
                if signal.name[0] in steps_per_mm:
                    scale = 1.0 / steps_per_mm[signal.name[0]]
                    data.points = [(x, y * scale) for x, y in data.points]
                    signal.create_simplified_data_sets()
        viewer.event_button_zoom_all_click(None)
        viewer.SetFocus()


def run_gui():
    """Run the GUI application."""
    global c3d_gui_window
    dpi.set_dpi_aware()
    # create the window
    app = wx.App()
    c3d_gui_window = AnalysisWindow(None)
    # set this as the top-level window
    app.SetTopWindow(c3d_gui_window)
    # show the window
    c3d_gui_window.Show()
    # start the GUI
    app.MainLoop()


if __name__ == "__main__":
    # data = interpret_data("incoming_data - Copy.c3d")
    # for i, x in enumerate(data):
    #    x.validate()
    # exit(0)
    run_gui()
