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
    name = "stand_prediction_kang"
    figure = make_subplots(rows=4, cols=1, shared_xaxes=True, vertical_spacing=0.05)

    anly = pd.read_csv("../measurement/processed_data/comparison_kang/analytic_test.csv", header=0)
    rf = pd.read_csv("../measurement/processed_data/comparison_kang/rf_test.csv", header=0)
    tcn = pd.read_csv("../measurement/processed_data/comparison_kang/tcn_test.csv", header=0)

    data = rf[["Time(s)", "Acceleration X (g)", "Acceleration Y (g)", "Acceleration Z (g)", "Angular Momentum X (dps)", "Angular Momentum Y (dps)", "Angular Momentum Z (dps)"]]

    cutoff_frequency = 100
    sampling_frequency = 1 / (rf["Time(s)"].diff().mean())

    b, a = iirfilter(4, Wn=cutoff_frequency, fs=sampling_frequency, btype="low", ftype="butter")
    filt = data.copy()
    filt.iloc[:, 1:] = filtfilt(b, a, data.iloc[:, 1:], axis=0)

    filt["Acceleration X (g)"] = filt["Acceleration X (g)"] / filt["Acceleration X (g)"].max()
    filt["Acceleration Y (g)"] = filt["Acceleration Y (g)"] / filt["Acceleration Y (g)"].max()
    filt["Acceleration Z (g)"] = filt["Acceleration Z (g)"] / filt["Acceleration Z (g)"].max()
    filt["Angular Momentum X (dps)"] = filt["Angular Momentum X (dps)"] / filt["Angular Momentum X (dps)"].max()
    filt["Angular Momentum Y (dps)"] = filt["Angular Momentum Y (dps)"] / filt["Angular Momentum Y (dps)"].max()
    filt["Angular Momentum Z (dps)"] = filt["Angular Momentum Z (dps)"] / filt["Angular Momentum Z (dps)"].max()

    def adjust_time_in_dfs(df):
        df = df[df["Time(s)"] > 0.55]
        df["Time(s)"] = df["Time(s)"] - df.iloc[0]["Time(s)"]
        df = df[df["Time(s)"] < 10]
        df["Time(s)"] = df["Time(s)"] - df.iloc[0]["Time(s)"]
        return df

    anly = adjust_time_in_dfs(anly)
    filt = adjust_time_in_dfs(filt)
    rf = adjust_time_in_dfs(rf)
    tcn = adjust_time_in_dfs(tcn)

    figure.add_trace(
        go.Scatter(x=filt["Time(s)"], y=filt["Angular Momentum Z (dps)"], mode="lines", name="Angular Velocity Z"),
        row=1,
        col=1,
    )

    figure.add_trace(
        go.Scatter(x=filt["Time(s)"], y=filt["Acceleration X (g)"], mode="lines", name="Acceleration X"),
        row=1,
        col=1,
    )

    figure.add_trace(
        go.Scatter(x=anly["Time(s)"], y=anly["predicted_heel_button"], mode="lines", name="ANLY"),
        row=2,
        col=1,
    )

    figure.add_trace(
        go.Scatter(x=rf["Time(s)"], y=rf["predicted_heel_button"], mode="lines", name="RF"),
        row=3,
        col=1,
    )

    figure.add_trace(
        go.Scatter(x=tcn["Time(s)"], y=tcn["predicted_heel_button"], mode="lines", name="TCN"),
        row=4,
        col=1,
    )

    figure.update_layout(
        legend=dict(x=1.10),
    )

    figure.update_yaxes(tickmode="array", tickvals=[-1, 0, 1], row=1, col=1)

    figure.update_yaxes(title_text="Stand detected", row=3, col=1)
    figure.update_xaxes(title_text="Time [s]", row=6, col=1)

    figure.update_xaxes(matches="x2")
    figure.update_traces(xaxis="x4")

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
    figure.write_image("image/" + name + ".png")
    # figure.write_html("../measurement/html/" + name + ".html")
    # figure.write_image("../measurement/image/" + name + ".png")


if __name__ == "__main__":
    main()
