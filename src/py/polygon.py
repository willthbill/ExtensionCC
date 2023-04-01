import os
import matplotlib.pyplot as plt
from loguru import logger

import py.io
import py.visualize

class Polygon:

    def __init__(self, name = "uninitialized", points = [], holes = []):
        self.name = name
        self.points = points
        self.holes = holes
        self.json = None
        self.filename = None

    def init_from_file(self, filename):
        self.filename = filename
        logger.info("reading polygon from {}".format(filename))
        name,points,holes,json = py.io.read_instance(filename)
        self.name = name
        self.points = points
        self.holes = holes
        self.json = json
        logger.info("read polygon {}".format(self.name))

    def get_number_of_points(self):
        res = len(self.points)
        for hole in self.holes:
            res += len(hole)
        return res

    def plot(self,should_show=False,should_write=False,filename=None,outdir="tmp/images/"):
        logger.info("plotting polygon {}".format(self.name))
        py.visualize.setup_plot_for_polygon(self, simple=True)
        if should_write:
            file = None
            if filename is None:
                file = os.path.join(outdir, self.name + ".png")
            else:
                file = os.path.join(outdir, filename)
            os.makedirs(outdir, exist_ok=True)
            plt.savefig(file, dpi=300)
            logger.info("polygon plot written to {}".format(file))
        if should_show:
            plt.show()
        plt.close()

