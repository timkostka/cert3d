import random

import wx

from AnalysisWindowBase import AnalysisWindowBase
from BilevelData import BilevelData
from ScopeChannel import ScopeChannel
from PlotData import PlotData

all_colors = [wx.RED, wx.GREEN, wx.YELLOW, wx.Colour(255, 0, 255), wx.CYAN]


class AnalysisWindow(AnalysisWindowBase):

    def __init__(self, parent):
        super(AnalysisWindow, self).__init__(parent)

        # add some junk data
        for name in ['X_STEP', 'X_DIR', 'Y_STEP', 'Y_DIR',
                     'Z_STEP', 'Z_DIR', 'E_STEP', 'E_DIR']:
            data = BilevelData()
            channel = ScopeChannel(data)
            channel.data.name = name
            channel.height = random.randint(30, 60)
            channel.color = random.choice(all_colors)
            channel.thickness = random.randint(1, 4)
            channel.data.start_high = random.choice([True, False])
            self.scope_panel.add_channel(channel)
        for name in ['X_POS', 'Y_POS', 'Z_POS']:
            break
            data = PlotData()
            channel = ScopeChannel(data)
            channel.data.name = name
            channel.height = random.randint(90, 120)
            channel.color = random.choice(all_colors)
            self.scope_panel.add_channel(channel)
        # zoom to all
        self.scope_panel.zoom_to_all()
