"""
This implements a custom wx control to view .

"""

import wx

from BilevelData import BilevelData


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
        assert step_channel.start_high == False
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
        print("Initializing!")
        super().__init__(parent, id_, position, size, style)
        self.SetBackgroundStyle(wx.BG_STYLE_PAINT)
        self.SetFont(
            wx.Font(
                12,
                wx.FONTFAMILY_MODERN,
                wx.FONTSTYLE_NORMAL,
                wx.FONTWEIGHT_BOLD,
                False,
                "Consolas",
            )
        )
        # list of channels
        self.channels = []
        # index of selected channel, or None
        self.selected_channel_index = None
        # margin on all sides of display

        # padding between channels
        self.padding = 11
        # padding between name and channel data
        self.padding2 = 5
        # thickness in pixels of channel separator bar
        self.channel_separator_thickness = 3
        # margin in pixels all around
        self.margin = 5
        ## number of channel
        # self.channel_count = 4
        # data for each channel
        #self.channels = [BilevelData() for _ in range(4)]
        #for i, channel in enumerate(self.channels):
        #    channel.name = "DATA%d" % i
        #data = decode_stepper(self.channels[0], self.channels[1])
        #print(data)
        #self.channels[0].name = "X_STEP"

        # height of each channel in pixels
        self.channel_height = 30
        # padding around timestamp label
        self.padding_timestamp_label = 2
        # length of channel
        self.channel_length = 150
        # leftmost time
        self.start_time = 0.0
        # seconds per pixel
        self.seconds_per_pixel = 1.0
        # mouse panning memory
        self.panning = False
        self.panning_start = 0
        # true when we're selecting a time delta
        self.selecting_time = False
        # time selection memory
        self.snap_distance = 10
        # font for labels
        self.font_label = wx.Font(
            9,
            wx.FONTFAMILY_MODERN,
            wx.FONTSTYLE_NORMAL,
            wx.FONTWEIGHT_NORMAL,
            False,
            "Consolas",
        )

        # font for timestamps
        self.font_timestamp = wx.Font(
            7,
            wx.FONTFAMILY_MODERN,
            wx.FONTSTYLE_NORMAL,
            wx.FONTWEIGHT_NORMAL,
            False,
            "Consolas",
        )
        # either None or (channel_number, time)
        self.snaptime_start = None
        self.snaptime_end = None
        # prevent panel from shrinking too much
        self.SetMinSize((200, 200))
        # set background
        self.SetBackgroundColour(wx.BLACK)
        self.SetForegroundColour(wx.WHITE)

        self.Bind(wx.EVT_PAINT, self.event_paint)

        self.Bind(wx.EVT_LEFT_DOWN, self.event_mouse_left_button_down)
        self.Bind(wx.EVT_LEFT_UP, self.event_mouse_left_button_up)
        self.Bind(wx.EVT_RIGHT_DOWN, self.event_mouse_right_button_down)
        self.Bind(wx.EVT_RIGHT_UP, self.event_mouse_right_button_up)
        self.Bind(wx.EVT_MOTION, self.event_mouse_motion)

        #self.Bind(wx.EVT_ENTER_WINDOW, self.asdf)
        self.Bind(wx.EVT_LEAVE_WINDOW, self.event_leave_window)

        self.Bind(wx.EVT_MOUSEWHEEL, self.event_mouse_wheel)

        self.zoom_to_all()

    def add_channel(self, channel):
        """Add a new channel."""
        self.channels.append(channel)

    def find_snaptime(self, position):
        """Return the snaptime at the given position, or None"""
        # find correct channel
        x, y = position
        channel_number = (y - self.margin) / (
            self.channel_height + self.padding
        )
        if channel_number < 0.0 or channel_number >= len(self.channels):
            return None
        channel_index = int(channel_number)
        channel = self.channels[channel_index]
        if len(channel.data) < 0:
            return None
        # convert pixel position to a time
        time = self.x_to_time(x)
        # find closest point in this channel data
        closest_time = channel.get_closest_edge_time(time)
        # find delta in pixels
        pixel_delta = (closest_time - time) / self.seconds_per_pixel
        if abs(pixel_delta) < self.snap_distance:
            result = (channel_index, closest_time)
            # print("Pixel delta of", pixel_delta)
            # print("Found snaptime of", result)
            return result
        return None

    def event_mouse_left_button_down(self, event):
        # find closest
        old_start = self.snaptime_start
        old_end = self.snaptime_end
        self.snaptime_start = None
        self.snaptime_end = None
        self.snaptime_start = self.find_snaptime(event.GetPosition())
        if self.snaptime_start != old_start or self.snaptime_end != old_end:
            self.Refresh()
        self.selecting_time = bool(self.snaptime_start)

    def event_mouse_left_button_up(self, event):
        self.selecting_time = False
        if not self.snaptime_start:
            return
        self.snaptime_end = self.find_snaptime(event.GetPosition())
        if not self.snaptime_end or self.snaptime_end == self.snaptime_start:
            self.snaptime_start = None
            self.snaptime_end = None
        self.Refresh()

    def get_time_at_mouse(self, event):
        """Return the time at the mouse coordinates."""
        pass

    def event_leave_window(self, event):
        if self.panning:
            self.event_mouse_right_button_up(event)
            #self.SetCursor(wx.StockCursor(wx.CURSOR_DEFAULT))
            #self.panning = False

    def event_mouse_right_button_down(self, event):
        #print('right button down')
        x = event.GetPosition()[0]
        if x >= self.margin + self.channel_length + self.padding2:
            self.panning_start = x
            self.panning = True
            self.SetCursor(wx.Cursor(wx.CURSOR_SIZEWE))

    def event_mouse_right_button_up(self, event):
        #print('right button up')
        if self.panning:
            self.SetCursor(wx.Cursor(wx.CURSOR_DEFAULT))
            self.panning = False

    def event_mouse_motion(self, event):
        #print('mouse motion')
        if self.panning:
            dx = event.GetPosition()[0] - self.panning_start
            # if no net motion, just return
            if not dx:
                return
            print('moved by %d pixels' % dx)
            delta = self.seconds_per_pixel * dx
            self.start_time -= delta
            self.panning_start += dx
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

    def time_to_x(self, time):
        """Return the x value corresponding to the given time."""
        x = self.margin + self.channel_length + self.padding2
        x += (time - self.start_time) / self.seconds_per_pixel
        return x

    def x_to_time(self, x):
        """Return the time value corresponding to the x position."""
        x -= self.margin + self.channel_length + self.padding2
        return self.start_time + x * self.seconds_per_pixel

    def get_channel_y_values(self, channel_index):
        """Return the top and bottom y pixels for the given channel."""
        top = self.margin
        top += sum(x.height for x in self.channels[:channel_index])
        top += channel_index * self.padding
        return top, top + self.channels[channel_index].height - 1

    def zoom_to_all(self):
        """Initialize the viewing window to see all data."""
        if not self.channels:
            return
        left = min(x.data.start_time for x in self.channels)
        right = max(x.data.start_time + x.data.get_length()
                    for x in self.channels)
        self.start_time = left
        self.seconds_per_pixel = 1.0
        if right != left:
            self.seconds_per_pixel = (right - left) / 1200

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
    def time_to_text(time):
        """Convert the time in seconds to a text value."""
        if time == 0:
            return "0"
        abstime = abs(time)
        if abstime < 10e-9:
            return "%.2f ns" % (time * 1e9)
        elif abstime < 100e-9:
            return "%.1fns" % (time * 1e9)
        elif abstime < 1000e9:
            return "%.0fns" % (time * 1e9)
        elif abstime < 10e6:
            return "%.2fus" % (time * 1e9)
        elif abstime < 100e6:
            return "%.1fus" % (time * 1e9)
        elif abstime < 1000e6:
            return "%.0fus" % (time * 1e9)
        elif abstime < 10e3:
            return "%.2fms" % (time * 1e9)
        elif abstime < 100e3:
            return "%.1fms" % (time * 1e9)
        elif abstime < 1000e3:
            return "%.0fms" % (time * 1e9)
        else:
            return "%gs" % time

    def event_paint(self, event):
        """Handle the EVT_PAINT event."""
        # dc = wx.PaintDC(self)
        dc = wx.AutoBufferedPaintDC(self)
        dc.Clear()
        # dc = wx.GCDC(dc)
        width = 1
        # draw channel names
        dc.SetPen(wx.Pen(wx.GREEN, 1))
        dc.SetBrush(wx.GREEN_BRUSH)
        # get leftmost pixel we can draw for scope view
        left = self.margin + self.channel_length + self.padding2
        # get rightmost pixel to draw for scope view
        right = self.GetSize()[0] - 1
        # get top pixel for next channel
        top = 0
        for i, channel in enumerate(self.channels):
            # draw the separator
            if i == 0:
                top += self.margin
            else:
                y0 = top + self.padding // 2
                dc.SetPen(wx.Pen(wx.LIGHT_GREY, 1))
                dc.DrawRectangle(0, y0, right + 1, 1)
                top += self.padding
            # get top (y1) and bottom (y2) of display
            y1 = top
            y2 = y1 + channel.height - 1 - (width - 1)
            y_mid = (y1 + y2) // 2
            if self.selected_channel_index == i:
                # draw background
                color = wx.Colour(63, 63, 63)
                dc.SetPen(wx.Pen(color, 1))
                dc.SetBrush(wx.Brush(color))
                add = self.padding // 2
                dc.DrawRectangle(0, y1 - add, right + 1, y2 - y1 + 1 + 2 * (width // 2) + add * 2)
            # set channel color
            dc.SetPen(wx.Pen(channel.color, 1))
            dc.SetBrush(wx.Brush(channel.color))
            # if not the first channel, draw the separator
            dc.SetTextForeground(channel.color)
            data = channel.data
            name = data.name
            # get x,y of middle center
            x = self.margin + self.channel_length
            rect = self.GetFullTextExtent(name)
            dc.DrawText(name, x - rect[0], y_mid - rect[1] // 2)
            # get pixels per tick
            pixels_per_tick = data.seconds_per_tick / self.seconds_per_pixel
            # draw the channel
            ticks = 0
            channel_left = left + (data.start_time - self.start_time) / self.seconds_per_pixel
            # true if signal is low
            signal_low = not data.start_high
            for i2, length in enumerate(data.data):
                x1 = int(channel_left + ticks * pixels_per_tick + 0.5)
                # if not the first point, draw the vertical line
                if i2 > 0 and left <= x1 <= right:
                    dc.DrawRectangle(x1 - width // 2, y1, width, y2 - y1 + 1)
                    pass
                # flip signal polarity
                signal_low = not signal_low
                ticks += length
                x2 = int(channel_left + ticks * pixels_per_tick + 0.5)
                # if in range, draw the edge
                if x2 < left or x1 > right:
                    pass
                else:
                    y = y2 if signal_low else y1
                    x11 = max(x1, left)
                    x12 = min(x2, right)
                    dc.DrawRectangle(x11 - width // 2, y, x12 - x11 + 1 + 2 * (width // 2), width)
            top += channel.height
        # draw snap time
        if self.snaptime_start:
            dc.SetPen(wx.Pen(wx.RED, 1))
            channel_index, start_time = self.snaptime_start
            x1 = self.time_to_x(start_time)
            y11, y12 = self.get_channel_y_values(channel_index)
            y1 = (y11 + y12) / 2
            dc.DrawRectangle(x1, y11, 1, self.channel_height)
            if self.snaptime_end:
                channel_index, end_time = self.snaptime_end
                x2 = self.time_to_x(end_time)
                y21, y22 = self.get_channel_y_values(channel_index)
                y2 = (y21 + y22) / 2
                dc.DrawLine(x2, y21, x2, y22)
                # draw line connecting them
                x3 = (x1 + x2) / 2
                dc.DrawLine(x1, y1, x3, y1)
                dc.DrawLine(x3, y1, x3, y2)
                dc.DrawLine(x3, y2, x2, y2)
                # draw time between
                dc.SetFont(self.font_timestamp)
                text = self.time_to_text(end_time - start_time)
                rect = dc.GetFullTextExtent(text)
                width = rect[0]
                height = rect[1]
                width += 2 * self.padding_timestamp_label
                height += 2 * self.padding_timestamp_label
                x = x3
                y = (y1 + y2) / 2
                x -= width / 2
                y -= height / 2
                dc.SetBrush(wx.BLACK_BRUSH)
                dc.DrawRectangle(x, y, width, height)
                x += self.padding_timestamp_label
                y += self.padding_timestamp_label
                dc.DrawText(text, x, y)
