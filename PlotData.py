import random
import math


class PlotData:
    """The PlotData class holds xy plot data for a given signal."""

    def __init__(self):
        # name of the data
        self.name = "DATA"
        # number of ticks per second
        self.seconds_per_tick = 1.0 / 180e6
        # starting time in seconds
        self.start_time = 0.0
        # xy plot data, where x is in ticks
        self.data = []
        self.invent_data()  # DEBUG

    def get_length(self):
        """Return the length of the data in seconds."""
        if not self.data:
            return 0
        return self.data[-1][0] * self.seconds_per_tick

    def invent_data(self):
        """Populate with randomly generated data."""
        self.start_time = 0.0
        phi = random.uniform(0.0, math.tau)
        period = random.uniform(20, 40)
        for i in range(200):
            self.data.append((5 * i, math.sin(phi + i * math.tau / period)))

    def get_closest_edge_time(self, target_time):
        """Return the edge time closest to the target time, or None."""
        if len(self.data) == 0:
            return None
        time = self.start_time
        tick_count = 0
        for ticks in self.data:
            tick_count += ticks
            this_time = self.start_time + tick_count * self.seconds_per_tick
            if abs(this_time - target_time) < abs(time - target_time):
                time = this_time
        return time
