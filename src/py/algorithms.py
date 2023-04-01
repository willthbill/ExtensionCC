from loguru import logger
from py.cover import Cover
from py.solution_manager import SolutionManager
import os

import cpp.triangulation as triangulation
#import cpp.simple_merge_convex_partition as simple_merge_convex_partition_lib
#import cpp.non_convex_polygon_partition as non_convex_polygon_partition_lib 
#import cpp.clique_decomposition as clique_decomposition_lib
#import cpp.clique_whole_polygon as clique_whole_polygon_lib
#import cpp.clique_all_edges as clique_all_edges_lib
import cpp.clique_visibility_delaunay as clique_visibility_delaunay_lib
import cpp.clique_visibility_extension as clique_visibility_extension_lib
#import cpp.extension_triangulation as extension_triangulation_lib
#import cpp.delaunay_extension_mapping as delaunay_extension_mapping_lib
import cpp.fully_visible_extension as fully_visible_extension_lib
import cpp.fully_visible_extension_graph_calculation as fully_visible_extension_graph_calculation_lib
import cpp.fully_visible_subextension as fully_visible_subextension_lib
import cpp.fully_visible_subextension_omp as fully_visible_subextension_omp_lib
import cpp.gridsolver as gridsolver_lib
import cpp.list_graphs as list_graphs_lib
import cpp.reduce_solutions as reduce_solutions_lib
import cpp.vispair as vispair_lib
import cpp.vispolsegs as vispolsegs_lib
import cpp.fully_visible_delaunay as fully_visible_delaunay_lib

def get_new_cover(name, p, faces):
    cover = Cover()
    cover.init(name, p, faces)
    return cover

def nothing():
    def get_cover(p):
        return get_new_cover(f"nothing", p, [])
    return "nothing", get_cover

def only_delaunay_triangulation():
    def get_cover(p):
        faces = triangulation.triangulate_polygon_delaunay(p.points, p.holes)
        return get_new_cover(f"standardcover", p, faces)
    return "only delaunay triangulation", get_cover

# def simple_merge_convex_partition():
#     def get_cover(p):
#         faces = simple_merge_convex_partition_lib.simple_merge_convex_partition(p.points, p.holes)
#         return Cover(f"standardcover", p, faces)
#     return "simple merge strategy", get_cover
# 
# def non_convex_polygon_partition():
#     def get_cover(p):
#         faces = non_convex_polygon_partition_lib.non_convex_polygon_partition(p.points, p.holes);
#         return Cover(f"invalidcover", p, faces)
#     return "invalid components", get_cover
# 
# def clique_decomposition():
#     def get_cover(p):
#         faces = clique_decomposition_lib.get_clique_decomposition(p.points, p.holes);
#         return Cover(f"clique_decomposition_{p.name}", p, faces)
#     return "clique decomposition", get_cover
# 
# def clique_whole_polygon():
#     def get_cover(p):
#         faces = clique_whole_polygon_lib.get_clique_decomposition(p.points, p.holes);
#         return Cover(f"clique_whole_polygon_{p.name}", p, faces)
#     return "clique whole polygon", get_cover
# 
# def clique_all_edges():
#     def get_cover(p):
#         faces = clique_all_edges_lib.get_clique_decomposition(p.points, p.holes);
#         return Cover(f"clique_all_edges{p.name}", p, faces)
#     return "clique all edges", get_cover


def get_cover_clique_visibility_delaunay(p):
    faces = clique_visibility_delaunay_lib.get_clique_decomposition(p.points, p.holes);
    return get_new_cover(f"clique_visibility_delaunay_{p.name}", p, faces)
def clique_visibility_delaunay():
    return "clique visibility_delaunay", get_cover_clique_visibility_delaunay

# def extension_triangulation():
#     def get_cover(p):
#         faces = extension_triangulation_lib.get_extension_partition(p.points, p.holes);
#         return Cover(f"extension_partition_{p.name}", p, faces)
#     return "extension partition", get_cover

