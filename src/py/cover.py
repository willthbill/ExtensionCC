import os
import matplotlib.pyplot as plt
from loguru import logger
import json

import py.io
import py.visualize
from py.polygon import Polygon

class Cover:

    def __init__(self):
        self.name = None
        self.polygon = None
        self.faces = None
        self.json = None

    def init(self, name, polygon, faces):
        self.name = name
        self.polygon = polygon
        self.faces = faces
        self.json = self.get_json()

    def init_from_file(self, polygon_filename, cover_filename):
        self.polygon = Polygon()
        self.polygon.init_from_file(polygon_filename)
        logger.info("reading cover from {}".format(cover_filename))
        name,faces = py.io.read_cover(cover_filename)
        self.name = name
        self.faces = faces
        logger.info("read cover {}".format(self.name))

    def plot(self,should_show=False,should_write=False,filename=None, outdir="tmp/images/"):
        logger.info(f"plotting Cover({self.name}) of Polygon({self.polygon.name})")
        py.visualize.setup_plot_for_cover(self, simple=True)
        if should_write:
            file = None
            if filename is None:
                file = os.path.join(outdir, self.name + self.polygon.name + ".png")
            else:
                file = os.path.join(outdir, filename)
            os.makedirs(outdir, exist_ok=True)
            plt.savefig(file, dpi=300)
            logger.info("cover plot written to {}".format(file))
        if should_show:
            plt.show()
        plt.close()

    def get_number(self, val):
        if self.get_number_type(val) == "fraction":
            return val[0] / val[1]
        elif self.get_number_type(val) == "fraction-string":
            idx = val.index('/')
            a = int(val[:idx])
            b = int(val[idx+1:])
            return a/b
        else:
            return val

    def get_number_type(self, v):
        if type(v) == tuple:
            return "fraction"
        elif type(v) == str:
            return "fraction-string"
        else:
            return "number"

    def get_number_faces(self):
        res = []
        for face in self.faces:
            res.append([
                (self.get_number(v[0]), self.get_number(v[1]))
                for v in face
            ])
        return res

    def get_representation(self, val):
        if self.get_number_type(val) == "fraction": return {"num": val[0], "den": val[1]}
        else: return val

    def get_json(self):
        polygons = [
            [{
                'x' : self.get_representation(p[0]),
                'y' : self.get_representation(p[1])
            } for p in face]
            for face in self.faces
        ]
        return {
            "type": "CGSHOP2023_Solution",
            "instance": self.polygon.name,
            "polygons" : polygons,
        }

    def write_to_file(self,filename,outdir="tmp/solutions/"):
        os.makedirs(outdir, exist_ok=True)
        filename = os.path.join(outdir, filename)
        with open(filename, 'w') as file:
            file.write(json.dumps(self.json))
        logger.info(f"wrote Cover({self.name}) to {filename}")

