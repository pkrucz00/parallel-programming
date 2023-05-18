#!/usr/bin/env python
"""word_count_whole_file.py"""

import sys

if __name__=="__main__":
    if len(sys.argv) != 2:
        raise Exception("Wrong number of arguments")
    
    filepath = sys.argv[1]
    with open(filepath, "r") as file:
        lines = file.readlines()
       
    word_count = {} 
    for line in lines:
        words = line.strip().split()
        for word in words:
            if word not in word_count:
                word_count[word] = 0
            word_count[word] += 1
            
    for word, count in word_count.items():
        print(f"{word}\t{count}")
    