def get_cover_clique_visibility_extension(p):
    faces = clique_visibility_extension_lib.get_convex_cover(p.points, p.holes);
    return get_new_cover(f"clique_visibility_extension_{p.name}", p, faces)
def clique_visibility_extension():
    return "clique visibility extension", get_cover_clique_visibility_extension

# def get_cover_delaunay_extension_mapping(p):
#     faces = delaunay_extension_mapping_lib.get_convex_cover(p.points, p.holes);
#     return Cover(f"delaunay_extension_mapping_{p.name}", p, faces)
# def delaunay_extension_mapping():
#     return "delaunay extension mapping", get_cover_delaunay_extension_mapping

def get_cover_fully_visible_extension(p):
    faces = fully_visible_extension_lib.get_convex_cover(p.points, p.holes);
    return get_new_cover(f"fully_visible_extension_{p.name}", p, faces)
def fully_visible_extension():
    return "fully visible extension", get_cover_fully_visible_extension

def get_cover_fully_visible_extension_graph_calculation(p):
    faces = fully_visible_extension_graph_calculation_lib.get_convex_cover(p.points, p.holes);
    return get_new_cover(f"fully_visible_extension_graph_calculation_{p.name}", p, faces)
def fully_visible_extension_graph_calculation():
    return "fully visible extension graph calculation", get_cover_fully_visible_extension_graph_calculation

def get_cover_fully_visible_subextension(p):
    faces = fully_visible_subextension_lib.get_convex_cover(p.points, p.holes);
    return get_new_cover(f"fully_visible_subextension_{p.name}", p, faces)
def fully_visible_subextension():
    return "fully visible subextension", get_cover_fully_visible_subextension

def get_cover_fully_visible_subextension_omp(p):
    faces = fully_visible_subextension_omp_lib.get_convex_cover(p.points, p.holes);
    return get_new_cover(f"fully_visible_subextension_omp_{p.name}", p, faces)
def fully_visible_subextension_omp():
    return "fully visible subextension_omp", get_cover_fully_visible_subextension_omp

def get_cover_gridsolver(p):
    faces = gridsolver_lib.get_convex_cover(p.points, p.holes);
    return get_new_cover(f"gridsolver_{p.name}", p, faces)
def gridsolver():
    return "gridsolver", get_cover_gridsolver

def get_cover_list_graphs(p):
    #if p.name.find("maze") != -1 and p.get_number_of_points() > 10000:
    #    return []
    faces = list_graphs_lib.list_graphs(p.points, p.holes);
    return get_new_cover(f"list_graphs_{p.name}", p, faces)
def list_graphs():
    return "list_graphs", get_cover_list_graphs

reduce_solutions_manager = SolutionManager("instances_v2")
def get_cover_reduce_solutions(p):
    only_best="ONLY_BEST" in os.environ and os.environ["ONLY_BEST"] == '1' 
    manager = reduce_solutions_manager
    filenames = manager.get_valid_solution_filenames_for_instance(p.name)
    assert(len(filenames) > 0)
    best_faces = None
    def eval_json(json):
        nonlocal best_faces
        n_polygons_old = len(json["polygons"])
        cover = [
            [(str(point['x']),str(point['y'])) for point in pol]
            for pol in json["polygons"]
        ]
        faces = reduce_solutions_lib.reduce_solution((p.points, p.holes),cover);
        logger.success(f"Instance {p.name}: reduced solution from {n_polygons_old} to {len(faces)}")
        if best_faces is None or len(faces) < len(best_faces):
            best_faces = faces
    best_filename = "?"
    best_n_polygons = -1
    for filename in filenames:
        filenamename = filename[:min(25,len(filename))]
        logger.info(f"Instance {p.name}: processing {filenamename}")
        if filename.find("reduce_solutions") != -1:
            logger.info(f"skipping {filenamename}")
            continue
        json = manager.read_solution(filename)
        if only_best:
            n_polygons = len(json["polygons"])
            if best_n_polygons == -1 or n_polygons < best_n_polygons:
                best_n_polygons = n_polygons
                best_filename = filename
        else:
            logger.info(f"instance {p.name}: reducing {filenamename}")
            eval_json(json)
    if only_best:
        assert(best_filename != "?")
        logger.info(f"Instance {p.name}: best solution so far has {best_n_polygons} polygons")
        eval_json(manager.read_solution(best_filename))
    return get_new_cover(f"reduce_solutions_{p.name}", p, best_faces)
