//generate stay region
//usage: g++ -O3 GenerateStayRegion.cpp -o GenerateStayRegion
// ./GenerateStayRegion > GenerateStayRegion.txt &
#include <iostream>
#include <string>
#include <utility>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h> 
#define pi 3.14159265358979323846
#define EPSILON 0.0000001
using namespace std;

int time_min=1266624000+18000;// start time: Feb 20, 2010
int time_max=1269907200+18000;// end time: March 30th;
string min_dur = "300"; // min stay duration
int time_limit=300;

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::  This function converts decimal degrees to radians             :*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
double deg2rad(double deg) {
  return (deg * pi / 180);
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::  This function converts radians to decimal degrees             :*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
double rad2deg(double rad) {
  return (rad * 180 / pi);
}


double distance(double lat1, double lon1, double lat2, double lon2, char unit) {
  double theta, dist;
  theta = lon1 - lon2;
  dist = sin(deg2rad(lat1)) * sin(deg2rad(lat2)) + cos(deg2rad(lat1)) * cos(deg2rad(lat2)) * cos(deg2rad(theta));
  dist = acos(dist);
  dist = rad2deg(dist);
  dist = dist * 60 * 1.1515;
  switch(unit) {
    case 'M':
      break;
    case 'K':
      dist = dist * 1.609344;
      break;
    case 'N':
      dist = dist * 0.8684;
      break;
  }
  return (dist);
}

// compare if two doubles are the same
bool AreSame(double a, double b)
{
    return fabs(a - b) < EPSILON;
}


int main(int argc, char** argv){
	string file_name1 = "Stays.txt";
	string file_name2 = "StayRegions.txt";
	double region_size=0.1*0.0095*3;
	ifstream fid_in;
	fid_in.open(file_name1.c_str(),ifstream::in);
	FILE * fout_id;
	fout_id=fopen(file_name2.c_str(), "w");
	string tline;
	vector<double> lons;
	vector<double> lats;
	vector<int> times;
	vector<int> durations;
	vector<int> points_index;
	vector<int> serial_number;
	vector<double> grid_lons;
	vector<double> grid_lats;
	vector<double> record_lons;
	vector<double> record_lats;
	vector<double> serial_lats;
	vector<double> serial_lons;
	double num1;
	double num2;
	int num3;
	int num4;
	double lon_min=999;
	double lon_max=-999;
	double lat_min=999;
	double lat_max=-999;
	int person_id=0;
	int person_total_count=0;
	int person_stay_count=0;
	int lon_grid_num=0;
	int lat_grid_num=0;
	int point_lat_grid=0;
	int point_lon_grid=0;
	int nun_empty_size=0;
	int temp_number;
	int cluster_count;
	int serial_number_count;
	int personal_record_count;
	int max_location_number;
	vector<int> nun_empty_number;
	vector<int> nun_empty_index;
	vector<int> grid_vec;
	vector<int> grid_sign;
	double sum_lons;
	double sum_lats;
	int sum_count;
	int max_recorded;
	//read in a person's data
	while (getline(fid_in,tline)){
		stringstream parse_line(tline);
		parse_line>>num1>>num2>>num3;
		
		// begin a new person
		if (num3<100000000){

			//deal with the last person's records first
			if (lons.size()>0){
				for (int k=0;k<lons.size();k++){
					if (lons[k]>lon_max){
						lon_max=lons[k];
					}
					if (lons[k]<lon_min){
						lon_min=lons[k];
					}
					if (lats[k]>lat_max){
						lat_max=lats[k];
					}
					if (lats[k]<lat_min){
						lat_min=lats[k];
					}	
				}
				lat_grid_num=(int)(ceil((lat_max-lat_min)/region_size)+0.00001)+1;
				lon_grid_num=(int)(ceil((lon_max-lon_min)/region_size)+0.00001)+1;
				// use one dimension vector
				grid_vec.reserve(lat_grid_num*lon_grid_num);
				grid_sign.reserve(lat_grid_num*lon_grid_num);
				for (int j=0;j<lat_grid_num;j++){
					for (int k=0;k<lon_grid_num;k++){
						grid_vec.push_back(0);
						grid_sign.push_back(-1);
						grid_lons.push_back(0);
						grid_lats.push_back(0);
					}
				}	
				for (int k=0;k<lons.size();k++){
					point_lat_grid=(int)((lats[k]-lat_min+0.00001)/region_size);
					point_lon_grid=(int)((lons[k]-lon_min+0.00001)/region_size);
					grid_vec[point_lat_grid*lon_grid_num+point_lon_grid]++;
					points_index.push_back(point_lat_grid*lon_grid_num+point_lon_grid);
					serial_number.push_back(0);
					record_lons.push_back(0);
					record_lats.push_back(0);
				}
				// record the nun empty grids and its index
				for (int j=0;j<lat_grid_num;j++){
					for (int k=0;k<lon_grid_num;k++){
						if(grid_vec[j*lon_grid_num+k]>0.5){
							nun_empty_number.push_back(grid_vec[j*lon_grid_num+k]);
							nun_empty_index.push_back(j*lon_grid_num+k);
						}
					}
				}
				// sorting, big to small
				nun_empty_size=nun_empty_number.size();		
				if (nun_empty_size>1){
					for (int j=0;j<nun_empty_size-1;j++){
						for (int k=j+1;k<nun_empty_size;k++){
							if (nun_empty_number[j]<nun_empty_number[k]){
								temp_number=nun_empty_number[k];
								nun_empty_number[k]=nun_empty_number[j];
								nun_empty_number[j]=temp_number;
								temp_number=nun_empty_index[k];
								nun_empty_index[k]=nun_empty_index[j];
								nun_empty_index[j]=temp_number;
							}
						}
					}
				}
				// do the clustering
				cluster_count=0;
				for (int j=0;j<nun_empty_size;j++){
					if (grid_sign[nun_empty_index[j]]==-1){
						cluster_count++;
						grid_sign[nun_empty_index[j]]=cluster_count;
						sum_lons=lon_min+(nun_empty_index[j]%lon_grid_num+0.5)*region_size;
						sum_lats=lat_min+(nun_empty_index[j]/lon_grid_num+0.5)*region_size;
						sum_count=1;
						//update 8 neighbours
						if (nun_empty_index[j]-lon_grid_num-1>0&&grid_sign[nun_empty_index[j]-lon_grid_num-1]==-1){
							grid_sign[nun_empty_index[j]-lon_grid_num-1]=cluster_count;
							sum_lons+=lon_min+((nun_empty_index[j]-lon_grid_num-1)%lon_grid_num+0.5)*region_size;
							sum_lats+=lat_min+((nun_empty_index[j]-lon_grid_num-1)/lon_grid_num+0.5)*region_size;
							sum_count++;
						}
						if (nun_empty_index[j]-lon_grid_num>0&&grid_sign[nun_empty_index[j]-lon_grid_num]==-1){
							grid_sign[nun_empty_index[j]-lon_grid_num]=cluster_count;
							sum_lons+=lon_min+((nun_empty_index[j]-lon_grid_num)%lon_grid_num+0.5)*region_size;
							sum_lats+=lat_min+((nun_empty_index[j]-lon_grid_num)/lon_grid_num+0.5)*region_size;
							sum_count++;

						}
						if (nun_empty_index[j]-lon_grid_num+1>0&&grid_sign[nun_empty_index[j]-lon_grid_num+1]==-1){
							grid_sign[nun_empty_index[j]-lon_grid_num+1]=cluster_count;
							sum_lons+=lon_min+((nun_empty_index[j]-lon_grid_num+1)%lon_grid_num+0.5)*region_size;
							sum_lats+=lat_min+((nun_empty_index[j]-lon_grid_num+1)/lon_grid_num+0.5)*region_size;
							sum_count++;
						}
						if (nun_empty_index[j]-1>0&&grid_sign[nun_empty_index[j]-1]==-1){
							grid_sign[nun_empty_index[j]-1]=cluster_count;
							sum_lons+=lon_min+((nun_empty_index[j]-1)%lon_grid_num+0.5)*region_size;
							sum_lats+=lat_min+((nun_empty_index[j]-1)/lon_grid_num+0.5)*region_size;
							sum_count++;
						}
						if (nun_empty_index[j]+1<lon_grid_num*lat_grid_num&&grid_sign[nun_empty_index[j]+1]==-1){
							grid_sign[nun_empty_index[j]+1]=cluster_count;
							sum_lons+=lon_min+((nun_empty_index[j]+1)%lon_grid_num+0.5)*region_size;
							sum_lats+=lat_min+((nun_empty_index[j]+1)/lon_grid_num+0.5)*region_size;
							sum_count++;
						}
						if (nun_empty_index[j]+lon_grid_num-1<lon_grid_num*lat_grid_num&&grid_sign[nun_empty_index[j]+lon_grid_num-1]==-1){
							grid_sign[nun_empty_index[j]+lon_grid_num-1]=cluster_count;
							sum_lons+=lon_min+((nun_empty_index[j]+lon_grid_num-1)%lon_grid_num+0.5)*region_size;
							sum_lats+=lat_min+((nun_empty_index[j]+lon_grid_num-1)/lon_grid_num+0.5)*region_size;
							sum_count++;
						}
						if (nun_empty_index[j]+lon_grid_num<lon_grid_num*lat_grid_num&&grid_sign[nun_empty_index[j]+lon_grid_num]==-1){
							grid_sign[nun_empty_index[j]+lon_grid_num]=cluster_count;
							sum_lons+=lon_min+((nun_empty_index[j]+lon_grid_num)%lon_grid_num+0.5)*region_size;
							sum_lats+=lat_min+((nun_empty_index[j]+lon_grid_num)/lon_grid_num+0.5)*region_size;
							sum_count++;
						}
						if (nun_empty_index[j]+lon_grid_num+1<lon_grid_num*lat_grid_num&&grid_sign[nun_empty_index[j]+lon_grid_num+1]==-1){
							grid_sign[nun_empty_index[j]+lon_grid_num+1]=cluster_count;
							sum_lons+=lon_min+((nun_empty_index[j]+lon_grid_num+1)%lon_grid_num+0.5)*region_size;
							sum_lats+=lat_min+((nun_empty_index[j]+lon_grid_num+1)/lon_grid_num+0.5)*region_size;
							sum_count++;
						}
						
						grid_lons[nun_empty_index[j]]=sum_lons/sum_count;
						grid_lats[nun_empty_index[j]]=sum_lats/sum_count;
						//update 8 neighbours
						if (nun_empty_index[j]-lon_grid_num-1>0&&grid_sign[nun_empty_index[j]-lon_grid_num-1]==cluster_count){
							grid_sign[nun_empty_index[j]-lon_grid_num-1]=cluster_count;
							grid_lons[nun_empty_index[j]-lon_grid_num-1]=sum_lons/sum_count;
							grid_lats[nun_empty_index[j]-lon_grid_num-1]=sum_lats/sum_count;
						}
						if (nun_empty_index[j]-lon_grid_num>0&&grid_sign[nun_empty_index[j]-lon_grid_num]==cluster_count){
							grid_sign[nun_empty_index[j]-lon_grid_num]=cluster_count;
							grid_lons[nun_empty_index[j]-lon_grid_num]=sum_lons/sum_count;
							grid_lats[nun_empty_index[j]-lon_grid_num]=sum_lats/sum_count;
						}
						if (nun_empty_index[j]-lon_grid_num+1>0&&grid_sign[nun_empty_index[j]-lon_grid_num+1]==cluster_count){
							grid_sign[nun_empty_index[j]-lon_grid_num+1]=cluster_count;
							grid_lons[nun_empty_index[j]-lon_grid_num+1]=sum_lons/sum_count;
							grid_lats[nun_empty_index[j]-lon_grid_num+1]=sum_lats/sum_count;
						}
						if (nun_empty_index[j]-1>0&&grid_sign[nun_empty_index[j]-1]==-1){
							grid_sign[nun_empty_index[j]-1]=cluster_count;
							grid_lons[nun_empty_index[j]-1]=sum_lons/sum_count;
							grid_lats[nun_empty_index[j]-1]=sum_lats/sum_count;
						}
						if (nun_empty_index[j]+1<lon_grid_num*lat_grid_num&&grid_sign[nun_empty_index[j]+1]==cluster_count){
							grid_sign[nun_empty_index[j]+1]=cluster_count;
							grid_lons[nun_empty_index[j]+1]=sum_lons/sum_count;
							grid_lats[nun_empty_index[j]+1]=sum_lats/sum_count;
						}
						if (nun_empty_index[j]+lon_grid_num-1<lon_grid_num*lat_grid_num&&grid_sign[nun_empty_index[j]+lon_grid_num-1]==cluster_count){
							grid_sign[nun_empty_index[j]+lon_grid_num-1]=cluster_count;
							grid_lons[nun_empty_index[j]+lon_grid_num-1]=sum_lons/sum_count;
							grid_lats[nun_empty_index[j]+lon_grid_num-1]=sum_lats/sum_count;
						}
						if (nun_empty_index[j]+lon_grid_num<lon_grid_num*lat_grid_num&&grid_sign[nun_empty_index[j]+lon_grid_num]==cluster_count){
							grid_sign[nun_empty_index[j]+lon_grid_num]=cluster_count;
							grid_lons[nun_empty_index[j]+lon_grid_num]=sum_lons/sum_count;
							grid_lats[nun_empty_index[j]+lon_grid_num]=sum_lats/sum_count;
						}
						if (nun_empty_index[j]+lon_grid_num+1<lon_grid_num*lat_grid_num&&grid_sign[nun_empty_index[j]+lon_grid_num+1]==cluster_count){
							grid_sign[nun_empty_index[j]+lon_grid_num+1]=cluster_count;
							grid_lons[nun_empty_index[j]+lon_grid_num+1]=sum_lons/sum_count;
							grid_lats[nun_empty_index[j]+lon_grid_num+1]=sum_lats/sum_count;
						}
					}
				}
				// mark the serial numbers
				for (int k=0;k<lons.size();k++){
					serial_number[k]=grid_sign[points_index[k]];
					record_lats[k]=grid_lats[points_index[k]];
					record_lons[k]=grid_lons[points_index[k]];
				}
				
				// change the order of the serial number
				serial_number_count=1;
				personal_record_count=lons.size();
				if (personal_record_count>1){
					for (int j=0;j<personal_record_count-1;j++){
						if (serial_number[j]>serial_number_count){
							temp_number=serial_number[j];
							serial_number[j]=serial_number_count;
							for (int k=j+1;k<personal_record_count;k++){
								if (serial_number[k]==serial_number_count){
									serial_number[k]=temp_number;
								}
								else if (serial_number[k]==temp_number){
									serial_number[k]=serial_number_count;
								}
							}
							serial_number_count++;
						}
						if (serial_number[j]==serial_number_count){
							serial_number_count++;
						}
					}
				}
				
				//max location number
				max_location_number=0;
				if (lons.size()>0){
					for (int k=0;k<lons.size();k++){
						if (serial_number[k]>max_location_number){
							max_location_number=serial_number[k];
						}
					}
				}
				
				// fix the 0 lon lat error
				max_recorded=0;
				for (int k=0;k<lons.size();k++){
					if (record_lats[k]>0&&serial_number[k]>max_recorded){
						max_recorded++;
						serial_lats.push_back(record_lats[k]);
						serial_lons.push_back(record_lons[k]);
					}
					if (record_lats[k]<0.5&&record_lons[k]>-0.5){
						record_lats[k]=serial_lats[serial_number[k]-1];
						record_lons[k]=serial_lons[serial_number[k]-1];
					}
				}
				serial_lats.clear();
				serial_lons.clear();
				
				// output the data
				if (lons.size()>0){
					fprintf(fout_id,"%d %d %d %d\n",person_total_count,(int)lons.size(),max_location_number,person_id);
					for (int k=0;k<lons.size();k++){
						fprintf(fout_id,"%d %d %d %11.6f %11.6f\n",serial_number[k],times[k],durations[k],record_lons[k],record_lats[k]);
					}
				}
				grid_vec.clear();
				grid_sign.clear();
				nun_empty_number.clear();
				nun_empty_index.clear();
				serial_number.clear();
				grid_lons.clear();
				grid_lats.clear();
				record_lons.clear();
				record_lats.clear();
			}
			// initial the new person
			person_id=(int)(num1+0.00001);
			person_total_count=(int)(num2+0.00001);
			person_stay_count=num3;
			lons.clear();
			lats.clear();
			times.clear();
			durations.clear();
			points_index.clear();
			lon_min=999;
			lon_max=-999;
			lat_min=999;
			lat_max=-999;
		}
		// not a new person, just push the records
		else{
			parse_line>>num4;
			
			if (num3>time_min&&num3<time_max&&num4>=time_limit){
				lons.push_back(num1);
				lats.push_back(num2);
				times.push_back(num3);
				durations.push_back(num4);
			}
		}
	}
	return 0;
}



