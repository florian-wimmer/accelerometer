#!/usr/bin/python3
import pandas as pd
import plotly.graph_objects as go
import plotly.io as pio
import numpy as np
import scipy.signal

# setting to disable window in pdf export
# pio.kaleido.scope.mathjax = None

pio.templates["default_theme"] = go.layout.Template(
    layout=go.Layout(
        font=dict(
            size=32,
            color="black",
            family="Computer Modern",
        ),
        title=dict(
            x=0.5,
        ),
        xaxis=dict(showgrid=True),
        yaxis=dict(showgrid=True),
        width=1784,
        height=892,
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
    # checkDelta()
    plotData()


def plotData():
    name = "acceleration"
    figure = go.Figure()

    df = pd.read_csv("../measurement/data/output_toe_heel_short.csv", header=2)

    df["Time(s)"] = df["Time(s)"] - df.iloc[0]["Time(s)"]
    df = df[df["Time(s)"] > 6]
    df["Time(s)"] = df["Time(s)"] - df.iloc[0]["Time(s)"]
    df = df[df["Time(s)"] < 6.4]
    # df = df[df["Time(s)"] < 200]

    sampling_frequency = 1 / (df["Time(s)"].diff().mean())
    cutoff_frequency_acc = 100
    cutoff_frequency_am = 150

    print("Sampling Frequency: ", sampling_frequency)

    b, a = scipy.signal.iirfilter(4, Wn=cutoff_frequency_acc, fs=sampling_frequency, btype="low", ftype="butter")
    c, d = scipy.signal.iirfilter(4, Wn=cutoff_frequency_am, fs=sampling_frequency, btype="low", ftype="butter")

    df["Acceleration X (g)"] = scipy.signal.lfilter(b, a, df["Acceleration X (g)"])
    df["Acceleration Y (g)"] = scipy.signal.lfilter(b, a, df["Acceleration Y (g)"])
    df["Acceleration Z (g)"] = scipy.signal.lfilter(b, a, df["Acceleration Z (g)"])
    df["Angular Momentum X (dps)"] = scipy.signal.lfilter(c, d, df["Angular Momentum X (dps)"])
    df["Angular Momentum Y (dps)"] = scipy.signal.lfilter(c, d, df["Angular Momentum Y (dps)"])
    df["Angular Momentum Z (dps)"] = scipy.signal.lfilter(c, d, df["Angular Momentum Z (dps)"])

    figure.add_trace(go.Scatter(x=df["Time(s)"], y=df["Acceleration X (g)"] / 16, mode="lines", name="Acceleration X (g)"))
    figure.add_trace(go.Scatter(x=df["Time(s)"], y=df["Acceleration Y (g)"] / 16, mode="lines", name="Acceleration Y (g)"))
    # figure.add_trace(go.Scatter(x=df["Time(s)"], y=df["Acceleration Z (g)"] / 16, mode="lines", name="Acceleration Z (g)"))

    # figure.add_trace(go.Scatter(x=df["Time(s)"], y=df["Angular Momentum X (dps)"] / 490, mode="lines", name="Angular Momentum X (dps)"))
    # figure.add_trace(go.Scatter(x=df["Time(s)"], y=df["Angular Momentum Y (dps)"] / 400, mode="lines", name="Angular Momentum Y (dps)"))
    figure.add_trace(go.Scatter(x=df["Time(s)"], y=df["Angular Momentum Z (dps)"] / 400, mode="lines", name="Angular Momentum Z (dps)"))

    # not really usefull
    # df["Derivative"] = np.append(np.diff(df["Acceleration X (g)"]) * sampling_frequency, np.nan)
    # figure.add_trace(go.Scatter(x=df["Time(s)"], y=df["Derivative"] / (200 * 16), mode="lines", name="Derivative Z (dps)"))

    df["Low Variance Signal"] = 3 * (df["Angular Momentum Z (dps)"] / 400) * (df["Acceleration Y (g)"] / 16)
    # df["Low Variance Signal"] = abs(df["Low Variance Signal"])

    figure.add_trace(go.Scatter(x=df["Time(s)"], y=df["Low Variance Signal"], mode="lines", name="Mixed"))

    # no useful data
    # figure.add_trace(go.Scatter(x=df["Time(s)"], y=df["Toe Button"] * 400, mode="lines", name="Toe Button"))

    df = fix_button_detection(df)
    figure.add_trace(go.Scatter(x=df["Time(s)"], y=df["Heel Button"], mode="lines", name="Heel Button"))

    df = analytical_step_detection(df)
    figure.add_trace(go.Scatter(x=df["Time(s)"], y=df["Stand detected"], mode="lines", name="Stand detected"))
    # figure.add_trace(go.Scatter(x=df["Time(s)"], y=df["Flight detected"], mode="lines", name="Flight detected"))
    # figure.add_trace(go.Scatter(x=df["Time(s)"], y=df["Stand init detected"], mode="lines", name="Stand init detected"))

    figure.add_trace(go.Scatter(x=df["Time(s)"], y=abs(df["Heel Button"] - df["Stand detected"]), mode="lines", name="Diffrenece"))

    equal_values = (df["Heel Button"] == df["Stand detected"]).sum()

    print(f"{equal_values} out of {df.shape[0]} entries are equal, which are {equal_values / df.shape[0] * 100:.2f}%")

    figure.update_layout(
        title=dict(
            text="Measurement Data",
        ),
        legend=dict(x=1.10),
        xaxis=dict(title="Time (s)"),
        yaxis=dict(
            title="Measurement",
        ),
    )

    figure.show()
    figure.write_html("../measurement/html/" + name + ".html")
    # figure.write_image("../measurement/image/" + name + ".png")


def checkDelta():
    """
    Check the delta_t column of the output.csv file and print the first 100 elements with the highest delta_t value.
    """
    df = pd.read_csv("../measurement/data/output.csv", header=2)

    df["delta_t"] = df["Time(s)"].diff()

    # Sort the DataFrame by the delta_t column in descending order
    sorted_df = df.sort_values(by="delta_t", ascending=False)

    # Get the first 100 elements of the sorted DataFrame
    first_100_max_delta_t = sorted_df.head(100)

    # Iterate over the first 100 rows and print index and delta_t value
    for i, (idx, row) in enumerate(first_100_max_delta_t.iterrows()):
        print(f"Pos: {i},Index: {idx}, Delta_t value: {row['delta_t']}")


def fix_button_detection(df, threshold=0.05):
    """
    df - data fram with 'Heel Button' column and 'Time(s)' column
    threshold - threshold for the duration of the short segments
    """

    # Identify where "Toe Button" is 0
    zero_segments = df["Heel Button"] == 0

    # Find the start and end of each zero segment
    df["Segment"] = (zero_segments != zero_segments.shift()).cumsum()
    zero_df = df[zero_segments]

    # Group by segments and calculate the duration
    segment_durations = zero_df.groupby("Segment").agg(start_time=("Time(s)", "first"), end_time=("Time(s)", "last"))

    segment_durations["duration"] = segment_durations["end_time"] - segment_durations["start_time"]

    # Find segments shorter than or equal to 0.075 seconds
    short_segments = segment_durations[segment_durations["duration"] <= threshold]

    # Change 'Toe Button' to 1 for these short segments
    for segment in short_segments.index:
        df.loc[df["Segment"] == segment, "Heel Button"] = 1

    # Drop the 'Segment' column as it was only needed for processing
    df.drop(columns=["Segment"], inplace=True)

    return df


def analytical_step_detection(df, threshold=0.1, threshold2=0.2):

    zero_crossing = 0
    flight_phase = False
    stand_phase_init = False
    stand_phase_state = 0

    df["Stand detected"] = 0
    df["Flight detected"] = 0
    df["Stand init detected"] = 0

    for index, row in df.iterrows():
        value = row["Low Variance Signal"]
        value_x = row["Acceleration X (g)"] / 16

        if flight_phase:
            df.at[index, "Flight detected"] = 1
        if stand_phase_init:
            df.at[index, "Stand init detected"] = 1

        # reset stand phase
        if value_x > threshold2 and flight_phase:
            zero_crossing = 4
            stand_phase_init = True
            flight_phase = False

        # detect flight phase
        if ((value < (threshold * -1)) or flight_phase) and not stand_phase_init:
            df.at[index, "Stand detected"] = 0
            flight_phase = True

            if zero_crossing == 0:
                if value > 0:
                    zero_crossing = 1
            if zero_crossing == 1:
                if value < 0:
                    zero_crossing = 2
            if zero_crossing == 2:
                if value > 0:
                    zero_crossing = 3
            if zero_crossing == 3:
                if value < 0:
                    zero_crossing = 4
                    flight_phase = False
                    stand_phase_init = True
        else:
            df.at[index, "Stand detected"] = 1
            zero_crossing = 0

            # wait for vibration to end
            if stand_phase_init and value_x > threshold2:
                stand_phase_state = 1
            if stand_phase_state == 1:
                if value_x < 0:
                    stand_phase_state = 0
                    stand_phase_init = False

    return df


main()
