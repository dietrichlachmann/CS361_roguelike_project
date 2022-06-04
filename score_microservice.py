import sys
import time
import os
import re

time.sleep(1)
total_score = 0

while 1:
    with open("score.txt", "r+") as inFile:
        line = inFile.readlines()
        line = [x.strip() for x in line]
        if "Total Score" in line[-1]:
            time.sleep(1)
            print("waiting in read")
        else:
            for num in line:
                total_score+= int(num)
            inFile.write("\nTotal Score: ")
            inFile.write(str(total_score))
            total_score = 0  
            

