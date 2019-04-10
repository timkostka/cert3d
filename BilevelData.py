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
        # time of each edge in ticks
        self.data = []
        self.invent_data(100000)  # DEBUG

    def get_length(self):
        """Return the length of the data in seconds, or None."""
        if not self.data:
            return None
        return (self.data[-1] - self.data[0]) * self.seconds_per_tick

    def find_index_after(self, target_time):
        """Return the first index at or after the given time."""
        if not self.data:
            return None
        low = 0
        high = len(self.data) - 1
        # loop until low and high are adjacent
        while low < high:
            test = (low + high) // 2
            time = self.start_time + self.data[test] * self.seconds_per_tick
            if time < target_time:
                assert low < test + 1
                low = test + 1
            else:
                assert high > test
                high = test
        return low

    def get_time_at_index(self, index):
        """Return the time at the given data index."""
        return self.start_time + self.data[index] * self.seconds_per_tick

    def find_closest_index(self, target_time):
        """Return the data index closest to the given time."""
        if not self.data:
            return None
        if target_time < self.start_time:
            return 0
        index = self.find_index_after(target_time)
        if index is None:
            return len(self.data) - 1
        # else it's either index or index - 1
        if index == 0:
            return index
        low = self.get_time_at_index(index - 1)
        high = self.get_time_at_index(index)
        if abs(target_time - low) < abs(target_time - high):
            return index - 1
        else:
            return index

    def invent_data(self, length=200):
        """Populate with randomly generated data."""
        tick_count = 0
        self.data = [0.0]
        for _ in range(length):
            tick_count += round(1.0 + 9.0 * random.random())
            self.data.append(tick_count)
        self.start_time = 0.0

    def get_closest_time(self, target_time):
        """Return the edge time closest to the target time, or None."""
        index = self.find_closest_index(target_time)
        if index is None:
            return None
        return self.get_time_at_index(index)
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
