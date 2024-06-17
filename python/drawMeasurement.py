#!/usr/bin/python3
import pandas as pd
import plotly.graph_objects as go
import plotly.io as pio

# setting to disable window in pdf export
#pio.kaleido.scope.mathjax = None

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

pio.templates.default = 'simple_white'+'+default_theme'


def main():
    plotData()


def plotData():
    name = "acceleration"
    figure = go.Figure()

    df = pd.read_csv("../measurement/data/output.csv", header = 2)

    # print(df)

    df["Time(s)"] = df["Time(s)"] - df.iloc[0]["Time(s)"]

    figure.add_trace(go.Scatter(x=df["Time(s)"], y=df["Acceleration X (g)"], mode="lines", name="Acceleration X (g)"))
    figure.add_trace(go.Scatter(x=df["Time(s)"], y=df["Acceleration Y (g)"], mode="lines", name="Acceleration Y (g)"))
    figure.add_trace(go.Scatter(x=df["Time(s)"], y=df["Acceleration Z (g)"], mode="lines", name="Acceleration Z (g)"))

    #figure.add_trace(go.Scatter(x=df["Time(s)"], y=df["Angular Momentum X (dps)"], mode="lines", name="Angular Momentum X (dps)"))
    #figure.add_trace(go.Scatter(x=df["Time(s)"], y=df["Angular Momentum Y (dps)"], mode="lines", name="Angular Momentum Y (dps)"))
    #figure.add_trace(go.Scatter(x=df["Time(s)"], y=df["Angular Momentum Z (dps)"], mode="lines", name="Angular Momentum Z (dps)"))

    figure.update_layout(
        title=dict(
            text='Measurement Data',
        ),
        legend=dict(
            x=1.10
        ),
        xaxis=dict(title='Time (s)'),
        yaxis=dict(
            title="Measurement",
        ),
    )

    figure.show()
    #figure.write_html("../measurement/html/" + name + ".html")
    #figure.write_image("../measurement/image/" + name + ".png")


main()
