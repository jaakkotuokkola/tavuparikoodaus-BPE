#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Yksinkertainen tavuparikoodaus toteutus.
/*   Tavuparikoodaus (Byte Pair Encoding, BPE): 
     Algoritmi, joka pakkaa tekstiä yhdistämällä toistuvasti yleisimmät
     vierekkäiset merkkiparit uusiksi tokeneiksi.
     Aloittaa yksittäisistä merkeistä ja luo "subword"-yksiköitä
      (esim. sana "jossain" voi muodostaa tokeneita,
       kuten "jo", "ss", "ain", 
       jos nämä tokenit esiintyvät syötteessä tarpeeksi).
     Algoritmi toistaa yhdistämistä,
     kunnes pareja ei löydetä tai sanaston koko saavuttaa
     rajan (esim. 50 000).
*/

// Simple BPE while learning C

#define MAX_VOCAB_SIZE 50000

typedef struct {
    char *value;
    int id;
} Token;

// vocabulary holds tokens and their ids
typedef struct {
    Token *tokens;
    size_t tok_count;
    size_t v_capacity;
} Vocabulary;

// the input is represented as a sequence of token ids
typedef struct {
    int *ids;
    size_t length;
} InputSeq;

// pair structure
// TODO: hash ?
typedef struct {
    int first, second;
    int count;
} PairCount;

// find token id in vocab
int vocab_find(Vocabulary *vocab, const char *val) {
    for (size_t i = 0; i < vocab->tok_count; i++) {
    // compare val with each token's value
        if (strcmp(vocab->tokens[i].value, val) == 0) return vocab->tokens[i].id;
    }
    return -1;
}

// add a new token to vocab
int vocab_add(Vocabulary *vocab, const char *val) {
        //resize
    if (vocab->tok_count >= vocab->v_capacity) {
        vocab->v_capacity *= 2;
        Token *new_tokens = realloc(vocab->tokens, vocab->v_capacity * sizeof(Token));
        if (!new_tokens) {
            fprintf(stderr, "malloc fail\n");
            exit(1);
        }
        vocab->tokens = new_tokens;
    }
    // copy the string value
    char *new_value = strdup(val);
    if (!new_value) {
        fprintf(stderr, "malloc fail\n");
        exit(1);
    }
    // assign the new token and increment the count
    vocab->tokens[vocab->tok_count].value = new_value;
    vocab->tokens[vocab->tok_count].id = vocab->tok_count;
    return vocab->tok_count++;
}

// initialize input sequence by splitting the text into tokens
InputSeq *create_input_seq(const char *text, Vocabulary *vocab) {
    InputSeq *seq = malloc(sizeof(InputSeq));
    if (!seq) {
        fprintf(stderr, "malloc fail\n");
        exit(1);
    }
    seq->ids = malloc(strlen(text) * sizeof(int));
    if (!seq->ids) {
        fprintf(stderr, "malloc fail\n");
        free(seq);
        exit(1);
    }
    seq->length = strlen(text);
    // split text into tokens by single characters
    for (size_t i = 0; i < seq->length; i++) {
        char tmp[2] = {text[i], 0};
        int tid = vocab_find(vocab, tmp);
    // only 1 token per unique character
        if (tid == -1) tid = vocab_add(vocab, tmp);
        seq->ids[i] = tid;
    }
    return seq;
}

