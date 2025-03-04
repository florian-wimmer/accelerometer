import os
import pandas as pd
import numpy as np
import plotly.graph_objects as go
from scipy.signal import iirfilter, filtfilt
from sklearn.preprocessing import StandardScaler
import dill
import torch
import torch.nn as nn
import torch.optim as optim
from tqdm import tqdm

"""
Date: 2024-06-20 15:47:56,,,,,,
Calibration: XL_ODR: 6667Hz, XL_FS: 16g, GY_ODR: 6667Hz, GY_FS: 2000dps,,,
Time(s),Acceleration X (g),Acceleration Y (g),Acceleration Z (g),Angular Momentum X (dps),Angular Momentum Y (dps),Angular Momentum Z (dps)
0.000083,0.693359,-0.720215,0.14209,-3.540039,0.366211,-3.051758
0.00098,0.70752,-0.755371,0.086914,1.953125,0.732422,-1.159668
"""

# Check if GPU is available and use it if possible
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
print("Using device:", device)

# dill.load_session('nn_env.db')

# Load training data
train_data = pd.read_csv(r"../measurement/processed_data/train_short.csv", header=2)
X_train = train_data[["Time(s)", "Acceleration X (g)", "Acceleration Y (g)", "Acceleration Z (g)", "Angular Momentum X (dps)", "Angular Momentum Y (dps)", "Angular Momentum Z (dps)"]]
y_train = train_data["Stand detected"]

sampling_frequency = 1 / (train_data["Time(s)"].diff().mean())
cutoff_frequency = 100

# Apply IIR filter to training data (excluding time series)
b, a = iirfilter(4, Wn=cutoff_frequency, fs=sampling_frequency, btype="low", ftype="butter")
X_train_filtered = X_train.copy()
X_train_filtered.iloc[:, 1:] = filtfilt(b, a, X_train.iloc[:, 1:], axis=0)

# Standardize the data
scaler = StandardScaler()
X_train_scaled = scaler.fit_transform(X_train_filtered.iloc[:, 1:])

# Increase the weight of specific features
feature_weights = np.array([1, 1, 1, 1, 1, 1])  # Higher weights for Acc X, Acc Y, and Angular Z
X_train_weighted = X_train_scaled * feature_weights

# Convert data to float32
X_train_weighted = X_train_weighted.astype(np.float32)

# Convert data to PyTorch tensors and move to device
X_train_tensor = torch.tensor(X_train_weighted).to(device)
y_train_tensor = torch.tensor(y_train.values, dtype=torch.float32).to(device)

sequence_length = 10


# Modified TimeSeriesDataset with step of 50 between elements
class TimeSeriesDataset(torch.utils.data.Dataset):
    def __init__(self, data, labels, seq_len, step=50):
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


# Create dataset for sequence training
train_dataset = TimeSeriesDataset(X_train_tensor, y_train_tensor, sequence_length)
dataloader = torch.utils.data.DataLoader(train_dataset, batch_size=32, shuffle=True)


# Modified GRU model with increased complexity: bidirectional GRU, extra fully connected layer, dropout, and ReLU activation.
class GRUModel(nn.Module):
    def __init__(self, input_size):
        super(GRUModel, self).__init__()
        # Bidirectional GRU with hidden size 64 (output dims = 128)
        self.gru = nn.GRU(input_size, 64, num_layers=2, dropout=0.5, batch_first=True, bidirectional=True)
        # Additional fully connected layers for post-processing
        self.fc1 = nn.Linear(128, 32)
        self.relu = nn.ReLU()
        self.dropout = nn.Dropout(0.5)
        self.fc2 = nn.Linear(32, 1)
        self.sigmoid = nn.Sigmoid()

    def forward(self, x):
        out, _ = self.gru(x)
        # Take the output from the final time step
        out = out[:, -1, :]  # shape: (batch, 128)
        out = self.fc1(out)
        out = self.relu(out)
        out = self.dropout(out)
        out = self.fc2(out)
        out = self.sigmoid(out)
        return out


model = GRUModel(input_size=X_train_weighted.shape[1]).to(device)
criterion = nn.BCELoss()
optimizer = optim.Adam(model.parameters(), lr=0.001)

# Train the model
num_epochs = 3
for epoch in range(num_epochs):
    with tqdm(dataloader, unit="batch") as tepoch:
        for batch_X, batch_y in tepoch:
            tepoch.set_description(f"Epoch {epoch+1}")
            batch_X, batch_y = batch_X.to(device), batch_y.to(device)
            optimizer.zero_grad()
            outputs = model(batch_X)
            loss = criterion(outputs.squeeze(), batch_y)
            loss.backward()
            optimizer.step()
            tepoch.set_postfix(loss=loss.item())

# Load test data
test_data = pd.read_csv(r"../measurement/processed_data/test.csv", header=2)
X_test = test_data[["Time(s)", "Acceleration X (g)", "Acceleration Y (g)", "Acceleration Z (g)", "Angular Momentum X (dps)", "Angular Momentum Y (dps)", "Angular Momentum Z (dps)"]]

# Apply IIR filter to test data (excluding time series)
X_test_filtered = X_test.copy()
X_test_filtered.iloc[:, 1:] = filtfilt(b, a, X_test.iloc[:, 1:], axis=0)

# Standardize the test data
X_test_scaled = scaler.transform(X_test_filtered.iloc[:, 1:])

# Convert test data to PyTorch tensor
X_test_tensor = torch.tensor(X_test_scaled, dtype=torch.float32).to(device)

# Prepare test dataset with sequence windows and change to batch prediction
test_dataset = TimeSeriesDataset(X_test_tensor, torch.zeros(len(X_test_tensor)), sequence_length)
test_loader = torch.utils.data.DataLoader(test_dataset, batch_size=32, shuffle=False)

all_preds = []
model.eval()
with torch.no_grad():
    for batch_X, _ in test_loader:
        batch_X = batch_X.to(device)  # shape: (batch, seq_len, features)
        batch_preds = (model(batch_X).squeeze() > 0.5).int().cpu().numpy()
        all_preds.extend(batch_preds.tolist())

# Align predictions with the original test indices.
step = 50  # same as in TimeSeriesDataset
pred_array = np.full(len(X_test_tensor), np.nan)
for i, pred in enumerate(all_preds):
    index = i + (sequence_length - 1) * step
    if index < len(pred_array):
        pred_array[index] = pred

test_data["predicted_heel_button"] = pred_array
