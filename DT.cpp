//usage: g++ -O3 DT.cpp -o DT
// ./DT > DT.txt &

#include <iostream>
#include <string>
#include <utility>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm> 
#include <math.h>
#include <stdlib.h>
#define NaNUM 8
#define LaNUM 10
#define NBINS 10
using namespace std;

bool commuter_mode=true;
double week_num_thres=1; //could filter out people with too short observation
double nw_thres=1; //could filter out people who travel infrequently
bool has_work;
int period=86400*7; //period
int slot_interval=600; //in second
int min_num_stay=50;
int max_num_stay=300;
int rand_range=slot_interval-50;
int day_end_at_home=0;
int day_not_end_at_home=0;
int best_day_end_at_home;
int best_day_not_end_at_home;
double best_nw;
int day_end_at_home_total=0;
int day_not_end_at_home_total=0;
double home_lat,home_lon,work_lat,work_lon;

vector<double> original_real_dt;
vector<double> rescaled_real_dt;
vector<double> original_simu_dt;
vector<double> rescaled_simu_dt;
vector<double> original_real_burst;
vector<double> rescaled_real_burst;
vector<double> original_simu_burst;
vector<double> rescaled_simu_burst;
double original_real_burst_sum=0;
double rescaled_real_burst_sum=0;
int real_burst_count=0;
double original_simu_burst_sum=0;
double rescaled_simu_burst_sum=0;
int simu_burst_count=0;
int burst_t;
int num1;
int num2;
int num3;
int num4=0;
int num5;
double num6;
double num7;
vector<int> times;
vector<int> locs;
vector<int> days;
vector<int> locids;
vector<double> lons;
vector<double> lats;
vector<double> daily_activeness;
vector<double> weekly_activeness;
vector<int>  period_begin_time;
vector<int>  simulated_time;
vector<int>  simulated_at_home;
string tline;
int stay_num=0;
int person_id=0;
int daily_slot_num=86400/slot_interval; 
int slot_num=period/slot_interval; 
int daily_slot;
int weekly_slot;
double nw;
int na;
double week_num;
int current_time;
int end_time;
int period_number;
double randnum;
int active_time;
int active_time_limit;
int addition_times[5]={0};
int counter=0;
vector<int> work_start;
vector<int> work_end;

bool at_home=false;
bool at_work=false;
//
double n1_arr[]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
double n1=0;
int n1_num=20;
//

double n2_arr[]={1, 6, 11, 16, 21, 26, 31, 36, 41, 46, 51, 56, 61, 66, 71, 76, 81, 86, 91, 96, 101};
double n2=0;
int n2_num=21;


double getOtherMovePr(int daily_slot,int weekly_slot, int index, double nw){
	double p_move;
    p_move=n1_arr[index]*nw*daily_activeness[daily_slot]*weekly_activeness[weekly_slot];
	return p_move;
}

double getOtherMoveToHomePr(int daily_slot,int weekly_slot, int index, double nw){
	double p_move_to_home;

    p_move_to_home=1-n2_arr[index]*nw*daily_activeness[daily_slot]*weekly_activeness[weekly_slot];

	return p_move_to_home;
}

double Burstness(vector<double> times){
	int stay_num=times.size();
	double sum_squared_dev=0;
	double sum_time=0;
	double mean_time;
	double mean_squared_dev;
	for (int i=0;i<stay_num;i++){
		sum_time+=times[i];
	}
	mean_time=sum_time/stay_num;
	//cout<<mean_time<<endl;
	for (int i=0;i<stay_num;i++){
		sum_squared_dev+=(times[i]-mean_time)*(times[i]-mean_time);
	}
	mean_squared_dev=sum_squared_dev/stay_num;
	return (sqrt(mean_squared_dev)-mean_time)/(sqrt(mean_squared_dev)+mean_time);
}

