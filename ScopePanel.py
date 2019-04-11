"""
This implements a custom wx control to view .

"""

import time
import math
import wx

from dpi import asize

from BilevelData import BilevelData
from PlotData import PlotData

# possible colors for signals
all_signal_colors = [
    ("Red", wx.RED),
    ("Orange", wx.Colour(255, 127, 0)),
    ("Yellow", wx.YELLOW),
    ("Green", wx.GREEN),
    ("Cyan", wx.CYAN),
    ("Blue", wx.BLUE),
    ("Magenta", wx.Colour(255, 0, 255)),
    ("White", wx.WHITE),
]


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


class ScopePanel(wx.Panel):
    def __init__(self, parent, id_, position, size, style):
        # print("Initializing!")
        super().__init__(parent, id_, position, size, style)

        # create popup menu for signal style
        self.style_popup_menu = self.create_style_menu()
        # create the context popup handler
        self.Bind(wx.EVT_MENU, self.event_popup_menu_item_selected)
        self.Bind(wx.EVT_CONTEXT_MENU, self.event_show_popup)
        self.SetBackgroundStyle(wx.BG_STYLE_PAINT)
        # list of channels
        self.channels = []
        # index of selected channel, or None
        self.selected_channel_index = None
        # (channel_index, y_value) used during rearranging channels
        self.dragging_channel = None
        # width of the snaptime display
        self.snaptime_frame_thickness = asize(1)
        # color of snaptime display
        self.snaptime_frame_color = wx.YELLOW
        # color of snaptime text
        self.snaptime_text_color = wx.WHITE
        # padding between name and channel data
        self.padding2 = asize(2)
        # thickness in pixels of channel separator bar
        self.channel_separator_thickness = 1
        # padding between channels
        self.padding = 2 * asize(2) + self.channel_separator_thickness
        # margin in pixels all around
        self.margin = asize(2)
        # spacing between signals
        self.signal_spacing = asize(2)
        # padding around timestamp label
        self.padding_timestamp_label = asize(1)
        # length of channel
        self.channel_length = asize(120)
        # leftmost time
        self.start_time = 0.0
        # seconds per pixel
        self.seconds_per_pixel = 1.0
        # mouse panning memory
        self.panning = False
        self.panning_start = 0
        # holds the (channel_index, signal_index) for a style menu popup
        self.signal_to_style = None
        # true when we're selecting a time delta
        self.selecting_time = False
        # maximum distance in pixels to snap to a time value
        self.snap_distance = asize(10)
        # delta time unit for horizontal axis (e.g. 10e-9)
        self.best_dt = None
        # name of the dt (e.g. "10 ns")
        self.best_dt_text = None
        # font for signal lables
        self.font_signal_name = wx.Font(
            9,
            wx.FONTFAMILY_MODERN,
            wx.FONTSTYLE_NORMAL,
            wx.FONTWEIGHT_BOLD,
            False,
            "Consolas",
        )
        # get height of signal names in pixels
        dc = wx.ClientDC(self)
        dc.SetFont(self.font_signal_name)
        # height of a signal name in pixels
        self.signal_name_height = dc.GetFullTextExtent("X")[1]
        # print("Signal name height is", self.signal_name_height)
        # font for timestamps
        self.font_timestamp = wx.Font(
            9,
            wx.FONTFAMILY_MODERN,
            wx.FONTSTYLE_NORMAL,
            wx.FONTWEIGHT_BOLD,
            False,
            "Consolas",
        )
        # margin for snaptime label
        self.snaptime_margin = [1, 0]
        # either None or (channel_index, signal_index, time)
        self.snaptime_start = None
        self.snaptime_end = None
        # prevent panel from shrinking too much
        self.SetMinSize((200, 200))
        # store stipple brushes
        self.stipple_brushes = {}
        # set background
        self.SetBackgroundColour(wx.BLACK)
        self.SetForegroundColour(wx.WHITE)

        self.Bind(wx.EVT_PAINT, self.event_paint)

        self.Bind(wx.EVT_LEFT_DOWN, self.event_mouse_left_button_down)
        self.Bind(wx.EVT_LEFT_UP, self.event_mouse_left_button_up)
        self.Bind(wx.EVT_MIDDLE_DOWN, self.event_mouse_middle_button_down)
        self.Bind(wx.EVT_MIDDLE_UP, self.event_mouse_middle_button_up)
        self.Bind(wx.EVT_MOTION, self.event_mouse_motion)

        # self.Bind(wx.EVT_ENTER_WINDOW, self.asdf)
        self.Bind(wx.EVT_LEAVE_WINDOW, self.event_leave_window)

        self.Bind(wx.EVT_MOUSEWHEEL, self.event_mouse_wheel)

        self.adjust_channel_name_size()

        self.zoom_to_all()

    def adjust_channel_name_size(self):
        """Find ideal value for self.channel_length."""
        max_length = 0
        dc = wx.ClientDC(self)
        dc.SetFont(self.font_signal_name)
        for channel in self.channels:
            for signal in channel.signals:
                rect = dc.GetFullTextExtent(signal.name)
                max_length = max(max_length, rect[0])
        if max_length > 0:
            self.channel_length = max_length + asize(5)

    def create_stipple_bitmap(self, color):
        """Return the stipple bitmap for the given color."""
        rgb = color.GetRGB()
        # if we already created and cached this one, just return it
        if rgb in self.stipple_brushes:
            return self.stipple_brushes[rgb]
        # create a new bitmap stipple mask
        image = wx.Image(32, 32, clear=True)
        image.InitAlpha()
        for x in range(image.GetWidth()):
            for y in range(image.GetHeight()):
                if (2 * x + y) % 16 < 8:
                    image.SetAlpha(x, y, wx.ALPHA_TRANSPARENT)
                else:
                    rgb = (color.Red(), color.Green(), color.Blue())
                    image.SetRGB(x, y, *rgb)
        bmp = wx.Bitmap(image)
        self.stipple_brushes[rgb] = bmp
        return bmp

    def create_style_menu(self):
        """Create and return a wx.Menu for the signal style popup."""
        menu = wx.Menu()
        # add colors
        # item = menu.Append(-1, "Color")
        submenu = wx.Menu()
        for (name, color) in all_signal_colors:
            submenu.AppendRadioItem(-1, name)
        menu.AppendSubMenu(submenu, "&Color")
        # add thickness submenu
        submenu = wx.Menu()
        for i in range(1, 6):
            submenu.AppendRadioItem(-1, str(i))
        menu.AppendSubMenu(submenu, "&Thickness")
        return menu

    def add_channel(self, channel):
        """Add a new channel."""
        self.channels.append(channel)

    def get_channel_from_y(self, y):
        """Return the channel index for the given y pixel, or None."""
        for i in range(len(self.channels)):
            y1, y2 = self.get_channel_y_values(i)
            # return None if we've passed it already
            if y1 > y:
                return None
            if y1 <= y <= y2:
                return i
        # return None if we're past the end of the data
        return None

    def get_signal_from_y(self, y):
        """Return the channel and signal index from the y pixel, or None."""
        channel_index = self.get_channel_from_y(y)
        if channel_index is None:
            return None
        # search through signals in this channel
        signal_count = len(self.channels[channel_index].signals)
        y1, y2 = self.get_channel_y_values(channel_index)
        # get y value of middle of channel
        y_mid = (y1 + y2) // 2
        # get y value of first signal
        name_height = (
            signal_count * self.signal_name_height
            + (signal_count - 1) * self.signal_spacing
        )
        y_top = y_mid - name_height // 2
        for signal_index in range(signal_count):
            if signal_index > 0:
                y_top += self.signal_spacing + self.signal_name_height
            if y_top <= y < y_top + self.signal_name_height:
                return channel_index, signal_index
        # return None if we're past the end of the data
        return None

    def find_snaptime(self, position):
        """Return the snaptime at the given position, or None"""
        # find correct channel
        x, y = position
        channel_index = self.get_channel_from_y(y)
        if channel_index is None:
            return None
        # alias this channel
        channel = self.channels[channel_index]
        # convert pixel position to a time
        target_time = self.get_time_from_x(x)
        # hold a list of closest time for each signal within the channel
        best_delta = 0.0
        closest_time = None
        # find the closest x value for each signal within the channel
        for signal_index, signal in enumerate(channel.signals):
            this_time = signal.data.get_closest_time(target_time)
            if this_time is not None:
                this_delta = abs(this_time - target_time)
                if closest_time is None or this_delta < best_delta:
                    best_delta = this_delta
                    closest_time = (channel_index, signal_index, this_time)
        if closest_time is None:
            return None
        # find delta in pixels
        pixel_delta = int(best_delta / self.seconds_per_pixel + 0.5)
        if abs(pixel_delta) < self.snap_distance:
            return closest_time
        return None

    def event_mouse_left_button_down(self, event):
        # we either try to drag a channel, or try to select a snapline
        x_separator = self.margin + self.channel_length + self.padding2
        if event.GetPosition()[0] < x_separator:
            # reset snaptime if present
            y = event.GetPosition()[1]
            for i in range(len(self.channels)):
                y1, y2 = self.get_channel_y_values(i)
                if y1 <= y <= y2:
                    # print('Dragging channel', self.dragging_channel)
                    self.dragging_channel = (i, y - y1)
                    self.selected_channel_index = i
                    self.Refresh()
                    break
        else:
            # find closest
            old_start = self.snaptime_start
            old_end = self.snaptime_end
            self.snaptime_start = None
            self.snaptime_end = None
            self.snaptime_start = self.find_snaptime(event.GetPosition())
            if (
                self.snaptime_start != old_start
                or self.snaptime_end != old_end
            ):
                self.Refresh()
            self.selecting_time = bool(self.snaptime_start)

    def event_mouse_left_button_up(self, event):
        if self.dragging_channel:
            self.dragging_channel = None
            self.selected_channel_index = None
            self.Refresh()
        self.selecting_time = False
        if self.snaptime_start:
            self.snaptime_end = self.find_snaptime(event.GetPosition())
            if (
                not self.snaptime_end
                or self.snaptime_end == self.snaptime_start
            ):
                self.snaptime_start = None
                self.snaptime_end = None
            self.Refresh()

    def get_time_at_mouse(self, event):
        """Return the time at the mouse coordinates."""
        pass

    def event_leave_window(self, event):
        if self.panning:
            self.event_mouse_middle_button_up(event)
        if self.dragging_channel:
            self.event_mouse_left_button_up(event)

    def event_mouse_middle_button_down(self, event):
        # print('right button down')
        x = event.GetPosition()[0]
        if x >= self.margin + self.channel_length + self.padding2:
            self.panning_start = x
            self.panning = True
            self.SetCursor(wx.Cursor(wx.CURSOR_SIZEWE))

    def event_mouse_middle_button_up(self, _event):
        # print('right button up')
        if self.panning:
            self.SetCursor(wx.Cursor(wx.CURSOR_DEFAULT))
            self.panning = False

    def event_mouse_motion(self, event):
        # print('mouse motion')
        if self.panning:
            dx = event.GetPosition()[0] - self.panning_start
            # if no net motion, just return
            if not dx:
                return
            # print('moved by %d pixels' % dx)
            delta = self.seconds_per_pixel * dx
            self.start_time -= delta
            self.panning_start += dx
            self.Refresh()
        elif self.dragging_channel:
            # get offset into current channel as-is
            index, offset = self.dragging_channel
            y1, _ = self.get_channel_y_values(index)
            this_offset = event.GetPosition()[1] - y1
            # get target amount of pixels to move channel
            current_delta = this_offset - offset
            chan = self.channels
            if current_delta > 0 and index < len(self.channels) - 1:
                # set a 2 pixel hysteresis
                current_delta -= 1
                # get delta if we move channel down
                increase = self.channels[index + 1].height + self.padding
                # see if it's advantageous to move it
                if abs(current_delta - increase) < abs(current_delta):
                    chan[index], chan[index + 1] = chan[index + 1], chan[index]
                    index += 1
                    self.dragging_channel = (index, offset)
                    self.selected_channel_index = index
                    self.Refresh()
            elif current_delta < 0 and index > 0:
                # set a 2 pixel hysteresis
                current_delta += 1
                # get delta if we move channel up
                increase = self.channels[index - 1].height + self.padding
                # see if it's advantageous to move it
                if abs(current_delta + increase) < abs(current_delta):
                    chan[index - 1], chan[index] = chan[index], chan[index - 1]
                    index -= 1
                    self.dragging_channel = (index, offset)
                    self.selected_channel_index = index
                    self.Refresh()
        if self.snaptime_start and self.selecting_time:
            new_end = self.find_snaptime(event.GetPosition())
            if new_end != self.snaptime_end:
                self.snaptime_end = new_end
                if self.snaptime_end == self.snaptime_start:
                    self.snaptime_end = None
                self.Refresh()

    def event_mouse_wheel(self, event):
        """Handle scrolling in/out via the mouse wheel."""
        scale = 1.3 if event.GetWheelRotation() < 0 else 1 / 1.3
        dx = self.margin + self.channel_length + self.padding2
        # time = self.start_time + (x - dx) * self.seconds_per_pixel
        # time2 = self.start_time2 + (x - dx) * self.seconds_per_pixel * scale
        # self.start_time2 = self.start_time + (x - dx) * self.seconds_per_pixel * (1 - scale)
        x = event.GetPosition()[0]
        self.start_time += (x - dx) * self.seconds_per_pixel * (1 - scale)
        self.seconds_per_pixel *= scale
        self.Refresh()

    def get_x_from_time(self, duration):
        """Return the x pixels corresponding to the given duration."""
        x = self.margin + self.channel_length + self.padding2
        x += (duration - self.start_time) / self.seconds_per_pixel
        return int(x + 0.5)

    def get_time_from_x(self, x):
        """Return the time value corresponding to the x position."""
        x -= self.margin + self.channel_length + self.padding2
        return self.start_time + x * self.seconds_per_pixel

    def get_channel_y_values(self, channel_index):
        """Return the top and bottom y pixels for the given channel."""
        top = self.margin
        top += sum(x.height for x in self.channels[:channel_index])
        top += channel_index * self.padding
        return top, top + self.channels[channel_index].height - 1

    def zoom_to_all(self, start=0.0, end=1.0):
        """Initialize the viewing window to see all data."""
        if not self.channels:
            return
        start_times = []
        end_times = []
        for channel in self.channels:
            for signal in channel.signals:
                start_times.append(signal.data.start_time)
                end_times.append(
                    signal.data.start_time + signal.data.get_length()
                )
        left = min(start_times) if start_times else 0.0
        right = max(end_times) if end_times else 0.0
        if start != 0.0 or end != 1.0:
            assert start < end
            duration = right - left
            right = left + end * duration
            left = left + start * duration
        self.start_time = left
        self.seconds_per_pixel = 1.0
        if right != left:
            self.seconds_per_pixel = (right - left) / self.GetSize()[0]
        # zoom channels
        for channel in self.channels:
            low_values = []
            high_values = []
            for signal in channel.signals:
                if isinstance(signal.data, PlotData) and signal.data.data:
                    low_values.append(min(x[1] for x in signal.data.data))
                    high_values.append(max(x[1] for x in signal.data.data))
            if low_values:
                channel.low_value = min(low_values)
                channel.high_value = max(high_values)

    @staticmethod
    def draw_clipped_rectangle(dc, x, y, w, h, x1, x2):
        # check that we're in bounds
        if x2 < x1 or x > x2 or x + w - 1 < x1:
            return
        if x < x1:
            w -= x1 - x
            x = x1
        if x + w - 1 > x2:
            w = x2 - x - 1
        dc.DrawRectangle(x, y, w, h)

    @staticmethod
    def time_to_text(time_s):
        """Convert the time in seconds to a text value."""
        if time_s == 0:
            return "0"
        abstime = abs(time_s)
        if abstime < 10e-9:
            return "%.2f ns" % (time_s * 1e9)
        elif abstime < 100e-9:
            return "%.1fns" % (time_s * 1e9)
        elif abstime < 1000e9:
            return "%.0fns" % (time_s * 1e9)
        elif abstime < 10e6:
            return "%.2fus" % (time_s * 1e9)
        elif abstime < 100e6:
            return "%.1fus" % (time_s * 1e9)
        elif abstime < 1000e6:
            return "%.0fus" % (time_s * 1e9)
        elif abstime < 10e3:
            return "%.2fms" % (time_s * 1e9)
        elif abstime < 100e3:
            return "%.1fms" % (time_s * 1e9)
        elif abstime < 1000e3:
            return "%.0fms" % (time_s * 1e9)
        else:
            return "%gs" % time_s

    def draw_bileveldata_channel(self, dc, rect, channel, signal):
        """Draw a channel of type BilevelData to the given rectangular region"""
        # clip to the specified region
        dc.SetClippingRegion(*rect)
        # alias the underlying data type
        data = signal.data
        # get pixels per tick
        pixels_per_tick = data.seconds_per_tick / self.seconds_per_pixel
        # find x pixel of start of channel data
        channel_left = (
            rect[0]
            + (data.start_time - self.start_time) / self.seconds_per_pixel
        )
        # adjust for thickness of line
        channel_left -= (signal.thickness - 1) / 2.0
        # true if signal is low
        # note we start on the opposite edge, since we flip it before drawing
        # the first plateau
        signal_low = data.start_high
        # alias some things to shorter names
        y1 = rect[1]
        y2 = y1 + rect[3] - 1
        height = y2 - y1 + 1
        thickness = signal.thickness
        left = rect[0]
        right = rect[0] + rect[2] - 1
        # find first index within window
        index = data.find_index_after(self.start_time)
        # if we're outside the data window, there is nothing to draw
        if index is None:
            dc.DestroyClippingRegion()
            return
        # find first index after window
        right_index = data.find_closest_index(
            self.start_time + (right - left + 1) * self.seconds_per_pixel
        )
        # if there are more than 1 edge per pixel, just draw a grayed out signal
        if right_index - index > (right - left) * 0.5:
            # clip rectangle to data region
            left_2 = self.get_x_from_time(signal.data.get_time_at_index(0))
            right_2 = self.get_x_from_time(signal.data.get_time_at_index(-1))
            left = max(left, left_2)
            right = min(right, right_2)
            # set pen and brush to stipple pattern
            pen = wx.Pen(signal.color, 1)
            brush = wx.Brush(signal.color)
            bmp = self.create_stipple_bitmap(signal.color)
            brush.SetStipple(bmp)
            pen.SetStipple(bmp)
            dc.SetPen(pen)
            dc.SetBrush(brush)
            # draw the rectangle
            dc.DrawRectangle(left, y1, right - left + 1, y2 - y1 + 1)
            dc.DestroyClippingRegion()
            return
        if index:
            index -= 1
        if index % 2 == 1:
            signal_low = not signal_low
        time = data.start_time + data.data[index] * data.seconds_per_tick
        x2 = left + round((time - self.start_time) / self.seconds_per_pixel)
        while True:
            x1 = x2
            # if we're past the viewing window, we're done
            if x1 > right:
                break
            # if not the first point, draw the vertical line
            # if i2 > 0: # and left <= x1 <= right:
            if index > 0:
                dc.DrawRectangle(x1, y1, thickness, height)
            # go to the next value
            index += 1
            signal_low = not signal_low
            if index >= len(data.data):
                break
            time = data.start_time + data.data[index] * data.seconds_per_tick
            # ticks += length
            x2 = left + round(
                (time - self.start_time) / self.seconds_per_pixel
            )
            # if in range, draw the edge
            # if False and (x2 < left or x1 > right):
            #    pass
            # else:
            if x1 <= right and x2 >= left:
                y = y2 - thickness + 1 if signal_low else y1
                dc.DrawRectangle(x1, y, x2 - x1 + thickness, thickness)
        dc.DestroyClippingRegion()
        return
        # while index < len(data.data):
        #    pass

        # number of ticks for current data point
        ticks = 0
        for i2, length in enumerate(data.data):
            x1 = int(channel_left + ticks * pixels_per_tick + 0.5)
            # if we're past the viewing window, we're done
            if x1 > right:
                break
            # if not the first point, draw the vertical line
            # if i2 > 0: # and left <= x1 <= right:
            if i2 > 0 and left <= x1 <= right:
                dc.DrawRectangle(x1, y1, thickness, height)
            # flip signal polarity
            signal_low = not signal_low
            ticks += length
            x2 = int(channel_left + ticks * pixels_per_tick + 0.5)
            # if in range, draw the edge
            # if False and (x2 < left or x1 > right):
            #    pass
            # else:
            if x1 <= right and x2 >= left:
                y = y2 - thickness + 1 if signal_low else y1
                dc.DrawRectangle(x1, y, x2 - x1 + thickness, thickness)
        dc.DestroyClippingRegion()

    def draw_plotdata_channel(self, dc, rect, channel, signal):
        """
        Draw a channel of type PlotData to the given rectangular region.

        bottom_value is the y value of a point on the bottom of the rect.
        top_value is the y value of a point on the top of the rect.

        """
        # alias the data
        data = signal.data
        # clip to the specified region
        dc.SetClippingRegion(*rect)
        # get pixels per tick (x scaling)
        pixels_per_tick = data.seconds_per_tick / self.seconds_per_pixel
        # x pixel of start of channel data
        channel_left = (
            rect[0]
            + (data.start_time - self.start_time) / self.seconds_per_pixel
        )
        # alias some things to shorter names
        bottom_value = channel.low_value
        top_value = channel.high_value
        y1 = rect[1]
        y2 = y1 + rect[3] - 1
        left = rect[0]
        right = rect[0] + rect[2] - 1
        # get pixels per value (y scaling)
        pixels_per_value = (y2 - y1 - 2 * (signal.thickness // 2)) / (
            bottom_value - top_value
        )
        top_value -= (signal.thickness // 2) / pixels_per_value
        # set the drawing pen
        dc.SetPen(wx.Pen(signal.color, signal.thickness))
        x2 = None
        y2 = None
        for point in data.data[1:]:
            x1, y1 = x2, y2
            x2 = int(channel_left + point[0] * pixels_per_tick + 0.5)
            y2 = int(rect[1] + (point[1] - top_value) * pixels_per_value + 0.5)
            # exit if we're drawing offscreen
            if x1 is None:
                continue
            if x1 > right:
                break
            if x2 >= left:
                dc.DrawLine(x1, y1, x2, y2)
        dc.DestroyClippingRegion()

    def find_dt(self):
        """Evaluate the best time delta for the given scale."""
        # get the width of some standard text
        dc = wx.PaintDC(self)
        dc.SetFont(
            wx.Font(
                9,
                wx.FONTFAMILY_MODERN,
                wx.FONTSTYLE_NORMAL,
                wx.FONTWEIGHT_NORMAL,
                False,
                "Consolas",
            )
        )
        rect = dc.GetFullTextExtent("Target width    ")
        target_width = rect[0]
        # print("Target dt width is %d pixels" % target_width)
        scale = [1.0, 1e-3, 1e-6, 1e-9]
        units = ["s", "ms", "us", "ns"]
        multiples = [1, 2, 5]
        target_dt = self.seconds_per_pixel * target_width
        # print("Looking for increment near %g s." % target_dt)
        # human dt
        best_diff = None
        best_dt = 0.0
        best_dt_text = None
        # best_dt_pixels = None
        for m in multiples:
            # find target of the form dt = m * 10^x for some interger x
            x = round(math.log10(target_dt / m))
            this_dt = m * 10 ** x
            this_diff = abs(target_dt - this_dt)
            if best_diff is None or this_diff < best_diff:
                best_diff = this_diff
                best_dt = this_dt
        # find the best text
        for unit, name in zip(scale, units):
            if unit <= best_dt:
                best_dt_text = "%d %s" % (round(best_dt / unit), name)
                break
        if best_dt_text is None and best_dt is not None:
            best_dt_text = "%g" % best_dt
        # print("Best dt was %s" % best_dt_text)
        self.best_dt = best_dt
        self.best_dt_text = best_dt_text

    def event_paint(self, _event):
        """Handle the EVT_PAINT event."""
        self.find_dt()
        dc = wx.AutoBufferedPaintDC(self)
        dc.Clear()
        # get leftmost pixel we can draw for scope view
        left = self.margin + self.channel_length + self.padding2
        # get size of panel
        panel_rect = self.GetSize()
        # get rightmost pixel to draw for scope view
        right = panel_rect[0] - 1
        # draw vertical bars for dt resolution
        dc.SetPen(wx.Pen(wx.Colour(31, 31, 31), 1))
        dt_pixels = round(self.best_dt / self.seconds_per_pixel)
        for x in range(left, right + 1, dt_pixels):
            dc.DrawRectangle(round(x), 0, 1, panel_rect[1])
        # get top pixel for next channel
        top = 0
        timings = []
        # set font for channel name
        dc.SetFont(self.font_signal_name)
        for i, channel in enumerate(self.channels):
            timings.append(time.perf_counter())
            # if not the first channel, draw the separator
            if i == 0:
                top += self.margin
            else:
                thickness = self.channel_separator_thickness
                y0 = top + self.padding // 2 - thickness // 2
                dc.SetPen(wx.Pen(wx.LIGHT_GREY, 1))
                dc.SetBrush(wx.LIGHT_GREY_BRUSH)
                dc.DrawRectangle(0, y0, right + 1, thickness)
                top += self.padding
            # get top (y1) and bottom (y2) of display
            y1 = top
            y2 = y1 + channel.height - 1
            y_mid = (y1 + y2) // 2
            # highlight channel if it's selected
            if self.selected_channel_index == i:
                # draw background
                color = wx.Colour(63, 63, 63)
                dc.SetPen(wx.Pen(color, 1))
                dc.SetBrush(wx.Brush(color))
                add = self.padding // 2
                dc.DrawRectangle(0, y1 - add, right + 1, y2 - y1 + 1 + add * 2)
            # draw channel names
            signal_count = len(channel.signals)
            for signal_index, signal in enumerate(channel.signals):
                # set channel color
                dc.SetPen(wx.Pen(signal.color, 1))
                dc.SetBrush(wx.Brush(signal.color))
                dc.SetTextForeground(signal.color)
                name = signal.name
                # get x,y of middle center
                rect = dc.GetFullTextExtent(name)
                x = self.signal_spacing + self.channel_length
                # adjust for multiple signals per channel
                y = y_mid - rect[1] // 2
                y -= (signal_count - 1) * ((rect[1] + 5) // 2)
                y += signal_index * (rect[1] + 5)
                dc.DrawText(name, x - rect[0], y)
                # get rect to clip channel data to
                rect = wx.Rect(left, y1, right - left + 1, y2 - y1 + 1)
                if isinstance(signal.data, BilevelData):
                    self.draw_bileveldata_channel(dc, rect, channel, signal)
                elif isinstance(signal.data, PlotData):
                    self.draw_plotdata_channel(dc, rect, channel, signal)
                else:
                    print("ERROR: unknown data type")
            top += channel.height
        timings.append(time.perf_counter())
        # output timing information
        print(
            "Timings: %s"
            % "/".join(
                "%.3g" % (1000 * x)
                for x in [
                    (timings[i + 1] - timings[i])
                    for i in range(len(self.channels))
                ]
            )
        )
        # draw snap time
        if self.snaptime_start:
            # set clipping region to right half
            dc.SetClippingRegion(left, 0, panel_rect[0] - left, panel_rect[1])
            # get amount to offset due to snaptime frame thickness
            thickness = self.snaptime_frame_thickness
            delta = thickness // 2
            dc.SetPen(wx.Pen(self.snaptime_frame_color, 1))
            dc.SetBrush(wx.Brush(self.snaptime_frame_color))
            dc.SetTextForeground(self.snaptime_text_color)
            dc.SetTextBackground(wx.BLACK)
            dc.SetBackgroundMode(wx.SOLID)
            channel_index, _, start_time = self.snaptime_start
            x1 = self.get_x_from_time(start_time)
            y11, y12 = self.get_channel_y_values(channel_index)
            height = y12 - y11 + 1
            y11 += height // 6
            y12 -= height // 6
            y1 = (y11 + y12) / 2
            # draw start time selection
            dc.DrawRectangle(x1 - delta, y11, thickness, y12 - y11 + 1)
            if self.snaptime_end:
                channel_index, _, end_time = self.snaptime_end
                x2 = self.get_x_from_time(end_time)
                y21, y22 = self.get_channel_y_values(channel_index)
                height = y22 - y21 + 1
                y21 += height // 6
                y22 -= height // 6
                y2 = (y21 + y22) / 2
                # draw end time selection
                # dc.DrawLine(x2, y21, x2, y22)
                dc.DrawRectangle(x2 - delta, y21, thickness, y22 - y21 + 1)
                # draw line connecting them
                x3 = (x1 + x2) // 2
                # dc.DrawLine(x1, y1, x3, y1)
                dc.DrawRectangle(
                    x1 - delta, y1 - delta, x3 - x1 + thickness, thickness
                )
                # dc.DrawLine(x3, y1, x3, y2)
                dc.DrawRectangle(
                    x3 - delta, y1 - delta, thickness, y2 - y1 + thickness
                )
                # dc.DrawLine(x3, y2, x2, y2)
                dc.DrawRectangle(
                    x3 - delta, y2 - delta, x2 - x3 + thickness, thickness
                )
                # draw time between
                dc.SetFont(self.font_timestamp)
                text = self.time_to_text(end_time - start_time)
                rect = dc.GetFullTextExtent(text)
                width = rect[0]
                height = rect[1]
                # add space for padding between label and frame
                width += 2 * self.snaptime_margin[0]
                height += 2 * self.snaptime_margin[1]
                # add space for frame
                width += 2 * thickness
                height += 2 * thickness
                x = x3
                y = (y1 + y2) / 2
                x -= width // 2
                y -= height // 2
                dc.DrawRectangle(x, y, width, height)

                # draw background of text
                dc.SetBrush(wx.BLACK_BRUSH)
                dc.DrawRectangle(x, y, width, height)
                dc.SetBrush(wx.Brush(self.snaptime_frame_color))
                # draw text
                x += self.snaptime_margin[0] + thickness
                y += self.snaptime_margin[1] + thickness
                dc.DrawText(text, x, y)
                x -= self.snaptime_margin[0] + thickness
                y -= self.snaptime_margin[1] + thickness
                # draw a rectangle using lines
                dc.DrawRectangle(x, y, width, thickness)
                dc.DrawRectangle(x, y + height - thickness, width, thickness)
                dc.DrawRectangle(x, y, thickness, height)
                dc.DrawRectangle(x + width - thickness, y, thickness, height)

    def event_show_popup(self, event):
        x, y = self.ScreenToClient(event.GetPosition())
        # if we're in a signal name, show the popup for signal styles
        if x < self.margin + self.channel_length:
            result = self.get_signal_from_y(y)
            if result:
                self.signal_to_style = result
                channel_index, signal_index = result
                signal = self.channels[channel_index].signals[signal_index]
                # select correct radio box for thickness
                id_ = self.style_popup_menu.FindItem(str(signal.thickness))
                self.style_popup_menu.FindItemById(id_).Check()
                # select correct radio box for color
                color_name = next(
                    x for x, y in all_signal_colors if y == signal.color
                )
                id_ = self.style_popup_menu.FindItem(color_name)
                self.style_popup_menu.FindItemById(id_).Check()
                # popup the style submenu
                self.PopupMenu(self.style_popup_menu, (x, y))
        # only show
        # popup style menu if we're within a signal name

    def event_popup_menu_item_selected(self, event):
        # alias the signal to modify
        channel_index, signal_index = self.signal_to_style
        signal = self.channels[channel_index].signals[signal_index]
        # get the text of the menu item selected
        item = self.style_popup_menu.FindItemById(event.GetId())
        text = item.GetText()
        # if it's numeric, change the thickness, else change the color
        if text.isdigit():
            signal.thickness = int(text)
        else:
            color = next(y for x, y in all_signal_colors if x == text)
            signal.color = color
        # redraw the screen
        self.Refresh()
