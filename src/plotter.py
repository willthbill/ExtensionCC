import sys
from main_functions import plot_both

polygon_filename = sys.argv[1]
cover_filename = sys.argv[2]

plot_both(polygon_filename, cover_filename)
