class DataAnalyzer:
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
