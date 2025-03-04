import torch


# Reintroduce TimeSeriesDataset
class TimeSeriesDataset(torch.utils.data.Dataset):
    def __init__(self, data, labels, seq_len=10, step=100):
        self.data = data
        self.labels = labels
        self.seq_len = seq_len
        self.step = step

    def __len__(self):
        return len(self.data) - (self.seq_len - 1) * self.step

    def __getitem__(self, idx):
        indices = [idx + i * self.step for i in range(self.seq_len)]
        x_seq = self.data[indices]
        y_val = self.labels[indices[-1]]
        return x_seq, y_val


print("Done.")

# Test case for TimeSeriesDataset
import numpy as np

# Create x and y arrays from 0 to 1000
x = np.arange(0, 1001)
y = np.arange(0, 1001)

# Instantiate TimeSeriesDataset with default seq_len and step
dataset = TimeSeriesDataset(x, y)

# Print the first two elements from the dataset
print(dataset[0])
print(dataset[1])
