import json

def convert_to_point_list(json_polygon):
    p = []
    for point in json_polygon:
        p.append((point["x"], point["y"]))
    return p

# Returns a tuple (name, polygon, holes) where
# * name is a string,
# * polygon is a list of points defining the polygon,
# * holes is a list of polygons that are the holes of P.
def read_instance(filename):
    with open(filename, 'r') as f:
        json_dict = json.load(f)

    p = convert_to_point_list(json_dict["outer_boundary"])
    H = []
    for hole in json_dict["holes"]:
        H.append(convert_to_point_list(hole))

    return json_dict["name"], p, H, json_dict

def read_cover(filename):
    with open(filename, 'r') as f:
        json_dict = json.load(f)

    faces = []
    for face in json_dict["polygons"]:
        faces.append(convert_to_point_list(face))

    return "cover_" + json_dict["instance"], faces
