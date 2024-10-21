import pandas as pd

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
