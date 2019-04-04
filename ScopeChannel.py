import wx

from BilevelData import BilevelData


class ScopeChannel:

    def __init__(self, data):
        # raw data
        assert isinstance(data, BilevelData)
        self.data = data
        # color of channel
        self.color = wx.GREEN
        # height of channel in pixels
        self.height = 30
