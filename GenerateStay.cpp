// usage: g++ -O3 GenerateStay.cpp -o GenerateStay
// ./GenerateStay > GenerateStay.txt &

//generate stay points
#include <iostream>
#include <string>
#include <utility>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>
#define pi 3.14159265358979323846
#define EPSILON 0.0000001
#define stay_dist_limit 0.3
using namespace std;

int time_limit=300; //min stay duration

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


int main(){
	
	//the input file 
	ifstream fid_in;
	//three output files
	FILE * fout_id;
	//three counts
	int count=0;
	int person_id=0;
	int person_total_count=0;
	//three sets of vectors
	vector<double> lons;
	vector<double> lats;
	vector<int> times;
	vector<int> durations;
	string tline;
	double num1;
	double num2;
	int num3;
	double old_num1=0;
	double old_num2=0;
	int old_num3=0;
	bool is_location_stack_full=false;
	int in_stack_count=0;
	double lon_centroid=0;
	double lat_centroid=0;
	int time_begin=0;
	fid_in.open("/home/yingxiang/Boston/RawData.txt",ifstream::in);
	fout_id=fopen("Stays.txt", "w");
	while (getline(fid_in,tline)){
		stringstream parse_line(tline);
		parse_line>>num1>>num2>>num3;
		// begin a new person
		if (AreSame(num3,0)){
			//deal with the last record first
			if (!AreSame(time_begin,0)&&(old_num3-time_begin)>=time_limit){
				lons.push_back(lon_centroid);
				lats.push_back(lat_centroid);
				times.push_back(time_begin);
				durations.push_back(old_num3-time_begin);
				count++;
			}
			if (lons.size()>0){
				fprintf(fout_id,"%d %d %d\n",person_id,person_total_count,count);
				for (int k=0;k<lons.size();k++){
					fprintf(fout_id,"%11.6f %11.6f %d %d\n",lons[k],lats[k],times[k],durations[k]);
				}
			}
			person_id=(int)(num1+0.00001);
			person_total_count=(int)(num2+0.00001);
			count=0;
			old_num1=0;
			old_num2=0;
			old_num3=0;
			lons.clear();
			lats.clear();
			times.clear();
			durations.clear();
			lon_centroid=0;
			lat_centroid=0;
			time_begin=0;
			is_location_stack_full=false;
		}
		//still the same person
		else{
			if (AreSame(num1,old_num1)&&AreSame(num2,old_num2)&&AreSame(num3,old_num3)){
				//skip this record
			}
			else{
				//same place constraint
				if (is_location_stack_full){// has records in staying stack
					if (distance(num2,num1,lat_centroid,lon_centroid,'K')>stay_dist_limit){ //a new moving
						//push the old one to stack
						if (!AreSame(lon_centroid,0)&&(old_num3-time_begin)>=time_limit){
							lons.push_back(lon_centroid);
							lats.push_back(lat_centroid);
							times.push_back(time_begin);
							durations.push_back(old_num3-time_begin);
							count++;
						}
						//initiate a new one
						lon_centroid=num1;
						lat_centroid=num2;
						time_begin=num3;
						in_stack_count=1;
						is_location_stack_full=true;
					}
					else{//not a new moving, update the centroid;
						double new_lon_centroid=(lon_centroid*in_stack_count+num1)/(in_stack_count+1);
						double new_lat_centroid=(lat_centroid*in_stack_count+num2)/(in_stack_count+1);
						lon_centroid=new_lon_centroid;
						lat_centroid=new_lat_centroid;
						in_stack_count++;
					}
				}
				else{// no record, initiate a new one
					lon_centroid=num1;
					lat_centroid=num2;
					time_begin=num3;
					in_stack_count=1;
					is_location_stack_full=true;
				}
			}
			old_num1=num1;
			old_num2=num2;
			old_num3=num3;
		}
	}
	return 0;
}


