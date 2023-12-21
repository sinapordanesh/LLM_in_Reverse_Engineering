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
    return f"Code:\n{code}\n\n\nPlease provide an explanation of this code in a concise and straightforward manner, similar to a brief textbook definition. Focus on the main components and their basic functions, avoiding technical jargon. Aim for an explanation of around 60-80 words, suitable for beginners in programming."


# Function to send the prompt to GPT-4 and get the response
def get_gpt4_response(prompt):
    try:
        response = client.chat.completions.create(
            model="gpt-4",
            messages=[
                {"role": "system", "content": "You are a code explainor."},
                {"role": "user", "content": prompt}
            ]
        )
        return response.choices[0].message.content
    except:
        print(f"An error occurred: {e}")
        return "Timeout or error occurred." 

# Read the CSV file
df = pd.read_csv('phase_1_data_decompiled.csv')

# Create a new column for responses
df['response_decompiled'] = ''

counter = 0
# Loop through each row
for index, row in df.iterrows():
    prompt = create_prompt(row['decompiled'])

    #response = get_gpt4_response(prompt)
    # Using concurrent.futures to implement a timeout
    with concurrent.futures.ThreadPoolExecutor() as executor:
        future = executor.submit(get_gpt4_response, prompt)
        try:
            response = future.result(timeout=15)  # Timeout in seconds
        except concurrent.futures.TimeoutError:
            response = "Timeout occurred."
    
    df.at[index, 'response_decompiled'] = response
    print(f"{(counter / 70) * 100}%")
    counter+=1

# Write the updated dataframe to a new CSV file
df.to_csv('p1_s3.csv', index=False)