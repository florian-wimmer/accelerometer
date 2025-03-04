import torch
import torch.nn as nn
import torch.nn.functional as F


# Helper module to trim convolution output to the original sequence length.
class Chomp1d(nn.Module):
    def __init__(self, chomp_size):
        super(Chomp1d, self).__init__()
        self.chomp_size = chomp_size

    def forward(self, x):
        return x[:, :, : -self.chomp_size].contiguous()


# A single Temporal Block (residual block) including two dilated convolution layers.
class TemporalBlock(nn.Module):
    def __init__(self, n_inputs, n_outputs, kernel_size, stride, dilation, padding, dropout=0.2):
        super(TemporalBlock, self).__init__()
        self.conv1 = nn.Conv1d(n_inputs, n_outputs, kernel_size, stride=stride, padding=padding, dilation=dilation)
        self.chomp1 = Chomp1d(padding)
        self.relu1 = nn.ReLU()
        self.dropout1 = nn.Dropout(dropout)

        self.conv2 = nn.Conv1d(n_outputs, n_outputs, kernel_size, stride=stride, padding=padding, dilation=dilation)
        self.chomp2 = Chomp1d(padding)
        self.relu2 = nn.ReLU()
        self.dropout2 = nn.Dropout(dropout)

        self.net = nn.Sequential(self.conv1, self.chomp1, self.relu1, self.dropout1, self.conv2, self.chomp2, self.relu2, self.dropout2)
        self.downsample = nn.Conv1d(n_inputs, n_outputs, 1) if n_inputs != n_outputs else None
        self.relu = nn.ReLU()

    def forward(self, x):
        out = self.net(x)
        res = x if self.downsample is None else self.downsample(x)
        return self.relu(out + res)


# The TCN model composed of multiple Temporal Blocks.
class TCN(nn.Module):
    def __init__(self, num_inputs, num_channels, kernel_size=2, dropout=0.2):
        """
        num_inputs: Number of features per time step.
        num_channels: List of output channels for each Temporal Block.
        """
        super(TCN, self).__init__()
        layers = []
        num_levels = len(num_channels)
        for i in range(num_levels):
            dilation_size = 2**i
            in_channels = num_inputs if i == 0 else num_channels[i - 1]
            out_channels = num_channels[i]
            layers += [TemporalBlock(in_channels, out_channels, kernel_size, stride=1, dilation=dilation_size, padding=(kernel_size - 1) * dilation_size, dropout=dropout)]
        self.network = nn.Sequential(*layers)
        self.fc = nn.Linear(num_channels[-1], 1)
        self.sigmoid = nn.Sigmoid()

    def forward(self, x):
        # x shape: (batch, seq_len, features)
        x = x.transpose(1, 2)  # (batch, features, seq_len)
        y = self.network(x)
        y = y[:, :, -1]  # take the output of the final time step
        y = self.fc(y)
        y = self.sigmoid(y)
        return y


