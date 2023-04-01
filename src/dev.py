from main_functions import evaluate_without_solution_interactive, summarize_without_solution, evaluate_without_solution, create_single_file_group
from py.algorithms import ALGORITHMS
import sys, os, shutil

algorithm_name = sys.argv[1]
input_group = sys.argv[2]
visualize = len(sys.argv) < 4 or sys.argv[3] != 'nogui'

if '/' in input_group: # a file
    input_group = create_single_file_group(input_group)

id = None
if visualize:
    id = evaluate_without_solution_interactive(ALGORITHMS[algorithm_name], input_group)
else:
    id = evaluate_without_solution(ALGORITHMS[algorithm_name], input_group)
summarize_without_solution(id)

