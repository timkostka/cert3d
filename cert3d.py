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

from AnalysisWindowBase import AnalysisWindowBase
from Data import *
from ScopePanel import *

vid = 0x0483
pid = 0x5740

verbose = True

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
    step_data: BilevelData, dir_data: BilevelData, idle_corrections=True
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
    if idle_corrections:
        # Note: the data above is taken by averaging over the distance between
        # active edges.  When this is a large value, such as when the stepper
        # is at rest and begins to move, it can produce misleading data.  To
        # avoid this, we reduce the duration in cases where the stepper motion
        # is very slow.
        durations = [y[0] - x[0] for x, y in zip(points[:-1], points[1:])]
        durations.sort()
        cutoff = durations[len(durations) // 2] * 5
        # cutoff = min(x for x in durations if x <= cutoff)
        i = 0
        while i < len(points) - 1:
            i += 1
            duration = points[i][0] - points[i - 1][0]
            if duration <= 2 * cutoff:
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
        self.c3d_port_thread = SlaveThread()
        # adjust signal name width
        self.scope_panel.adjust_channel_name_size()
        # zoom to all
        self.scope_panel.zoom_to_all()

    def event_close(self, event):
        print("Handling EVT_CLOSE event")
        # hide window immediately
        print("Hiding window")
        self.Hide()
        # signal child thread to exit
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
        for index, this_data in enumerate(data):
            channel = c3d_gui_window.scope_panel.channels[index]
            name = channel.signals[0].name
            channel.signals = []
            channel.add_signal(Signal(name=name, data=this_data))
        # create step channels if possible
        del c3d_gui_window.scope_panel.channels[8:]
        postprocess_signals(self.scope_panel)
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
        signals = interpret_data(self.c3d_port_thread.log_filename)
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
        expected_offset = -ticks_per_packet // 8
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
        vel_data = derivate_data(pos_data)
        acc_data = derivate_data(vel_data)
        new_signals.append(Signal(name + "_POS", wx.CYAN, 1, pos_data))
        new_signals.append(Signal(name + "_VEL", wx.CYAN, 1, vel_data))
        new_signals.append(Signal(name + "_ACC", wx.CYAN, 1, acc_data))
    # add new channels for the new signals
    for signal in new_signals:
        scope_panel.add_channel(ScopeChannel(height=120, signal=signal))


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


class SlaveThread:
    """This is a class to control and communicate with the slave thread."""

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