//input time is standart time, not Boston time
void RescaleTime(vector<int> times, bool real_time){
	vector<double> slot_activeness;
	int stay_num=times.size();
	double rescaled_time;
	int record_slot;
	int slot1;
	int slot2;
	int interval_num;
	double dt;
	vector<double> personal_original_real_dt;
	vector<double> personal_rescaled_real_dt;
	vector<double> personal_original_simu_dt;
	vector<double> personal_rescaled_simu_dt;
	for (int i=0;i<slot_num;i++){
		slot_activeness.push_back(0);
	}
	for (int i=0;i<stay_num;i++){
		record_slot=(int)((times[i]-18000)%period / slot_interval);
		slot_activeness[record_slot]+=1;
	}
	
	for (int i=1;i<stay_num;i++){
		slot1=(int)((times[i-1]-18000) / slot_interval);
		slot2=(int)((times[i]-18000) / slot_interval);
		interval_num = (times[i]-times[i-1]) / slot_interval;
		dt = times[i]-times[i-1];
		
		int day1=(int)((slot1+0.0)/daily_slot_num);
		int day2=(int)((slot2+0.0)/daily_slot_num);
		if (day2-day1>1){
			interval_num-=(day2-day1-1)*daily_slot_num;
			dt-=(day2-day1-1)*86400;
			continue; //don't consider intervals that are too long
		}
		
		rescaled_time=0;
		for (int j=slot1;j<slot1+interval_num;j++){
			rescaled_time+=slot_activeness[j%slot_num];
		}
		
		
		if (real_time){
			original_real_dt.push_back(dt);
			rescaled_real_dt.push_back(rescaled_time);
			personal_original_real_dt.push_back(dt);
			personal_rescaled_real_dt.push_back(rescaled_time);
		}
		else{
			original_simu_dt.push_back(dt);
			rescaled_simu_dt.push_back(rescaled_time);
			personal_original_simu_dt.push_back(dt);
			personal_rescaled_simu_dt.push_back(rescaled_time);
		}
	}
	
	if (real_time){
		original_real_burst_sum+=Burstness(personal_original_real_dt);
		rescaled_real_burst_sum+=Burstness(personal_rescaled_real_dt);
		real_burst_count++;
	}
	else{
		original_simu_burst_sum+=Burstness(personal_original_simu_dt);
		rescaled_simu_burst_sum+=Burstness(personal_rescaled_simu_dt);
		simu_burst_count++;
	}
}



