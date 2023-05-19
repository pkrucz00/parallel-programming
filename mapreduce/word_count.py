#!/usr/bin/env python
"""word_count_whole_file.py"""

import sys
import time
from smart_open import open

if __name__=="__main__":
    for filename in ["s3://mpr-balis/gutenberg-1G.txt","s3://mpr-balis/gutenberg-5G.txt","s3://mpr-balis/gutenberg-10G.txt"]:
        for i in range(4):
            word_count = {}
            time1 = time.time()
            
            for line in open('s3://mpr-balis/gutenberg-1G.txt', encoding="latin-1"):
                words = line.strip().split()
                for word in words:
                    if word not in word_count:
                        word_count[word] = 0
                    word_count[word] += 1
                
            time_elapsed = round(time.time() - time1, 2)
            print(f"{i+1}\t{filename}\t{time_elapsed}[s]")