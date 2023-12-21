import pandas as pd
from openai import OpenAI
import time
import os
import concurrent.futures


# Set your OpenAI API key
client = OpenAI(
  api_key=os.environ['OPENAI_API_KEY'], 
)

# Function to create the prompt
def create_prompt(code):
    return f"""I have a section of C code and need to know specific aspects of its functionality and security implications. Please answer each of the following questions with either 'Y' for Yes or 'N' for No, and match your answers with the corresponding question ID.\nCan you identify the main functions in the code? (YN)\nIs this code a network-related program? (YN)\nDo you think the code is scalable? (YN)\nDoes the code make calls to suspicious or potentially harmful APIs, such as those related to keyboard logging, screen capturing, or modifying system files? (YN)\nDoes the code establish network connections or send data over the network without user consent? (YN)\nAre there routines for encrypting files or data that seem unrelated to the program's core functionality? (YN)\nDoes the code attempt to hide its presence or activity, such as disguising its processes or modifying log files? (YN)\nAre there mechanisms ensuring the code executes on system startup or remains active in the background? (YN)\nHere is the code snippet:\n{code}\nI need the answers in a concise format, with each response directly corresponding to the question number.\nJust return Y or N in front of each question ID, without any extra word!!"""

# Function to send the prompt to GPT-4 and get the response
def get_gpt4_response(prompt):
    try:
        response = client.chat.completions.create(
            model="gpt-4-1106-preview",
            messages=[
                {"role": "system", "content": "You are a reverse engineer helper."},
                {"role": "user", "content": prompt}
            ]
        )
        return response.choices[0].message.content
    except:
        print(f"An error occurred: {e}")
        return "Timeout or error occurred." 


def status(counter, total):

    # Calculate the percentage
    percent_complete = (counter / total) * 100

    # Simple visual representation of the progress bar
    bar = '#' * (counter * 2)  # Multiply by 2 for visual effect, adjust as needed

    # Print the progress bar with the percentage
    print(f'\rProgress: [{bar:30}] {percent_complete:.2f}%', end='')


# Read the CSV file
df = pd.read_csv('phase_2.csv')

# Create a new column for responses
df['original_binary'] = ''
df['decompiled_binary_Ghidra'] = ''
df['decompiled_binary_RetDec'] = ''

print("Original code questioning starts...\n")
counter = 0
# Loop through each row
for index, row in df.iterrows():
    prompt = create_prompt(row['original_code'])

    #response = get_gpt4_response(prompt)
    # Using concurrent.futures to implement a timeout
    with concurrent.futures.ThreadPoolExecutor() as executor:
        future = executor.submit(get_gpt4_response, prompt)
        try:
            response = future.result(timeout=25)  # Timeout in seconds
        except concurrent.futures.TimeoutError:
            response = "Timeout occurred."
    
    df.at[index, 'original_binary'] = response
    status(counter, 15)
    counter+=1

print("Ghidra decompiled code questioning starts...\n")
counter = 0
# Loop through each row
for index, row in df.iterrows():
    prompt = create_prompt(row['decompiled_Ghidra'])

    #response = get_gpt4_response(prompt)
    # Using concurrent.futures to implement a timeout
    with concurrent.futures.ThreadPoolExecutor() as executor:
        future = executor.submit(get_gpt4_response, prompt)
        try:
            response = future.result(timeout=25)  # Timeout in seconds
        except concurrent.futures.TimeoutError:
            response = "Timeout occurred."
    
    df.at[index, 'decompiled_binary_Ghidra'] = response
    status(counter, 15)
    counter+=1

print("RetDec decompiled code questioning starts...\n")
counter = 0
# Loop through each row
for index, row in df.iterrows():
    prompt = create_prompt(row['decompiled_RetDec'])

    #response = get_gpt4_response(prompt)
    # Using concurrent.futures to implement a timeout
    with concurrent.futures.ThreadPoolExecutor() as executor:
        future = executor.submit(get_gpt4_response, prompt)
        try:
            response = future.result(timeout=25)  # Timeout in seconds
        except concurrent.futures.TimeoutError:
            response = "Timeout occurred."
    
    df.at[index, 'decompiled_binary_RetDec'] = response
    status(counter, 15)
    counter+=1

# Write the updated dataframe to a new CSV file
df.to_csv('p2_s3_result.csv', index=False)