double AreaTestStat(vector<int> real_time, vector<int> simu_time, int n_bins){
    
    int slot1;
    int slot2;
    double dt;
    int m1 = real_time.size();
    int m2 = simu_time.size();
    int real_stay_num=m1;
    int simu_stay_num=m2;
    double days_real = (real_time[m1-1]-real_time[0] -0.0)/86400.0;
    double days_simu = (simu_time[m2-1]-simu_time[0] -0.0)/86400.0;
    int day_num1 = 1;
    int day_num2 = 1;
    for (int dd=1;dd<m1;dd++){
        int prev_day = (real_time[dd-1]-18000)/86400;
        int curr_day = (real_time[dd]-18000)/86400;
        if (prev_day!=curr_day){
            day_num1++;
        }
        
    }
    for (int dd=1;dd<m2;dd++){
        int prev_day = (simu_time[dd-1]-18000)/86400;
        int curr_day = (simu_time[dd]-18000)/86400;
        if (prev_day!=curr_day){
            day_num2++;
        }
        
    }
    vector<double> dt_real;
    vector<double> dt_simu;
    
    for (int i=0;i<m1-1;i++){
        dt = real_time[i+1]-real_time[i];
        slot1=(int)((real_time[i]-18000)%period / slot_interval);
        slot2=(int)((real_time[i+1]-18000)%period / slot_interval);
        int day1=(int)((slot1+0.0)/slot_num);
        int day2=(int)((slot2+0.0)/slot_num);
        if (day2-day1>1){
            dt-=(day2-day1-1)*86400;
            days_real=days_real-(day2-day1-1);
            continue; //don't consider these intervals
        }
        dt_real.push_back(dt);
    }
    
    for (int i=0;i<m2-1;i++){
        dt = simu_time[i+1]-simu_time[i];
        slot1=(int)((simu_time[i]-18000)%period / slot_interval);
        slot2=(int)((simu_time[i+1]-18000)%period / slot_interval);
        int day1=(int)((slot1+0.0)/slot_num);
        int day2=(int)((slot2+0.0)/slot_num);
        if (day2-day1>1){
            dt-=(day2-day1-1)*86400;
            days_simu=days_simu-(day2-day1-1);
            continue; //don't consider these intervals
        }

        dt_simu.push_back(dt);
    }
    
    m1 = dt_real.size();
    m2 = dt_simu.size();
    
    double min_lnt=999;
    double max_lnt=-999;
    for (int i=0;i<m1;i++){
        if (log(dt_real[i])>max_lnt){
            max_lnt=log(dt_real[i]);
        }
        if (log(dt_real[i])<min_lnt){
            min_lnt=log(dt_real[i]);
        }
    }
    for (int i=0;i<m2;i++){
        if (log(dt_simu[i])>max_lnt){
            max_lnt=log(dt_simu[i]);
        }
        if (log(dt_simu[i])<min_lnt){
            min_lnt=log(dt_simu[i]);
        }
    }
    max_lnt=max_lnt+0.0001;
    min_lnt=min_lnt-0.0001;
    double lnt_range=max_lnt - min_lnt;
    //divide the range to b_bins equal intervals and calculate the ferquency in each
    vector<double> real_freq;
    vector<double> simu_freq;
    for (int i=0;i<n_bins;i++){
        real_freq.push_back(0);
        simu_freq.push_back(0);
    }
    for (int i=0;i<m1;i++){
        int index = (int)((log(dt_real[i])-min_lnt)/lnt_range*n_bins);
        real_freq[index]+=1.00/m1;
    }
    for (int i=0;i<m2;i++){
        int index = (int)((log(dt_simu[i])-min_lnt)/lnt_range*n_bins);
        simu_freq[index]+=1.00/m2;
    }
    //calculate the stat
    double ats=0;
    for (int i=0;i<n_bins;i++){
        ats+=fabs(simu_freq[i]-real_freq[i]);
    }
    ats+=0.035*fabs((real_stay_num+0.0)/day_num1 - (simu_stay_num+0.0)/day_num2);
    return ats;
}


