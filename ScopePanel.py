"""
This implements a custom wx control.

"""

import wx

from ScopeData import ScopeData


class ScopePanel(wx.Panel):
    def __init__(self, parent, id, position, size, style):
        print("Initializing!")
        super().__init__(parent, id, position, size, style)
        self.SetBackgroundStyle(wx.BG_STYLE_PAINT)
        self.SetFont(
            wx.Font(
                9,
                wx.FONTFAMILY_MODERN,
                wx.FONTSTYLE_NORMAL,
                wx.FONTWEIGHT_NORMAL,
                False,
                "Consolas",
            )
        )
        ## number of channel
        # self.channel_count = 4
        # data for each channel
        self.channels = [ScopeData() for _ in range(4)]
        for i, channel in enumerate(self.channels):
            channel.name = "DATA%d" % i
        self.channels[0].name = "X_STEP"
        # height of each channel in pixels
        self.channel_height = 40
        # margin in pixels all around
        self.margin = 5
        # padding between channels
        self.padding = 5
        # padding between name and channel data
        self.padding2 = 5
        # length of channel
        self.channel_length = 150
        # leftmost time
        self.start_time = 0.0
        # seconds per pixel
        self.seconds_per_pixel = 1.0
        # mouse stuff
        self.panning = False
        self.panning_start = 0
        # prevent panel from shrinking too much
        self.SetMinSize((200, 200))
        # set background
        self.SetBackgroundColour(wx.BLACK)
        self.SetForegroundColour(wx.WHITE)

        self.Bind(wx.EVT_PAINT, self.event_paint)

        self.Bind(wx.EVT_RIGHT_DOWN, self.event_mouse_right_button_down)
        self.Bind(wx.EVT_RIGHT_UP, self.event_mouse_right_button_up)
        self.Bind(wx.EVT_MOTION, self.event_mouse_motion)

        self.Bind(wx.EVT_MOUSEWHEEL, self.event_mouse_wheel)

        self.initialize_viewing_window()

    def get_time_at_mouse(self, event):
        """Return the time at the mouse coordinates."""
        pass

    def event_mouse_right_button_down(self, event):
        self.panning = True
        self.panning_start = event.GetPosition()[0]

    def event_mouse_right_button_up(self, event):
        self.panning = False

    def event_mouse_motion(self, event):
        if self.panning:
            dx = event.GetPosition()[0] - self.panning_start
            delta = self.seconds_per_pixel * dx
            self.start_time -= delta
            self.panning_start += dx
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

    def initialize_viewing_window(self):
        """Initialize the viewing window to see all data."""
        if not self.channels:
            return
        left = min(x.start_time for x in self.channels)
        right = max(x.start_time + x.get_length() for x in self.channels)
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
        left = self.margin + self.channel_length + self.padding
        # get rightmost pixel to draw for scope view
        right = self.GetSize()[0] - 1 - self.margin
        for i, channel in enumerate(self.channels):
            # get x,y of middle center
            x = self.margin + self.channel_length
            y = (
                self.margin
                + i * (self.channel_height + self.padding)
                + self.channel_height // 2
            )
            rect = self.GetFullTextExtent(channel.name)
            dc.DrawText(channel.name, x - rect[0], y - rect[1] // 2)
            # get top (y1) and bottom (y2) of channel display
            y1 = self.margin + i * (self.channel_height + self.padding)
            y2 = y1 + self.channel_height - width + 1
            # get pixels per tick
            pixels_per_tick = channel.seconds_per_tick / self.seconds_per_pixel
            # get x of start of the time
            x = self.margin + self.channel_length + self.padding2
            x -= (
                self.start_time - channel.start_time
            ) / self.seconds_per_pixel
            # draw the channel
            ticks = 0
            x1 = x
            first_point = True
            for length in channel.data:
                y1, y2 = y2, y1
                # draw vertical line
                if not first_point:
                    self.draw_clipped_rectangle(
                        dc,
                        x1,
                        min(y1, y2),
                        width,
                        max(y1, y2) - min(y1, y2) + 1,
                        left,
                        right,
                    )
                first_point = False
                # dc.DrawRectangle(x1, min(y1, y2), width, max(y1, y2) - min(y1, y2) + 1)
                # dc.DrawLine(x1, y1, x1, y2)
                ticks += length
                x2 = x + ticks * pixels_per_tick
                self.draw_clipped_rectangle(
                    dc, x1, y1, x2 - x1 + width, width, left, right
                )
                # dc.DrawRectangle(x1, y1, x2 - x1 + width, width)
                # dc.DrawLine(x1, y1, x2, y1)
                x1 = x2
        return

        # dc.Dra

        dc.SetBrush(wx.RED_BRUSH)
        dc.DrawRectangle(0, 0, 100, 100)
        dc.DrawText("Hello!", 20, 20)
        dc.DrawLine(0, 0, 100, 100)
        dc.DrawLine(100, 0, 0, 100)
