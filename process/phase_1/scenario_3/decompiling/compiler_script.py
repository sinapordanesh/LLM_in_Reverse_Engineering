import csv
import subprocess
import os

# Path to your CSV file
csv_file_path = './phase_1_data.csv'

# Directory to store compiled output files
output_dir = './binaries'

# Ensure the output directory exists
if not os.path.exists(output_dir):
    os.makedirs(output_dir)

counter = 0

# Read the CSV file
with open(csv_file_path, newline='') as csvfile:
    reader = csv.DictReader(csvfile)
  
    for row in reader:

        # Extract the ID and code
        row_id = row['\ufeffid']
        code = row['comment_less_code']

        # Name of the temporary .c file and output file
        temp_c_file = os.path.join(output_dir, f'temp_{row_id}.c')
        output_file = os.path.join(output_dir, f'binary_{row_id}')

        # Write the code to a temporary .c file
        with open(temp_c_file, 'w') as file:
            file.write(code)

        # Compile the code
        compile_command = f"gcc -O3 -s -fvisibility=hidden -flto -fno-unwind-tables -fno-asynchronous-unwind-tables -fmerge-all-constants -fomit-frame-pointer -fPIC -o {output_file} {temp_c_file}"
        result = subprocess.run(compile_command, shell=True, text=True, stderr=subprocess.PIPE)

        # Check for compilation errors
        if result.returncode != 0:
            print(f"Error compiling ID {row_id}: {result.stderr}")
        else:
            print(f"Successfully compiled ID {row_id}")

        # Optionally, remove the temporary .c file
        os.remove(temp_c_file)

        print(f"%{(counter / 70) * 100}")
        counter+=1

print("Compilation process completed.")
