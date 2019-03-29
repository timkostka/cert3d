"""
This implements a custom wx control.

"""

import wx

from ScopeData import ScopeData


class ScopePanel(wx.Panel):

    def __init__(self, parent, id, position, size, style):
        print('Initializing!')
        super().__init__(parent, id, position, size, style)
        ## number of channel
        #self.channel_count = 4
        # data for each channel
        self.channels = [ScopeData() for _ in range(4)]
        for i, channel in enumerate(self.channels):
            channel.name = 'DATA%d' % i
        self.channels[0].name = 'XCCC_STEP'
        # height of each channel in pixels
        self.channel_height = 60
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
        # prevent panel from shrinking too much
        self.SetMinSize((200, 200))
        # set background
        self.SetBackgroundColour(wx.BLACK)
        self.SetForegroundColour(wx.WHITE)

        self.Bind(wx.EVT_PAINT, self.event_paint)

        self.initialize_viewing_window()

    def initialize_viewing_window(self):
        """Initialize the viewing window to see all data."""
        if not self.channels:
            return
        left = min(x.start for x in self.channels)
        right = max(x.start + x.get_length() for x in self.channels)
        self.start_time = left
        self.seconds_per_pixel = 1.0
        if right != left:
            self.seconds_per_pixel = (right - left) / 800

    def event_paint(self, event):
        """Handle the EVT_PAINT event."""
        dc = wx.PaintDC(self)
        #dc = wx.GCDC(dc)
        # draw channel names
        for i, channel in enumerate(self.channels):
            # get x,y of middle center
            x = self.margin + self.channel_length
            y = self.margin + i * (self.channel_height + self.padding) + self.channel_height // 2
            rect = self.GetFullTextExtent(channel.name)
            dc.DrawText(channel.name, x - rect[0], y - rect[1] // 2)
            # get x of left of channel display
            x = self.margin + self.channel_length + self.padding2
            # get top (y1) and bottom (y2) of channel display
            y1 = self.margin + i * (self.channel_height + self.padding)
            y2 = y1 + self.channel_height - 1
            # get pixels per tick
            pixels_per_tick = channel.seconds_per_tick / self.seconds_per_pixel
            # draw the channel
            ticks = 0
            x1 = x
            dc.SetPen(wx.Pen(wx.GREEN, 3))
            for length in channel.data:
                y1, y2 = y2, y1
                #dc.DrawRectangle(x1, min(y1, y2), 1, max(y1, y2) - min(y1, y2) + 1)
                dc.DrawLine(x1, y1, x1, y2)
                ticks += length
                x2 = x + ticks * pixels_per_tick
                #dc.DrawRectangle(x1, y1, x2 - x1 + 1, 1)
                dc.DrawLine(x1, y1, x2, y1)
                x1 = x2
        return


        #dc.Dra


        dc.SetBrush(wx.RED_BRUSH)
        dc.DrawRectangle(0, 0, 100, 100)
        dc.DrawText("Hello!", 20, 20)
        dc.DrawLine(0, 0, 100, 100)
        dc.DrawLine(100, 0, 0, 100)
