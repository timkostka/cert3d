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

exit_children = False

# flag to clear the log file
clear_log_file = False

# flag to log to file
log_to_file = True

# flag for open log file (not thread safe but probably fine)
log_file_open = False

# thread for monitoring the port
port_thread = None

c3d_port = None

# data rate in Mbps
c3d_data_rate = 0.0

# bytes received since last data rate check
c3d_data_rate_bytes = 0

# time of last data rate check
c3d_data_rate_time = 0

# frequency of data rate calculations
c3d_data_rate_frequency = 4.0

# main window
c3d_gui_window = None


def decode_stepper(step_channel: BilevelData, dir_channel: BilevelData):
    """Given the STEP and DIR channels, return position."""
    time = step_channel.start_time == dir_channel.start_time
    dir_is_up = dir_channel.start_high
    # get microstep vs time
    data = []
    data.append((0, 0))
    steps = 0
    # loop through until we reach the end of either stream
    try:
        dir_it = iter(dir_channel.data)
        dir_is_low = not dir_channel.start_high
        remaining_dir_ticks = next(dir_it)
        assert step_channel.start_high is False
        step_is_low = not step_channel.start_high
        ticks = 0
        for pulse in step_channel.data:
            ticks += pulse
            step_is_low = not step_is_low
            remaining_dir_ticks -= pulse
            while remaining_dir_ticks < 0:
                dir_is_low = not dir_is_low
                remaining_dir_ticks += next(dir_it)
            # if we transitioned high, increase or decrease step
            if dir_is_low:
                steps -= 1
            else:
                steps += 1
            data.append((ticks, steps))
    except StopIteration:
        pass
    return data


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


def find_and_open_c3d_port():
    """Attempt to find and open a C3D port and set it to c3d_port."""
    global c3d_port
    for port in find_c3d_ports():
        try:
            serial_port = serial.Serial(
                port=port.device,
                baudrate=115200,
                timeout=0,
                parity=serial.PARITY_EVEN,
            )
            c3d_port = serial_port
            print("Connected to C3D board on %s." % port.device)
        except serial.serialutil.SerialException as e:
            # There is a bug somewhere (win10 usbser?) that causes a call to
            # win32.SetCommState from serialwin32.py:220 to fail and return
            # with: OSError(22, 'The parameter is incorrect.', None, 87)
            # No solution to this has been identified.  Many users have
            # encountered it.  If we simply ignore this error, the program
            # works fine, so that is exactly what we do.
            #
            # Except we can't, since the object never gets saved to serial_port
            if "OSError(22, 'The parameter is incorrect.', None, 87)" in str(
                e
            ):
                raise
            print("Unable to connect to %s" % port.device)


def c3d_update_data_rate(byte_count):
    """Update the data rate variable with X additional bytes since last call."""
    this_time = time.time()
    global c3d_data_rate_bytes
    global c3d_data_rate_time
    global c3d_data_rate
    global c3d_gui_window
    c3d_data_rate_bytes += byte_count
    period = 1.0 / c3d_data_rate_frequency
    if this_time - c3d_data_rate_time > period:
        c3d_data_rate = c3d_data_rate_bytes * 8.0 / 1e6 / period
        c3d_data_rate_bytes = 0
        c3d_data_rate_time = this_time
        if c3d_data_rate:
            print("Data rate is %g Mbps" % c3d_data_rate)
        text = "%.2g Mbps" % c3d_data_rate
        c3d_gui_window.static_text_data_rate.SetLabel(text)


def c3d_monitor_port(filename):
    """Connect to a serial port and store incoming data to the given file."""
    global c3d_port
    global exit_children
    global clear_log_file
    global log_file_open
    if not log_to_file:
        time.sleep(0.010)
        return
    clear_log_file = False
    with open("incoming_data.c3d", "bw") as f:
        log_file_open = True
        # read data and put it into file
        try:
            while not exit_children and not clear_log_file and log_to_file:
                time.sleep(0.001)
                data = c3d_port.read(c3d_port.in_waiting)
                c3d_update_data_rate(len(data))
                if data:
                    f.write(data)
        except (
            serial.serialutil.SerialTimeoutException,
            serial.serialutil.SerialException,
        ):
            # port was likely closed or removed
            print("Port disappeared!")
            c3d_port = None
    log_file_open = False


