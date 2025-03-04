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
        width=960,
        height=480,
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
    name = "stand_prediction"
    figure = make_subplots(rows=6, cols=1, shared_xaxes=True, vertical_spacing=0.05)

    anly = pd.read_csv("../measurement/processed_data/comparison/analytic_backward.csv", header=0)
    gru = pd.read_csv("../measurement/processed_data/comparison/gru_backward.csv", header=0)
    lstm = pd.read_csv("../measurement/processed_data/comparison/lstm_backward.csv", header=0)
    rf = pd.read_csv("../measurement/processed_data/comparison/rf_backward.csv", header=0)
    tcn = pd.read_csv("../measurement/processed_data/comparison/tcn_backward.csv", header=0)

    lstm_data = lstm[["Time(s)", "Acceleration X (g)", "Acceleration Y (g)", "Acceleration Z (g)", "Angular Momentum X (dps)", "Angular Momentum Y (dps)", "Angular Momentum Z (dps)"]]

    cutoff_frequency = 100
    sampling_frequency = 1 / (lstm["Time(s)"].diff().mean())

    b, a = iirfilter(4, Wn=cutoff_frequency, fs=sampling_frequency, btype="low", ftype="butter")
    lstm_filt = lstm_data.copy()
    lstm_filt.iloc[:, 1:] = filtfilt(b, a, lstm_data.iloc[:, 1:], axis=0)

    lstm_filt["Acceleration X (g)"] = lstm_filt["Acceleration X (g)"] / lstm_filt["Acceleration X (g)"].max()
    lstm_filt["Acceleration Y (g)"] = lstm_filt["Acceleration Y (g)"] / lstm_filt["Acceleration Y (g)"].max()
    lstm_filt["Acceleration Z (g)"] = lstm_filt["Acceleration Z (g)"] / lstm_filt["Acceleration Z (g)"].max()
    lstm_filt["Angular Momentum X (dps)"] = lstm_filt["Angular Momentum X (dps)"] / lstm_filt["Angular Momentum X (dps)"].max()
    lstm_filt["Angular Momentum Y (dps)"] = lstm_filt["Angular Momentum Y (dps)"] / lstm_filt["Angular Momentum Y (dps)"].max()
    lstm_filt["Angular Momentum Z (dps)"] = lstm_filt["Angular Momentum Z (dps)"] / lstm_filt["Angular Momentum Z (dps)"].max()

    def adjust_time_in_dfs(df):
        df = df[df["Time(s)"] > 0.55]
        df["Time(s)"] = df["Time(s)"] - df.iloc[0]["Time(s)"]
        df = df[df["Time(s)"] < 2]
        df["Time(s)"] = df["Time(s)"] - df.iloc[0]["Time(s)"]
        return df

    anly = adjust_time_in_dfs(anly)
    gru = adjust_time_in_dfs(gru)
    lstm = adjust_time_in_dfs(lstm)
    lstm_filt = adjust_time_in_dfs(lstm_filt)
    rf = adjust_time_in_dfs(rf)
    tcn = adjust_time_in_dfs(tcn)

    figure.add_trace(
        go.Scatter(x=lstm_filt["Time(s)"], y=lstm_filt["Angular Momentum Z (dps)"], mode="lines", name="Angular Velocity Z"),
        row=1,
        col=1,
    )

    figure.add_trace(
        go.Scatter(x=lstm_filt["Time(s)"], y=lstm_filt["Acceleration X (g)"], mode="lines", name="Acceleration X"),
        row=1,
        col=1,
    )

    figure.add_trace(
        go.Scatter(x=anly["Time(s)"], y=anly["predicted_heel_button"], mode="lines", name="ANLY"),
        row=2,
        col=1,
    )

    figure.add_trace(
        go.Scatter(x=gru["Time(s)"], y=gru["predicted_heel_button"], mode="lines", name="GRU"),
        row=3,
        col=1,
    )

    figure.add_trace(
        go.Scatter(x=lstm["Time(s)"], y=lstm["predicted_heel_button"], mode="lines", name="LSTM"),
        row=4,
        col=1,
    )

    figure.add_trace(
        go.Scatter(x=rf["Time(s)"], y=rf["predicted_heel_button"], mode="lines", name="RF"),
        row=5,
        col=1,
    )

    figure.add_trace(
        go.Scatter(x=tcn["Time(s)"], y=tcn["predicted_heel_button"], mode="lines", name="TCN"),
        row=6,
        col=1,
    )

    figure.update_layout(
        legend=dict(x=1.10),
    )

    figure.update_yaxes(tickmode="array", tickvals=[-1, 0, 1], row=1, col=1)

    figure.update_yaxes(title_text="Stand detected", row=3, col=1)
    figure.update_xaxes(title_text="Time [s]", row=6, col=1)

    figure.update_xaxes(matches="x2")
    figure.update_traces(xaxis="x6")

    figure.update_layout(
        legend=dict(
            yanchor="top",
            y=0.99,
            xanchor="right",
            x=0.995,
        ),
        margin=dict(l=1, r=1, t=1, b=1),
    )

    # figure.show()
    figure.write_image("image/" + name + ".pdf")
    # figure.write_html("../measurement/html/" + name + ".html")
    # figure.write_image("../measurement/image/" + name + ".png")


if __name__ == "__main__":
    main()
