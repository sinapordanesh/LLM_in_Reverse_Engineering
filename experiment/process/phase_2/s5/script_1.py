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
    return f""" Analyze the provided code snippet and answer the following questions. \nCode:\n\n{code}\n\nFor each of the following questions, provide a concise response of approximately 50 words only:\n1. What is the primary functionality of the code?\n2. Identify and describe the purpose of three key functions.\n3. Explain the role of a selected variable.\n4. Describe the error handling mechanism.\n5. Outline the flow of execution in a specific section.\n6. List any external libraries or dependencies.\n7. Highlight any evident security concerns.\n\nAPRROXIMATLY 350 WORDS IN RESPONSE ONLY! NO MORE!"""

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
df['original_questionnaire'] = ''
df['decompiled_Ghidra_questionnairex'] = ''
df['decompiled_RetDec_questionnaire'] = ''

print("\nOriginal code questioning starts...")
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
    
    df.at[index, 'original_questionnaire'] = response
    status(counter, 15)
    counter+=1

print("\nGhidra decompiled code questioning starts...")
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
    
    df.at[index, 'decompiled_Ghidra_questionnairex'] = response
    status(counter, 15)
    counter+=1

print("\nRetDec decompiled code questioning starts...")
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
    
    df.at[index, 'decompiled_RetDec_questionnaire'] = response
    status(counter, 15)
    counter+=1

# Write the updated dataframe to a new CSV file
df.to_csv('p2_s5_result.csv', index=False)