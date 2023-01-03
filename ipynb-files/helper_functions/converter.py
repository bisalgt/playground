# a simple program to process data from image processing module and Redpitaya's ultrasonic sensor and merging the data to json file based on timestamp.

import argparse
import pandas as pd
import numpy as np
import time

parser = argparse.ArgumentParser(
                    prog='ConverterProgram',
                    description='A simple program to compare and convert two input files : from redpitaya sensor data and imageprocessing module "json file" and save the merged data to csv file',
                    epilog='Program for processing sensor data and merging.'
                    )

parser.add_argument('-f_cv', '--filename_cv') # file from image processing module data, json file
parser.add_argument('-f_rps', '--filename_rps') # file from Redpitaya sensor, .txt file
# parser.add_argument('-o', '--filename_output', required=False)
parser.add_argument('-sametime', '--similar_timestamp', action='store_true')
parser.add_argument('-single_file', '--save_single_file', action='store_true')
parser.add_argument('-timestamp_col_rps', '--timestamp_column_rps', default=18)

args = parser.parse_args()
print("Image Processing Sensor Data : ", args.filename_cv)
print("RedPitaya Sensor Data: ", args.filename_rps)

# first processing the data from redpitaya

print("Working with RedPitaya Sensor Data ")

def replace_missing(x):
    if x in ['', None, np.nan]:
        return np.nan
    return x

start_rps = time.time()

df_rps = pd.read_csv(args.filename_rps, header=None, sep='\n')
df_rps = df_rps[0].str.split(pat='\t', expand=True)
df_rps[18] = df_rps[18].astype('int64')
df_rps = df_rps.applymap(replace_missing)

end_rps = time.time()

print("Time taken to process Redpitaya Sensor data : ", end_rps-start_rps)


# second - processing the data from computer vision model

print("Working with Image Processing module's data")

start_cv = time.time()

df_cv = pd.read_json(args.filename_cv)
#df_cv['timestamp'] = df_cv['timestamp'].apply(lambda x: x - 3600)
df_cv = df_cv[['timestamp', 'prediction']]

# if the timestamp is not similar, need to reduce 3600 because utc is 3600 seconds behind the local time of Germany
if not args.similar_timestamp:
    df_cv['timestamp'] = df_cv['timestamp'].apply(lambda x: x - 3600)

end_cv = time.time()

print("Time taken to process Image Processing module's data : ", end_cv - start_cv)

# if args.filename_output:
#     outputfile_name = args.filename_output + ".json"
#     outputfile_name_3cols = args.filename_output+"_3cols.json"
# else:
#     outputfile_name = "output.json"
#     outputfile_name_3cols = "output_3cols.json"



# merge the data based on timestamp

print("Merging data of two sensors! based on timestamp", args.timestamp_column_rps)

start_merge = time.time()

df_mg = pd.merge(df_cv, df_rps, how='right', right_on=args.timestamp_column_rps, left_on='timestamp')
df_mg.loc[df_mg['prediction'].isna(), 'prediction'] = 'others'
df_mg.dropna(axis=1, how='all', inplace=True)
df_mg.dropna(axis=0, how='all', inplace=True)

end_merge = time.time()


print("Time taken to merge the sensor data : ", end_merge - start_merge)


if args.save_single_file:
    df_mg.to_csv('combined_data.csv', index=False)
else:
    df_mg[df_mg['prediction'] == 'others'].to_csv('others.csv', index=False)
    df_mg[df_mg['prediction'] == 'person'].to_csv('person.csv', index=False)
    df_mg[df_mg['prediction'] == 'chair'].to_csv('chair.csv', index=False)


print("DONE!!!")

