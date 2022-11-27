# a simple program to process data from image processing module and Redpitaya's ultrasonic sensor and merging the data to json file based on timestamp.

import argparse
import pandas as pd

parser = argparse.ArgumentParser(
                    prog='ConverterProgram',
                    description='A simple program to compare and convert two input files from redpitaya sensor data and imageprocessing module"json" and save the merged data to output.json file',
                    epilog='This is an epilog'
                    )

parser.add_argument('-f_cv', '--filename_cv')
parser.add_argument('-f_rps', '--filename_rps')
parser.add_argument('-o', '--filename_output', required=False)
parser.add_argument('-same', '--similar_timestamp', action='store_true')
parser.add_argument('-3cols', '--save_3cols', action='store_true')

args = parser.parse_args()
print(args.filename_cv, " Printed fcv")
print(args.filename_rps, " Printed frps")

# first processing the data from redpitaya

df_rps = pd.read_csv(args.filename_rps, header=None, sep="\t")

df_rps.dropna(axis=1, inplace=True)

df_rps_data = df_rps.iloc[:, 18:]

df_rps_data.rename(columns={18: "timestamp"}, inplace=True)


# second - processing the data from computer vision model

df_cv = pd.read_json(args.filename_cv)

df_cv.drop(["total_count"], axis=1, inplace=True)

# if the timestamp is not similar, need to reduce 3600 because utc is 3600 seconds behind the local time of Germany
if not args.similar_timestamp:
    df_cv.loc[:, 'timestamp'] = df_cv.loc[:, 'timestamp'].apply(lambda x: x - 3600)

if args.filename_output:
    outputfile_name = args.filename_output + ".json"
    outputfile_name_3cols = args.filename_output+"_3cols.json"
else:
    outputfile_name = "output.json"
    outputfile_name_3cols = "output_3cols.json"



# merge the data based on timestamp
print(df_cv.columns, df_rps_data.columns)
df_merged = df_cv.merge(df_rps_data, on="timestamp")

df_merged.to_json(outputfile_name, orient="records")

if args.save_3cols:
    df_merged_3cols = pd.DataFrame()
    df_merged_3cols["timestamp"] = df_merged["timestamp"]
    df_merged_3cols["prediction"] = df_merged["prediction"]
    df_merged_3cols["data"] = df_merged.iloc[:, 2:].values.tolist()
    df_merged_3cols.to_json(outputfile_name_3cols, orient="records")