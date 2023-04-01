from main_functions import generate_subset
import sys

assert(len(sys.argv)==4)
min_size = sys.argv[1]
max_size = sys.argv[2]
match = sys.argv[3]

generate_subset(int(min_size), int(max_size), match)

