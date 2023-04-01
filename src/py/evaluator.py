from loguru import logger
from cgshop2023_pyutils import verify
from datetime import datetime
import time, glob, os
from multiprocessing import Pool

logger.level("RESULT", no=15, color="<blue>", icon="@")

# Evaluator
# Takes a func which returns a algorithm_name and get_cover function.
# Given a set of polygons it evaluates get_cover on all the polygons
# If a polygon has already been evaluated (with the same evaluation_id)
# then this already computed result is reused (this is useful is an evaluation_id
# is given).
# If should_show plot are shown at runtime
# If should_write plots and solutions are written to files depending on
# is_solution. If is_solution then the output directries are under output
# otherwise its under tmp.

class Evaluator:

    def __init__(self, func, polygon_set,
            is_solution=False,
            evaluation_id=None,
            should_show=False,
            should_write=False,
            in_parallel=False,
            extra_data=None
        ):
        self.algorithm_name, self.getcover = func()
        self.should_show = should_show
        self.should_write = should_write
        self.in_parallel = in_parallel
        self.polygon_set = polygon_set
        self.extra_data = extra_data
        if evaluation_id is None:
            self.evaluation_id = str(time.time_ns())
        else:
            self.evaluation_id = str(evaluation_id)
        x = ""
        if self.extra_data is None:
            x = f"{self.algorithm_name.replace(' ', '_')}==={self.polygon_set.name}==={self.evaluation_id}"
        else:
            x = f"{self.algorithm_name.replace(' ', '_')}==={self.extra_data.replace(' ','_')}==={self.polygon_set.name}==={self.evaluation_id}"
        if is_solution:
            self.solutiondir = f"output/solutions/{x}"
        else:
            self.solutiondir = f"tmp/solutions/{x}"
        if is_solution:
            self.plotdir = f"output/images/{x}"
        else:
            self.plotdir = f"tmp/images/{x}"
        os.makedirs(self.solutiondir, exist_ok=True)
        os.makedirs(self.plotdir, exist_ok=True)
    
    def verify(self, polygon, cover):
        assert(polygon == cover.polygon);
        instance = polygon.json
        solution = cover.json
        err_msg = verify(instance, solution)
        if err_msg:
            logger.error("Error message: ", err_msg)
            return False
        return True

    def evaluate_single(self, polygon):
        logger.info(f"Evaluating algorithm({self.algorithm_name}) on Polgyon({polygon.name})")
        if self.should_write and self.does_single_evaluation_exist(polygon):
            logger.info(f"Evaluation exits. Skipping")
            return
        tic = time.perf_counter()
        cover = self.getcover(polygon)
        toc = time.perf_counter()
        valid=True
        if polygon.json is None: logger.success(f"Cannot verify Polygon({polygon.name})")
        elif self.verify(polygon, cover): logger.success(f"Cover is valid on Polgyon({polygon.name})")
        else:
            valid=False
            logger.warning(f"Cover is invalid on Polgyon({polygon.name})")
        logger.log("RESULT", f"Number of faces: {len(cover.faces)}")
        if self.should_write:
            if valid:
                cover.write_to_file(
                    self.get_filename_for_solution(cover, polygon),
                    outdir=self.solutiondir
                )
            cover.plot(should_write=True, outdir=self.plotdir)
        if self.should_show:
            polygon.plot(should_show=True)
            cover.plot(should_show=True)
        logger.info(f"Finished evaluation of algorithm({self.algorithm_name}) on Polygon({polygon.name}) [{toc - tic:0.4f} sec]")
        return "hi"

    def evaluate(self):
        logger.info(f"Evaluating algorithm({self.algorithm_name}) on PolygonSet({self.polygon_set.name})")
        if self.should_write: logger.info(f"Solutions will be written to {self.solutiondir}")

        if self.in_parallel:
            number_of_processes=int(os.environ["NPROCMCC"]) if "NPROCMCC" in os.environ else 15
            logger.info(f"evaluating using {number_of_processes} processes")
            pool = Pool(processes=number_of_processes)
            pool.map(self.evaluate_single, self.polygon_set.polygons)
            pool.close()
            pool.join()
        else:
            for polygon in self.polygon_set.polygons: self.evaluate_single(polygon)

        logger.info(f"Finished evaluation of algorithm({self.algorithm_name}) on PolygonSet({self.polygon_set.name})")
        if self.should_write: logger.info(f"Solutions written to {self.solutiondir}")
        if self.should_write: logger.info(f"Plots written to {self.solutiondir}")

    def does_single_evaluation_exist(self, polygon):
        assert(self.solutiondir[-1] != '/')
        filename = f"solution==={self.algorithm_name}==={self.polygon_set.name}==={polygon.name}===*===*==={self.evaluation_id}.json"
        filename = filename.replace(' ', '_')
        filenames = glob.glob(self.solutiondir + f"/{filename}", recursive=True)
        return len(filenames) > 0

    def get_filename_for_solution(self, cover, polygon):
        x = datetime.now()
        time = x.strftime("%H:%M_%z_%d-%m-%Y")
        # filename = f"solution==={self.algorithm_name}==={self.polygon_set.name}==={polygon.name}==={cover.name}==={time}==={self.evaluation_id}.json"
        filename = f"solution==={self.algorithm_name}==={self.polygon_set.name}==={polygon.name}==={time}==={self.evaluation_id}.json"
        filename = filename.replace(' ', '_')
        return filename