def reduce_solutions():
    return "reduce_solutions", get_cover_reduce_solutions

def get_cover_reduce_solutions_merge(p):
    manager = reduce_solutions_manager
    filenames = manager.get_valid_solution_filenames_for_instance(p.name)
    assert(len(filenames) > 0)
    all_covers = []
    best_n_polygons = 10000000000
    for filename in filenames:
        logger.info(f"instance {p.name}: merging {filename}")
        json = manager.read_solution(filename)
        cover = [
            [(str(point['x']),str(point['y'])) for point in pol]
            for pol in json["polygons"]
        ]
        n_polygons = len(json["polygons"])
        best_n_polygons = min(best_n_polygons, n_polygons)
        for pol in cover: all_covers.append(pol)
    faces = reduce_solutions_lib.reduce_solution((p.points, p.holes),all_covers);
    logger.info(f"instance {p.name}: reduced solution from {best_n_polygons} to {len(faces)}")
    return get_new_cover(f"reduce_solutions_merge__{p.name}", p, faces)
def reduce_solutions_merge():
    return "reduce_solutions_merge", get_cover_reduce_solutions_merge

def get_cover_vispair(p):
    faces = vispair_lib.get_convex_cover(p.points, p.holes);
    return get_new_cover(f"vispair_{p.name}", p, faces)
def vispair():
    return "vispair", get_cover_vispair

def get_cover_vispolsegs(p):
    faces = vispolsegs_lib.get_convex_cover(p.points, p.holes);
    return get_new_cover(f"vispolsegs_{p.name}", p, faces)
def vispolsegs():
    return "vispolsegs", get_cover_vispolsegs

def get_cover_fully_visible_delaunay(p):
    faces = fully_visible_delaunay_lib.get_convex_cover(p.points, p.holes);
    return get_new_cover(f"fully_visible_delaunay_{p.name}", p, faces)
def fully_visible_delaunay():
    return "fully visible delaunay", get_cover_fully_visible_delaunay

ALGORITHMS = {
    "nothing" : nothing,
    #"only_delaunay_triangulation" : only_delaunay_triangulation,
    #"simple_merge_convex_partition" : simple_merge_convex_partition,
    #"non_convex_polygon_partition" : non_convex_polygon_partition,
    #"clique_decomposition" : clique_decomposition,
    #"clique_whole_polygon" : clique_whole_polygon,
    #"clique_all_edges" : clique_all_edges,
    "clique_visibility_delaunay" : clique_visibility_delaunay,
    "clique_visibility_extension" : clique_visibility_extension,
    #"extension_triangulation" : extension_triangulation,
    #"delaunay_extension_mapping" : delaunay_extension_mapping,
    "fully_visible_extension" : fully_visible_extension,
    "fully_visible_extension_graph_calculation" : fully_visible_extension_graph_calculation,
    "fully_visible_subextension" : fully_visible_subextension,
    "fully_visible_subextension_omp" : fully_visible_subextension_omp,
    "gridsolver" : gridsolver,
    "list_graphs" : list_graphs,
    "reduce_solutions" : reduce_solutions,
    "reduce_solutions_merge" : reduce_solutions_merge,
    "vispair" : vispair,
    "vispolsegs" : vispolsegs,
    "fully_visible_delaunay" : fully_visible_delaunay
}
