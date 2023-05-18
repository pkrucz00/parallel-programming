#!/usr/bin/env python
"""word_count_whole_file.py"""

import sys

if __name__=="__main__":
    word_count = {}
    
    for line in sys.stdin:
        words = line.strip().split()
        for word in words:
            if word not in word_count:
                word_count[word] = 0
            word_count[word] += 1
            
    for word, count in word_count.items():
        print(f"{word}\t{count}")