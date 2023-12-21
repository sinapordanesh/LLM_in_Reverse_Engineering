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
    return f""" I have a C code that I need explained from the perspectives of both a reverse engineer and a security specialist. The code is as follows:\n\n{code}\n\nPlease provide a detailed but concise analysis in no more than 200 words. Your explanation should cover the following points in a repetitive format for each code snippet:\nFunctionality Overview: Briefly describe the main purpose and functionality of the code.\nKey Observations by a Reverse Engineer: Highlight any notable structures, patterns, or algorithms from a reverse engineering standpoint.\nSecurity Analysis: Identify any potential security risks or vulnerabilities present in the code.\n\nRemember, the format and structure of your analysis should be consistent for each code snippet, as the responses will be compared using BLEU score.\n\nNO MORE THAN 200 WORDS!!"""

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
    bar = '#' * (counter * 2)  # Multiply by 2 for visual effect.

    # Print the progress bar with the percentage
    print(f'\rProgress: [{bar:30}] {percent_complete:.2f}%', end='')


# Read the CSV file
df = pd.read_csv('phase_2.csv')

# Create a new column for responses
df['original_ex'] = ''
df['decompiled_Ghidra_ex'] = ''
df['decompiled_RetDec_ex'] = ''

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
    
    df.at[index, 'original_ex'] = response
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
    
    df.at[index, 'decompiled_Ghidra_ex'] = response
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
    
    df.at[index, 'decompiled_RetDec_ex'] = response
    status(counter, 15)
    counter+=1

# Write the updated dataframe to a new CSV file
df.to_csv('p2_s4_result.csv', index=False)