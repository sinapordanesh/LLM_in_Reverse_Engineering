"""import pandas as pd
from nltk.translate.bleu_score import sentence_bleu
from nltk.tokenize import word_tokenize

# Function to calculate BLEU score for two texts
def calculate_bleu(reference, candidate):
    reference_tokens = word_tokenize(reference.lower())
    candidate_tokens = word_tokenize(candidate.lower())
    return sentence_bleu([reference_tokens], candidate_tokens)

# Read the CSV file
df = pd.read_csv('p1_s3.csv')

# Calculate BLEU scores for each row and add to new column
df['bleu_score'] = df.apply(lambda row: calculate_bleu(row['explanation'], row['response_decompiled']), axis=1)

# Save the DataFrame with BLEU scores
df.to_csv('p1_s3_result.csv', index=False)"""

import pandas as pd
from nltk.translate.bleu_score import sentence_bleu
from nltk.tokenize import word_tokenize

# Function to calculate BLEU score for two texts
def calculate_bleu(reference, candidate):
    reference_tokens = [word_tokenize(reference.lower())]  # List of lists for reference
    candidate_tokens = word_tokenize(candidate.lower())   # List for candidate
    try:
        return sentence_bleu(reference_tokens, candidate_tokens)
    except ZeroDivisionError:
        return 0

# Read the CSV file
df = pd.read_csv('p1_s3_test.csv')

# Calculate BLEU scores for each row and add to new column
df['bleu_score'] = df.apply(lambda row: calculate_bleu(row['response_S1'], row['response_decompiled']), axis=1)

# Save the DataFrame with BLEU scores
df.to_csv('p1_s3_test.csv', index=False)
