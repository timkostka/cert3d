import random


verbose = True


def create_signal_cluster(data):
    """
    Create a signal cluster out of the given data array.

    Data is assumed to be in (x, y) format where x is a value in ticks.

    """
    # screen size in pixels
    pixels_per_screen = asize(1000)
    # maximum number of edges on screen at once
    max_edges_per_screen = 1000
    # hold data set cluster
    cluster = []
    # add original data set
    cluster.append([0, data])
    # loop until data set is small enough
    while len(data) > max_edges_per_screen:
        # find edge durations
        durations = [y[0] - x[0] for x, y in zip(data[:-2], data[2:])]
        durations.sort()
        # get threshold duration to collapse
        threshold = durations[len(durations) // 2] * 2.0
        new_data = []
        last_index = 0
        new_data.append(data[0])
        while last_index < len(data) - 1:
            # collapse edges until threshold duration is met
            index = last_index + 1
            while (
                index < len(data) - 1
                and data[index][0] - durations[index - 1][0] <= threshold
            ):
                index += 1
            if index > last_index + 1:
                new_data.append((data[index][0], 2))
            else:
                new_data.append(data[index])
            last_index = index
        data = new_data
        cluster.append([0, data])
    # find min zoom levels for each data set
    if verbose:
        print('Collapsed data set into the following cluster:')
        for zoom, data in cluster:
            print('- %g pixels/tick: %d data points' % (zoom, len(data)))
    return cluster


class DataCluster:
    """
    A DataFrame contains a series of data sets.

    """

    def __init__(self, data):
        # holds frame sets in the form
        # (max_ticks_per_pixel, data_set)
        # and data_set is a TriStateData or BilevelData or PlotData object
        clusters = create_signal_cluster(data)
        # time in seconds corresponding to x=0 value
        self.start_time = 0.0
        # data is given with x in seconds
        self.name = "DATA"

    def get_master_data(self):
        return self.clusters[0][1]

    def get_total_duration(self):
        """Return the total duration of this data."""
        data = self.get_master_data()
        if not data:
            return 0.0
        return data[-1][0] - data[0][0]

    def get_data(self, seconds_per_pixel):



class TriStateData:
    """
    The TriStateData class holds data about edges for a given signal.

    For a given time, the signal is either low, high or tri-state.  The third
    state is displayed as high-z.
    """

    def __init__(self):
        # name of the channel
        self.name = "DATA"
        # number of ticks per second
        self.seconds_per_tick = 1.0 / 168e6
        # time offset
        self.start_time = 0.0
        # (x, y) tuples for each state, where (y=0 low, 1 high, or 2 tri-state)
        # y gives the value of the data prior to this time
        self.data = []
        self.invent_data(1000)  # DEBUG

    def get_length(self):
        """Return the length of the data in seconds, or None."""
        if not self.data:
            return None
        return (self.data[-1][0] - self.data[0][0]) * self.seconds_per_tick

    def find_index_after(self, target_time):
        """Return the first index at or after the given time."""
        if not self.data:
            return None
        low = 0
        high = len(self.data) - 1
        # loop until low and high are adjacent
        while low < high:
            test = (low + high) // 2
            time = self.start_time + self.data[test][0] * self.seconds_per_tick
            if time < target_time:
                assert low < test + 1
                low = test + 1
            else:
                assert high > test
                high = test
        return low

    def get_time_at_index(self, index):
        """Return the time at the given data index."""
        return self.start_time + self.data[index][0] * self.seconds_per_tick

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
        new_value = 0
        self.data.append((tick_count, new_value))
        for _ in range(length):
            tick_count += round(1.0 + 9.0 * random.random())
            value = new_value
            new_value = random.randint(0, 1)
            if value == new_value:
                new_value = 2
            self.data.append((tick_count, new_value))

    def get_closest_time(self, target_time):
        """Return the edge time closest to the target time, or None."""
        index = self.find_closest_index(target_time)
        if index is None:
            return None
        return self.get_time_at_index(index)


class BilevelData:
    """The BilevelData class holds data about edges for a given signal."""

    def __init__(self):
        # name of the channel
        self.name = "DATA"
        # number of ticks per second
        self.seconds_per_tick = 1.0 / 168e6
        # starting time in seconds
        self.start_time = 0.0
        # if True, signal starts high
        self.start_high = False
        # time of each edge in ticks
        self.data = []
        self.invent_data(10000)  # DEBUG

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
        self.start_high = random.choice([True, False])
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
