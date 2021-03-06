//
// Created by utena on 17-7-3.
//

#include "amber_netcdf.hpp"
#include "amber_parm_1.0.hpp"
#include "vector_calc.h"


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
#define z_points    320
#define max_sampling 160000
#define max_time 100
#define dt 4
#define frame_extend 0
#define stable_width_cutoff 0.24
#define name_parm7 "density_dis9a5.parm7"

//~ #define name_nc "water_ion_graphene_10a5"
typedef std::vector<double>::size_type index;
double newstate_refresh(std::vector<index> &state_vector,int frame, double newstate_coor, int start_frame,int end_frame){
    if (frame==start_frame){
        state_vector[0]=frame;

    }
}

int main() {
    static double stable_time_by_z [z_points]={0};
    static double time_by_z_long [z_points]={0};
    static double** stable_time_distribution=new double* [z_points];
    for (int i=0;i<z_points;i++){
        stable_time_distribution[i]=new double[max_time];
    }
    static std::vector<double> total_wat(z_points,0);
    static double** last_stablestate=new double* [z_points];
    for (int i=0; i<z_points;i++){
        last_stablestate[i]=new double[3];
        last_stablestate[i][0]=0;
        last_stablestate[i][1]=0;
        last_stablestate[i][2]=0;
    }
    double dz, Z_UP, Z_DOWN, Y_UP, Y_DOWN, X_UP, X_DOWN;
    std::ifstream infile;
    static std::vector<double> cavity_frames;
    std::cout << "program to calculate waiting time distribution in metastable states" << "\n" << std::endl;
    static std::vector<int> cavity_frame;
    index frame=0;
    char name_nc[64];
    sprintf(name_nc, "nc/density_dis9a5_%d.nc", start_nc);
    amber_parm parm_name(name_parm7);
    nctraj nc_data(name_nc);
    std::vector<index> O_WAT_id = parm_name.id_by_type("OW");
    std::vector<index> O_WAT_IN_C_id, select_wat_id;
    std::vector<double> O_coor,O_coor_initial;

    infile.open("jump/cutoff_1a1/transition_path_index_start_finish_down");
    while (!infile.fail()) {
        double num[4];
        infile >> num[0] >> num[1]>>num[2]>>num[3];
        int frame_r_st = num[1]-frame_extend;
        int frame_r_ed = num[2]+frame_extend;
        int Target_ID = num[0];
        int nc = frame_r_st / 10000;
        sprintf(name_nc, "nc/density_dis9a5_%d.nc", nc);
        nctraj nc_data(name_nc);

        std::cout<<Target_ID<<std::setw(10)<<frame_r_st<<std::endl;
        std::vector<double> O1_coor, O2_coor, H1_coor, H2_coor;
        for (int frame_r=frame_r_st;frame_r<frame_r_ed&&frame_r<1000000;frame_r+=dt) {
            int nc = start_nc;
            int total_frame = start_nc * 10000;
            while (total_frame <= frame_r) {
                sprintf(name_nc, "nc/density_dis9a5_%d.nc", nc);
                nctraj nc_data(name_nc);
                total_frame += nc_data.frames_number();
                nc++;
            }
            nc--;
            if (frame_r==frame_r_st) {
                double C_z_coor_sum_1 = 0, C_z_coor_sum_2 = 0;
                double C_z_coor_average_1, C_z_coor_average_2;
                double C_x_coor_sum = 0, C_y_coor_sum = 0;
                double C_x_coor_center, C_y_coor_center;
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
                dz=(Z_UP-Z_DOWN)/z_points;
            }
            double temp_distance;
            sprintf(name_nc, "nc/density_dis9a5_%d.nc", nc);
            nctraj nc_data(name_nc);
            index frame = frame_r - total_frame + nc_data.frames_number();
            select_wat_id.clear();
            O1_coor = nc_data.atom_coordinate(frame, Target_ID);
            for (int i=1;i<z_points;i++){
                if (last_stablestate[i][0]==frame_r-dt){

                   temp_distance=sqrt((pow((O1_coor[2]- (Z_DOWN+i*dz)),2)+pow(last_stablestate[i][2],2)*int(last_stablestate[i][1]/dt))/(int(last_stablestate[i][1])/dt+1));
                    if (temp_distance<stable_width_cutoff){
                        //std::cout<<temp_distance<<std::endl;
                        last_stablestate[i][0]=frame_r;
                        last_stablestate[i][1]+=dt;
                        last_stablestate[i][2]=temp_distance;
                    }
                    else{
                        stable_time_distribution[i][int((last_stablestate[i][1])/dt)]++;
                        stable_time_by_z[i]+=last_stablestate[i][1];
                        if (last_stablestate[i][1]>=100) {
                            time_by_z_long[i]+=last_stablestate[i][1];
                        }
                        total_wat[i]++;
                    }
                }
                else{
                    temp_distance=sqrt((pow((O1_coor[2]- (Z_DOWN+i*dz)),2)));
                    if (temp_distance<stable_width_cutoff){
                        last_stablestate[i][0]=frame_r;
                        last_stablestate[i][1]=dt;
                        last_stablestate[i][2]=temp_distance;
                    }
                }
            }

        }
    }
    infile.close();
    std::ofstream outfile;
    outfile.open("jump/cutoff_1a1/stabletime_distribution_cutoff_1a1_0a24_dt=16fs_extend=0ps");
    std::ofstream outfile1;
    outfile1.open("jump/cutoff_1a1/stabletime_distribution_by_z_cutoff_1a1_0a24_dt=16fs_extend=0ps");
    std::ofstream outfile2;
    outfile2.open("jump/cutoff_1a1/stabletime_longtime_by_z_cutoff_1a1_0a24_dt=16fs_extend=0ps");
    for(int i =1; i !=z_points; ++i)
    {
        for(int j=0; j !=max_time; ++j)
        {
            outfile<<std::setw(15)<<stable_time_distribution[i][j]/total_wat[i];
        }
        //std::cout<<total_wat[i]<<std::setw(5);
        outfile<<std::endl;
        outfile1<<Z_DOWN+i*dz<<std::setw(12)<<4*stable_time_by_z[i]/total_wat[i]<<std::endl;
    }
    for(int i =1; i !=z_points; ++i)
    {
        outfile2<<Z_DOWN+i*dz<<std::setw(14)<<4*time_by_z_long[i]/total_wat[i]<<std::endl;
    }
    //std::cout<<std::endl;
    outfile.close();
    outfile1.close();
    outfile2.close();
    //infile1.close();
    return 0;
}