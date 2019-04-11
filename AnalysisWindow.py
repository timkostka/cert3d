import random

import wx

from AnalysisWindowBase import AnalysisWindowBase
from BilevelData import BilevelData
from ScopeChannel import ScopeChannel
from ScopeChannel import Signal
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
            channel.height = random.randint(20, 20)
            channel.signals[-1].name = name
            channel.signals[-1].color = wx.GREEN  # random.choice(all_colors)
            channel.signals[-1].thickness = random.randint(1, 1)
            channel.signals[-1].start_high = random.choice([True, False])
            self.scope_panel.add_channel(channel)
        for name in ['X_POS', 'Y_POS', 'Z_POS', 'E_POS']:
            data = PlotData()
            channel = ScopeChannel(data)
            channel.signals[-1].name = name
            channel.signals[-1].color = wx.RED # random.choice(all_colors)
            channel.signals[-1].thickness = random.randint(1, 1)
            channel.height = random.randint(120, 120)
            data2 = PlotData()
            signal2 = Signal(data2)
            signal2.name = name + '2'
            signal2.color = wx.CYAN  # random.choice(all_colors)
            signal2.thickness = random.randint(1, 1)
            channel.add_signal(signal2)
            self.scope_panel.add_channel(channel)

        # ratio of the entire display to take up for the initial window
        ratio = 0.6
        # reduce window size to maintain 16:9 ratio
        rect = wx.Display().GetClientArea()
        scaler = min(x / y * ratio for x, y in zip(rect[2:], [16, 9]))
        window_size = [round(16 * scaler), round(9 * scaler)]
        self.SetSize(window_size)
        self.Centre()

        # zoom to all
        self.scope_panel.zoom_to_all() #0.5, 0.50001)

    def event_file_exit( self, event ):
        self.Close()
