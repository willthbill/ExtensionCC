from main_functions import evaluate, summarize, evaluate_in_parallel, create_single_file_group
from py.algorithms import ALGORITHMS
import sys

algorithm_name = sys.argv[1]
input_group = sys.argv[2]
in_parallel = len(sys.argv) >= 4 and sys.argv[3] == "parallel"
extra_data = sys.argv[4] if len(sys.argv) >= 5 else None

print("extra data:", extra_data)

if '/' in input_group: # a file
    input_group = create_single_file_group(input_group)

id = None
if in_parallel:
    id = evaluate_in_parallel(ALGORITHMS[algorithm_name], input_group, extra_data)
else:
    id = evaluate(ALGORITHMS[algorithm_name], input_group, extra_data)
summarize(id, extra_data)
