import pandas as pd
from openai import OpenAI
import time
import os

# Set your OpenAI API key
client = OpenAI(
  api_key=os.environ['OPENAI_API_KEY'], 
)

# Function to create the prompt
def create_prompt(problem_name, original_code):
    return f"Problem Name: {problem_name}\n\nOriginal Code:\n\n//C code\n{original_code}\n\nPlease provide an explanation of this code in a concise and straightforward manner, similar to a brief textbook definition. Focus on the main components and their basic functions, avoiding technical jargon. Aim for an explanation of around 60-80 words, suitable for beginners in programming."

# Function to send the prompt to GPT-4 and get the response
def get_gpt4_response(prompt):
    response = client.chat.completions.create(
        model="gpt-4",
        messages=[
            {"role": "system", "content": "You are a C code explainor."},
            {"role": "user", "content": prompt}
        ]
    )
    return response.choices[0].message.content

# Read the CSV file
df = pd.read_csv('phase_1_data.csv')

# Create a new column for responses
df['response'] = ''

# Loop through each row
for index, row in df.iterrows():
    prompt = create_prompt(row['problem_name'], row['original_code'])
    response = get_gpt4_response(prompt)
    # Wait a bit to avoid hitting rate limits
    time.sleep(12)  # Adjust the sleep time as needed
    df.at[index, 'response'] = response


# Write the updated dataframe to a new CSV file
df.to_csv('p1_s1.csv', index=False)