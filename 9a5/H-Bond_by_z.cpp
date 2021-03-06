#include "amber_netcdf.hpp"
#include "amber_parm_1.0.hpp"
#include "vector_calc.h"
//~ #include <string.h>
//~ #include <fstream>
//~ #include <iostream>
//~ #include <stdlib.h>
//~ #include <stdio.h>


#define z_axis_modify  2
#define deviation_y_center 10
#define deviation_x_center 10
#define start_nc 37
#define end_nc  100
#define select_boundary 3.55
#define hbond_cutoff_up 3.5
#define hbond_cutoff_down 2.0
#define hbond_cutoff_angle_up 180.0
#define hbond_cutoff_angle_down 135.0
#define bond_length_number 80
#define bond_angle_number 60
#define z_points    160
#define max_sampling 1000000
#define dt 20
#define frame_extend 100
#define name_parm7 "density_dis9a5.parm7"
//~ #define name_nc "water_ion_graphene_10a5"
typedef std::vector<double>::size_type index;

int judge_layer(std::vector<double> coor,double Z_DOWN,double Z_UP){
    if (coor[2] > (Z_DOWN + Z_UP) / 2) {
        return 1;
    }
    else {return -1;}
}

int judge_H_Bond(std::vector<double> O1_coor,std::vector<double> H_coor,std::vector<double> O2_coor){
    double distance = sqrt(pow((O1_coor[0] -
                                O2_coor[0]), 2) +
                           pow((O1_coor[1] -
                                O2_coor[1]), 2) +
                           pow((O1_coor[2] -
                                O2_coor[2]), 2));
    if (hbond_cutoff_down < distance && distance < hbond_cutoff_up) {
        double angle;
        std::vector<double> coor1, coor2;
        coor1 = vector_calc::vector_minus(O1_coor, H_coor);
        coor2 = vector_calc::vector_minus(O2_coor, H_coor);
        angle = vector_calc::vector_angle(coor1, coor2);

        if (angle < cos(hbond_cutoff_angle_down * vector_calc::PI / 180.0)) {
            return 1;
        }
    }
    return 0;
}

