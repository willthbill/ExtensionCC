from math import inf
import os, shutil, json
from loguru import logger
import cgshop2023_pyutils
from py.polygon_set import PolygonSet
import importlib

class SolutionManager:

    def __init__(self, group="instances_v2"):
        self.group = group
        self.P = None

    def get_all_file_names(self):
        return [
            os.path.join(path, name)
            for path, _, files in os.walk("output/solutions")
            for name in files
        ]

    def read_solution(self, filename):
        with open(filename) as f:
           cover_json = json.load(f)
        return cover_json

    def init_P(self):
        if self.P is None:
            self.P = PolygonSet(self.group)
            self.P.init_from_directory(f"input/{self.group}")

    def is_valid_for_instance(self, instance_name, solution_filename):
        cover_json = self.read_solution(solution_filename)
        if cover_json["instance"] != instance_name: return False
        self.init_P()
        pol = self.P.get_polygon_by_name(instance_name)
        err_msg = cgshop2023_pyutils.verify(pol.json, cover_json)
        if err_msg: return False
        else: return True

    def get_valid_solution_filenames_for_instance(self, instance_name):
        filenames = self.get_all_file_names()
        res = []
        for filename in filenames:
            if self.is_valid_for_instance(instance_name, filename):
                res.append(filename)
        return res

    def find_best_solutions(self):
        self.init_P()
        polygons_json = {}
        for polygon in self.P.polygons:
            polygons_json[polygon.name] = polygon.json
        filenames = self.get_all_file_names()
        best_npolygons = {}
        all_npolygons = {}
        best_filename = {}
        for idx, filename in enumerate(filenames):
            if idx % 30 == 0:
                logger.info(f"checking solution {idx + 1}/{len(filenames)}")
            cover_json = self.read_solution(filename)
            instance = cover_json["instance"]
            if instance not in polygons_json: continue
            # importlib.reload(cgshop2023_pyutils)
            # err_msg = cgshop2023_pyutils.verify(polygons_json[instance], cover_json)
            #if err_msg: continue
            n_polygons = len(cover_json["polygons"])
            if n_polygons == 0: continue
            if instance not in best_npolygons:
                assert(instance not in best_filename)
                best_npolygons[instance] = inf
                best_filename[instance] = ""
                all_npolygons[instance] = []
            all_npolygons[instance].append(n_polygons)
            if n_polygons < best_npolygons[instance]:
                best_npolygons[instance] = n_polygons
                best_filename[instance] = filename
        for polygon in self.P.polygons:
            if polygon.name in best_npolygons:
                best = best_npolygons[polygon.name]
                filename = best_filename[polygon.name]
                evaluation = filename.split("/")[2]
                print(f"{best} {polygon.name}({polygon.get_number_of_points()}) {evaluation}")
                print("    all evaluations:", *sorted(all_npolygons[polygon.name]))
            else:
                print(f"{polygon.name} not evaluated")
            print()
        input("continue?")
        outdir = f"output/bestsolutions"
        input(f"This will override {outdir}?")
        os.makedirs(outdir, exist_ok=True)
        shutil.rmtree(outdir)
        os.makedirs(outdir, exist_ok=True)
        for polygon in self.P.polygons:
            filename = best_filename[polygon.name]
            shutil.copy(filename, outdir)
        logger.info(f"produced output directory: {outdir}")

    def solutions_to_json(self, output_filename):
        self.init_P()
        polygons_json = {}
        polygon_info = {}
        for polygon in self.P.polygons:
            polygon_info[polygon.name] = {}
            polygon_info[polygon.name]["size"] = polygon.get_number_of_points()
            polygons_json[polygon.name] = polygon.json
        filenames = self.get_all_file_names()
        all_polygons = {}
        for idx, filename in enumerate(filenames):
            if idx % 30 == 0:
                logger.info(f"checking solution {idx + 1}/{len(filenames)}")
            cover_json = self.read_solution(filename)
            # if "instance" not in cover_json: continue
            instance = cover_json["instance"]
            if instance not in polygons_json: continue
            # importlib.reload(cgshop2023_pyutils)
            # err_msg = cgshop2023_pyutils.verify(polygons_json[instance], cover_json)
            #if err_msg: continue
            n_polygons = len(cover_json["polygons"])
            if n_polygons == 0: continue
            if instance not in all_polygons:
                all_polygons[instance] = []
            all_polygons[instance].append((n_polygons, filename))
        for polygon in all_polygons:
            all_polygons[polygon].sort()
        with open(output_filename, 'w') as f:
            print("writing to", output_filename)
            json.dump({
                "solutions" : all_polygons,
                "info" : polygon_info
            }, f)
