cmake_minimum_required(VERSION 3.6)
project(qzr)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_COMPILER g++)
set(SOURCE_FILES
        9a5/enthaply/amber_netcdf.hpp
        9a5/enthaply/amber_parm_1.0.hpp
        #9a5/cxx_diffusion.cpp
        #9a5/Cavity.cpp
        #9a5/find_jump.cpp
        #9a5/z_distribution_wat.cpp
        #9a5/cavity_jump_correlation.cpp
        #9a5/find_jump_reverse.cpp
        #9a5/find_jump_reverse_start.cpp
        #9a5/jump_time_distribution.cpp
        #9a5/jumptime_with_transition_test.cpp
        #9a5/recrossing_frequency_during_transition.cpp
        #9a5/stabletime_distribution_during_transition
        9a5/jumptime_transition_WAT.cpp
        )
#add_executable(stable_time ${SOURCE_FILES})
#target_link_libraries(stable_time -lnetcdf_c++)
#add_executable(get_target_trajectory   9a5/enthaply/amber_netcdf.hpp 9a5/enthaply/amber_parm_1.0.hpp 9a5/output_z_trajectory.cpp)
#target_link_libraries(get_target_trajectory -lnetcdf_c++)
#add_executable(format_translate   9a5/enthaply/amber_netcdf.hpp 9a5/enthaply/amber_parm_1.0.hpp 9a5/format_translation_frame-nc.cpp)
#target_link_libraries(format_translate -lnetcdf_c++)
set(SOURCE_Correlation
        9a5/enthaply/amber_netcdf.hpp
        9a5/enthaply/amber_parm_1.0.hpp
        #9a5/cxx_diffusion.cpp
        #9a5/find_jump.cpp
        #9a5/z_distribution_wat.cpp
        #9a5/cavity_jump_correlation.cpp
        #9a5/find_jump_reverse.cpp
        #9a5/find_jump_reverse_start.cpp
        #9a5/jump_time_distribution.cpp
        #9a5/Hbond_x_z_transiton_correlation.cpp
        9a5/cavitysize_over_wat_with_transition.cpp
        #9a5/cavity_size_over_water.cpp
        #9a5/cavity_size_over_water_dummy.cpp
        )
#add_executable(J-C_correlation ${SOURCE_Correlation})
#target_link_libraries(J-C_correlation -lnetcdf_c++)
set(SOURCE_FILES1
        9a5/enthaply/amber_netcdf.hpp
        9a5/enthaply/amber_parm_1.0.hpp
        #

        #9a5/Cavity.cpp
        #9a5/cavity_size_change-distribution.cpp
        #9a5/H-Bond_with_transition.cpp
        #water/Find_cavity_in_bulk_water.cpp 9a5/output_z_trajectory.cpp

        #9a5/format_translation_frame-nc.cpp
        9a5/H-Bond_transitionCV_getzeropoint.cpp
        )
set(SOURCE_FILES2
        9a5/enthaply/amber_netcdf.hpp
        9a5/enthaply/amber_parm_1.0.hpp
        #9a5/find_jump_reverse_start.cpp
        #9a5/H_BOND_distribution.cpp
        #9a5/H-Bond_with_transition.cpp
        #9a5/H_BOND_distribution_O2.cpp
        #9a5/H-Bond_by_z.cpp
        #9a5/H-Bond_Hdonor_index_forH1H2_read.cpp
        9a5/H_Bond_H1H2.cpp
        #9a5/Hbond_transition_averHbond_UL.cpp
        )

#add_executable(jump ${SOURCE_FILES} )
#add_executable(cavity ${SOURCE_FILES1})
#add_executable(H-Bond ${SOURCE_FILES2} )
#add_executable(H-Bond_Hdonor_index_forH1H2_read_1a2_TST 9a5/H-Bond_Hdonor_index_forH1H2_read)
#add_executable(zeropoint 9a5/H-Bond_transitionCV_getzeropoint.cpp)
add_executable(Bulk_water water/Find_cavity_in_bulk_water.cpp)
#add_executable(calc_jump 9a5/jumptime_with_transition_test.cpp)
add_executable(2D_analysis water_period/Analysis/2D-properties.cpp)
add_executable(2PhaseModel 2pt/bulkwater_entropy.cpp)
add_executable(z_distribution WATBOX/Analysis/z_distribution_WAT.cpp)
add_executable(z_diffusion WATBOX/Analysis/z_axis_diffusion.cpp)
add_executable(calc_density_by_dist WATBOX/Analysis/Density.cpp)
#add_executable(calc_HBnetwork water_period/Analysis/HB_network.hpp)
#add_executable(calc_HBdistribution water_period/Analysis/H_BOND_distribution.hpp)
#add_executable(calc_cluster water_period/Analysis/Identification_Cluster.hpp)
#add_executable(RCF_HBNum water_period/Analysis/Rotation_Relaxation_test.cpp)
#add_executable(RCF_Diff water_period/Analysis/Rotation_Relaxiation&Diffusion.cpp)
include_directories(/usr/local/include)
find_package(MPI)
include_directories(${MPI_INCLUDE_PATH})
link_libraries(/usr/local/lib)
link_directories(/usr/local/lib)

#target_link_libraries(cavity -lnetcdf_c++ )
#target_link_libraries(H-Bond -lnetcdf_c++)
#target_link_libraries(jump -lnetcdf_c++)
target_link_libraries(Bulk_water -lnetcdf_c++)
#target_link_libraries(H-Bond_Hdonor_index_forH1H2_read_1a2_TST -lnetcdf_c++)
#target_link_libraries(zeropoint -lnetcdf_c++)
target_link_libraries(z_distribution -lnetcdf_c++)
#target_link_libraries(calc_jump -lnetcdf_c++)
target_link_libraries(2D_analysis -lnetcdf_c++)
target_link_libraries(calc_density_by_dist -lnetcdf_c++)
target_link_libraries(z_diffusion -lnetcdf_c++)
target_link_libraries(2PhaseModel -larmadillo  -lnetcdf_c++ -larpack -lgsl -L$GSL_LD -lgslcblas -lopenblas -llapack -lm -std=c++11)
#target_link_libraries(calc_HBnetwork -lnetcdf_c++)
#target_link_libraries(calc_HBdistribution -lnetcdf_c++)
#target_link_libraries(calc_cluster -lnetcdf_c++)
#target_link_libraries(RCF_Diff -lnetcdf_c++)
#target_link_libraries(RCF_HBNum -lnetcdf_c++)