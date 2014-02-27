#!/bin/bash

export runnumber=$1;
export eventstouse=$2;

./mainApp testdata/CERN_files/selectedRange/$runnumber.root resources/GHENT/TDCChMapping.txt $eventstouse testdata/CERN_files/jsonConditions/data_taking_01_05_2013.json $runnumber results/sigmoidsFolder/