// find most frequent adjacent token pair using the dynamic array
// linear search, TODO; try hash, or something else
typedef struct { int first, second, count; } PairStat;
PairStat find_most_frequent_pair(InputSeq *seq) {
    PairCount *counts = NULL;
    size_t count_capacity = 1024, count_size = 0;
    counts = malloc(count_capacity * sizeof(PairCount));
    if (!counts) {
        fprintf(stderr, "malloc fail\n");
        exit(1);
    }

    // count frequency of each adjacent pair
    for (size_t i = 0; i + 1 < seq->length; i++) {
        int first = seq->ids[i], second = seq->ids[i+1];
        int found = 0;
        for (size_t j = 0; j < count_size; j++) {
            if (counts[j].first == first && counts[j].second == second) {
                counts[j].count++;
                found = 1;
                break;
            }
        }
        if (!found) {
                //resize
            if (count_size >= count_capacity) {
                count_capacity *= 2;
                PairCount *new_counts = realloc(counts, count_capacity * sizeof(PairCount));
                if (!new_counts) {
                    fprintf(stderr, "malloc fail\n");
                    free(counts);
                    exit(1);
                }
                counts = new_counts;
            }
            counts[count_size].first = first;
            counts[count_size].second = second;
            counts[count_size].count = 1;
            count_size++;
        }
    }
    // find highest frequency pair
    PairStat max = {0, 0, 0};
    for (size_t i = 0; i < count_size; i++) {
        if (counts[i].count > max.count) {
            max.first = counts[i].first;
            max.second = counts[i].second;
            max.count = counts[i].count;
        }
    }

    free(counts);
    return max;
}

// merge the most frequent pair in the input sequence
void merge_pair(InputSeq *seq, int first, int second, int new_token) {
    size_t j = 0;
    // iterate and replace the pair with the new token id
    for (size_t i = 0; i < seq->length; ) {
        if (i + 1 < seq->length && seq->ids[i] == first && seq->ids[i+1] == second) {
            seq->ids[j++] = new_token;
            i += 2;
        } else {
            seq->ids[j++] = seq->ids[i++];
        }
    }
    seq->length = j;
}

void free_input_seq(InputSeq *seq) {
    free(seq->ids);
    free(seq);
}

void free_vocab(Vocabulary *vocab) {
    for (size_t i = 0; i < vocab->tok_count; i++) {
        free(vocab->tokens[i].value);
    }
    free(vocab->tokens);
    free(vocab);
}

void print_input_seq(InputSeq *seq, Vocabulary *vocab) {
    for (size_t i = 0; i < seq->length; i++) {
        printf("%s ", vocab->tokens[seq->ids[i]].value);
    }
    printf("\n");
}

int main() {
    const char *text = "aaabcdaaabdc";
    Vocabulary *vocab = malloc(sizeof(Vocabulary));
    if (!vocab) {
        fprintf(stderr, "malloc fail\n");
        exit(1);
    }
    vocab->tok_count = 0;
    vocab->v_capacity = 128;
    vocab->tokens = malloc(vocab->v_capacity * sizeof(Token));
    if (!vocab->tokens) {
        fprintf(stderr, "malloc fail\n");
        free(vocab);
        exit(1);
    }
    // initialize the text as a sequence of tokens
    InputSeq *seq = create_input_seq(text, vocab);

    printf("Original: ");
    print_input_seq(seq, vocab);

    // main BPE loop, merge pairs until no pairs found or max vocabulary size reached
    while (vocab->tok_count < MAX_VOCAB_SIZE) {
        PairStat max = find_most_frequent_pair(seq);

    // stop when no more pairs with frequency over 1
    // I'd imagine some use cases probably use a higher threshold to avoid noise
    // but noise can always be extracted after tokenization, depends on the use case.
        if (max.count < 2) break;

        size_t len = strlen(vocab->tokens[max.first].value) + strlen(vocab->tokens[max.second].value) + 1;
        char *newval = malloc(len);
        if (!newval) {
            fprintf(stderr, "malloc fail\n");
            exit(1);
        }
        snprintf(newval, len, "%s%s", vocab->tokens[max.first].value, vocab->tokens[max.second].value);
        int new_token = vocab_add(vocab, newval);
        merge_pair(seq, max.first, max.second, new_token);
        printf("Merged '%s %s' -> '%s': ", vocab->tokens[max.first].value, 
               vocab->tokens[max.second].value, newval);
        print_input_seq(seq, vocab);
        free(newval);
    }

    printf("Final tokens:\n");
    for (size_t i = 0; i < vocab->tok_count; i++) {
        printf("Token %d: %s\n", vocab->tokens[i].id, vocab->tokens[i].value);
    }

    free_input_seq(seq);
    free_vocab(vocab);
    return 0;
}
