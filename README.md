#Steps of running the code

##Step 1: Stay generation

g++ -O3 GenerateStay.cpp -o GenerateStay

./GenerateStay > GenerateStay.txt &

##Step 2: Stay region generation

g++ -O3 GenerateStayRegion.cpp -o GenerateStayRegion

./GenerateStayRegion > GenerateStayRegion.txt &


##Step 3: Filter out infrequent users and users with no identified home locations

g++ -O3 FilterStayRegion.cpp -o FilterStayRegion

./FilterStayRegion > FilterStayRegion.txt &

##Step 4: Calibrate the parameters for the temporal choices

g++ -O3 DT.cpp -o DT

./DT > DT.txt &

Some meta-parameters that can be adjusted are:

1. commuter_mode; if set to “true”, will calculate results for commuters, otherwise non-commuters.

2. week_num_thres; only selecting users with obervation more than week_num_thres weeks.

3. nw_thres; filter out users with too small nw values.

The output include stay duration (DT), daily location number (DN), estimated parameter values for each individual, and real&simulated trajectory of each person.

##Step 5 Estimation of the slopt alpha in rank selection probability

g++ -O3 RankProbability.cpp -o RankProbability

./RankProbability > RankProbability.txt &

##Step 6 Calculate trip distance distribution of exploration trips

There are two types of trips, exploration and returning trips. This script compares emporical & simulated exploration trip distance

g++ -O3 Pr.cpp -o Pr

./Pr HomeLocation.txt OtherLocation.txt PrSimu.txt > Pr.txt &


##Step 7 Location visiting frequency

g++ -O3 FL.cpp -o FL

./FL 10 20 &

./FL 20 30 &

##Step 8: Check results

plots.m: The script of doing plots in matlab 

