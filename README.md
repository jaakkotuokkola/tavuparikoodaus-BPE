
### Description
Byte Pair Encoding is a text compression and tokenization algorithm. 
It repeatedly merges the most frequent adjacent pairs of tokens (starting from single characters) to build a vocabulary of subword units. Used in NLP and large language models to tokenize text into a vocabulary of subword units, enabling the  model to map input text to tokens for processing.

This implementation uses a linear search for pair counting, resulting in O(n²) time for sequence of length n per merge step.

For large texts, a hash table would make pair counting much faster.
### Example

```
Input: aaabcdaaabdc

Single-character tokenization
  Initial: a a a b c d a a a b d c

BPE merge steps
  Merged 'a a' -> 'aa': aa a b c d aa a b d c 
  Merged 'aa a' -> 'aaa': aaa b c d aaa b d c 
  Merged 'aaa b' -> 'aaab': aaab c d aaab d c

Final tokens: aaab c d aaab d c

    Full vocabulary:
    Token 0: a
    Token 1: b
    Token 2: c
    Token 3: d
    Token 4: aa
    Token 5: aaa
    Token 6: aaab

Final tokenized input: [6,2,3,6,3,2]
(as a sequence of token IDs) 

```
