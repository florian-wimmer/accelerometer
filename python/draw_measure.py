#!/usr/bin/python3
import pandas as pd
import plotly.graph_objects as go
import plotly.io as pio
from plotly.subplots import make_subplots
from scipy.signal import iirfilter, filtfilt

# setting to disable window in pdf export
pio.kaleido.scope.mathjax = None

pio.templates["default_theme"] = go.layout.Template(
    layout=go.Layout(
        font=dict(
            size=25,
            color="black",
            family="Times New Roman",
        ),
        xaxis=dict(showgrid=True),
        yaxis=dict(showgrid=True),
        width=920,
        height=360,
    )
)

pio.templates.default = "simple_white" + "+default_theme"

# Acceleration Axis Description
# X: Parallel to the Leg (Up)
# Y: Normal to the Leg (Forward)
# Z: Normal to the Leg (Right)

# Angular Momentum Axis Description
# X: Leg Rotation around its own axis
# Y: Leg Rotation from left to right
# Z: Leg Rotation from front to back

# Most Useful Axis (guess)
# Acc X and Y
# Angular Z

# test filter


def main():
    name = "acceleration_data"
    figure = go.Figure()

    lstm = pd.read_csv("../measurement/processed_data/test.csv", header=2)

    lstm_data = lstm[["Time(s)", "Acceleration X (g)", "Acceleration Y (g)", "Acceleration Z (g)", "Angular Momentum X (dps)", "Angular Momentum Y (dps)", "Angular Momentum Z (dps)"]]

    cutoff_frequency = 100
    sampling_frequency = 1 / (lstm["Time(s)"].diff().mean())

    b, a = iirfilter(4, Wn=cutoff_frequency, fs=sampling_frequency, btype="low", ftype="butter")
    lstm_filt = lstm_data.copy()
    lstm_filt.iloc[:, 1:] = filtfilt(b, a, lstm_data.iloc[:, 1:], axis=0)

    lstm_filt["Acceleration X (g)"] = lstm_filt["Acceleration X (g)"] / 10
    lstm_filt["Acceleration Y (g)"] = lstm_filt["Acceleration Y (g)"] / 10
    lstm_filt["Acceleration Z (g)"] = lstm_filt["Acceleration Z (g)"] / 10
    lstm_filt["Angular Momentum X (dps)"] = lstm_filt["Angular Momentum X (dps)"] / 100
    lstm_filt["Angular Momentum Y (dps)"] = lstm_filt["Angular Momentum Y (dps)"] / 100
    lstm_filt["Angular Momentum Z (dps)"] = lstm_filt["Angular Momentum Z (dps)"] / 100

    def adjust_time_in_dfs(df):
        df = df[df["Time(s)"] > 6.8]
        df["Time(s)"] = df["Time(s)"] - df.iloc[0]["Time(s)"]
        df = df[df["Time(s)"] < 0.6]
        df["Time(s)"] = df["Time(s)"] - df.iloc[0]["Time(s)"]
        return df

    lstm_filt = adjust_time_in_dfs(lstm_filt)
    lstm = adjust_time_in_dfs(lstm)

    figure.add_trace(go.Scatter(x=lstm_filt["Time(s)"], y=lstm_filt["Acceleration X (g)"], mode="lines", name="Acceleration X [g/10]"))
    figure.add_trace(go.Scatter(x=lstm_filt["Time(s)"], y=lstm_filt["Acceleration Y (g)"], mode="lines", name="Acceleration Y [g/10]"))
    figure.add_trace(go.Scatter(x=lstm_filt["Time(s)"], y=lstm_filt["Acceleration Z (g)"], mode="lines", name="Acceleration Z [g/10]"))
    figure.add_trace(go.Scatter(x=lstm_filt["Time(s)"], y=lstm_filt["Angular Momentum X (dps)"], mode="lines", name="Angular Velocity X [dps/100]"))
    figure.add_trace(go.Scatter(x=lstm_filt["Time(s)"], y=lstm_filt["Angular Momentum Y (dps)"], mode="lines", name="Angular Velocity Y [dps/100]"))
    figure.add_trace(go.Scatter(x=lstm_filt["Time(s)"], y=lstm_filt["Angular Momentum Z (dps)"], mode="lines", name="Angular Velocity Z [dps/100]"))

    figure.add_trace(go.Scatter(x=lstm_filt["Time(s)"], y=lstm["Heel Button"], mode="lines", name="Heel Button"))

    figure.update_layout(
        legend=dict(x=1.10),
        xaxis=dict(title="Time (s)"),
        yaxis=dict(
            title="Measurement Values",
        ),
        margin=dict(l=10, r=10, t=10, b=10),
    )

    figure.write_image("image/" + name + ".png")
    # figure.show()
    # figure.write_html("../measurement/html/" + name + ".html")
    # figure.write_image("../measurement/image/" + name + ".png")


if __name__ == "__main__":
    main()
