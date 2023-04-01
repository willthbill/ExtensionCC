from loguru import logger
import glob

from py.polygon import Polygon

class PolygonSet:

    def __init__(self, name):
        self.name = name
        self.polygons = []

    def get_file_names(self, dirname):
        return glob.glob(dirname + "/*")

    def get_polygon_by_name(self, instance_name):
        for polygon in self.polygons:
            if polygon.name == instance_name:
                return polygon
        return None

    def init_from_directory(self, dirname):
        logger.info("reading polygons from {}".format(dirname))
        filenames = self.get_file_names(dirname)
        filenames.sort()
        def get_polygon(filename):
            p = Polygon()
            p.init_from_file(filename)
            return p
        self.polygons = [get_polygon(filename) for filename in filenames]
        self.polygons.sort(key=lambda e: e.get_number_of_points())

    def plot_all(self,should_show=False,should_write=False,outdir=None):
        if outdir is None:
            outdir = "tmp/images/{}".format(self.name)
        for polygon in self.polygons:
            polygon.plot(should_show=should_show,should_write=should_write,outdir=outdir)

