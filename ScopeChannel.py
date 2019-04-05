import wx

from BilevelData import BilevelData
from PlotData import PlotData


class Signal:

    def __init__(self, data):
        # raw data
        assert isinstance(data, BilevelData) or isinstance(data, PlotData)
        # data set
        self.data = data
        # color of channel
        self.color = wx.GREEN
        # width of the channel plot in pixels
        self.thickness = 3


class ScopeChannel:

    def __init__(self, data=None):
        # signals within this scope channel
        self.signals = []
        # height of channel in pixels
        self.height = 30
        # value at low end of channel
        # (not used for bilevel channels)
        self.low_value = 0.0
        # value at high end of channel
        # (not used for bilevel channels)
        self.high_value = 1.0
        # add data if needed
        if data:
            self.add_signal(Signal(data))

    def add_signal(self, signal):
        self.signals.append(signal)

    def draw(self, dc):
        """Draw the channel clipped to the given rectangle DC."""
