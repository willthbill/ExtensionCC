from main_functions import summarize_without_solution, evaluate_in_parallel_without_solution, create_single_file_group
from py.algorithms import ALGORITHMS
import sys

algorithm_name = sys.argv[1]
input_group = sys.argv[2]

if '/' in input_group: # a file
    input_group = create_single_file_group(input_group)

id = evaluate_in_parallel_without_solution(ALGORITHMS[algorithm_name], input_group)
summarize_without_solution(id)