def port_monitor_thread():
    """Entry point for the C3D board port monitor."""
    # run until children are killed
    global c3d_gui_window
    global exit_children
    global c3d_port
    while not exit_children:
        # pause 50ms between polling for C3D port
        time.sleep(0.050)
        if not c3d_port:
            find_and_open_c3d_port()
            # update label
            if c3d_port:
                text = "C3D on %s" % c3d_port.port
                c3d_gui_window.static_text_usb_port_status.SetLabel(text)
                text = "..."
                c3d_gui_window.static_text_data_rate.SetLabel(text)
        else:
            c3d_monitor_port("incoming_buffer.c3d")
            # update label
            if not c3d_port:
                text = "Disconnected"
                c3d_gui_window.static_text_usb_port_status.SetLabel(text)
                text = "n/a"
                c3d_gui_window.static_text_data_rate.SetLabel(text)
    print("Child thread exited")


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

        # adjust signal name width
        self.scope_panel.adjust_channel_name_size()
        # zoom to all
        self.scope_panel.zoom_to_all()

    def event_close(self, event):
        print("Handling EVT_CLOSE event")
        # signal child thread to exit
        global exit_children
        exit_children = True
        # hide window immediately
        print("Hiding window")
        self.Hide()
        # join child thread
        print("Joining child thread")
        port_thread.join()
        # close window
        print("Closing window")
        # process this event
        event.Skip()

    def event_file_exit(self, _event):
        # trigger an EVT_CLOSE event
        self.Close()

    def event_button_start_stream_click(self, _event):
        if c3d_port:
            c3d_port.write(b"start")

    def event_button_stop_stream_click(self, _event):
        if c3d_port:
            c3d_port.write(b"stop")

    def event_button_zoom_all_click(self, event):
        self.scope_panel.zoom_to_all()

    def event_button_debug_click(self, event):
        if c3d_port:
            c3d_port.write(b"debug")

    def event_button_reset_click(self, event):
        if c3d_port:
            c3d_port.write(b"reset")

    def event_button_clear_log_click(self, event):
        global clear_log_file
        clear_log_file = True

    def event_button_debug_2_click(self, event):
        data = interpret_data("incoming_data - Copy.c3d")
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
        self.scope_panel.zoom_to_all()
        self.scope_panel.Refresh()
        del c3d_gui_window.scope_panel.channels[8:]

    def event_button_interpret_click(self, _event):
        data = interpret_data("incoming_data.c3d")
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
        self.scope_panel.zoom_to_all()
        self.scope_panel.Refresh()
        del c3d_gui_window.scope_panel.channels[8:]


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


def packets_to_signals(packets):
    """Process packets and return signals."""
    signal_count = 8
    # number of ticks per packet
    ticks_per_packet = 2 ** 15
    # number of ticks in a timer (before it rolls over)
    cycle_count = 2 ** 16
    # hold edges for each signal
    edges = [[0] for _ in range(signal_count)]
    deltas = [[] for _ in range(signal_count)]
    # hold a tick value just below what is expected in this cycle
    expected = -ticks_per_packet // 4
    # print([x.channel_edges for x in packets[:3]])
    for packet_index, packet in enumerate(packets):
        # process each channel
        for channel_index, cycle in enumerate(packet.channel_edges):
            for delta in cycle:
                deltas[channel_index].append((delta, packet_index, expected))
                this_tick = (delta - expected) % cycle_count + expected
                edges[channel_index].append(this_tick)
        expected += ticks_per_packet
    # add data to finish signals
    for i in range(signal_count):
        edges[i].append(len(packets) * ticks_per_packet)
    # DEBUG
    print("Edges in each channel:", [len(x) - 2 for x in edges])
    print([x[:10] for x in edges])
    #    print("Edges in each channel:", [len(x) - 2 for x in edges])
    # process each signal into a BilevelData object
    signals = []
    for i in range(signal_count):
        data = BilevelData()
        data.start_time = 0.0
        data.start_high = False
        if edges[i][:2] == [0, 0]:
            del edges[i][0]
            data.start_high = True
        data.edges = edges[i]
        signals.append(data)
    return signals


def interpret_data(filename):
    """Interpret streamed data within the file and return signals."""
    # close the log file
    global log_to_file
    global log_file_open
    log_to_file = False
    print("\nWaiting for log file to be closed")
    while log_file_open:
        time.sleep(0.010)
    print("\nReading info block")
    with open(filename, "rb") as f:
        # read info block
        start = f.read(9).decode("utf-8")
        if start != "InfoStart":
            return None
        print("- InfoStart")
        # read clock speed
        clock = struct.unpack("L", f.read(4))[0]
        print("- Clock speed: %d" % clock)
        signal_count = struct.unpack("B", f.read(1))[0]
        print("- Signal count: %d" % signal_count)
        adc_count = struct.unpack("B", f.read(1))[0]
        print("- ADC count: %d" % adc_count)
        signal_clocks = [
            struct.unpack("L", f.read(4))[0] for _ in range(signal_count)
        ]
        # print(signal_clocks)
        adc_ranges = [
            (
                struct.unpack("f", f.read(4))[0],
                struct.unpack("f", f.read(4))[0],
            )
            for _ in range(adc_count)
        ]
        # print(adc_ranges)
        stop = f.read(8).decode("utf-8")
        if stop != "InfoStop":
            return None
        # now read each packet
        print("- InfoStop")
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
    # log data
    log_to_file = True
    # convert bilevel data
    signals = packets_to_signals(packets)
    return signals


def run_gui():
    """Run the GUI application."""
    global c3d_gui_window
    global port_thread
    global exit_children
    dpi.set_dpi_aware()
    # create the window
    app = wx.App()
    c3d_gui_window = AnalysisWindow(None)
    # DEBUG
    if False:
        signals = interpret_data("incoming_data - Copy.c3d")
        # replace signal data with data from file
        for index, signal in enumerate(signals):
            channel = c3d_gui_window.scope_panel.channels[index]
            channel.signals[0].data = signal
        del c3d_gui_window.scope_panel.channels[8:]
        c3d_gui_window.scope_panel.zoom_to_all()
    # set this as the top-level window
    app.SetTopWindow(c3d_gui_window)

    if False:
        data = TriStateData()
        data.invent_data(10000)
        print(data.points[:10])
        cluster = create_signal_cluster(data.points)

    # start the child thread
    assert port_thread is None
    port_thread = Thread(target=port_monitor_thread)
    port_thread.start()
    c3d_gui_window.Show()
    app.MainLoop()


if __name__ == "__main__":
    # data = interpret_data("incoming_data - Copy.c3d")
    # for i, x in enumerate(data):
    #    x.validate()
    # exit(0)
    run_gui()
