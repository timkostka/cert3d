import wx

from BilevelData import BilevelData
from PlotData import PlotData


class ScopeChannel:

    def __init__(self, data):
        # raw data
        assert isinstance(data, BilevelData) or isinstance(data, PlotData)
        self.data = data
        # color of channel
        self.color = wx.GREEN
        # width of the channel plot in pixels
        self.thickness = 3
        # height of channel in pixels
        self.height = 30
        # value at low end of channel
        self.low_value = 0.0
        # value at high end of channel
        self.high_value = 1.0

    def draw(self, dc):
        """Draw the channel clipped to the given rectangle DC."""
