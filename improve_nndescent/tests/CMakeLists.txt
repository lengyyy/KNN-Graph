set(CMAKE_CXX_STANDARD 11)

add_executable(test_nndescent test_nndescent.cpp)
target_link_libraries(test_nndescent ${PROJECT_NAME})

add_executable(test_nndescent_refine test_nndescent_refine.cpp)
target_link_libraries(test_nndescent_refine ${PROJECT_NAME})

add_executable(test_kdtree_graph test_kdtree_graph.cpp)
target_link_libraries(test_kdtree_graph ${PROJECT_NAME})

add_executable(kdtree_buildall_and_evaluate kdtree_buildall_and_evaluate.cpp)
#target_link_libraries(kdtree_buildall_and_evaluate ${PROJECT_NAME})
target_link_libraries(kdtree_buildall_and_evaluate ${PROJECT_NAME} -lprofiler)

add_executable(inner_product inner_product.cpp)
#target_link_libraries(inner_product ${PROJECT_NAME})
target_link_libraries(inner_product ${PROJECT_NAME} -lprofiler)