int main() {
    //std::ifstream infile1;
    std::vector<double> total_wat_z(z_points,0);
    double bond_distribution[z_points][z_points]={0};
    double bond_distribution_H[z_points][z_points]={0};
    double aver_Hbond_num_upperlayer_acceptor[z_points]={0};
    double aver_Hbond_num_lowerlayer_acceptor[z_points]={0};
    double aver_Hbond_num_upperlayer_donor[z_points]={0};
    double aver_Hbond_num_lowerlayer_donor[z_points]={0};
    std::ifstream infile;
    static std::vector<double> cavity_frames;
    std::cout << "program to calculate H Bond distribution" << "\n" << std::endl;
    static std::vector<int> cavity_frame;
    index frame=0;
    char name_nc[64];
    sprintf(name_nc, "nc/density_dis9a5_%d.nc", start_nc);
    amber_parm parm_name(name_parm7);
    nctraj nc_data(name_nc);
    std::vector<index> O_WAT_id = parm_name.id_by_type("OW");
    std::vector<index> O_WAT_IN_C_id, select_wat_id;
    std::vector<double> O_coor,O_coor_initial;
    for (int nc=start_nc;nc<=end_nc;nc++) {
        sprintf(name_nc, "nc/density_dis9a5_%d.nc", nc);
        nctraj nc_data(name_nc);
        double C_z_coor_sum_1 = 0, C_z_coor_sum_2 = 0;
        double C_z_coor_average_1, C_z_coor_average_2;
        double C_x_coor_sum = 0, C_y_coor_sum = 0;
        double C_x_coor_center, C_y_coor_center;
        double Z_UP, Z_DOWN, Y_UP, Y_DOWN, X_UP, X_DOWN;
        for (index C_index = 0; C_index != 1400; ++C_index) {
            C_z_coor_sum_1 += nc_data.atom_coordinate(frame, C_index)[2];
            C_z_coor_sum_2 += nc_data.atom_coordinate(frame, (1400 + C_index))[2];
            C_y_coor_sum += nc_data.atom_coordinate(frame, C_index)[1];
            C_x_coor_sum += nc_data.atom_coordinate(frame, C_index)[0];
        }
        C_z_coor_average_1 = C_z_coor_sum_1 / 1400;
        C_z_coor_average_2 = C_z_coor_sum_2 / 1400;
        C_y_coor_center = C_y_coor_sum / 1400;
        C_x_coor_center = C_x_coor_sum / 1400;
        Z_UP = C_z_coor_average_2;
        Z_DOWN = C_z_coor_average_1;
        Y_UP = C_y_coor_center + deviation_y_center;
        Y_DOWN = C_y_coor_center - deviation_y_center;
        X_UP = C_x_coor_center + deviation_x_center;
        X_DOWN = C_x_coor_center - deviation_x_center;
        double dz=(Z_UP-Z_DOWN)/z_points;
        std::vector<double> O1_coor, O2_coor, H1_coor, H2_coor;
        for (index frame=0;frame<nc_data.frames_number();frame+=dt) {
            if (frame%100==0) std::cout<<frame<<std::endl;
            O_WAT_IN_C_id.clear();select_wat_id.clear();
            for(index i = 0; i != O_WAT_id.size(); ++i)
            {
                O_coor = nc_data.atom_coordinate(frame, O_WAT_id[i]);
                if( O_coor[2] < Z_UP && O_coor[2] > Z_DOWN && O_coor[0] < X_UP && O_coor[0] > X_DOWN && O_coor[1] < Y_UP && O_coor[1] > Y_DOWN)
                {
                    O_WAT_IN_C_id.push_back(O_WAT_id[i]);
                }
                if( O_coor[2] < Z_UP && O_coor[2] > Z_DOWN && O_coor[0] < (X_UP+select_boundary) && O_coor[0] > (X_DOWN-select_boundary) && O_coor[1] < (Y_UP+select_boundary) && O_coor[1] > (Y_DOWN-select_boundary))
                {
                    select_wat_id.push_back(O_WAT_id[i]);
                }
            }
            for(index i =0; i != O_WAT_IN_C_id.size(); ++i) {
                int Target_ID = O_WAT_IN_C_id[i];
                O1_coor = nc_data.atom_coordinate(frame, Target_ID);
                if (total_wat_z[int(floor((O1_coor[2] - Z_DOWN) / ((Z_UP - Z_DOWN) / z_points)))]<max_sampling) {
                    total_wat_z[int(floor((O1_coor[2] - Z_DOWN) / ((Z_UP - Z_DOWN) / z_points)))]++;
                    for (index j = 0; j != select_wat_id.size(); ++j) {
                        if (Target_ID != select_wat_id[j]) {
                            O2_coor = nc_data.atom_coordinate(frame, select_wat_id[j]);
                            H1_coor = nc_data.atom_coordinate(frame, select_wat_id[j] + 1);
                            H2_coor = nc_data.atom_coordinate(frame, select_wat_id[j] + 2);
                            if (judge_layer(O2_coor,Z_DOWN,Z_UP)==1){
                                aver_Hbond_num_upperlayer_acceptor [int(floor((O1_coor[2] - Z_DOWN) / ((Z_UP - Z_DOWN) / z_points)))]
                                        += (judge_H_Bond(O1_coor, H1_coor, O2_coor) + judge_H_Bond(O1_coor, H2_coor, O2_coor));
                            }
                            else if (judge_layer(O2_coor,Z_DOWN,Z_UP)==-1){
                                aver_Hbond_num_lowerlayer_acceptor [int(floor((O1_coor[2] - Z_DOWN) / ((Z_UP - Z_DOWN) / z_points)))]
                                        += (judge_H_Bond(O1_coor, H1_coor, O2_coor) + judge_H_Bond(O1_coor, H2_coor, O2_coor));
                            }
                            /*bond_distribution[int(floor((O1_coor[2] - Z_DOWN) / ((Z_UP - Z_DOWN) / z_points)))]
                            [int(floor((O2_coor[2] - Z_DOWN) / ((Z_UP - Z_DOWN) / z_points)))]
                                    += (judge_H_Bond(O1_coor, H1_coor, O2_coor) +
                                        judge_H_Bond(O1_coor, H2_coor, O2_coor));*/
                        }
                    }
                    for (index j = 0; j != select_wat_id.size(); ++j) {
                        if (Target_ID != select_wat_id[j]) {
                            O2_coor = nc_data.atom_coordinate(frame, select_wat_id[j]);
                            H1_coor = nc_data.atom_coordinate(frame, Target_ID + 1);
                            H2_coor = nc_data.atom_coordinate(frame, Target_ID + 2);
                            if (judge_layer(O2_coor,Z_DOWN,Z_UP)==1){
                                aver_Hbond_num_upperlayer_donor [int(floor((O1_coor[2] - Z_DOWN) / ((Z_UP - Z_DOWN) / z_points)))]
                                        += (judge_H_Bond(O1_coor, H1_coor, O2_coor) + judge_H_Bond(O1_coor, H2_coor, O2_coor));
                            }
                            else if (judge_layer(O2_coor,Z_DOWN,Z_UP)==-1){
                                aver_Hbond_num_lowerlayer_donor [int(floor((O1_coor[2] - Z_DOWN) / ((Z_UP - Z_DOWN) / z_points)))]
                                        += (judge_H_Bond(O1_coor, H1_coor, O2_coor) + judge_H_Bond(O1_coor, H2_coor, O2_coor));
                            }
                            /*bond_distribution_H[int(floor((O1_coor[2] - Z_DOWN) / ((Z_UP - Z_DOWN) / z_points)))]
                            [int(floor((O2_coor[2] - Z_DOWN) / ((Z_UP - Z_DOWN) / z_points)))]
                                    += (judge_H_Bond(O1_coor, H1_coor, O2_coor) +
                                        judge_H_Bond(O1_coor, H2_coor, O2_coor));*/
                        }

                    }
                }
            }
        }
        std::ofstream outfile;
        outfile.open("H-Bond/average_Hbond_num_ULlayer_allwat");//0 Z 1 AU 2 AL 3 DU 4 DL
        for(int i =1; i !=z_points; ++i)
        {
            outfile<<Z_DOWN+i*dz<<std::setw(12)<<aver_Hbond_num_upperlayer_acceptor[i]/total_wat_z[i]<<std::setw(12)<<aver_Hbond_num_lowerlayer_acceptor[i]/total_wat_z[i]
                   <<std::setw(12)<<aver_Hbond_num_upperlayer_donor[i]/total_wat_z[i]<<std::setw(12)<<aver_Hbond_num_lowerlayer_donor[i]/total_wat_z[i]<<std::endl;
            //std::cout <<total_wat[i]<<std::setw(2);
        }
        outfile.close();
        /*
        for(int i =0; i !=z_points; ++i){
            std::cout<<total_wat_z[i]<<' ';
        }
        std::cout<<std::endl;
        std::ofstream outfile;
        outfile.open("H-Bond/H_Bond_by_z_O");
        for(int i =0; i !=z_points; ++i)
        {
            for(int j=0; j !=z_points; ++j)
            {
                outfile<<std::setw(12)<<bond_distribution[i][j]/total_wat_z[i];
            }
            outfile<<std::endl;
        }

        outfile.close();
        std::ofstream outfile1;
        outfile1.open("H-Bond/H_Bond_by_z_H");
        for(int i =0; i !=z_points; ++i)
        {
            for(int j=0; j !=z_points; ++j)
            {
                outfile1<<std::setw(12)<<bond_distribution_H[i][j]/total_wat_z[i];
            }
            outfile1<<std::endl;
        }

        outfile1.close();
         */
    }


    //infile1.close();
    return 0;
}

