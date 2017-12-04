//
// Created by utena on 17-10-20.
//
#include "Identification_of_grid_water.hpp"
#define frame_segment 1000
//~#include <string.h>
//~ #include <fstream>
//~ #include <iostream>
//~ #include <stdlib.h>
//~ #include <stdio.h>

double factor_cos(double input){
    //return (3*pow(input,2)-1)/2;
    return input;
}

int RCF_test(char* dist,int temperature,int start_nc,int end_nc) {
    CreatDir("Analysis/Rotational_Relaxation");
    CreatDir("Analysis/Diffusion");
    typedef std::vector<double>::size_type index;
    std::cout << "program to calculate the H-BOND and cluster distribution"<< "\n"<< std::endl;
    char out_name[64];
    amber_parm parm_name(name_parm7);
    std::vector <index0> O_WAT_id = parm_name.id_by_type("OW");
    long long cluster_size_distribution[O_WAT_id.size()]={0};
    int state_by_id[O_WAT_id.size()]={0};
    std::vector<std::vector<double>> RCF_sum_OH (7,std::vector<double>(frame_segment,0));
    std::vector<std::vector<double>> RCF_sum_dip(7,std::vector<double>(frame_segment,0));
    std::vector<std::vector<double>> RCF_sum_nor(7,std::vector<double>(frame_segment,0));
    std::vector<std::vector<double>> Diffusion_sum_O(7,std::vector<double>(frame_segment,0));
    int tot_count[7][frame_segment]={0};
    std::vector<double> OH1_vec(3,0),OH2_vec(3,0),Dip_vec(3,0),Nor_vec(3,0);
    std::vector<double> OH1_vec_0(3,0),OH2_vec_0(3,0),Dip_vec_0(3,0),Nor_vec_0(3,0);
    std::vector<double> O_coor, O1_coor,O2_coor, H1O_coor, H2O_coor,H1_coor, H2_coor,O_coor_0;
    for (int nc = start_nc; nc <= end_nc; nc++) {
        char name_nc[64];
        sprintf(name_nc, "density_dis%s_WAT1150_NVT_%d_%d.nc",dist,temperature,nc);
        nctraj nc_data(name_nc);
        int tot_frame=nc_data.frames_number();
        char infile_name[64];
        sprintf(infile_name, "Analysis/HB_Network/nc_%02d.dat",nc);
        int WAT_ID_to_i[12000]={-1};
        for (int i=0; i<12000;i++){
            WAT_ID_to_i[i]=-1;
        }
        for (int i=0; i<O_WAT_id.size();i++){
            WAT_ID_to_i[O_WAT_id[i]]=i;
        }
        std::ifstream infile;
        std::vector<int> nc_HB_connection_list[tot_frame/frame_segment][O_WAT_id.size()]={std::vector<int>()};
        std::ofstream outfile;
        double num[5];
        infile.open(infile_name);
        infile>>num[0]>>num[1]>>num[2]>>num[3]>>num[4];
        for (int frame_id=0;frame_id<tot_frame;frame_id++) {
            int HB_Number_SUM[5] = {0};
            //~ double total_hbond = 0;
            double total_wat = 0;
            //std::cout<<nc<<"  "<<frame_id<<std::endl;
            int start_frame = frame_id;
            int end_frame = frame_id + 1;

            while (!infile.fail()) {
                int Atom0 = num[0];
                int nc = num[1];
                int frame = num[2] - start_frame;
                int O1 = num[3];
                int O2 = num[4];
                //std::cout<<num[3]<<std::endl;
                if (frame_id % frame_segment == 0) {

                    if (num[3] != -1) {
                        nc_HB_connection_list[frame_id / frame_segment][WAT_ID_to_i[Atom0]].push_back(O1);
                        nc_HB_connection_list[frame_id / frame_segment][WAT_ID_to_i[O1]].push_back(Atom0);
                    }
                    if (num[4] != -1) {
                        nc_HB_connection_list[frame_id / frame_segment][WAT_ID_to_i[Atom0]].push_back(O2);
                        nc_HB_connection_list[frame_id / frame_segment][WAT_ID_to_i[O2]].push_back(Atom0);
                    }
                }
                if (num[2] >= start_frame && num[2] < end_frame) {
                    infile >> num[0] >> num[1] >> num[2] >> num[3] >> num[4];
                }
                if (num[2] >= end_frame) { break; }
            }
        }
        for (int i = 0; i != O_WAT_id.size(); i++) {//按氢键数区分的转动驰豫
            std::cout<<"#nc"<<nc<<" #ID"<<i<<std::endl;
            int Target_ID=O_WAT_id[i];
            for (int frame_id = 0; frame_id < tot_frame; frame_id++) {

                O_coor = nc_data.atom_coordinate(frame_id, Target_ID);
                H1_coor = nc_data.atom_coordinate(frame_id, Target_ID + 1);
                H2_coor = nc_data.atom_coordinate(frame_id, Target_ID + 2);
                OH1_vec = vector_calc::vector_minus(H1_coor, O_coor);
                OH2_vec = vector_calc::vector_minus(H2_coor, O_coor);
                Dip_vec = vector_calc::vector_minus(
                        vector_calc::vector_divide(vector_calc::vector_add(H1_coor, H2_coor), 2), O_coor);
                Nor_vec = vector_calc::cross_product(OH2_vec, OH1_vec);
                if (frame_id % frame_segment == 0) {//时间零点
                    OH1_vec_0=OH1_vec;
                    OH2_vec_0=OH2_vec;
                    Dip_vec_0=Dip_vec;
                    Nor_vec_0=Nor_vec;
                    O_coor_0=O_coor;
                }
                int frame_temp=floor(frame_id/frame_segment);
                int HBN=nc_HB_connection_list[frame_temp][i].size();
                int frame_r=frame_id%frame_segment;
                //std::cout<<second_order_legendre_function(vector_calc::vector_angle(OH1_vec,OH1_vec_0))<<std::endl;
                tot_count[HBN][frame_r]+=1;
                tot_count[6][frame_r]+=1;
                RCF_sum_dip[HBN][frame_r]+=factor_cos(vector_calc::vector_angle(Dip_vec,Dip_vec_0));
                RCF_sum_nor[HBN][frame_r]+=factor_cos(vector_calc::vector_angle(Nor_vec,Nor_vec_0));
                RCF_sum_OH[HBN][frame_r]+=factor_cos(vector_calc::vector_angle(OH1_vec,OH1_vec_0));
                RCF_sum_OH[HBN][frame_r]+=factor_cos(vector_calc::vector_angle(OH2_vec,OH2_vec_0));
                Diffusion_sum_O[HBN][frame_r]+=pow(vector_calc::vector_module(vector_calc::vector_minus(O_coor,O_coor_0)),2);
                RCF_sum_dip[6][frame_r]+=factor_cos(vector_calc::vector_angle(Dip_vec,Dip_vec_0));
                RCF_sum_nor[6][frame_r]+=factor_cos(vector_calc::vector_angle(Nor_vec,Nor_vec_0));
                RCF_sum_OH[6][frame_r]+=factor_cos(vector_calc::vector_angle(OH1_vec,OH1_vec_0));
                RCF_sum_OH[6][frame_r]+=factor_cos(vector_calc::vector_angle(OH2_vec,OH2_vec_0));
                Diffusion_sum_O[6][frame_r]+=pow(vector_calc::vector_module(vector_calc::vector_minus(O_coor,O_coor_0)),2);
            }
        }
        infile.close();
        std::ofstream distribution_out;
        sprintf(out_name,"Analysis/Rotational_Relaxation/OH_HB_test_nc%02d.dat",nc);
        distribution_out.open(out_name);
        for (int i = 0; i != frame_segment; i++) {
            distribution_out << i*0.25 << std::setw(15);
            distribution_out << RCF_sum_OH[0][i]/(tot_count[0][i]*2) << std::setw(15);
            distribution_out << RCF_sum_OH[1][i]/(tot_count[1][i]*2) << std::setw(15);
            distribution_out << RCF_sum_OH[2][i]/(tot_count[2][i]*2) << std::setw(15);
            distribution_out << RCF_sum_OH[3][i]/(tot_count[3][i]*2) << std::setw(15);
            distribution_out << RCF_sum_OH[4][i]/(tot_count[4][i]*2) << std::setw(15);
            distribution_out<<std::endl;

        }
        distribution_out.close();
        sprintf(out_name,"Analysis/Rotational_Relaxation/Dip_HB_test_nc%02d.dat",nc);
        distribution_out.open(out_name);
        for (int i = 0; i != frame_segment; i++) {
            distribution_out << i*0.25 << std::setw(15);
            distribution_out << RCF_sum_dip[0][i]/(tot_count[0][i]) << std::setw(15);
            distribution_out << RCF_sum_dip[1][i]/(tot_count[1][i]) << std::setw(15);
            distribution_out << RCF_sum_dip[2][i]/(tot_count[2][i]) << std::setw(15);
            distribution_out << RCF_sum_dip[3][i]/(tot_count[3][i]) << std::setw(15);
            distribution_out << RCF_sum_dip[4][i]/(tot_count[4][i]) << std::setw(15);
            distribution_out<<std::endl;
        }
        distribution_out.close();
        sprintf(out_name,"Analysis/Rotational_Relaxation/Nor_HB_test_nc%02d.dat", nc);
        distribution_out.open(out_name);
        for (int i = 0; i != frame_segment; i++) {
            distribution_out << i*0.25 << std::setw(15);
            distribution_out << RCF_sum_nor[0][i]/(tot_count[0][i]) << std::setw(15);
            distribution_out << RCF_sum_nor[1][i]/(tot_count[1][i]) << std::setw(15);
            distribution_out << RCF_sum_nor[2][i]/(tot_count[2][i]) << std::setw(15);
            distribution_out << RCF_sum_nor[3][i]/(tot_count[3][i]) << std::setw(15);
            distribution_out << RCF_sum_nor[4][i]/(tot_count[4][i]) << std::setw(15);
            distribution_out<<std::endl;
        }
        distribution_out.close();
        sprintf(out_name,"Analysis/Diffusion/Diffusion_test_nc%02d.dat", nc);
        distribution_out.open(out_name);
        for (int i = 0; i != frame_segment; i++) {
            distribution_out << i*0.25 << std::setw(15);
            distribution_out <<Diffusion_sum_O [0][i]/(tot_count[0][i]) << std::setw(15);
            distribution_out << Diffusion_sum_O[1][i]/(tot_count[1][i]) << std::setw(15);
            distribution_out << Diffusion_sum_O[2][i]/(tot_count[2][i]) << std::setw(15);
            distribution_out << Diffusion_sum_O[3][i]/(tot_count[3][i]) << std::setw(15);
            distribution_out << Diffusion_sum_O[4][i]/(tot_count[4][i]) << std::setw(15);
            distribution_out << Diffusion_sum_O[6][i]/(tot_count[6][i]) << std::setw(15);
            distribution_out<<std::endl;
        }
        distribution_out.close();
        sprintf(out_name,"Analysis/Rotational_Relaxation/RCF_average_test_nc%02d.dat", nc);
        distribution_out.open(out_name);
        for (int i = 0; i != frame_segment; i++) {
            distribution_out << i*0.25 << std::setw(15);
            distribution_out << RCF_sum_OH[6][i]/(2*tot_count[6][i]) << std::setw(15);
            distribution_out << RCF_sum_dip[6][i]/(tot_count[6][i]) << std::setw(15);
            distribution_out << RCF_sum_nor[6][i]/(tot_count[6][i]) << std::setw(15);
            distribution_out<<std::endl;
        }
        distribution_out.close();
    }
}