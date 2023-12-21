# Phase_1

In Phase 1 of our experiment, we explore the capability of GPT-4 in providing explanations for C programming code. Our objective is to assess the AI model's proficiency in understanding and elucidating programming concepts, which is critical for educational purposes and assisting novice programmers.

# Data

## Source

We collected 70 C programming exercises from [Programiz](https://www.programiz.com/), a renowned tutorial website. This source was chosen due to its comprehensive coverage of programming concepts, ranging from basic to advanced levels.

## **Structure**

The data is organized in a **`.csv`** file with the following columns:

- **`id`**: Problem number on the website.
- **`problem_name`**: Title of the problem.
- **`explanation`**: Human-written explanations for each problem.
- **`output`**: Expected output of the code.
- **`original_code`**: The complete C code solution, typically containing comments.
- **`comment_less_code`**: The original code stripped of comments for a cleaner look.

## **Why This Data?**

This dataset was selected for its diversity in programming problems and its simplicity of application in learning environments. The structured format facilitates systematic analysis and comparison.

# ****Experiment Design****

## **GPT-4**

We choose GPT-4 as our chosen model for this phase of the experiment, as this model is the most capable OpenAI model, as well as one of the best current LLMs through all open source and commercial models at this time.  

## **BLEU Score Evaluation**

We utilized the BLEU score, a standard metric for evaluating machine-generated text against human-written references, to assess the quality of explanations provided by GPT-4 compared to the original explanations in the dataset.

## **Token Estimation**

For each scenario, we estimated the token size of prompts and responses to manage our usage of the GPT-4 model efficiently and based on [OpenAI API pricing](https://openai.com/pricing). Based on OpenAI's tokenizer, each 4 characters are one token. As a result, our token estimation for each scenario in the phase_1 is based on the following:

- Input
    
    **`((fixed prompt # of token + average of code # of characters)/4) * 70 samples * input gpt-4 api price`**
    
- Output
    
    `**(requested output characters 80 / 4) * 70 samples * output gpt-4 api pricing**`
    

## Scenario_1

<img src="./images/Untitled 1.svg" alt="phase1" width="600"/>

In the first scenario, we will pass commented original code to the GPT-4 and ask the model for generating explanation on the code. Then, we'll store the result for later BLEU score calculation versus original explanation on the application that we gathered from the source code website. We will query 70 times on GPT4 and storing responses on a `.csv` file 

### Prompt

```
Problem Name: <problem_name>

<code> 

Please provide an explanation of this C code in a concise and straightforward manner, similar to a brief textbook definition. Focus on the main components and their basic functions, avoiding technical jargon. Aim for an explanation of around 60-80 words, suitable for beginners in programming.
```

- This prompt contains the <problem_name> for guiding better the model on what it's going to explain.
- The code which is being used for this prompt is the <code> which contains reasonable comments on the code.
- Also, we have `C code` comment for giving the model about the programming language.

### Experimental Data

Dat for this phase can be found in this [link](https://github.com/sinapordanesh/ENEL-592-Project/tree/main/data/phase_1).

- `phase_1_data.csv`

## Scenario_2

<img src="./images/Untitled 2.svg" alt="phase2" width="600"/>

In the second scenario, we will pass **un-commented** original code to the GPT-4 and ask the model for generating explanation on the code. We also removed the problem_name from the prompt for making the code a bit more unclear. Then, we'll store the result for later BLEU score calculation versus original explanation on the application that we gathered from the source code website. We will query 70 times on GPT4 and storing responses on a `.csv` file 

### Prompt

```
<code> 

Please provide an explanation of this code in a concise and straightforward manner, similar to a brief textbook definition. Focus on the main components and their basic functions, avoiding technical jargon. Aim for an explanation of around 60-80 words, suitable for beginners in programming.
```

- This prompt contains the <problem_name> for guiding better the model on what it's going to explain.
- The uncommented code will be placed on <code>.

### Experimental Data

Dat for this phase can be found in this [link](https://github.com/sinapordanesh/ENEL-592-Project/tree/main/data/phase_1).

- `phase_1_data.csv`

## Scenario_3

<img src="./images/Untitled 3.svg" alt="phase3" width="600"/>


In the third scenario, we will pass decompiled code to the GPT-4 and ask the model for generating explanation on the code. For reaching to the decompiled code, we first compiled the code using gcc and then decompiled all resulted binary using Ghidra. Afterwards, we gathered all resulted decompiled code on a new `.csv` file. Then, we'll store the result for later BLEU score calculation versus original explanation on the application that we gathered from the source code website. We will query 70 times on GPT4 and storing responses on a `.csv` file.

### Prompt

```
<code> 

Please provide an explanation of this C code in a concise and straightforward manner, similar to a brief textbook definition. Focus on the main components and their basic functions, avoiding technical jargon. Aim for an explanation of around 60-80 words, suitable for beginners in programming.
```

- We put decompiled code on <code> section of each prompt in each iteration over dataset.

### Experimental Data

Dat for this phase can be found in this [link](https://github.com/sinapordanesh/ENEL-592-Project/tree/main/experiment/data/phase_1).

- `phase_1_data.csv`
- `phase_1_data_decompiled`