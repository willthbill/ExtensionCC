import os, json, glob, re
from py.polygon_set import PolygonSet
from tabulate import tabulate
from loguru import logger

class MultiSummarizer:

    def __init__(self, group):
        self.group = group 
        self.summarydir = "output/summaries"
        self.data = None
        self.evaluations = None
        self.output_directory = f"output/multisummaries"
        os.makedirs(self.output_directory, exist_ok=True)

    def get_output_filename(self):
        return f"{self.output_directory}/{self.group}.txt"

    def get_summary_files(self):
        return glob.glob(f"{self.summarydir}/*.json")

    def get_instances(self):
        instances = []
        pset = PolygonSet(self.group)
        pset.init_from_directory("input/" + self.group)
        for pol in pset.polygons:
            instances.append({
                "name" : pol.name,
                "n_points" : pol.get_number_of_points()
            })
        return instances

    def get_key(self, algorithm, evaluation_id):
        return algorithm + "===" + str(evaluation_id)

    def load_data(self):
        instances = self.get_instances()
        instance_names = [instance["name"] for instance in instances]
        self.data = {}
        for instance in instances:
            self.data[instance["name"]] = {
                "results" : {},
                "name" : instance["name"],
                "n_points" : instance["n_points"]
            }
        self.evaluations = {}
        for filename in self.get_summary_files():
            with open(filename) as f:
                summary = json.load(f)
            for result in summary["results"]:
                instance_name = result["instance"]
                if instance_name in instance_names:
                    key = self.get_key(result["algorithm"], result["evaluation_id"])
                    self.evaluations[key] = {
                        "algorithm" : result["algorithm"],
                        "evaluation_id" : result["evaluation_id"],
                        "group" : result["group"],
                        "extra_data" : result["extra_data"] if "extra_data" in result else ""
                    }
                    self.data[instance_name]["results"][key] = {
                        "algorithm" : result["algorithm"],
                        "evaluation_id" : result["evaluation_id"],
                        "n_polygons" : result["n_polygons"]
                    }

    def summarize(self):
        self.load_data()
        assert(self.data is not None)
        assert(self.evaluations is not None)
        rows = []
        idx = 0
        for key in self.evaluations.keys():
            self.evaluations[key]["idx"] = idx
            idx += 1
        for instance_name in self.data.keys():
            row = ["-1"] * len(self.evaluations)
            instance = self.data[instance_name]
            best = int(1e18)
            for key in instance["results"].keys():
                n_polygons = instance["results"][key]["n_polygons"]
                if n_polygons == -1: continue
                row[self.evaluations[key]["idx"]] = str(n_polygons)
                best = min(best, n_polygons)
            row.insert(0, str(best))
            name = instance["name"]
            n_points = instance["n_points"]
            row.insert(0, f"{name} ({n_points})")
            rows.append(row)
        headers = ["Instance (number of points)", "Best"]
        evaluations = []
        for key in self.evaluations.keys():
            algorithm = self.evaluations[key]["algorithm"]
            evaluation_id = self.evaluations[key]["evaluation_id"]
            group = self.evaluations[key]["group"]
            idx = self.evaluations[key]["idx"]
            extra_data = self.evaluations[key]["extra_data"]
            evaluations.append(f"#{idx}: {algorithm}{'' if extra_data == '' else '_' + extra_data} on {group} ({evaluation_id})")
            headers.append(f"#{idx}")
        outfile = self.get_output_filename()
        summary = ""
        summary += f"Summary of {self.group}\n\n"
        for e in evaluations: summary += e + "\n"
        summary += "\n"
        summary += tabulate(rows, headers=headers, tablefmt='orgtbl')
        with open(outfile, 'w') as f:
            f.write(summary)
        print()
        print(summary,end="\n\n")
        logger.info(f"Wrote table to {outfile}")

