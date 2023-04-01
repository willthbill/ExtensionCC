import os, json, glob, re

# Summarizer
# Takes an evaluation_id and produces summary of the results of that evaluation
# If is_solution then data is written to output/summaries otherwise it is only written to tmp/summarycache

class Summarizer:

    def __init__(self, evaluation_id, is_solution=False, extra_data=None):
        self.extra_data = extra_data
        self.is_solution=is_solution
        self.evaluation_id = evaluation_id
        self.cachedir = "tmp/summarycache"
        self.datadir = "output/solutions" if self.is_solution else "tmp/solutions"
        self.summarydir = "output/summaries" if self.is_solution else "tmp/summaries"
        self.data = None
        self.algorithm_name = None
        self.group_name = None
        os.makedirs(self.cachedir, exist_ok=True)
        os.makedirs(self.datadir, exist_ok=True)
        os.makedirs(self.summarydir, exist_ok=True)

    def get_json_cache_filename(self):
        return os.path.join(self.cachedir, f"summary_cache_{self.evaluation_id}.json")

    def get_filename_wo_extension(self):
        assert(self.algorithm_name is not None)
        if self.extra_data is None:
            return os.path.join(self.summarydir, f"summary==={self.algorithm_name}==={self.group_name}==={self.evaluation_id}")
        else: 
            return os.path.join(self.summarydir, f"summary==={self.algorithm_name}==={self.extra_data}==={self.group_name}==={self.evaluation_id}")

    def get_json_filename(self):
        return f"{self.get_filename_wo_extension()}.json"

    def get_txt_filename(self):
        return f"{self.get_filename_wo_extension()}.txt"

    def load_data_from_cache(self):
        filename = self.get_json_cache_filename()
        if os.path.isfile(filename):
            with open(filename, 'r') as file:
                self.data = json.load(file)

    def generate_data(self):
        assert(self.datadir[-1] != '/')
        filenames = glob.iglob(self.datadir + f"/**/*{self.evaluation_id}*.json", recursive=True)
        self.data = {
            "evaluation_id" :  self.evaluation_id,
            "results" : []
        }
        for filename in filenames:
            # solution===only_delaunay_triangulation===examples_01===cheese4023===standardcover===22:45__14-12-2022===1671054273942292384.json
            x = re.search("solution===(.*)===(.*)===(.*)===(.*)===(.*)===(.*).json", filename)
            algorithm, group, instance, cover_name, time, id = None,None,None,None,None,None
            if x is not None:
                algorithm, group, instance, cover_name, time, id = x.group(1), x.group(2), x.group(3), x.group(4), x.group(5), x.group(6)
            else:
                x = re.search("solution===(.*)===(.*)===(.*)===(.*)===(.*).json", filename)
                algorithm, group, instance, time, id = x.group(1), x.group(2), x.group(3), x.group(4), x.group(5)
                cover_name = "not given"

            assert(id == self.evaluation_id)
            with open(filename, 'r') as file:
                content = json.load(file)
            n_polygons = len(content["polygons"])
            if n_polygons == 0: n_polygons = -1 # it did not produce a solution
            assert(content["instance"] == instance)
            self.algorithm_name = algorithm
            self.group_name = group
            self.data["results"].append({
                "algorithm" : algorithm,
                "group" : group,
                "extra_data" : "" if self.extra_data is None else self.extra_data,
                "instance" : instance,
                "cover_name" : cover_name,
                "time" : time,
                "evaluation_id" : self.evaluation_id,
                "n_polygons" : n_polygons,
            })
        assert(len(self.data["results"]) > 0)
        assert(len(set([r["algorithm"] for r in self.data["results"]])) == 1)
        assert(len(set([r["group"] for r in self.data["results"]])) == 1)
    
    def save_data_in_cache(self):
        filename = self.get_json_cache_filename()
        with open(filename, 'w') as file:
            file.write(json.dumps(self.data))

    def save_data(self, summary):
        filename_json = self.get_json_filename()
        filename_txt = self.get_txt_filename()
        with open(filename_json, 'w') as file:
            file.write(json.dumps(self.data, indent=4))
        with open(filename_txt, 'w') as file:
            file.write(summary)

    def load_data(self):
        if self.data is not None: return
        self.load_data_from_cache() 
        if self.data is None:
            self.generate_data()
            self.save_data_in_cache()
        self.data["results"].sort(key=lambda e : e["group"] + "===" + e["instance"]) #type: ignore

    def summarize(self):
        self.load_data()
        summary = f"Summary of evaluation with id={self.evaluation_id}\n"
        assert(self.data is not None)
        for result in self.data['results']:
            summary += f"{result['n_polygons']} {result['group']}/{result['instance']}\n"
        print(summary)
        self.save_data(summary)

