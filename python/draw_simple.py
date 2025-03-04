#!/usr/bin/python3
import pandas as pd
import plotly.graph_objects as go
import plotly.io as pio

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
    name = "acceleration"
    figure = go.Figure()

    df = pd.read_csv("../measurement/processed_data/kanguru_test.csv", header=2)

    # df["Time(s)"] = df["Time(s)"] - df.iloc[0]["Time(s)"]
    # df = df[df["Time(s)"] > 600]
    # df["Time(s)"] = df["Time(s)"] - df.iloc[0]["Time(s)"]
    # df = df[df["Time(s)"] < 200]

    if False:
        # All
        for column in df.columns:
            if column != "Time(s)":
                figure.add_trace(go.Scatter(x=df["Time(s)"], y=df[column], mode="lines", name=column))
    else:
        df["Acceleration X (g)"] = df["Acceleration X (g)"] / df["Acceleration X (g)"].max()
        df["Acceleration Y (g)"] = df["Acceleration Y (g)"] / df["Acceleration Y (g)"].max()
        df["Acceleration Z (g)"] = df["Acceleration Z (g)"] / df["Acceleration Z (g)"].max()
        df["Angular Momentum X (dps)"] = df["Angular Momentum X (dps)"] / df["Angular Momentum X (dps)"].max()
        df["Angular Momentum Y (dps)"] = df["Angular Momentum Y (dps)"] / df["Angular Momentum Y (dps)"].max()
        df["Angular Momentum Z (dps)"] = df["Angular Momentum Z (dps)"] / df["Angular Momentum Z (dps)"].max()

        figure.add_trace(go.Scatter(x=df["Time(s)"], y=df["Acceleration X (g)"], mode="lines", name="Acceleration X (g)"))
        figure.add_trace(go.Scatter(x=df["Time(s)"], y=df["Acceleration Y (g)"], mode="lines", name="Acceleration Y (g)"))
        figure.add_trace(go.Scatter(x=df["Time(s)"], y=df["Acceleration Z (g)"], mode="lines", name="Acceleration Z (g)"))
        figure.add_trace(go.Scatter(x=df["Time(s)"], y=df["Angular Momentum X (dps)"], mode="lines", name="Angular Momentum X (dps)"))
        figure.add_trace(go.Scatter(x=df["Time(s)"], y=df["Angular Momentum Y (dps)"], mode="lines", name="Angular Momentum Y (dps)"))
        figure.add_trace(go.Scatter(x=df["Time(s)"], y=df["Angular Momentum Z (dps)"], mode="lines", name="Angular Momentum Z (dps)"))
        figure.add_trace(go.Scatter(x=df["Time(s)"], y=df["Heel Button"], mode="lines", name="Measured"))
        figure.add_trace(go.Scatter(x=df["Time(s)"], y=df["Stand detected"], mode="lines", name="Analytical"))
        # figure.add_trace(go.Scatter(x=df["Time(s)"], y=df["predicted_heel_button"], mode="lines", name="Prediciton"))

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
    # figure.write_html("../measurement/html/" + name + ".html")
    # figure.write_image("../measurement/image/" + name + ".png")


if __name__ == "__main__":
    main()
