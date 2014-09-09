#!/bin/bash

export runnumber=$1;
export eventstouse=$2;

#./mainApp testdata/CERN_files/selectedRange/$runnumber.root resources/GHENT/TDCChMapping.txt $eventstouse testdata/CERN_files/jsonConditions/data_taking_01_05_2013.json $runnumber results/sigmoidsFolder/
#./mainApp testdata/CERN_files/2014_runs/$runnumber.root resources/GHENT/TDCChMapping.txt $eventstouse testdata/CERN_files/jsonConditions/data_taking_7_3_2014.json $runnumber results/sigmoidsFolder/ results/CERN/data_taking_7_3_2014/
./mainApp testdata/CERN_files/2014_runs/4116.root resources/GHENT/TDCChMapping.txt 30000 resources/CERN/jsonFiles/data_taking_7_3_2014.json 4116 results/test/ results/test/ 4116_failedTracks