if __name__ == "__main__":
    import os
    import pandas as pd
    import numpy as np
    from scipy.signal import iirfilter, filtfilt
    from sklearn.preprocessing import StandardScaler
    import torch.optim as optim
    from torch.utils.data import DataLoader
    from tqdm import tqdm

    # Load training data
    train_data = pd.read_csv("../measurement/processed_data/train_short.csv", header=2)

    window_size = 10
    averaged_rows = []

    for i in range(0, len(train_data), window_size):
        chunk = train_data.iloc[i : i + window_size]
        chunk_mean = chunk.mean(numeric_only=True)
        # Use mean time for the new row
        chunk_mean["Time(s)"] = chunk["Time(s)"].mean()
        averaged_rows.append(chunk_mean)

    train_data = pd.DataFrame(averaged_rows, columns=train_data.columns)

    X_train = train_data[["Time(s)", "Acceleration X (g)", "Acceleration Y (g)", "Acceleration Z (g)", "Angular Momentum X (dps)", "Angular Momentum Y (dps)", "Angular Momentum Z (dps)"]]
    y_train = train_data["Stand detected"]

    # Compute sampling frequency and filter parameters
    sampling_frequency = 1 / (train_data["Time(s)"].diff().mean())
    cutoff_frequency = 100
    b, a = iirfilter(4, Wn=cutoff_frequency, fs=sampling_frequency, btype="low", ftype="butter")

    # Apply filter, standardize and weight features
    X_train_filtered = X_train.copy()
    X_train_filtered.iloc[:, 1:] = filtfilt(b, a, X_train.iloc[:, 1:], axis=0)
    scaler = StandardScaler()
    X_train_scaled = scaler.fit_transform(X_train_filtered.iloc[:, 1:])
    feature_weights = np.array([1, 1, 1, 1, 1, 1])
    X_train_weighted = (X_train_scaled * feature_weights).astype(np.float32)

    # Convert to tensor
    X_train_tensor = torch.tensor(X_train_weighted)
    y_train_tensor = torch.tensor(y_train.values, dtype=torch.float32)

    # Reintroduce the custom TimeSeriesDataset with sequence length and step
    sequence_length = 5
    step = 10
    batch_size = 256

    train_dataset = TimeSeriesDataset(X_train_tensor, y_train_tensor, sequence_length, step)
    train_loader = DataLoader(train_dataset, batch_size=256, shuffle=True)

    # Define TCN model instance
    num_features = X_train_tensor.shape[1]
    model = TCN(num_inputs=num_features, num_channels=[32, 32], kernel_size=2, dropout=0.2)
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    model = model.to(device)

    criterion = nn.BCELoss()
    optimizer = optim.Adam(model.parameters(), lr=0.001)

    # Training loop
    num_epochs = 1
    model.train()
    for epoch in range(num_epochs):
        with tqdm(train_loader, unit="batch") as tepoch:
            for batch_X, batch_y in tepoch:
                tepoch.set_description(f"Epoch {epoch+1}")
                batch_X, batch_y = batch_X.to(device), batch_y.to(device)
                optimizer.zero_grad()
                # TCN expects input shape (batch, seq_len, features)
                outputs = model(batch_X)
                loss = criterion(outputs.squeeze(), batch_y)
                loss.backward()
                optimizer.step()
                tepoch.set_postfix(loss=loss.item())

    print("Training complete.")

    torch.save(model.state_dict(), "tcn_model.pth")

    model = TCN(num_inputs=num_features, num_channels=[32, 32], kernel_size=2, dropout=0.2).to(device)
    model.load_state_dict(torch.load("gru_model_fast.pth"))

    # Load test data
    test_data = pd.read_csv(r"../measurement/processed_data/moving_all_around/random.csv", header=2)

    averaged_rows = []

    for i in range(0, len(test_data), window_size):
        chunk = test_data.iloc[i : i + window_size]
        chunk_mean = chunk.mean(numeric_only=True)
        # Use mean time for the new row
        chunk_mean["Time(s)"] = chunk["Time(s)"].mean()
        averaged_rows.append(chunk_mean)

    test_data = pd.DataFrame(averaged_rows, columns=test_data.columns)

    X_test = test_data[["Time(s)", "Acceleration X (g)", "Acceleration Y (g)", "Angular Momentum Z (dps)"]]

    # Apply IIR filter to test data (excluding time series)
    X_test_filtered = X_test.copy()
    X_test_filtered.iloc[:, 1:] = filtfilt(b, a, X_test.iloc[:, 1:], axis=0)

    # Standardize the test data
    X_test_scaled = scaler.transform(X_test_filtered.iloc[:, 1:])

    # Convert test data to PyTorch tensor
    X_test_tensor = torch.tensor(X_test_scaled, dtype=torch.float32).to(device)

    # Prepare test dataset with sequence windows and change to batch prediction
    test_dataset = TimeSeriesDataset(X_test_tensor, torch.zeros(len(X_test_tensor)), sequence_length, 10)
    test_loader = torch.utils.data.DataLoader(test_dataset, batch_size=batch_size, shuffle=False)

    all_preds = []
    model.eval()
    with torch.no_grad():
        for batch_X, _ in test_loader:
            batch_X = batch_X.to(device)  # shape: (batch, seq_len, features)
            batch_preds = model(batch_X).squeeze() > 0.5
            all_preds.extend(batch_preds.tolist())

    # Align predictions with the original test indices.

    pred_array = np.full(len(X_test_tensor), np.nan)
    for i, pred in enumerate(all_preds):
        index = i + (sequence_length - 1) * step
        if index < len(pred_array):
            pred_array[index] = pred

    test_data["predicted_heel_button"] = pred_array

    import plotly.graph_objects as go

    # Plot Angular Momentum Z axis (filtered and scaled)
    fig = go.Figure()
    # [:, 5]
    fig.add_trace(go.Scatter(x=test_data["Time(s)"], y=X_test_scaled[:, 0], mode="lines", name="AC X (Filtered & Scaled)"))
    fig.add_trace(go.Scatter(x=test_data["Time(s)"], y=X_test_scaled[:, 1], mode="lines", name="AC Y (Filtered & Scaled)"))
    fig.add_trace(go.Scatter(x=test_data["Time(s)"], y=X_test_scaled[:, 2], mode="lines", name="AM Z (Filtered & Scaled)"))
    # fig.add_trace(go.Scatter(x=test_data["Time(s)"], y=test_data["Heel Button"], mode="lines", name="Heel Button"))
    fig.add_trace(go.Scatter(x=test_data["Time(s)"], y=test_data["Stand detected"], mode="lines", name="Analytical"))
    fig.add_trace(go.Scatter(x=test_data["Time(s)"], y=test_data["predicted_heel_button"], mode="lines", name="Predicted Heel Button"))

    equal_values = (test_data["predicted_heel_button"] == test_data["Stand detected"]).sum()

    print(f"{equal_values} out of {test_data.shape[0]} entries are equal, which is {equal_values / test_data.shape[0] * 100:.2f}%")

    fig.show()
