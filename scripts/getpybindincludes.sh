python3 -m pybind11 --includes | tr ' ' '\n' | awk -F' ' '{ print substr($1,3)}' | tr '\n' ';'
