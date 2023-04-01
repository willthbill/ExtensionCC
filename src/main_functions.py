import os
import shutil
from loguru import logger
import random

from py.polygon_set import PolygonSet, Polygon
from py.polygon import Polygon
from py.evaluator import Evaluator
from py.summarizer import Summarizer
from py.multi_summarizer import MultiSummarizer
from py.solution_manager import SolutionManager
from py.cover import Cover

def plot_polygon(filename="input/examples_01/cheese102.instance.json"):
    p = Polygon()
    p.init_from_file(filename)
    p.plot(should_show=True)

def plot_polygons(group):
    P = PolygonSet(group)
    P.init_from_directory(f"input/{group}")
    P.plot_all(should_write=True, outdir=f"output/images/{group}")

def evaluate(algorithm, group, extra_data=None):
    P = PolygonSet(group)
    P.init_from_directory(f"input/{group}")
    evaluator = Evaluator(algorithm, P, is_solution=True, should_write=True, extra_data=extra_data)
    evaluator.evaluate()
    return evaluator.evaluation_id

def evaluate_with_id(algorithm, group, id):
    P = PolygonSet(group)
    P.init_from_directory(f"input/{group}")
    evaluator = Evaluator(
        algorithm,
        P,
        is_solution=True,
        evaluation_id=id,
        should_write=True
    )
    evaluator.evaluate()
    return evaluator.evaluation_id

def evaluate_without_solution_interactive(algorithm, group):
    P = PolygonSet(group)
    P.init_from_directory(f"input/{group}")
    evaluator = Evaluator(algorithm, P, should_write=True, should_show=True)
    evaluator.evaluate()
    return evaluator.evaluation_id

def evaluate_without_solution(algorithm, group):
    P = PolygonSet(group)
    P.init_from_directory(f"input/{group}")
    evaluator = Evaluator(algorithm, P, should_write=True)
    evaluator.evaluate()
    return evaluator.evaluation_id

def evaluate_in_parallel_without_solution(algorithm, group):
    P = PolygonSet(group)
    P.init_from_directory(f"input/{group}")
    evaluator = Evaluator(algorithm, P, should_write=True, in_parallel=True)
    evaluator.evaluate()
    return evaluator.evaluation_id

def evaluate_in_parallel(algorithm, group, extra_data=None):
    P = PolygonSet(group)
    P.init_from_directory(f"input/{group}")
    evaluator = Evaluator(algorithm, P, should_write=True, in_parallel=True, is_solution=True, extra_data=extra_data)
    evaluator.evaluate()
    return evaluator.evaluation_id

def summarize(id, extra_data=None):
    summarizer = Summarizer(id, is_solution=True, extra_data=extra_data)
    summarizer.summarize()

def summarize_without_solution(id):
    summarizer = Summarizer(id, is_solution=False)
    summarizer.summarize()

def summarize_group(group):
    summarizer = MultiSummarizer(group)
    summarizer.summarize()

def generate_subset(min_size, max_size, match):
    group = "instances_v2"
    P = PolygonSet("instances_v2")
    P.init_from_directory(f"input/{group}")
    if min_size > max_size:
        for polygon in P.polygons:
            if match == "all" or polygon.name.find(match) != -1:
                print(polygon.get_number_of_points(), polygon.name)
        return
    outgroup = f"{match}_{min_size}_{max_size}_{group}"
    outdir = f"input/{outgroup}"
    os.makedirs(outdir, exist_ok=True)
    cnt = 0
    for polygon in P.polygons:
        if min_size <= polygon.get_number_of_points() <= max_size and (match == "all" or polygon.name.find(match) != -1):
            filename = str(polygon.filename)
            shutil.copy(filename, outdir)
            cnt += 1
    logger.info(f"created group: {outgroup}")
    logger.info(f"number of testcases: {cnt}")
    logger.info(f"created directory: {outdir}")

def find_best_solutions(group="instances_v2"):
    manager = SolutionManager(group)
    manager.find_best_solutions()

def create_single_file_group(filename):
    file = os.path.basename(filename)
    dir = f"input/tmpdirforpython_{file}"#_{random.randint(0,999999999)}"
    # assert(not os.path.exists(os.path.abspath(dir)))
    os.makedirs(dir, exist_ok=True)
    distfile = os.path.join(dir, f"{file}")
    shutil.copyfile(filename, distfile)
    return f"tmpdirforpython_{file}"

def plot_cover(polygon_filename, cover_filename):
    c = Cover()
    c.init_from_file(polygon_filename, cover_filename)
    print("Size of solution:", len(c.faces))
    c.plot(should_show=True)

def plot_both(polygon_filename, cover_filename):
    # plot_polygon(polygon_filename)
    plot_cover(polygon_filename, cover_filename)

def solutions_to_json(group, filename):
    manager = SolutionManager(group)
    manager.solutions_to_json(filename)
