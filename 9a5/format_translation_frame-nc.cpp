
#include "amber_netcdf.hpp"
#include "amber_parm_1.0.hpp"
#include "vector_calc.h"
#define start_nc 37
#define end_nc  100
#define dt 4
#define frame_extend 100
#define name_parm7 "density_dis9a5.parm7"
//~ #define name_nc "water_ion_graphene_10a5"
typedef std::vector<double>::size_type index;

int main() {
    std::ifstream infile;
    std::ofstream outfile0;
    outfile0.open("jump/cutoff_1a2/transition_path_stat");
    static std::vector<double> cavity_frames;
    static std::vector<int> cavity_frame;
    char name_nc[64];
    sprintf(name_nc, "nc/density_dis9a5_%d.nc", start_nc);
    amber_parm parm_name(name_parm7);
    nctraj nc_data(name_nc);
    infile.open("jump/cutoff_1a2/transition_path_index_start_finish_down");
    int frame;
    int nc=start_nc;
    int total_frame = start_nc*10000;
    int totframe_nc=nc_data.frames_number();
    double num[4];
    while (!infile.fail()) {
        infile >> num[0] >> num[1]>>num[2]>>num[3];
        int frame_r_st = num[1]-frame_extend;
        int frame_r_ed = num[2]+frame_extend;
        int Target_ID = num[0];
        char name_nc[64];
        nc=start_nc;
        total_frame = start_nc*10000;
        totframe_nc=nc_data.frames_number();
        for (int frame_r=frame_r_st;frame_r<frame_r_ed&&frame_r<1000000;frame_r+=dt) {
            if (frame_r==frame_r_st || frame_r>=(total_frame+totframe_nc)){
                while (total_frame+totframe_nc <= frame_r) {
                    total_frame += totframe_nc;
                    nc++;
                    sprintf(name_nc, "nc/density_dis9a5_%d.nc", nc);
                    nctraj nc_data(name_nc);
                    totframe_nc=nc_data.frames_number();
                }
            }
            sprintf(name_nc, "nc/density_dis9a5_%d.nc", nc);
            nctraj nc_data(name_nc);
            frame = frame_r - total_frame ;
            outfile0<<Target_ID<<std::setw(10)<<nc<<std::setw(10)<<frame<<std::setw(10)<<num[3]<<std::endl;
        }
    }
    infile.close();
    outfile0.close();
    //infile1.close();
    return 0;
}
