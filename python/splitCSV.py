import pandas as pd

if False:
    # Split the original CSV file into three parts with equal number of rows

    # Load the original CSV file
    input_file = "../measurement/data/output_toe_heel.csv"
    df = pd.read_csv(input_file, header=2)

    # Calculate the number of rows for each part
    total_rows = len(df)
    rows_per_part = total_rows // 3

    # Split the DataFrame into three parts
    df_part1 = df.iloc[:rows_per_part]
    df_part2 = df.iloc[rows_per_part : 2 * rows_per_part]
    df_part3 = df.iloc[2 * rows_per_part :]

    # Save each part into separate CSV files
    df_part1.to_csv("../measurement/data/output_toe_heel_part1.csv", index=False)
    df_part2.to_csv("../measurement/data/output_toe_heel_part2.csv", index=False)
    df_part3.to_csv("../measurement/data/output_toe_heel_part3.csv", index=False)

    print("CSV file split into three parts successfully!")

if True:
    # Split the original CSV file into different parts, dependent on the time
    folder = "../measurement/data/moving_all_around/"

    input_file = folder + "output_moving_all_around.csv"

    with open(input_file, "r") as f:
        header = [next(f) for _ in range(3)]
    df = pd.read_csv(input_file, header=2)

    stand = df.index[df["Time(s)"] > 20][0]
    forward = df.index[df["Time(s)"] > 40][0]
    backward = df.index[df["Time(s)"] > 60][0]
    left = df.index[df["Time(s)"] > 80][0]
    right = df.index[df["Time(s)"] > 100][0]
    random = df.index[-1]

    df_stand = df.iloc[:stand]
    df_forward = df.iloc[stand:forward]
    df_backward = df.iloc[forward:backward]
    df_left = df.iloc[backward:left]
    df_right = df.iloc[left:right]
    df_random = df.iloc[right:random]

    def save_with_header(df, filename):
        with open(filename, "w", newline="") as f:
            f.writelines(header)
            df.to_csv(f, header=False, index=False)

    save_with_header(df_stand, folder + "stand.csv")
    save_with_header(df_forward, folder + "forward.csv")
    save_with_header(df_backward, folder + "backward.csv")
    save_with_header(df_left, folder + "left.csv")
    save_with_header(df_right, folder + "right.csv")
    save_with_header(df_random, folder + "random.csv")

    print("CSV file split up successfull!")
