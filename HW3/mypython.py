#Christien Hotchkiss
#Assignment 3
#13 February 2020

#import string and random to print random letters an random numbers
import string
import random

#create array of file names
fileNames = ["file1", "file2", "file3"]
#store all lowercase alphabet
lower = string.ascii_lowercase

#loop over every file
for i in range(3):
	#create file
	f = open(fileNames[i], "w")
	#write to the file 10 random lowercase letters
	for j in range(10):
		random_letter = random.choice(lower)
		f.write(random_letter)
	#write newline to end file
	f.write("\n")
	f.close()
	#print the contents of file to stdout. remove additional newline
	print(open(fileNames[i]).read(), end="")
	#close file
	f.close()

#generate two random numbers between 1 and 42
r1 = random.randint(1,42)
r2 = random.randint(1,42)
#print them
print(r1)
print(r2)
#print their product
print(r1*r2)
