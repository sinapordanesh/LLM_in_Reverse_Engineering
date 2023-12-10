import csv
import nltk
from nltk.translate.bleu_score import sentence_bleu
from nltk.tokenize import word_tokenize

# Download NLTK data (needed for tokenization)
nltk.download('punkt')

def calculate_bleu_score(reference, candidate):
    reference_tokens = word_tokenize(reference.lower())
    candidate_tokens = word_tokenize(candidate.lower())
    return sentence_bleu([reference_tokens], candidate_tokens)

def process_csv(input_file, output_file):
    with open(input_file, 'r', encoding='utf-8') as infile, open(output_file, 'w', newline='', encoding='utf-8') as outfile:
        reader = csv.DictReader(infile)
        fieldnames = reader.fieldnames + ['BLEU_Score_RetDec']
        writer = csv.DictWriter(outfile, fieldnames=fieldnames)
        
        writer.writeheader()
        for row in reader:
            original_ex = row['original_ex']
            decompiled_RetDec_ex = row['decompiled_RetDec_ex']
            bleu_score = calculate_bleu_score(original_ex, decompiled_RetDec_ex)
            row['BLEU_Score_RetDec'] = bleu_score
            writer.writerow(row)

# Replace 'input.csv' and 'output.csv' with your file paths
input_csv_file = 'p2_s4_result.csv'
output_csv_file = 'p2_s4_evaluation.csv'
process_csv(input_csv_file, output_csv_file)
