from main_functions import solutions_to_json
import sys

assert(len(sys.argv) == 3)
input_group = sys.argv[1]
filename = sys.argv[2]
solutions_to_json(input_group, filename)
