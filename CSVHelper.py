import pandas as pd

# Read the CSV (no header to preserve structure)
df = pd.read_csv("/home/dksoren/DroneBoys/Images/dinmor.csv", header=None)

# Drop rows that are completely empty
df_clean = df.dropna(how='all')

# Optional: reset index
df_clean.reset_index(drop=True, inplace=True)


df_clean.to_csv("/home/dksoren/DroneBoys/Images/dataFinal_clean.csv", index=False)

print("Empty rows removed and cleaned file saved!")