int main(int argc, char** argv){

	string dur_thres = "300";
    
	string file_name0;
	string file_name1;
	string file_name2;
	string file_name3;
	string file_name4;
	string file_name5;
    string file_name6;
    string file_name7;
	//input/output files

	file_name0="StayRegionsFiltered.txt";
    if (commuter_mode){
    	file_name1 = "DTRealCommuters.txt";
    	file_name2 = "DTSimuCommuters.txt";
    	file_name3 = "DNRealCommuters.txt";
    	file_name4 = "DNSimuCommuters.txt";
    	file_name5 = "SimuLocCommuters.txt";
        file_name6 = "ParametersCommuters.txt";
        file_name7 = "RealLocCommuters.txt";
    }
    else{
        file_name1 = "DTRealNonCommuters.txt";
        file_name2 = "DTSimuNonCommuters.txt";
        file_name3 = "DNRealNonCommuters.txt";
        file_name4 = "DNSimuNonCommuters.txt";
        file_name5 = "SimuLocNonCommuters.txt";
        file_name6 = "ParametersNonCommuters.txt";
        file_name7 = "RealLocNonCommuters.txt";
    }

	ifstream fid_in;
	fid_in.open(file_name0.c_str(),ifstream::in);
	
	FILE * fout_id1;
	fout_id1 = fopen(file_name1.c_str(), "w");
	FILE * fout_id2;
	fout_id2 = fopen(file_name2.c_str(), "w");
	FILE * fout_id3;
	fout_id3 = fopen(file_name3.c_str(), "w");
	FILE * fout_id4;
	fout_id4 = fopen(file_name4.c_str(), "w");
	FILE * fout_id5;
	fout_id5 = fopen(file_name5.c_str(), "w");
    FILE * fout_id6;
    fout_id6 = fopen(file_name6.c_str(), "w");
    FILE * fout_id7;
    fout_id7 = fopen(file_name7.c_str(), "w");
	
	daily_activeness.clear();
	for (int i=0;i<daily_slot_num;i++){
		daily_activeness.push_back(0);
	}
	weekly_activeness.clear();
	for (int i=0;i<7;i++){
		weekly_activeness.push_back(0);
	}
	int activity_count=0;
	has_work=false;
	while (getline(fid_in,tline)){

		stringstream parse_line(tline);
		parse_line>>num1>>num2>>num3>>num4>>num5>>num6>>num7;

		//same person
		if (num1==person_id){
			times.push_back(num2);
			locs.push_back(num3);
			days.push_back(num4);
            locids.push_back(num5);
            lons.push_back(num6);
            lats.push_back(num7);
            if (num3==2){
                has_work=true;
            }
		}
		else{
			//deal with the old person
			stay_num=times.size();
			if (stay_num>min_num_stay && stay_num<max_num_stay && has_work==commuter_mode){
				bool valid_sign=true;
				week_num = (times[stay_num-1]-times[0])/604800;
				if (week_num<week_num_thres){
					valid_sign=false;
				}
                for (int i=0;i<stay_num-1;i++){
                    if (times[i+1]<=times[i]){
                        valid_sign=false;
                    }
                }
				if (valid_sign==false){
					person_id=num1;
					times.clear();
					locs.clear();
					days.clear();
                    locids.clear();
                    lons.clear();
                    lats.clear();
                    has_work=false;
					continue;
				}
				if (++counter>100000){
					break;
				}
				//calclate the weekly and daily trend of non work activities
				for (int i=0;i<stay_num;i++){
                    if (locs[i]!=2){
    					daily_slot=(int)((times[i]-18000)%86400 / slot_interval);
    					daily_activeness[daily_slot]+=1;
    					weekly_slot=((int)((times[i]-18000)/86400)) % 7;
    					weekly_activeness[weekly_slot]+=1;
    					activity_count++;
                    }
				}
			}
			//begin the new person
			person_id=num1;
			times.clear();
			locs.clear();
            locids.clear();
            lons.clear();
            lats.clear();
			days.clear();
            has_work=false;
		}
	}
	for (int i=0;i<daily_slot_num;i++){
		daily_activeness[i]/=activity_count;
	}
	for (int i=0;i<7;i++){
		weekly_activeness[i]/=activity_count;

	}
	fid_in.close();
	
	ifstream fid_in1;
	fid_in1.open(file_name0.c_str(),ifstream::in);
	person_id=-1;
	counter=0;
    has_work=false;
	while (getline(fid_in1,tline)){
		stringstream parse_line(tline);
		parse_line>>num1>>num2>>num3>>num4>>num5>>num6>>num7;
		//same person
		if (num1==person_id){
			times.push_back(num2);
			locs.push_back(num3);
			days.push_back(num4);
            locids.push_back(num5);
            lons.push_back(num6);
            lats.push_back(num7);
            if (num3==2){
                has_work=true;
            }
		}
		else{
			//deal with the old person
			stay_num=times.size();
			if (stay_num>min_num_stay && stay_num<max_num_stay && has_work==commuter_mode){
                double work_duration = 0;
                double total_duration = times[stay_num-1]-times[0];
				bool valid_sign=true;
				week_num = (times[stay_num-1]-times[0])/604800;
				if (week_num<2){
					valid_sign=false;
				}
                
                for (int i=0;i<stay_num-1;i++){
                    if (times[i+1]<=times[i]){
                        valid_sign=false;
                    }
                }
                //identify the home and work location
                home_lon=0;
                home_lat=0;
                work_lon=0;
                work_lat=0;
                
                for (int i=0;i<stay_num;i++){
                    if (locs[i]==1){
                        home_lon=lons[i];
                        home_lat=lats[i];
                        break;
                    }
                }
                for (int i=0;i<stay_num;i++){
                    if (locs[i]==2){
                        work_lon=lons[i];
                        work_lat=lats[i];
                        break;
                    }
                }
                
                int day_num=1;
                int home_based_trip=1;
                for (int dd=1;dd<stay_num;dd++){
                    int prev_day = (times[dd-1]-18000)/86400;
                    int curr_day = (times[dd]-18000)/86400;
                    if (prev_day!=curr_day){
                        day_num++;
                    }
                }
                work_end.clear();
                work_start.clear();
                for (int dd=0;dd<stay_num-1;dd++){
                    //trips not going to work
                    if (locs[dd]==1 && locs[dd+1]!=2){
                        home_based_trip++;
                    }
                    if (locs[dd]==2){
                        work_start.push_back(times[dd]);
                        work_end.push_back(times[dd+1]);
                        work_duration+=times[dd+1]-times[dd];
                    }
                }
                int max_work_index=work_start.size()-1;

                nw = home_based_trip/(day_num/7.000);
                if (commuter_mode && total_duration>work_duration){
                    nw=nw*total_duration/(total_duration-work_duration);
                }
                
                if (nw<nw_thres){
                    valid_sign=false;
                }

				if (valid_sign==false){
					person_id=num1;
					times.clear();
					locs.clear();
					days.clear();
                    lons.clear();
                    lats.clear();
                    has_work=false;
					continue;
				}
				
				if (++counter>100000){
					break;
				}

				//best fit parameter
				double best_ats=999;
				double ats;
				int best_index1;
				int best_index2;
				int best_sf;
				int index1_num;
				int index2_num;
                bool need_move=false;
				vector<int> best_simu_time;
				vector<int> best_simu_at_home;
                index1_num=n1_num;
                index2_num=n2_num;

				//nw = home_based_trip/(day_num/7.000); // average number of active periods in a week
				for (int index1=0; index1<index1_num; index1++){
					for (int index2=0; index2<index2_num; index2++){
						day_end_at_home=0;
						day_not_end_at_home=0;
						current_time=times[0];
						end_time=times[stay_num-1];
						period_begin_time.clear(); //the time of the first event in each period
						simulated_time.clear(); 
						simulated_at_home.clear();
						at_home=true;
                        at_work=false;
                        int current_work_index=0;

						simulated_time.push_back(current_time);
						simulated_at_home.push_back(1);
						double pt;
						double p_other_move;
						double p_other_home;
						//the movement logic of the markovian model
						while (current_time<end_time){
                            current_time+=slot_interval+rand()%rand_range-((int)(0.5*rand_range));
                            if (commuter_mode && current_work_index>max_work_index){
                                need_move=false;
                                at_work=false;
                            }
                            
                            if (commuter_mode && current_work_index<=max_work_index){
                                need_move=false;
                                if (current_time>=work_end[current_work_index]){
                                    current_work_index++;
                                    if (at_work){
                                        need_move=true;
                                    }
                                    at_work=false;
                                }
           
                                if (current_work_index<=max_work_index && current_time>work_start[current_work_index]){
                                    if (at_work==false){
                                        //go to work
                                        simulated_time.push_back(current_time);
                                        at_home=false;
                                        at_work=true;
                                        simulated_at_home.push_back(2);
                                        continue;
                                    }
                                }
                            }
                            
							
							daily_slot=(int)((current_time-18000)%86400 / slot_interval);
							weekly_slot=((int)((current_time-18000)/86400)) % 7;
							if (daily_slot==daily_slot_num-1){
								if (at_home){
									day_end_at_home++;
								}
								else{
									day_not_end_at_home++;
								}
							}
							
							if (at_home){
                                pt=nw*daily_activeness[daily_slot]*weekly_activeness[weekly_slot];
								//from home to other
								if ((rand()%100000)/100000.0<pt){
									simulated_time.push_back(current_time);
									at_home=false;
									simulated_at_home.push_back(0);
								}
								//keep at home, do nothing
							}
							else{
                                if (need_move){
                                    p_other_move=1;
                                }
                                else{
                                    p_other_move=getOtherMovePr(daily_slot,weekly_slot,index1,nw);
                                }
								if ((rand()%100000)/100000.0<p_other_move){
									//move to other place
									simulated_time.push_back(current_time);
									//move to home or another other
									p_other_home=getOtherMoveToHomePr(daily_slot,weekly_slot,index2,nw);
									if ((rand()%100000)/100000.0<p_other_home){
										at_home=true;
										simulated_at_home.push_back(1);
									}
									else{
										simulated_at_home.push_back(0);
									}
								}
								//else keep at the current other place, do nothing
							}
						}

						//calculate the goodness of fit
						ats=AreaTestStat(times,simulated_time,NBINS);
						if (ats<best_ats){
							best_ats=ats;
							best_index1=index1;
							best_index2=index2;
							best_simu_time=simulated_time;
							best_simu_at_home=simulated_at_home;
							best_day_end_at_home=day_end_at_home;
							best_day_not_end_at_home=day_not_end_at_home;
							best_nw=nw;
						}
                        
					}
				}

                //run the simulation again
                int index1=best_index1;
                int index2=best_index2;
                
                day_end_at_home=0;
                day_not_end_at_home=0;
                current_time=times[0];
                end_time=times[stay_num-1];
                period_begin_time.clear(); //the time of the first event in each period
                simulated_time.clear();
                simulated_at_home.clear();
                at_home=true;
                at_work=false;
                int current_work_index=0;
                max_work_index=work_start.size()-1;
                simulated_time.push_back(current_time);
                simulated_at_home.push_back(1);
                double pt;
                double p_other_move;
                double p_other_home;
                //the movement logic of the markovian model
                
                while (current_time<end_time){
                    current_time+=slot_interval+rand()%rand_range-((int)(0.5*rand_range));
                    if (commuter_mode && current_work_index>max_work_index){
                        need_move=false;
                        at_work=false;
                    }
                    
                    if (commuter_mode && current_work_index<=max_work_index){
                        need_move=false;
                        if (current_time>=work_end[current_work_index]){
                            current_work_index++;
                            if (at_work){
                                need_move=true;
                            }
                            at_work=false;
                        }
                        
                        if (current_work_index<=max_work_index && current_time>work_start[current_work_index]){
                            if (at_work==false){
                                //go to work
                                simulated_time.push_back(current_time);
                                at_home=false;
                                at_work=true;
                                simulated_at_home.push_back(2);
                                continue;
                            }
                        }
                    }
                    
                    
                    daily_slot=(int)((current_time-18000)%86400 / slot_interval);
                    weekly_slot=((int)((current_time-18000)/86400)) % 7;
                    if (daily_slot==daily_slot_num-1){
                        if (at_home){
                            day_end_at_home++;
                        }
                        else{
                            day_not_end_at_home++;
                        }
                    }
                    
                    if (at_home){
                        pt=nw*daily_activeness[daily_slot]*weekly_activeness[weekly_slot];
                        //from home to other
                        if ((rand()%100000)/100000.0<pt){
                            simulated_time.push_back(current_time);
                            at_home=false;
                            
                            
                            simulated_at_home.push_back(0);
                        }
                        //keep at home, do nothing
                    }
                    else{
                        if (need_move){
                            p_other_move=1;
                        }
                        else{
                            p_other_move=getOtherMovePr(daily_slot,weekly_slot,index1,nw);
                        }
                        if ((rand()%100000)/100000.0<p_other_move){
                            //move to other place
                            simulated_time.push_back(current_time);
                            //move to home or another other
                            p_other_home=getOtherMoveToHomePr(daily_slot,weekly_slot,index2,nw);
                            if ((rand()%100000)/100000.0<p_other_home){
                                at_home=true;
                                simulated_at_home.push_back(1);
                            }
                            else{
                                simulated_at_home.push_back(0);

                            }
                        }
                        //else keep at the current other place, do nothing
                    }
                }

                
                best_simu_time=simulated_time;
                best_simu_at_home=simulated_at_home;
                best_day_end_at_home=day_end_at_home;
                best_day_not_end_at_home=day_not_end_at_home;
                best_nw=nw;

				day_end_at_home_total+=best_day_end_at_home;
				day_not_end_at_home_total+=best_day_not_end_at_home;
				
				//count the daily number of activities
				//for times and best_simu_time
				int current_day;
				int daily_count;
				int daily_location_count;
				int previous_day;
				
				//simulated
				int stay_num_simu;
				bool was_home;
                bool was_work;
				stay_num_simu=best_simu_time.size();
				current_day=(best_simu_time[0]-18000.0)/86400;
				daily_count=1;
				daily_location_count=1;
				if (best_simu_at_home[0]==1){
					was_home=true;
				}
				else{
					was_home=false;
				}
                
                if (best_simu_at_home[0]==2){
                    was_work=true;
                }
                else{
                    was_work=false;
                }
                
				int gap_days;
				for (int i=1;i<stay_num_simu;i++){
					previous_day=current_day;
					current_day=(best_simu_time[i]-18000.0)/86400;
					if (current_day==previous_day){
						daily_count++;
						if (was_home==true&&best_simu_at_home[i]==1){
							//already visited home
						}
                        else if(was_work==true&&best_simu_at_home[i]==2){
                            //already visited work
                        }
						else{
							daily_location_count++;
						}
                        
						if (best_simu_at_home[i]==1){
							was_home=true;
						}
                        
                        if (best_simu_at_home[i]==2){
                            was_work=true;
                        }
					}
					else{
						gap_days=current_day-previous_day-1;
						if (gap_days>0){
							for (int j=0;j<gap_days;j++){
								fprintf(fout_id4, "%d %d\n", 0, 1);
							}
						}
						fprintf(fout_id4, "%d %d\n", daily_count, daily_location_count);
						daily_count=1;
						daily_location_count=1;
                        
						if (best_simu_at_home[i]==1){
							was_home=true;
						}
						else{
							was_home=false;
						}
                        
                        if (best_simu_at_home[i]==2){
                            was_work=true;
                        }
                        else{
                            was_work=false;
                        }
					}
				}
				
				//real
				current_day=(times[0]-18000.0)/86400;
				daily_count=1;
				daily_location_count=1;
                double total_daily_loc_count=0;
                double total_day=0;
				if (locs[0]==1){
					was_home=true;
				}
				else{
					was_home=false;
				}
                if (locs[0]==2){
                    was_work=true;
                }
                else{
                    was_work=false;
                }
                
				for (int i=1;i<stay_num;i++){
					previous_day=current_day;
					current_day=(times[i]-18000.0)/86400;
					if (current_day==previous_day){
						daily_count++;
						if (was_home==true&&locs[i]==1){
							//already visited home
						}
                        else if (was_work==true&&locs[i]==2){
                            //already visited work
                        }
						else{
							daily_location_count++;
						}
						if (locs[i]==1){
							was_home=true;
						}
                        if (locs[i]==2){
                            was_work=true;
                        }
					}
					else{
						gap_days=current_day-previous_day-1;
						if (gap_days>0){
							for (int j=0;j<gap_days;j++){
								fprintf(fout_id3, "%d %d\n", 0, 1);
							}
						}
						fprintf(fout_id3, "%d %d\n", daily_count, daily_location_count);
                        total_daily_loc_count+=daily_location_count;
                        total_day+=1;
						daily_count=1;
						daily_location_count=1;
						if (locs[i]==1){
							was_home=true;
						}
						else{
							was_home=false;
						}
                        if (locs[i]==2){
                            was_work=true;
                        }
                        else{
                            was_work=false;
                        }
					}
				}
                //output the empirical data
                for (int i=0;i<stay_num;i++){
                    fprintf(fout_id7, "%d %d %d %f %d\n", person_id, times[i], locs[i], (times[i]-18000.0)/86400, locids[i]);
                }
				
				//output the location id based on preferential return
				//person id, time, if home, day, location id
				//loc 0 is home, 1 is the first other place
				int visited_location=0; //home
				vector<int> visit_count;
				double visit_count_sum=0;
				vector<double> visit_cdf;
				double p_return;
				double rand_num;
				double rho=0.6;
				double gamma=-0.21;
				int prev_loc=-1;
				for (int i=1;i<stay_num_simu;i++){
					if (best_simu_at_home[i]==1){
						fprintf(fout_id5, "%d %d %d %f %d\n", person_id, best_simu_time[i], 1, (best_simu_time[i]-18000.0)/86400, 0);
						prev_loc=0;
					}
                    else if (best_simu_at_home[i]==2){
                        fprintf(fout_id5, "%d %d %d %f %d\n", person_id, best_simu_time[i], 2, (best_simu_time[i]-18000.0)/86400, 1);
                        prev_loc=0;
                    }
					else{
						if (visited_location<2){
							visited_location++;
							fprintf(fout_id5, "%d %d %d %f %d\n", person_id, best_simu_time[i], 0, (best_simu_time[i]-18000.0)/86400, 2+visited_location);
							visit_count.push_back(1);
							prev_loc=1;
						}
						else{
							double r_return_unconditioned = 0.6*(1-rho*pow(visited_location,gamma));
							p_return=r_return_unconditioned/(r_return_unconditioned+rho*pow(visited_location,gamma));
							if ((rand()%9999999)/9999999.0<p_return){
								//return
								visit_count_sum=0;
								for (int j=0;j<visited_location;j++){
									visit_count_sum+=visit_count[j];
								}
								visit_cdf.clear();
								visit_cdf.push_back(visit_count[0]/visit_count_sum);
								if (visited_location>1){
									for (int j=1;j<visited_location;j++){
										visit_cdf.push_back(visit_cdf[j-1]+visit_count[j]/visit_count_sum);
									}
								}
								
								bool found_sign=false;
								while (found_sign==false){
									rand_num=(rand()%9999999)/9999999.0;
									for (int j=0;j<visited_location;j++){
										if (rand_num<visit_cdf[j]&&j!=prev_loc-1){
											++visit_count[j];
											fprintf(fout_id5, "%d %d %d %f %d\n", person_id, best_simu_time[i], 0, (best_simu_time[i]-18000.0)/86400, j+1+2);
											prev_loc=j+1;
											found_sign=true;
											break;
										}
									}
								}
							}
							else{
								//new location
								visited_location++;
								fprintf(fout_id5, "%d %d %d %f %d\n", person_id, best_simu_time[i], 0, (best_simu_time[i]-18000.0)/86400, visited_location+2);
								visit_count.push_back(1);
							}
						}
					}
				}
			
                fprintf(fout_id6, "%d %d %f %f %f %f %f %f\n", best_index1, best_index2, best_nw, total_daily_loc_count/total_day, home_lon, home_lat, work_lon, work_lat);
				//rescale real and best simulated times
				RescaleTime(times,true);
				RescaleTime(best_simu_time,false);

			}
			//begin the new person
			person_id=num1;
			times.clear();
			locs.clear();
			days.clear();
            lons.clear();
            lats.clear();
            locids.clear();
            has_work=false;
		}
	}
	

	// output the results
	for (int i=0;i<original_real_dt.size();i++){
		fprintf(fout_id1, "%f %f\n",original_real_dt[i],rescaled_real_dt[i]); 
	}
	for (int i=0;i<original_simu_dt.size();i++){
		fprintf(fout_id2, "%f %f\n",original_simu_dt[i],rescaled_simu_dt[i]); 
	}
	fclose(fout_id1);
	fclose(fout_id2);

	return 0;
}
		