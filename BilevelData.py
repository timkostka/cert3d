import random


class BilevelData:
    """The BilevelData class holds data about edges for a given signal."""

    def __init__(self):
        # name of the channel
        self.name = "DATA"
        # number of ticks per second
        self.seconds_per_tick = 1.0 / 180e6
        # starting time in seconds
        self.start_time = 0.0
        # if True, signal starts high
        self.start_high = False
        # length of each pulse in ticks
        self.data = []
        self.invent_data()  # DEBUG

    def get_length(self):
        """Return the length of the data in seconds."""
        return sum(self.data) * self.seconds_per_tick

    def invent_data(self):
        """Populate with randomly generated data."""
        self.data = [random.randint(1, 10) for _ in range(200)]
        self.start_time = 0.0

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
