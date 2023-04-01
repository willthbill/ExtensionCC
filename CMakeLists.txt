cmake_minimum_required(VERSION 3.1...3.15)

project( min-convex-cover )

# CGAL and its components
find_package( CGAL QUIET COMPONENTS  )
if ( NOT CGAL_FOUND )
    message(STATUS "This project requires the CGAL library, and will not be compiled.")
    return()  
endif()

# Boost and its components
find_package( Boost REQUIRED COMPONENTS system filesystem) 
if ( NOT Boost_FOUND )
    message(STATUS "This project requires the Boost library, and will not be compiled.")
    return()  
endif()

find_package(OpenMP)
if( NOT OpenMP_CXX_FOUND)
    message(STATUS "This project requires OMP library, and will not be compiled.")
    return()
endif()

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "cpp")
set(CMAKE_CXX_FLAGS "-Wfatal-errors -D__local__")
set(CMAKE_CXX_FLAGS_DEBUG "")

message("FLAGS:")
message("- all: "${CMAKE_CXX_FLAGS})
message("- release: "${CMAKE_CXX_FLAGS_RELEASE})
message("- debug: "${CMAKE_CXX_FLAGS_DEBUG})

# test
add_executable(example_cgal src/cpp/example_cgal.cpp)
target_link_libraries(example_cgal CGAL::CGAL)

# for pybind11 libraries
execute_process(COMMAND ./scripts/getpybindincludes.sh OUTPUT_VARIABLE pybind11_paths)
include_directories(${pybind11_paths})
execute_process(COMMAND ./scripts/getpybindsuffix.sh OUTPUT_VARIABLE pybind11_suffix OUTPUT_STRIP_TRAILING_WHITESPACE)

# test
add_library(example_pybind11${pybind11_suffix} SHARED src/cpp/example_pybind11.cpp) # PIC is on by default

# dsu lib
add_library(dsu SHARED src/cpp/dsu.cpp)

# geometry_utils for cpp
add_library(geometry_utils SHARED src/cpp/geometry_utils.cpp)
target_link_libraries(geometry_utils CGAL::CGAL)

# communication lib
add_library(com SHARED src/cpp/com.cpp)
target_link_libraries(com CGAL::CGAL geometry_utils)

# intersection_predicates for cpp
add_library(intersection_predicates SHARED src/cpp/intersection_predicates.cpp)
target_link_libraries(intersection_predicates geometry_utils CGAL::CGAL)

# dualgraph lib
add_library(dualgraph SHARED src/cpp/dualgraph.cpp)
target_link_libraries(dualgraph CGAL::CGAL)

# rng for cpp
add_library(rng SHARED src/cpp/rng.cpp)

# vcc for cpp
add_library(vcc SHARED src/cpp/vcc.cpp)
target_link_libraries(vcc rng)

# find_components lib
add_library(find_components SHARED src/cpp/find_components.cpp)
target_link_libraries(find_components CGAL::CGAL rng)

# tree for cpp
add_library(tree SHARED src/cpp/tree.cpp)
target_link_libraries(tree CGAL::CGAL)

# reduce for cpp
add_library(reduce SHARED src/cpp/reduce.cpp)
target_link_libraries(reduce CGAL::CGAL partition_constructor tree geometry_utils intersection_predicates)

# triangulation for python
add_library(triangulation${pybind11_suffix} SHARED src/cpp/triangulation/triangulation.cpp)
target_link_libraries(triangulation${pybind11_suffix} CGAL::CGAL com)

# triangulation for cpp
add_library(triangulation SHARED src/cpp/triangulation/triangulation.cpp)
target_link_libraries(triangulation CGAL::CGAL com)

# polygon_cover for cpp
add_library(polygon_cover SHARED src/cpp/polygon_cover.cpp)
target_link_libraries(polygon_cover CGAL::CGAL find_components dualgraph)

# visibility for cpp
add_library(visibility  SHARED src/cpp/visibility.cpp)
target_link_libraries(visibility CGAL::CGAL)

# chgraph for cpp
add_library(chgraph SHARED src/cpp/chgraph.cpp)
target_link_libraries(chgraph CGAL::CGAL dualgraph geometry_utils intersection_predicates visibility ${Boost_LIBRARIES} OpenMP::OpenMP_CXX)

# clique for cpp
add_library(clique SHARED src/cpp/clique.cpp)
target_link_libraries(clique CGAL::CGAL rng chgraph dsu vcc ${Boost_LIBRARIES})

# partition_constructor for cpp
add_library(partition_constructor SHARED src/cpp/partition_constructor.cpp)
target_link_libraries(partition_constructor geometry_utils CGAL::CGAL visibility triangulation intersection_predicates rng)

# grid_solver for cpp
add_library(gridsolver SHARED src/cpp/gridsolver.cpp)
target_link_libraries(gridsolver CGAL::CGAL intersection_predicates dualgraph geometry_utils)

# # simple_merge_convex_partition for py
# add_library(simple_merge_convex_partition${pybind11_suffix} SHARED src/cpp/algorithms/simple_merge_convex_partition.cpp)
# target_link_libraries(simple_merge_convex_partition${pybind11_suffix} CGAL::CGAL com dualgraph triangulation)

# # non_convex_polygon_partition for py
# add_library(non_convex_polygon_partition${pybind11_suffix} SHARED src/cpp/algorithms/non_convex_polygon_partition.cpp)
# target_link_libraries(non_convex_polygon_partition${pybind11_suffix} CGAL::CGAL com dualgraph triangulation find_components polygon_cover)

