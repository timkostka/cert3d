import random

import wx

from AnalysisWindowBase import AnalysisWindowBase
from BilevelData import BilevelData
from ScopeChannel import ScopeChannel

all_colors = [wx.RED, wx.GREEN, wx.YELLOW, wx.Colour(255, 0, 255), wx.CYAN]


class AnalysisWindow(AnalysisWindowBase):

    def __init__(self, parent):
        super(AnalysisWindow, self).__init__(parent)

        # add some junk data
        for i in ['X_STEP', 'X_DIR', 'Y_STEP', 'Y_DIR']:
            data = BilevelData()
            channel = ScopeChannel(data)
            channel.height = random.randint(30, 60)
            channel.color = random.choice(all_colors)
            channel.data.start_high = random.choice([True, False])
            self.scope_panel.add_channel(channel)

        # zoom to all
        self.scope_panel.zoom_to_all()
