#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Implementation of a basic byte pair encoder while learning C


typedef struct {
    char *text;
    size_t length;
} Input;

typedef struct {
    char first;
    char second;
    int count;
} BytePair;

typedef struct {
    char *value;
    int id;
} Token;

typedef struct {
    Token *tokens;
    size_t tok_count;
    size_t v_capacity;
} Vocabulary;

Input *create_input(const char *text) {
    // Casting for void *(Input * in this case) for clarity while learning.
    // Modern compiler does not require the cast.
    Input *input = (Input *)malloc(sizeof(Input));
    if (!input) {return NULL;} 
    input->length =strlen(text);
    input->text = (char *)malloc(input->length +1);
    if (!input->text){
        free(input);
        return NULL;
    }
    strcpy(input->text, text); // copies the string, including the null terminator, from text to input->text memory block
    return input;
}

Vocabulary *create_vocabulary(const char *text) {
    Vocabulary *vocab = (Vocabulary *)malloc(sizeof(Vocabulary));
    if (!vocab) {return NULL;}

    vocab->tok_count = 0;
    vocab->v_capacity = 128;  
    vocab->tokens = (Token *)malloc(vocab->v_capacity * sizeof(Token));
    if (!vocab->tokens) {
        free(vocab);
        return NULL;
    }

    int id = 0;
    char processed[256] = {0};
    for (size_t i = 0; text[i]; i++){
        if (!processed[(unsigned char)text[i]]){
            processed[(unsigned char)text[i]] = 1;
            // allocates 2 bytes for the value of the token
            vocab->tokens[vocab->tok_count].value = (char *)malloc(2);
            // assigns the character at current index of text to that block, and a null termination to the second byte
            vocab->tokens[vocab->tok_count].value[0] = text[i];
            vocab->tokens[vocab->tok_count].value[1] = '\0';
            vocab->tokens[vocab->tok_count].id = id++;
            vocab->tok_count++;
        }
    }
    return vocab;
}
// finds a frequent bytepair in the input data
BytePair find_pair (Input *input){
    BytePair max_pair = {0, 0, 0};
    int counts[256][256] = {0};

    for (size_t i = 0; i < input->length - 1; i++){
        unsigned char first = (unsigned char)input->text[i];
        unsigned char second = (unsigned char)input->text[i + 1];
        counts[first][second]++;
        if(counts[first][second] > max_pair.count){
            max_pair.first = first;
            max_pair.second = second;
            max_pair.count = counts[first][second];
        }
    }
    return max_pair;
}

// free functions