# # clique_decomposition for py
# add_library(clique_decomposition${pybind11_suffix} SHARED src/cpp/algorithms/clique_decomposition.cpp)
# target_link_libraries(clique_decomposition${pybind11_suffix} CGAL::CGAL com triangulation polygon_cover clique)

# # clique_whole_graph for py
# add_library(clique_whole_polygon${pybind11_suffix} SHARED src/cpp/algorithms/clique_whole_polygon.cpp)
# target_link_libraries(clique_whole_polygon${pybind11_suffix} CGAL::CGAL com triangulation clique dualgraph )

# # clique_all_edges for py
# add_library(clique_all_edges${pybind11_suffix} SHARED src/cpp/algorithms/clique_all_edges.cpp)
# target_link_libraries(clique_all_edges${pybind11_suffix} CGAL::CGAL com triangulation clique dualgraph)

# clique_visibility_delaunay for py
add_library(clique_visibility_delaunay${pybind11_suffix} SHARED src/cpp/algorithms/clique_visibility_delaunay.cpp)
target_link_libraries(clique_visibility_delaunay${pybind11_suffix} CGAL::CGAL com triangulation clique chgraph partition_constructor)

# clique_visibility_extension for py
add_library(clique_visibility_extension${pybind11_suffix} SHARED src/cpp/algorithms/clique_visibility_extension.cpp)
target_link_libraries(clique_visibility_extension${pybind11_suffix} CGAL::CGAL com clique chgraph partition_constructor)

# # extension_partition for py
# add_library(extension_triangulation${pybind11_suffix} SHARED src/cpp/algorithms/extension_triangulation.cpp)
# target_link_libraries(extension_triangulation${pybind11_suffix} CGAL::CGAL com partition_constructor)

# # delaunay_extension_mapping for py
# add_library(delaunay_extension_mapping${pybind11_suffix} SHARED src/cpp/algorithms/delaunay_extension_mapping.cpp)
# target_link_libraries(delaunay_extension_mapping${pybind11_suffix} CGAL::CGAL com partition_constructor chgraph clique triangulation)

# fully_visible_extension for py
add_library(fully_visible_extension${pybind11_suffix} SHARED src/cpp/algorithms/fully_visible_extension.cpp)
target_link_libraries(fully_visible_extension${pybind11_suffix} CGAL::CGAL com partition_constructor chgraph clique)

# fully_visible_extension_graph_calculation for py
add_library(fully_visible_extension_graph_calculation${pybind11_suffix} SHARED src/cpp/algorithms/fully_visible_extension_graph_calculation.cpp)
target_link_libraries(fully_visible_extension_graph_calculation${pybind11_suffix} CGAL::CGAL com partition_constructor chgraph clique)

# fully_visible_subextension for py
add_library(fully_visible_subextension${pybind11_suffix} SHARED src/cpp/algorithms/fully_visible_subextension.cpp)
target_link_libraries(fully_visible_subextension${pybind11_suffix} CGAL::CGAL com partition_constructor chgraph clique)

# fully_visible_subextension_omp for py
add_library(fully_visible_subextension_omp${pybind11_suffix} SHARED src/cpp/algorithms/fully_visible_subextension_omp.cpp)
target_link_libraries(fully_visible_subextension_omp${pybind11_suffix} CGAL::CGAL com partition_constructor chgraph clique)

# gridsolver for py
add_library(gridsolver${pybind11_suffix} SHARED src/cpp/algorithms/gridsolver.cpp)
target_link_libraries(gridsolver${pybind11_suffix} CGAL::CGAL com gridsolver)

# list_graphs for py
add_library(list_graphs${pybind11_suffix} SHARED src/cpp/algorithms/list_graphs.cpp)
target_link_libraries(list_graphs${pybind11_suffix} CGAL::CGAL com chgraph partition_constructor triangulation)

# reduce_solutions for py
add_library(reduce_solutions${pybind11_suffix} SHARED src/cpp/algorithms/reduce_solutions.cpp)
target_link_libraries(reduce_solutions${pybind11_suffix} CGAL::CGAL com reduce)

# vispair for py
add_library(vispair${pybind11_suffix} SHARED src/cpp/algorithms/vispair.cpp)
target_link_libraries(vispair${pybind11_suffix} CGAL::CGAL com partition_constructor chgraph clique)

# vispolsegs for py
add_library(vispolsegs${pybind11_suffix} SHARED src/cpp/algorithms/vispolsegs.cpp)
target_link_libraries(vispolsegs${pybind11_suffix} CGAL::CGAL com partition_constructor chgraph clique)

# fully_visible_delaunay for py
add_library(fully_visible_delaunay${pybind11_suffix} SHARED src/cpp/algorithms/fully_visible_delaunay.cpp)
target_link_libraries(fully_visible_delaunay${pybind11_suffix} CGAL::CGAL com partition_constructor chgraph clique)

# headers
# target_precompile_headers(clique_whole_polygon${pybind11_suffix}
#     PRIVATE
#         <CGAL/ch_akl_toussaint.h>
#         <CGAL/Boolean_set_operations_2.h>
#         <CGAL/Range_segment_tree_traits.h>
#         <CGAL/Range_tree_k.h>
#         <CGAL/Range_segment_tree_traits.h>
#         <CGAL/Segment_tree_k.h>
# )
