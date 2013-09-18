#define rpc_offline_cxx

#include <fstream>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <string>
#include <cstdio>

#include "rpc_offline.h"

using namespace std;
using std::cout;
using std::endl;
using std::vector;
using std::stringstream;


// Definition of SCINT. channels
#define scint_top1a 3
#define scint_top2a 4
#define scint_top3a 5
#define scint_top4a 6
#define scint_top5a 7
#define scint_top1b 8
#define scint_top2b 9
#define scint_top3b 0
#define scint_top4b 1
#define scint_top5b 2

#define scint_bot1a 19
#define scint_bot2a 20
#define scint_bot3a 21
#define scint_bot4a 22
#define scint_bot5a 23
#define scint_bot1b 24
#define scint_bot2b 25
#define scint_bot3b 16
#define scint_bot4b 17
#define scint_bot5b 18

// Size of cosmic stand
#define size 20

int FEBS[13][size], run;
string name_set_tn[size], name_set_tw[size], name_set_b[size], etapartition, etapartitiontowrite, Trigger_layer;
float hv_set_tn[size], hv_set_tw[size], hv_set_b[size], i_set_tn[size], i_set_tw[size], i_set_b[size], hv_mon_tn[size], hv_mon_tw[size], hv_mon_b[size];
float temperature, humidity, pressure;

int main(int argc, char **argv) {
    
    int feb[13], station;
    char temp1[128], run_config_file[256];
    string rff_conf_line, rff_mat_line, rff_mat_line1, rff_mat_line2, rff_mat_line3, rff_mat_line4;
    string run_str_tmp, run_config;
    string variablestring1, variablestring2, variablestring3, variablestring4, chamber_tn, chamber_tw, chamber_b;
    float curr1, vset1, vmon1, curr2, vset2, vmon2, curr3, vset3, vmon3;
    
    bool verbose_openfile = 0;
    
    stringstream stst;
    ifstream in(argv[1]);
    strcpy(temp1,"/home/user/re4data/TDC/");
    strcat(temp1,argv[1]);
    strcat(temp1,".root");
    
    run = atoi (argv[1]);
    
    if (verbose_openfile) cout << " " << endl << " " << endl << "-----------------------------------------------------" << endl;
    if (verbose_openfile) cout << "Opening file: " << temp1 << endl;
    rpc_offline pa(temp1);
    stst << temp1;
    stst >> run_str_tmp;
    
    /*
     // Reading the run number
     ifstream runname("/home/user/re4data/TDC/run");
     if (runname.is_open()) {
     while(runname.good()) {
     runname >> run;
     }
     }
     */
    
    // Reading the daq info
    
    strcpy(run_config_file,"/home/user/re4data/TDC/");
    strcat(run_config_file,argv[1]);
    strcat(run_config_file,".info");
    
    
    //run_config = "/home/user/re4data/TDC/daqinfo";
    if (verbose_openfile) cout << "Run config file: " << run_config << endl;
    //strcpy(run_config_file,run_config.c_str());
    
    ifstream rff(run_config_file);
    if (rff.is_open()) {
        if (verbose_openfile) cout << "Run config file: " << run_config_file << " is opened." << endl;
        
        // This is the header of the daqinfo file
        rff >> variablestring1  >> temperature >> humidity >> pressure;
        if (verbose_openfile) cout << "Temp: " << temperature << " humidity: " << humidity << " pressure: " << pressure << endl;
        
        rff >> variablestring2  >> etapartition >> variablestring3 >> Trigger_layer;
        if (verbose_openfile) cout << "Eta partition: " << etapartition << " trigger: " << Trigger_layer << endl;
        
        getline(rff, rff_conf_line);
        if (verbose_openfile) cout << "reading this line: " << rff_conf_line << endl;
        
        getline(rff, rff_conf_line);
        if (verbose_openfile) cout << "reading this line: " << rff_conf_line << endl;
        
        while(rff.good()) {
            getline(rff, rff_conf_line);
            
            rff >> chamber_tn >> curr1 >> vset1 >> vmon1 >> station;
            rff >> chamber_tw >> curr2 >> vset2 >> vmon2 >> station;
            rff >> chamber_b >> curr3 >> vset3 >> vmon3 >> station;
            rff >> variablestring1  >> feb[1] >> feb[2] >> feb[3] >> feb[4] >> feb[5] >> feb[6] >> feb[7] >> feb[8] >> feb[9] >> feb[10] >> feb[11] >> feb[12];
            
            if (verbose_openfile) cout << " " << endl << " " << endl;
            if (verbose_openfile) cout << "Gap: " << chamber_tn << " current: " << curr1 << " vset: " << vset1 << " vmon1: " << vmon1 << " station=" << station << endl;
            if (verbose_openfile) cout << "Gap: " << chamber_tw << " current: " << curr2 << " vset: " << vset2 << " vmon2: " << vmon2 << " station=" << station << endl;
            if (verbose_openfile) cout << "Gap: " << chamber_b << " current: " << curr3 << " vset: " << vset3 <<  " vmon3: " << vmon3 << " station=" << station << endl;
            if (verbose_openfile) cout << "FEB values: " << feb[1] <<" "<< feb[2] <<" "<< feb[3] <<" "<< feb[4] <<" "<< feb[5] <<" "<< feb[6] <<" "<< feb[7] <<" "<< feb[8] <<" "<< feb[9] <<" "<< feb[10] <<" "<< feb[11] <<" "<< feb[12] << endl;
            
            hv_set_tn[station] = vset1;
            hv_set_tw[station] = vset2;
            hv_set_b[station] = vset3;
            
            hv_mon_tn[station] = vmon1;
            hv_mon_tw[station] = vmon2;
            hv_mon_b[station] = vmon3;
            
            name_set_tn[station] = chamber_tn;
            name_set_tw[station] = chamber_tw;
            name_set_b[station] = chamber_b;
            
            i_set_tn[station] = curr1;
            i_set_tw[station] = curr2;
            i_set_b[station] = curr3;
            
            for ( int ll = 1; ll <= 12; ll++ ) {
                FEBS[ll][station] = feb[ll];
            }
        }
    }
    if (verbose_openfile) cout << "------------------------------ " << endl << " " << endl;
    
    pa.StripProfile();
}

void rpc_offline::StripProfile() {
    
    // Defining variables
    char hname[256], htitle19[256], htitle1[256], htitle2[256], htitle3[256], htitle4[256], htitle5[256], htitle6[256], htitle7[256], htitle8[256], htitle9[256], infovalue311[256], infovalue312[256], infovalue313[256], infovalue211[256], infovalue212[256], infovalue213[256], infovalue111[256], infovalue112[256], infovalue113[256], infovalue114[256], infovalue115[256], infovalue116[256], infovalue117[256], infovalue118[256], infovalue119[256], infovalue[256], infovalue0[256], infovalue1[256], infovalue11[256], infovalue12[256], infovalue13[256], infovalue2[256], infovalue3[256], infovalue4[256], infovalue5[256], infovalue55[256], infovalue97[256], infovalue91[256], infovalue92[256], infovalue93[256], infovalue98[256], infovalue99[256], infovalue81[256], infovalue83[256], infovalue84[256], infovalue85[256], infovalue86[256], infovalue881[256], infovalue882[256], infovalue883[256], infovalue884[256], infovalue811[256], infovalue87[256], infovalue88[256], stringaexec[256], temp[20], string_temperature[100], string_humidity[100], string_pressure[100], string_feb[100], string_run[100], infovalue51[256], infovalue52[256], infovalue53[256], infovalue54[256];
    string conf_line, mat_line, titlestring, titlestringnoise, info, strip_prof_string = "", strip_prof_noise_string = "", clustersizedistr = "", clustersizedistrA = "", clustersizedistrB = "", clustersizedistrC = "", timestamp;
    int multievents, map_module, map_module1, map_station, map_start, map_start1, map_finish, map_finish1, trigger_bit=-99, rpctrigger_bit, aModule, aModule1, i_station, channeltosee, firedchannel, firedchannelA, firedchannelB, firedchannelC, counter=0, kk, kka, kkb, kkc, kka_noise, kkb_noise, kkc_noise, nn, jj, canale, temp1, temp2, temp1noise, temp2noise, cluster, clusterA, clusterB, clusterC, clusterAnoise, clusterBnoise, clusterCnoise, cut, tworpctriggermultiplicity=0, threerpctriggermultiplicity=0, max_station, nicehit=0, nicehitA=0, nicehitB=0, nicehitC=0, nicehitA_T2=0, nicehitB_T2=0, nicehitC_T2=0, nicehitA_T3, nicehitB_T3, nicehitC_T3, counterA=0, counterB=0, counterC=0, onoffcounter=0, hitmultiplicity, clustermultiplicity, hitmultiplicitynoise, clustermultiplicitynoise, trigger_biteta = 0, trg_module = 0, reference_station, reference_station_t2,reference_station_t3,reference_station_t4, reference_module, reference_map_start, reference_map_finish, reference_map_module1, reference_map_start1, reference_map_finish1, reference_module_t2, reference_map_start_t2, reference_map_finish_t2, reference_map_module1_t2, reference_map_start1_t2, reference_map_finish1_t2, reference_module_t3, reference_map_start_t3, reference_map_finish_t3, reference_map_module1_t3, reference_map_start1_t3, reference_map_finish1_t3, reference_module_t4, reference_map_start_t4, reference_map_finish_t4, reference_map_module1_t4, reference_map_start1_t4, reference_map_finish1_t4, counterdecrease, counterout[size], onerpctriggermultiplicity=0, channel_start, channel_finish, channel_module,refcycle_max, rpctrigger_start,rpctrigger_finish,rpctrigger_module;
    int scint_top1,scint_top2,scint_top3,scint_top4,scint_top5,scint_bot1,scint_bot2,scint_bot3,scint_bot4,scint_bot5,scint_total_top,scint_total_bot,shower_count1top=0,shower_count1bot=0,shower_count2top=0,shower_count2bot=0,shower_count3top=0,shower_count3bot=0,shower_count4top=0,shower_count4bot=0,shower_count5top=0,shower_count5bot=0, refcanale, total_tdc_channels, total_time_bins, maxstationreal,cycle_max, trackingreconstruction, triggerhit, triggerhit2, triggerhit3, realcluster, realcluster2, realcluster3, cluster_ok=0, timehit, kind[size];
    float chamber_efficiency[size], chamber_unefficiency[size][96], timing_strip[size][96][50], triggers_per_channels[size][96], chamber_efficiencyA[size], chamber_efficiencyB[size], chamber_efficiencyC[size], noiserate[size], noiserateA[size], noiserateB[size], noiserateC[size], chamber_clustersize, chamber_clustersizeA, chamber_clustersizeB, chamber_clustersizeC, err_eff, err_effA, err_effB, err_effC, err_noise, err_noiseA, err_noiseB, err_noiseC, amountoftime, timewindow, clusterizationA[96], clusterizationB[96], clusterizationC[96], clusterizationAnoise[96], clusterizationBnoise[96], clusterizationCnoise[96], bunchIDmod[size], xtalk, emme, qu, reco1, reco2, reco3, temp_residual, fastest_strip, fastest_time, slowest_time, surfaceA, surfaceB, surfaceC, strip_surf, fastest_striptime[size][96];
    int reco_hit3[96], reco_hit2[96], reco_hit1[96], rpc_track, onetrackatleast, tracked_sector_isA=0, tracked_sector_isB=0, tracked_sector_isC=0, chi2max, qumax, resmax, mmax, trackselected, alreadywritten=0, ch_uneff, channel_by_channel_trg;
    float reco_cluster1[96], reco_cluster2[96], reco_cluster3[96], correct_timing;
    long canale_ev[50];
    
    struct stat st1;
    struct stat st2;
    bool threetrackingrpc = 0;                          // Once enabled (=1) it uses three RPC-based trigger
    bool twotrackingrpc = 1;                            // Once enabled (=1) it uses two RPC-based trigger
    bool largeacceptance = 0;                           // Once enabled (=1) trigger is just TOP&&BOTTOM SCINT. counters
    bool smallacceptance = 0;                           // Once enable (=1) trigger is just small SCINT.
    bool master_trigger, multihit;
    FILE *stream;
    
    // Variables settings
    int trg_windows_min = 0, trg_windows_max = 200;      // Time window in ns.
    int delaytrg = 100;                                  // Time in ns between: [master trigger] and [small counter]
    int scint_multiplicity_max = 3;                      // Number of counts at same time found in one SCINT layer (min. value = 2).
    bool master_trigger_cutedge = 0;                     // Once enabled (=1) it does not consider trigger in lateral SCINT.
    int scint_max = 500;                                 // Max distance in time between SCINT. (ns*10) for shower detection
    bool shower = 1;                                     // Activates shower detection 1=ON, 0=OFF
    int canale_cut = 0;                                  // Strips not to be considered (at the edges)
    int reflimit = 2;                                    // Number of strips not to be
    int maxprocessable = 0;      			             // Leave zero for automatic, put a value if you like to impose a value
    string site = "CERN";					             // This is the site production
    int onerpctriggermultiplicity_max = 5;               // Maximum number of hits in the 1-RPC ref trigger
    int tworpctriggermultiplicity_max = 5;               // Maximum number of hits in the 2-RPC ref trigger
    int threerpctriggermultiplicity_max = 5;             // Maximum number of hits in the 3-RPC ref trigger
    float vertical_space = 110;                          // Vertical space between stations [mm]
    float efficiency_radius = 30;                        // Efficiency radius [mm]
    
    // Cosmetics settings strip, noise, cluster plots
    int strip_start = 0, strip_finish = 96;                         // MIN-MAX X AXIS limit for occupancy
    int maxbins = 100;                                              // MAX Y AXIS - Strip profile
    int hit_max = 100;                                              // MAX Y AXIS - Hit multiplicity
    int noisemaxbins = maxbins*80;                                  // MAX Y AXIS - Noise profile
    int csize_max = maxbins*3;                                      // MAX Y AXIS - Cluster size
    int cmulti_max = maxbins*3;                                     // MAX Y AXIS - Number of clusters
    int timemin = trg_windows_min, timemax = trg_windows_max;       // MAX/MIN Y AXIS 2D histo
    int framext_start = 0, framext_finish = 10;                     // MIN-MAX X AXIS limit for CrossTalk
    int xtmax = maxbins * 2;                                        // MAX Y AXIS limit for CrossTalk
    int max_modules = 7;                                            // MAX modules to be considered (automatically from ROOT file)

    
    // Histogram inizialization
    TH1F *h_Strip[size];
    TH1F *h_Striptiming[size];
    TH1F *h_Stripeffi[size];
    TH1F *h_Stripnoise[size];

    TH1F *h_clustersizeA[size];
    TH1F *h_clustersizeB[size];
    TH1F *h_clustersizeC[size];
    TH1F *h_clustermultiplicity[size];
    TH1F *h_hitmultiplicity[size];

    TH1F *h_clustersizeAnoise[size];
    TH1F *h_clustersizeBnoise[size];
    TH1F *h_clustersizeCnoise[size];
    TH1F *h_clustermultiplicitynoise[size];
    TH1F *h_hitmultiplicitynoise[size];
    
    TH1F *h_xtalk[size];
    TH2F *h_Strip2d;
    TH1F *h_chi2;
    TH1F *h_m;
    TH1F *h_qu;
    TH1F *h_residual[size];
    TH1F *h_residuals;

    
    // Label settings (managed automatically on y)
    int text_xmin = 55, text_xmax = 104, text_ymin = (maxbins - maxbins/3.0), text_ymax = maxbins;                // Label Strip prof.
    int text_xminn = 55, text_xmaxn = 104, text_yminn = (int)TMath::Power(10, ( ( log10(noisemaxbins) ) - (log10(noisemaxbins)/3) )), text_ymaxn = noisemaxbins;          // Label Noise prof.
    int text_xmincs = 6, text_xmaxcs = 10, text_ymincs = (csize_max - csize_max/7), text_ymaxcs = csize_max;     // Label C.Size
    int text_xmincm = 6, text_xmaxcm = 10, text_ymincm = (cmulti_max - cmulti_max/7), text_ymaxcm = cmulti_max;  // Label N.Clusters
    int text_xminh = 6, text_xmaxh = 10, text_yminh = (hit_max - hit_max/7), text_ymaxh = hit_max;               // Label N.Hits
    int text_xminh2, text_xmaxh2, text_yminh2 = (timemax - timemax/2), text_ymaxh2 = timemax;                    // Label 2D
    int text_xminxt = 6, text_xmaxxt = 10, text_yminxt = (xtmax - xtmax/7), text_ymaxxt = xtmax;                 // Label XTalk
    int text_xminchi2 = 10, text_xmaxchi2 = 20;                                                                  // Label FIT chi2
    int text_xminqu = 50, text_xmaxqu = 96;                                                                      // Label FIT q
    int text_xmin_res = 0, text_xmax_res = 16;                                                                   // Label FIT q
    float text_xminm = 0, text_xmaxm = 0.15;                                                                     // Label FIT m
    int text_yminqu, text_ymaxqu, text_yminchi2, text_ymaxchi2, text_yminm, text_ymaxm, text_ymin_res, text_ymax_res;

    
    // Defining verboses
    bool verbose_trg = 1, verbose_map = 0, verbose_filling = 0, verbose_filling2 = 0, verbose_filling_detailed = 0, verbose_filling_eff_counts = 0, verbose_dump = 0, verbose_filling_eff_noise = 0, verbose_clustering1 = 0, verbose_showers = 0, detailed_printout = 0, verbose_fit = 0;
    
    
    Float_t deltastripfast;
    Float_t deltatfastime;
    Float_t deltatfastslowtime;
    
    TFile * hfile = new TFile("/home/user/temp/data.root","RECREATE");
    TTree *tree904 = new TTree("tree904","tree904");
    //tree904->Branch("m_ang", &m_ang, "m_ang/F");
    tree904->Branch("station", &i_station, "i_station/I");
    tree904->Branch("deltatfastime", &deltatfastime, "deltatfastime/F");
    tree904->Branch("deltatfastslowtime", &deltatfastslowtime, "deltatfastslowtime/F");
    tree904->Branch("deltastripfast", &deltastripfast, "deltastripfast/F");
    
    int proceditracking;
    // Initialization
    {
        // Let's avoid unuseful warning
        gErrorIgnoreLevel = kFatal;
        
        for ( kk = 0; kk <= size; kk++ ) {
            chamber_efficiency[kk] = 0;
            chamber_efficiencyA[kk] = 0;
            chamber_efficiencyB[kk] = 0;
            chamber_efficiencyC[kk] = 0;

            for ( int kkk = 0; kkk <= 96; kkk++ ) {
                chamber_unefficiency[kk][kkk] = 0;
                triggers_per_channels[kk][kkk] = 0;

                for ( int kkkk = 1; kkkk <= 50; kkkk++ ) {
                    timing_strip[kk][kkk][kkkk] = 0;
                }
            }
            
            noiserateA[kk] = 0;
            noiserateB[kk] = 0;
            noiserateC[kk] = 0;
            counterout[kk] = 0;
        }
        Int_t numberofbranches = fChain->GetTree()->GetNbranches();
        max_modules = numberofbranches / 130;
        
        // Label for 2D plot
        text_xminh2 = max_modules*128;
        text_xmaxh2 = text_xminh2 + 150;
        //cout << "List of branches: " << numberofbranches << " module = " << max_module << endl;

    }
/*
        // dumping into text file
        FILE *trackfileout; 
        trackfileout = fopen("/home/user/temp/dump_tracks.txt","w");
        fprintf(trackfileout,"%s\n", "Track data strip,angular coeff");    
        fclose(trackfileout);
  */  
    

TEveRecTrack* trackeve = new TEveRecTrack();
TString filename = "/home/user/temp/events_tree.root";
    ostringstream convert;
TFile * hfile2 = new TFile(filename,"RECREATE");
    char c[20];

    if (fChain == 0) return;
    if( c1 ) delete c1;
    c1 = new TCanvas(3);
    Long64_t nentries = fChain->GetEntriesFast();
    if (maxprocessable > 0) nentries = maxprocessable;
    Long64_t nbytes = 0, nb = 0;
    
    trg_windows_min = trg_windows_min*5;
    trg_windows_max = trg_windows_max*5;
    int strip_bin = strip_finish - strip_start;
    int framext_bin = framext_finish - framext_start;
    int xtbins = xtmax;
    //timemin = timemin*10;
    //timemax = timemax*10;
    
    // Setting Module bunchID
    {
        bunchIDmod[1] = 0;
        bunchIDmod[2] = 0;
        bunchIDmod[3] = 1;
        bunchIDmod[4] = 0;
        bunchIDmod[5] = 1;
        bunchIDmod[6] = 0;
        bunchIDmod[7] = 0;
        bunchIDmod[8] = 0;
        bunchIDmod[9] = 0;
        bunchIDmod[10] = 0;
    }
    
    // Reading the runtime
    {
        ifstream runtime("/home/user/re4data/TDC/runtime");
        if (runtime.is_open()) {
            while(runtime.good()) {
                runtime >> amountoftime;
            }
        }
    }
    
    // Reading the reference stations
    {
        
        // REFERENCE STATION FOR 4/2 CHAMBERS
        ifstream refch("/srv/www/htdocs/offlinerefchamber.txt");
        if (refch.is_open()) {
            while(refch.good()) {
                refch >> reference_station;
            }
        }
        else {
            reference_station = -1;
        }
        refch.close();
        
        ifstream refch2("/srv/www/htdocs/offlinerefchamber_t2.txt");
        if (refch2.is_open()) {
            while(refch2.good()) {
                refch2 >> reference_station_t2;
            }
        }
        else {
            reference_station_t2 = -1;
        }
        refch2.close();
        
        
        
        // REFERENCE STATION FOR 4/3 CHAMBERS
        ifstream refch3("/srv/www/htdocs/offlinerefchamber_t3.txt");
        if (refch3.is_open()) {
            while(refch3.good()) {
                refch3 >> reference_station_t3;
            }
        }
        else {
            reference_station_t3 = -1;
        }
        refch3.close();
        
        
        
        ifstream refch4("/srv/www/htdocs/offlinerefchamber_t4.txt");
        if (refch4.is_open()) {
            while(refch4.good()) {
                refch4 >> reference_station_t4;
            }
        }
        else {
            reference_station_t4 = -1;
        }
        refch4.close();
        
        
        
    }
    
    // Reading the mapping file, retrieving reference station(s)
    {
        ifstream map_file("/home/user/workspace/rpcupgrade/Offline/mapping.txt");
        if (map_file.is_open()){
            while (!map_file.eof() ) {
                //while(map_file.good()){
                map_file >> map_station >> map_module >> map_start >> map_finish >> map_module1 >> map_start1 >> map_finish1;
                if ( map_station == reference_station ) {
                    reference_module = map_module;
                    reference_map_start = map_start;
                    reference_map_finish = map_finish;
                    reference_map_module1 = map_module1 ;
                    reference_map_start1 = map_start1;
                    reference_map_finish1 = map_finish1;
                }
                
                if ( map_station == reference_station_t2 ) {
                    reference_module_t2 = map_module;
                    reference_map_start_t2 = map_start;
                    reference_map_finish_t2 = map_finish;
                    reference_map_module1_t2 = map_module1 ;
                    reference_map_start1_t2 = map_start1;
                    reference_map_finish1_t2 = map_finish1;
                }
                
                
                if ( map_station == reference_station_t3 ) {
                    reference_module_t3 = map_module;
                    reference_map_start_t3 = map_start;
                    reference_map_finish_t3 = map_finish;
                    reference_map_module1_t3 = map_module1 ;
                    reference_map_start1_t3 = map_start1;
                    reference_map_finish1_t3 = map_finish1;
                }
                
                if ( map_station == reference_station_t4 ) {
                    reference_module_t4 = map_module;
                    reference_map_start_t4 = map_start;
                    reference_map_finish_t4 = map_finish;
                    reference_map_module1_t4 = map_module1 ;
                    reference_map_start1_t4 = map_start1;
                    reference_map_finish1_t4 = map_finish1;
                }
                
            }
        }
        map_file.close();
    }
    
    // Reading the timewindow
    {
        IniFile * ini = new IniFile( "/home/user/workspace/rpcupgrade/TDC/rpc.ini"  );
        ini->Read();
        cout << "Trigger Windows Width: " << ini->Int( "Module2", "TriggerWindowWidth", 1000 ) << endl;
        timewindow = ini->Int( "Module2", "TriggerWindowWidth", 1000 );
        timewindow=timewindow*25/1000000000;      // expressing time windows in sec:
    }
    
    // TRIGGER
    {
        // Reading the trigger bit
        {
            ifstream conf_file("/home/user/workspace/rpcupgrade/Offline/offline.cfg");
            if (conf_file.is_open()){
                while(conf_file.good()){
                    getline(conf_file, conf_line);
                    mat_line = "trigger : ";
                    if ( ((int)conf_line.find(mat_line))!=-1){
                        conf_line.erase(0,mat_line.length());
                        trigger_bit = atoi(conf_line.c_str());
                    }
                    conf_file.close();
                }
            }
            conf_file.close();
        }
        
        // Reading the trigger bit (eta partition)
        {
            ifstream conf_file2("/home/user/workspace/rpcupgrade/Offline/offline_eta.cfg");
            if (conf_file2.is_open()){
                while(conf_file2.good()){
                    getline(conf_file2, conf_line);
                    mat_line = "trigger : ";
                    if ( ((int)conf_line.find(mat_line))!=-1){
                        conf_line.erase(0,mat_line.length());
                        trigger_biteta = atoi(conf_line.c_str());
                    }
                    conf_file2.close();
                }
            }
            conf_file2.close();
        }
        
        // Reading eta partition under test
        {
            ifstream conf_file3("/srv/www/htdocs/offlinetrg_eta.txt");
            if (conf_file3.is_open()){
                while(conf_file3.good()){
                    conf_file3 >> etapartition;
                }
            }
            conf_file3.close();
        }
        
        // Reading the large acceptance bit
        {
            
            ifstream conf_file4("/home/user/workspace/rpcupgrade/Offline/offlinelarge.cfg");
            if (conf_file4.is_open()) {
                while(conf_file4.good()) {
                    conf_file4 >> largeacceptance;
                }
            }
            conf_file4.close();
        }
        
        // Reading the small acceptance bit
        {
            ifstream conf_file4("/home/user/workspace/rpcupgrade/Offline/offlinesmall.cfg");
            if (conf_file4.is_open()) {
                while(conf_file4.good()) {
                    conf_file4 >> smallacceptance;
                }
            }
            conf_file4.close();
        }
        
        // Reading the two rpc trigger bit
        {
            ifstream conf_file5("/home/user/workspace/rpcupgrade/Offline/offlinetworpc.cfg");
            if (conf_file5.is_open()) {
                while(conf_file5.good()) {
                    conf_file5 >> twotrackingrpc;
                }
            }
            conf_file5.close();
        }
        
        // Reading the three rpc trigger bit
        {
            ifstream conf_file51("/home/user/workspace/rpcupgrade/Offline/offlinethreerpc.cfg");
            if (conf_file51.is_open()) {
                while(conf_file51.good()) {
                    conf_file51 >> threetrackingrpc;
                }
            }
            conf_file51.close();
        }
        
        // Reading the three reconstruction bit
        {
            ifstream conf_file52("/home/user/workspace/rpcupgrade/Offline/reconstruction.cfg");
            if (conf_file52.is_open()) {
                while(conf_file52.good()) {
                    conf_file52 >> trackingreconstruction;
                }
            }
            conf_file52.close();
        }
        
        // Printing Trigger Information
        {
            if (verbose_trg) {
                
                cout << " " << endl << " " << endl << "-----------------------------------------------------" << endl;
                cout << "Master trigger bit :  " << trigger_bit << endl;
                cout << "-----------------------------------------------------" << endl << " " << endl << " " << endl;
                cout << " " << endl;
                
                if (largeacceptance == 1) {
                    cout << " " << endl;
                    cout << "---------------------------------------- " << endl;
                    cout << " Using large acceptance trigger, eta=ALL " << endl;
                    cout << "---------------------------------------- " << endl;
                    cout << " " << endl;
                    if (etapartition !="ALL" ) {
                        cout << "Warning eta partition was not set to 'ALL'... so I am setting it." << endl << endl;
                        etapartition="ALL";
                    }
                }
                
                if (twotrackingrpc == 1) {
                    cout << " " << endl;
                    cout << "---------------------------------------- " << endl;
                    cout << " Using two tracking RPCs in the trigger " << endl;
                    cout << "---------------------------------------- " << endl;
                    
                    if ( (reference_station<=0) || (reference_station_t2<=0) ) {
                        cout << " Warning REFERENCE STATIONS NOT PROPERLY configured, going out..." << endl << endl;
                        return;
                    }
                    
                    // First reference
                    {
                        cout << " First reference detectors is on station " << reference_station << " ( module = " << reference_module << ", " << reference_map_start << "<channel<" << reference_map_finish;
                        
                        if ( reference_map_module1 > 0 ) {
                            cout << " and module = " << reference_map_module1 << ", " << reference_map_start1 << "<channel<" << reference_map_finish1 << " )" << endl;
                        }
                        else {
                            cout << " )" << endl;
                        }
                    }
                    
                    // Second reference
                    {
                        cout << " Second reference detectors is on station " << reference_station_t2 << " ( module = " << reference_module_t2 << ", " << reference_map_start_t2 << "<channel<" << reference_map_finish_t2;
                        
                        if ( reference_map_module1_t2 > 0 ) {
                            cout << " and module = " << reference_map_module1_t2 << ", " << reference_map_start1_t2 << "<channel<" << reference_map_finish1_t2 << " )" << endl;
                        }
                        else {
                            cout << " )" << endl;
                        }
                        cout << "--------------------------" << endl;
                        cout << " " << endl;
                    }
                    
                    /*
                     // Fourth reference
                     {
                     cout << " Forth reference detectors is on station " << reference_station_t4 << " ( module = " << reference_module_t4 << ", " << reference_map_start_t4 << "<channel<" << reference_map_finish_t4;
                     
                     if ( reference_map_module1_t4 > 0 ) {
                     cout << " and module = " << reference_map_module1_t4 << ", " << reference_map_start1_t4 << "<channel<" << reference_map_finish1_t4 << " )" << endl;
                     }
                     else {
                     cout << " )" << endl;
                     }
                     cout << "--------------------------" << endl;
                     cout << " " << endl;
                     }
                     */
                    
                    if (etapartition != "ALL") {
                        etapartition="ALL";
                        cout << "Warning eta partition was not set to 'ALL'... so I am setting it to 'ALL'." << endl << endl;
                    }
                }
                
                if (threetrackingrpc == 1) {
                    cout << " " << endl;
                    cout << "----------------------------------------- " << endl;
                    cout << " Using three tracking RPCs in the trigger " << endl;
                    cout << "----------------------------------------- " << endl;
                    if ( trackingreconstruction == 0 ) cout << " Adopting coincidence between detectors... " << endl << endl;
                    if ( trackingreconstruction == 1 ) cout << " Adopting tracking alorithm between detectors... " << endl << endl;

                    if ( (reference_station<=0) || (reference_station_t2<=0) || (reference_station_t3<=0) ) {
                        cout << " Warning REFERENCE STATIONS NOT PROPERLY configured, going out..." << endl << endl;
                        return;
                    }
                    
                    // First reference
                    {
                        cout << " First reference detectors is on station " << reference_station << " ( module = " << reference_module << ", " << reference_map_start << "<channel<" << reference_map_finish;
                        
                        if ( reference_map_module1 > 0 ) {
                            cout << " and module = " << reference_map_module1 << ", " << reference_map_start1 << "<channel<" << reference_map_finish1 << " )" << endl;
                        }
                        else {
                            cout << " )" << endl;
                        }
                    }
                    
                    // Second reference
                    {
                        cout << " Second reference detectors is on station " << reference_station_t2 << " ( module = " << reference_module_t2 << ", " << reference_map_start_t2 << "<channel<" << reference_map_finish_t2;
                        
                        if ( reference_map_module1_t2 > 0 ) {
                            cout << " and module = " << reference_map_module1_t2 << ", " << reference_map_start1_t2 << "<channel<" << reference_map_finish1_t2 << " )" << endl;
                        }
                        else {
                            cout << " )" << endl;
                        }
                    }
                    
                    // Third reference
                    {
                        cout << " Third reference detectors is on station " << reference_station_t3 << " ( module = " << reference_module_t3 << ", " << reference_map_start_t3 << "<channel<" << reference_map_finish_t3;
                        
                        if ( reference_map_module1_t3 > 0 ) {
                            cout << " and module = " << reference_map_module1_t3 << ", " << reference_map_start1_t3 << "<channel<" << reference_map_finish1_t3 << " )" << endl;
                        }
                        else {
                            cout << " )" << endl;
                        }
                        cout << "--------------------------" << endl;
                        cout << " " << endl;
                    }
                    
                    if (etapartition != "ALL") {
                        etapartition="ALL";
                        cout << "Warning eta partition was not set to 'ALL'... so I am setting it to 'ALL'." << endl << endl;
                    }
                }
                
                if ( (twotrackingrpc==1) && (largeacceptance==1) ) {
                    cout << " " << endl;
                    cout << "---------------------------------------- " << endl;
                    cout << " You selected two tracking RPCs and Large acceptance!!!!!!!!!!!!!! " << endl;
                    cout << " You selected two tracking RPCs and Large acceptance!!!!!!!!!!!!!! " << endl;
                    cout << " L I K E L Y     Y O U     D I D     A     M I S T A K E. . . .  going out.   " << endl;
                    cout << "---------------------------------------- " << endl;
                    cout << " " << endl;
                    return;
                }
                
                if ( (twotrackingrpc==0) && (largeacceptance==0) && (threetrackingrpc==0) ) {
                    
                    if (smallacceptance==1) {
                        
                        cout << " " << endl;
                        cout << "---------------------------------------- " << endl;
                        cout << " You selected the SMALL SCINT pad, (Trigger bit is "<< trigger_biteta << ")" << endl;
                        cout << " (small pad is on eta=" << etapartition << ")" << endl;
                        cout << "---------------------------------------- " << endl;
                        cout << " " << endl;
                        
                        if (etapartition == "ALL") {
                            etapartition="A";
                            cout << "Warning eta partition was set to 'ALL'... so I am setting it to 'A'." << endl << endl;
                        }
                    }
                    else {
                        cout << " " << endl;
                        cout << "---------------------------------------- " << endl;
                        cout << " You selected ONE tracking RPCs " << endl;
                        cout << "---------------------------------------- " << endl;
                        
                        if ( reference_station<=0 ) {
                            cout << " Warning REFERENCE STATION NOT PROPERLY configured, going out..." << endl << endl;
                            return;
                        }
                        
                        cout << " Reference detectors is on station " << reference_station << " ( module = " << reference_module << ", " << reference_map_start << "<channel<" << reference_map_finish;
                        
                        if ( reference_map_module1 > 0 ) {
                            cout << " and module = " << reference_map_module1 << ", " << reference_map_start1 << "<channel<" << reference_map_finish1 << " )" << endl;
                        }
                        else {
                            cout << " )" << endl;
                        }
                        cout << "--------------------------" << endl;
                        
                        if (etapartition != "ALL") {
                            etapartition="ALL";
                            cout << "Warning eta partition was not set to 'ALL'... so I am setting it to 'ALL'." << endl << endl;
                        }
                    }
                    
                }
            }
        }
    }
    
    // Defining empty plot templates (also finding max_station)
    {
        int tdhistogramon = 1;
        ifstream map_file1("/home/user/workspace/rpcupgrade/Offline/mapping.txt");
        if (map_file1.is_open()){
            if (verbose_map) cout << " " << endl;
            //while(map_file1.good()){
            while ( map_file1 >> map_station >> map_module >> map_start >> map_finish >> map_module1 >> map_start1 >> map_finish1 ) {
                //map_file1 >> map_station >> map_module >> map_start >> map_finish >> map_module1 >> map_start1 >> map_finish1;
                if (verbose_map) cout << " Station: " << map_station << " module=" << map_module << " starting at channel: " << map_start << " and finishing at: " << map_finish << " while module2 is: " << map_module1 << " starting at channel: " << map_start1 << " and finishing at: " << map_finish1 << endl;
                if (!map_file1.good()) break;
                
                
                // Extracting chamber ID
                size_t pos, pos2;
                pos = name_set_tn[map_station].find("_tn");
                pos2 = name_set_tn[map_station].find("4-3");
                if (verbose_map) cout << "Forming the template for chamber: " << name_set_tn[map_station] << " " << endl;
                if ( (pos>0) && (pos<25) ) name_set_tn[map_station] = name_set_tn[map_station].substr (15,3);
                if ( (pos2>0) && (pos2<25) ) {
                    //cout << " Found 4/3 chamber! " << name_set_tn[map_station] << endl;
                    kind[map_station] = 1;
                }
                else {
                    //cout << " Found 4/2 chamber! " << name_set_tn[map_station] << endl;
                    kind[map_station] = 0;
                }
                
                // Forming a nice histogram title for strip profile
                titlestring = "Strips Chamber ID: ";
                titlestring = titlestring + name_set_tn[map_station];
                titlestring = titlestring + " at station: ";
                stringstream sstm;
                sstm << titlestring << map_station;
                titlestring = sstm.str();
                titlestring = titlestring + " TRG: ";
                titlestring = titlestring + Trigger_layer;
                titlestring = titlestring + " (eta=";
                titlestring = titlestring + etapartition;
                titlestring = titlestring + ")";
                strcpy( htitle1, titlestring.c_str() );
                h_Strip[map_station] = new TH1F(htitle1, htitle1, strip_bin, strip_start, strip_finish);

                
                // Forming a nice histogram title for strip profile (efficient hits!)
                strcpy( htitle1, titlestring.c_str() );
                h_Stripeffi[map_station] = new TH1F(htitle1, htitle1, strip_bin, strip_start, strip_finish);

                
                // Forming a nice histogram title for noise profile
                titlestring = "Noisy strips Chamber ID: ";
                titlestring = titlestring + name_set_tn[map_station];
                titlestring = titlestring + " at station: ";
                stringstream sstm2;
                sstm2 << titlestring << map_station;
                titlestring = sstm2.str();
                titlestring = titlestring + " TRG: ";
                titlestring = titlestring + Trigger_layer;
                titlestring = titlestring + " (eta=";
                titlestring = titlestring + etapartition;
                titlestring = titlestring + ")";
                strcpy( htitle2, titlestring.c_str() );
                h_Stripnoise[map_station] = new TH1F(htitle2, htitle2, strip_bin, strip_start, strip_finish);
                
                
                // Forming a nice histogram title for cluster sizes
                titlestring = "Cluster size Chamber ID: ";
                titlestring = titlestring + name_set_tn[map_station];
                titlestring = titlestring + " at station: ";
                stringstream sstm3;
                sstm3 << titlestring << map_station;
                titlestring = sstm3.str();
                titlestring = titlestring + " TRG: ";
                titlestring = titlestring + Trigger_layer;
                strcpy( htitle3, titlestring.c_str() );
                h_clustersizeA[map_station] = new TH1F(htitle3, htitle3, 10, 1, 11);
                h_clustersizeB[map_station] = new TH1F(htitle3, htitle3, 10, 1, 11);
                h_clustersizeC[map_station] = new TH1F(htitle3, htitle3, 10, 1, 11);

                h_clustersizeAnoise[map_station] = new TH1F(htitle3, htitle3, 32, 1, 32);
                h_clustersizeBnoise[map_station] = new TH1F(htitle3, htitle3, 32, 1, 32);
                h_clustersizeCnoise[map_station] = new TH1F(htitle3, htitle3, 32, 1, 32);

                
                // Forming a nice histogram title for cluster multiplicity
                titlestring = "Cluster multiplicity Chamber ID: ";
                titlestring = titlestring + name_set_tn[map_station];
                titlestring = titlestring + " at station: ";
                stringstream sstm4;
                sstm4 << titlestring << map_station;
                titlestring = sstm4.str();
                titlestring = titlestring + " TRG: ";
                titlestring = titlestring + Trigger_layer;
                titlestring = titlestring + " (eta=";
                titlestring = titlestring + etapartition;
                titlestring = titlestring + ")";
                strcpy( htitle4, titlestring.c_str() );
                h_clustermultiplicity[map_station] = new TH1F(htitle4, htitle4, 96, 0, 96);

                h_clustermultiplicitynoise[map_station] = new TH1F(htitle4, htitle4, 96, 0, 96);
                
                
                // Forming a nice histogram title for xtalk profile
                titlestring = "CrossTalk Chamber ID: ";
                titlestring = titlestring + name_set_tn[map_station];
                titlestring = titlestring + " at station: ";
                stringstream sstm21;
                sstm21 << titlestring << map_station;
                titlestring = sstm2.str();
                titlestring = titlestring + " TRG: ";
                titlestring = titlestring + Trigger_layer;
                titlestring = titlestring + " (eta=";
                titlestring = titlestring + etapartition;
                titlestring = titlestring + ")";
                strcpy( htitle2, titlestring.c_str() );
                h_xtalk[map_station] = new TH1F(htitle2, htitle2, xtbins, 0, xtmax);
                
                
                // Forming a nice histogram title for hit multiplicity
                titlestring = "Hit multiplicity Chamber ID: ";
                titlestring = titlestring + name_set_tn[map_station];
                titlestring = titlestring + " at station: ";
                stringstream sstm92;
                sstm92 << titlestring << map_station;
                titlestring = sstm92.str();
                titlestring = titlestring + " TRG: ";
                titlestring = titlestring + Trigger_layer;
                titlestring = titlestring + " (eta=";
                titlestring = titlestring + etapartition;
                titlestring = titlestring + ")";
                strcpy( htitle5, titlestring.c_str() );
                h_hitmultiplicity[map_station] = new TH1F(htitle5, htitle5, 96, 0, 96);
                h_hitmultiplicitynoise[map_station] = new TH1F(htitle5, htitle5, 96, 0, 96);
                
                
                
                // Forming a nice histogram for timing strips
                titlestring = "Strip timing: ";
                titlestring = titlestring + name_set_tn[map_station];
                titlestring = titlestring + " at station: ";
                stringstream sstm39;
                sstm39 << titlestring << map_station;
                titlestring = sstm39.str();
                titlestring = titlestring + " TRG: ";
                titlestring = titlestring + Trigger_layer;
                strcpy( htitle3, titlestring.c_str() );
                h_Striptiming[map_station] = new TH1F(htitle3, htitle3, 10000, 0, 10000);
                
                
                
                // Forming a nice histogram title for chi2, m, qu
                titlestring = "chi2";
                strcpy( htitle7, titlestring.c_str() );
                h_chi2 = new TH1F(htitle7, htitle7, 100, 0, 20);

                titlestring = "m";
                strcpy( htitle8, titlestring.c_str() );
                h_m = new TH1F(htitle8, htitle8, 200, -0.2, 0.2);

                titlestring = "q";
                strcpy( htitle9, titlestring.c_str() );
                h_qu = new TH1F(htitle9, htitle9, 96, 0, 96);

                titlestring = "residualX";
                strcpy( htitle19, titlestring.c_str() );
                h_residual[map_station] = new TH1F(htitle19, htitle9, 96, -16, 16);

                titlestring = "residualX";
                strcpy( htitle19, titlestring.c_str() );
                h_residuals = new TH1F(htitle9, htitle19, 96, -16, 16);

                
                max_station = map_station;
                //max_modules = map_module;
                
                // 2D Special histo preparation
                total_tdc_channels = 128 * max_modules;
                total_time_bins = 10*abs(timemax) + abs(timemin);
                maxstationreal =  (total_tdc_channels - 32 ) / 96;
                maxstationreal = (int)maxstationreal;
                if (tdhistogramon == 1) h_Strip2d = new TH2F("2D hit distribution","DAQ SUMMARY", total_tdc_channels, strip_start, total_tdc_channels, total_time_bins, timemin, timemax);
                tdhistogramon = 0;
            }
            if (verbose_map) cout << " " << endl;
        }
        map_file1.close();
    }
    
    // Calculating time of run
    {
        strcpy(stringaexec,"/home/user/re4data/TDC/");
        sprintf(temp, "%d",run);
        strcat(stringaexec,temp);
        strcat(stringaexec,".info");
        //int ierr1 = stat (stringaexec,&st1);
        //int newdate1 = st1.st_mtime;
        // Writing the current timestamps on a file (do debug)
        //if( (stream = fopen( "/home/user/re4data/TDC/timestamp1", "w" )) != NULL ) {
        //   fprintf (stream, "%d\n",newdate1);
        //   fclose( stream );
        //}
        
        
        strcpy(stringaexec,"/home/user/re4data/TDC/");
        sprintf(temp, "%d",run);
        strcat(stringaexec,temp);
        strcat(stringaexec,".root");
        //int ierr2 = stat (stringaexec,&st2);
        //int newdate2 = st2.st_mtime;
        // Writing the current timestamps on a file (do debug)
        //if( (stream = fopen( "/home/user/re4data/TDC/timestamp2", "w" )) != NULL ) { 
        //   fprintf (stream, "%d\n",newdate2);
        //   fclose( stream );
        //}
        
        //int timerun = nentries * cosmicrate;
    }


    // Data Processing
    {
        
        // Opening and reading the mapping file
        ifstream map_file("/home/user/workspace/rpcupgrade/Offline/mapping.txt");
        
        if (map_file.is_open()){
            while ( map_file >> i_station >> aModule  >> map_start >> map_finish >> aModule1  >> map_start1 >> map_finish1 ) {

                // Go out if reached max station
                if (    ( aModule >= max_modules ) || ( aModule1 >= max_modules ) ) break;
                
                // Printing info
                {
                    if (verbose_map) cout << " " << endl << "Module: " << aModule << " Station: " << i_station << " starting at channel: " << map_start << " and finishing at: " << map_finish << " " << " while module2 is: " << aModule1 << " starting at channel: " << map_start1 << " and finishing at: " << map_finish1 << endl;
                }

                // Looping over all entries
                
                for ( Long64_t jentry = 0; jentry < nentries; jentry++ ) {
                    
                    if ( jentry%50 == 0 && verbose_filling ) cout << " " << jentry << "/" << nentries << "\r" << flush;
                    
                    // Initialization working variables and Cluster vectors
		    
                    {
                        //cout << Starting with Initialization << endl;
                        master_trigger = 1;
                        onerpctriggermultiplicity = 0;
                        tworpctriggermultiplicity = 0;
                        threerpctriggermultiplicity = 0;
                        timehit = 0;
                        rpc_track = 0;
                        triggerhit = 0;
                        triggerhit2 = 0;
                        triggerhit3 = 0;
                        onetrackatleast = 0;
                        realcluster = 0;
                        realcluster2 = 0;
                        realcluster3 = 0;
                        trackselected = 0;
                        temp_residual = 0;
                        
                        ch_uneff = 0;
                        channel_by_channel_trg = 0;

                        for ( kk = 0; kk <= size; kk++ ) {
                            for ( int kkk = 0; kkk <= 96; kkk++ ) {
                                
                                fastest_striptime[kk][kkk] = 0;
                                for ( int kkkk = 1; kkkk <= 50; kkkk++ ) {
                                    timing_strip[kk][kkk][kkkk] = 0;
                                }
                            }
                        }
                        
                        nicehit = 0;
                        nicehitA = 0;
                        nicehitB = 0;
                        nicehitC = 0;
                        nicehitA_T2 = 0;
                        nicehitB_T2 = 0;
                        nicehitC_T2 = 0;
                        nicehitA_T3 = 0;
                        nicehitB_T3 = 0;
                        nicehitC_T3 = 0;                        
                        
                        onoffcounter = 0;

                        proceditracking = 0;
 
                        firedchannel = 0;
                        firedchannelA = 0;
                        firedchannelB = 0;
                        firedchannelC = 0;
                        
                        counterdecrease = 0;
                        
                        kka = 1;
                        kkb = 1;
                        kkc = 1;
                        
                        kka_noise = 1;
                        kkb_noise = 1;
                        kkc_noise = 1;
			
                        clustermultiplicity = 0;
                        hitmultiplicity = 0;
			
                        clustermultiplicitynoise = 0;
                        hitmultiplicitynoise = 0;
			
                        multihit = 1;
                        cluster=0;
                        xtalk = 0;
                        
                        for ( jj = 0; jj <= 95; jj++ ) {
                            clusterizationA[jj] = 0;
                            clusterizationB[jj] = 0;
                            clusterizationC[jj] = 0;
                            
                            clusterizationAnoise[jj] = 0;
                            clusterizationBnoise[jj] = 0;
                            clusterizationCnoise[jj] = 0;

                            reco_hit1[jj] = 0;
                            reco_hit2[jj] = 0;
                            reco_hit3[jj] = 0;
                            reco_cluster1[jj] = 0;
                            reco_cluster2[jj] = 0;
                            reco_cluster3[jj] = 0;
                        }
                        
                        /*
                        for ( jj = 0; jj <= 1000; jj++ ) {
                            table[1][jj] = 999;
                            table[2][jj] = 999;
                            table[3][jj] = 999;
                            table[4][jj] = 999;
                        }
                        */
                                                
                        Long64_t ientry = this->LoadTree(jentry); // LoadTree(jentry);
                        if (ientry < 0) break; // kind of protection
                        
                        nb = fChain->GetEntry(jentry);
                        nbytes += nb;
                        // if (Cut(ientry) < 0) continue;
                    }
                    
                    //if (i_station==1) cout << " LEGGIMI " << setprecision(4) << jentry << " A = " << tracked_sector_isA << " B = " << tracked_sector_isB << " C = " << tracked_sector_isC << " effiA = " << chamber_efficiencyA[1] << " effiB = " << chamber_efficiencyB[1] << " effiC = " << chamber_efficiencyC[1] << endl;
                    
                    // Initialization of master trigger and small counter
                    
                    vector<unsigned int> * TRIG = _Data[ trg_module ][ trigger_bit ];
                    vector<unsigned int> * TRIGETA = _Data[ trg_module ][ trigger_biteta ];
                    vector<unsigned int> * HITS_PER_CHANNEL = HitsPerChannel[ trg_module ];
                    if ( HITS_PER_CHANNEL->at( trigger_bit ) < 1 ) master_trigger = 0;
                    
                    long trig = 99999;
                    trig = TRIG->at( 0 );

                    // Showers rejection and trigger general selection
                    if (shower && master_trigger) {
                        
                        // Trigger initialization
                        long trig = TRIG->at( 0 );
                        
                        // SCINT. Counters Initialization
                        {
                            scint_total_top = 0;
                            scint_total_bot = 0;
                            
                            scint_top1 = 0;
                            scint_top2 = 0;
                            scint_top3 = 0;
                            scint_top4 = 0;
                            scint_top5 = 0;
                            
                            scint_bot1 = 0;
                            scint_bot2 = 0;
                            scint_bot3 = 0;
                            scint_bot4 = 0;
                            scint_bot5 = 0;
                        }
                        
                        // SCINT. TOP LAYER
                        {
                            vector<unsigned int> * SCINT_TIME_top1a = _Data[ trg_module ][ scint_top1a ];
                            vector<unsigned int> * SCINT_TIME_top2a = _Data[ trg_module ][ scint_top2a ];
                            vector<unsigned int> * SCINT_TIME_top3a = _Data[ trg_module ][ scint_top3a ];
                            vector<unsigned int> * SCINT_TIME_top4a = _Data[ trg_module ][ scint_top4a ];
                            vector<unsigned int> * SCINT_TIME_top5a = _Data[ trg_module ][ scint_top5a ];
                            vector<unsigned int> * SCINT_TIME_top1b = _Data[ trg_module ][ scint_top1b ];
                            vector<unsigned int> * SCINT_TIME_top2b = _Data[ trg_module ][ scint_top2b ];
                            vector<unsigned int> * SCINT_TIME_top3b = _Data[ trg_module ][ scint_top3b ];
                            vector<unsigned int> * SCINT_TIME_top4b = _Data[ trg_module ][ scint_top4b ];
                            vector<unsigned int> * SCINT_TIME_top5b = _Data[ trg_module ][ scint_top5b ];
                            
                            
                            if( HITS_PER_CHANNEL->at( scint_top1a )  > 0 ) {
                                long scintillator = SCINT_TIME_top1a->at( 0 );
                                if ( abs(trig - scintillator) < scint_max ) scint_top1=1;
                            }
                            
                            if( HITS_PER_CHANNEL->at( scint_top1b )  > 0 ) {
                                long scintillator = SCINT_TIME_top1b->at( 0 );
                                if ( abs(trig - scintillator) < scint_max ) scint_top1=1;
                            }
                            
                            if( HITS_PER_CHANNEL->at( scint_top2a )  > 0 ) {
                                long scintillator = SCINT_TIME_top2a->at( 0 );
                                if ( abs(trig - scintillator) < scint_max ) scint_top2=1;
                            }
                            
                            if( HITS_PER_CHANNEL->at( scint_top2b )  > 0 ) {
                                long scintillator = SCINT_TIME_top2b->at( 0 );
                                if ( abs(trig - scintillator) < scint_max ) scint_top2=1;
                            }
                            
                            if( HITS_PER_CHANNEL->at( scint_top3a )  > 0 ) {
                                long scintillator = SCINT_TIME_top3a->at( 0 );
                                if ( abs(trig - scintillator) < scint_max ) scint_top3=1;
                            }
                            
                            if( HITS_PER_CHANNEL->at( scint_top3b )  > 0 ) {
                                long scintillator = SCINT_TIME_top3b->at( 0 );
                                if ( abs(trig - scintillator) < scint_max ) scint_top3=1;
                            }
                            
                            if( HITS_PER_CHANNEL->at( scint_top4a )  > 0 ) {
                                long scintillator = SCINT_TIME_top4a->at( 0 );
                                if ( abs(trig - scintillator) < scint_max ) scint_top4=1;
                            }
                            
                            if( HITS_PER_CHANNEL->at( scint_top4b )  > 0 ) {
                                long scintillator = SCINT_TIME_top4b->at( 0 );
                                if ( abs(trig - scintillator) < scint_max ) scint_top4=1;
                            }
                            
                            if( HITS_PER_CHANNEL->at( scint_top5a )  > 0 ) {
                                long scintillator = SCINT_TIME_top5a->at( 0 );
                                if ( abs(trig - scintillator) < scint_max ) scint_top5=1;
                            }
                            
                            if( HITS_PER_CHANNEL->at( scint_top5b )  > 0 ) {
                                long scintillator = SCINT_TIME_top5b->at( 0 );
                                if ( abs(trig - scintillator) < scint_max ) scint_top5=1;
                            }
                        }
                        
                        // SCINT. BOTTOM LAYER
                        {
                            vector<unsigned int> * SCINT_TIME_bot1a = _Data[ trg_module ][ scint_bot1a ];
                            vector<unsigned int> * SCINT_TIME_bot2a = _Data[ trg_module ][ scint_bot2a ];
                            vector<unsigned int> * SCINT_TIME_bot3a = _Data[ trg_module ][ scint_bot3a ];
                            vector<unsigned int> * SCINT_TIME_bot4a = _Data[ trg_module ][ scint_bot4a ];
                            vector<unsigned int> * SCINT_TIME_bot5a = _Data[ trg_module ][ scint_bot5a ];
                            vector<unsigned int> * SCINT_TIME_bot1b = _Data[ trg_module ][ scint_bot1b ];
                            vector<unsigned int> * SCINT_TIME_bot2b = _Data[ trg_module ][ scint_bot2b ];
                            vector<unsigned int> * SCINT_TIME_bot3b = _Data[ trg_module ][ scint_bot3b ];
                            vector<unsigned int> * SCINT_TIME_bot4b = _Data[ trg_module ][ scint_bot4b ];
                            vector<unsigned int> * SCINT_TIME_bot5b = _Data[ trg_module ][ scint_bot5b ];
                            
                            
                            if( HITS_PER_CHANNEL->at( scint_bot1a )  > 0 ) {
                                long scintillator = SCINT_TIME_bot1a->at( 0 );
                                if ( abs(trig - scintillator) < scint_max ) scint_bot1=1;
                            }
                            
                            if( HITS_PER_CHANNEL->at( scint_bot1b )  > 0 ) {
                                long scintillator = SCINT_TIME_bot1b->at( 0 );
                                if ( abs(trig - scintillator) < scint_max ) scint_bot1=1;
                            }
                            
                            if( HITS_PER_CHANNEL->at( scint_bot2a )  > 0 ) {
                                long scintillator = SCINT_TIME_bot2a->at( 0 );
                                if ( abs(trig - scintillator) < scint_max ) scint_bot2=1;
                            }
                            
                            if( HITS_PER_CHANNEL->at( scint_bot2b )  > 0 ) {
                                long scintillator = SCINT_TIME_bot2b->at( 0 );
                                if ( abs(trig - scintillator) < scint_max ) scint_bot2=1;
                            }
                            
                            if( HITS_PER_CHANNEL->at( scint_bot3a )  > 0 ) {
                                long scintillator = SCINT_TIME_bot3a->at( 0 );
                                if ( abs(trig - scintillator) < scint_max ) scint_bot3=1;
                            }
                            
                            if( HITS_PER_CHANNEL->at( scint_bot3b )  > 0 ) {
                                long scintillator = SCINT_TIME_bot3b->at( 0 );
                                if ( abs(trig - scintillator) < scint_max ) scint_bot3=1;
                            }
                            
                            if( HITS_PER_CHANNEL->at( scint_bot4a )  > 0 ) {
                                long scintillator = SCINT_TIME_bot4a->at( 0 );
                                if ( abs(trig - scintillator) < scint_max ) scint_bot4=1;
                            }
                            
                            if( HITS_PER_CHANNEL->at( scint_bot4b )  > 0 ) {
                                long scintillator = SCINT_TIME_bot4b->at( 0 );
                                if ( abs(trig - scintillator) < scint_max ) scint_bot4=1;
                            }
                            
                            if( HITS_PER_CHANNEL->at( scint_bot5a )  > 0 ) {
                                long scintillator = SCINT_TIME_bot5a->at( 0 );
                                if ( abs(trig - scintillator) < scint_max ) scint_bot5=1;
                            }
                            
                            if( HITS_PER_CHANNEL->at( scint_bot5b )  > 0 ) {
                                long scintillator = SCINT_TIME_bot5b->at( 0 );
                                if ( abs(trig - scintillator) < scint_max ) scint_bot5=1;
                            }
                        }
                        
                        //if ( (  HITS_PER_CHANNEL->at(scint_top1a)  ) || (  HITS_PER_CHANNEL->at(scint_top1b)  )  >= 1) scint_top1=1;
                        
                        scint_total_top = scint_top1 + scint_top2 + scint_top3 + scint_top4 + scint_top5;
                        scint_total_bot = scint_bot1 + scint_bot2 + scint_bot3 + scint_bot4 + scint_bot5;
                        
                        if ( i_station == 1 ) {
                            if (scint_total_top == 1) shower_count1top++;
                            if (scint_total_bot == 1) shower_count1bot++;
                            
                            if (scint_total_top == 2) shower_count2top++;
                            if (scint_total_bot == 2) shower_count2bot++;
                            
                            if (scint_total_top == 3) shower_count3top++;
                            if (scint_total_bot == 3) shower_count3bot++;
                            
                            if (scint_total_top == 4) shower_count4top++;
                            if (scint_total_bot == 4) shower_count4bot++;
                            
                            if (scint_total_top == 5) shower_count5top++;
                            if (scint_total_bot == 5) shower_count5bot++;
                            
                            if (verbose_showers) cout << "Event: " << jentry << " ; counts on top: " << scint_total_top << " counts on bottom: " << scint_total_bot << endl;
                        }
                        
                        if ( (scint_total_top >= scint_multiplicity_max) || (scint_total_bot >= scint_multiplicity_max) ) {
                            if (verbose_showers) cout << " ----> SHOWER Event!! " << endl;
                            continue;
                        }
                        
                        
                        // Trigger check (and reducing trigger
                        if ( (scint_total_top < 1) || (scint_total_bot < 1) ) master_trigger = 0;
                        if (master_trigger_cutedge) {
                            if ( (scint_top1 >= 1) || (scint_top5 >= 1) || (scint_bot1 >= 1) || (scint_bot5 >= 1) ) {
                                master_trigger = 0;
                                //cout << "MASTER TRIGGER -> not considered " << endl;
                            }
                        }
                    }
                    
                    
                    // Starting Calculations...
                    if (master_trigger) {
                        
                        // Trigger Initialization and track reconstruction
                        {
                            // Initialization of SmallCounter-based trigger mode
                            if ( etapartition != "ALL" ) {
                                
                                if (   HITS_PER_CHANNEL->at( trigger_biteta ) >= 1  )   {
                                    long trig1 = TRIG->at( 0 );
                                    long trigeta1 = TRIGETA->at( 0 );
                                    if (   (abs(trig1 - trigeta1) < delaytrg*10) ) {
                                        nicehit = 1;
                                    }
                                    else {
                                        //cout << " Small counter out of sync " <<  (trig1 - trigeta1)*10 << ") ns distance" <<  endl;
                                        nicehit = 0;
                                    }
                                    //cout << "Master trigger is : " << trig1 << " , while small counter time is: " << trigeta1 << endl;
                                    if (   (abs(trig1 - trigeta1) < delaytrg*10) && (i_station == 1) && (onoffcounter==0) ) {
                                        counter++ ;
                                        onoffcounter = 1;
                                    }
                                    
                                    //if (i_station == 1) counter++;
                                }
                                else {
                                    nicehit = 0;
                                }
                            }
                            
                            // Large SCINT counters trigger mode
                            if ( (etapartition == "ALL") && (largeacceptance == 1) ) {
                                nicehit = 1;
                                
                                if ( (onoffcounter == 0) && ( i_station == 1) ) {
                                    counter++ ;
                                    nicehit = 1;
                                    onoffcounter = 1;
                                }
                            }
                            
                            // Initialization of 1-RPC-based trigger mode and Large SCINT counters
                            if ( (etapartition == "ALL") && (largeacceptance == 0) && (twotrackingrpc == 0) && (threetrackingrpc == 0) ) {
                                
                                // Number of iterations per trigger station
                                {
                                    if ( reference_map_module1 > 0 ) {
                                        refcycle_max = 2;
                                    }
                                    else {
                                        refcycle_max = 1;
                                    }
                                }
                                
                                // Looping over all channels of the station
                                for ( int cycle = 1; cycle <= refcycle_max; cycle++ ) {
                                    
                                    if (cycle == 1) rpctrigger_start = reference_map_start, rpctrigger_finish = reference_map_finish, rpctrigger_module = reference_module;
                                    if (cycle == 2) rpctrigger_start = reference_map_start1, rpctrigger_finish = reference_map_finish1, rpctrigger_module = reference_map_module1;
                                    
                                    // Looping on channels...
                                    for ( rpctrigger_bit = rpctrigger_start; rpctrigger_bit < rpctrigger_finish; rpctrigger_bit++ ) {
                                        
                                        // Hit initializations
                                        vector<unsigned int> *RPCHITS_PER_CHANNEL = HitsPerChannel[ rpctrigger_module ];
                                        vector<unsigned int> *TRIGRPC = _Data[ rpctrigger_module ][ rpctrigger_bit ];
                                        
                                        // Event analyzing
                                        if( RPCHITS_PER_CHANNEL->at( rpctrigger_bit ) > 0 ) {
                                            
                                            long rpctrig = TRIGRPC->at( 0 );
                                            long trig1 = 99999;
                                            trig1 = TRIG->at( 0 );
                                            
                                            // Readout Mapping
                                            refcanale = rpctrigger_bit - rpctrigger_start;
                                            if ( (rpctrigger_start == 0) && (rpctrigger_finish == 64) ) refcanale = rpctrigger_bit + 32;
                                            if ( (rpctrigger_start == 0) && (rpctrigger_finish == 32) ) refcanale = rpctrigger_bit + 64;
                                            
                                            if ( (refcanale>=reflimit) && (refcanale<32-reflimit) ) {
                                                if ( ( (trig1-rpctrig) > trg_windows_min ) && ( (trig1-rpctrig) < trg_windows_max) ) nicehitA = 1, onerpctriggermultiplicity++;
                                            }
                                            
                                            if ( (refcanale>=32+reflimit) && (refcanale<64-reflimit) ) {
                                                if ( ( (trig1-rpctrig) > trg_windows_min ) && ( (trig1-rpctrig) < trg_windows_max) ) nicehitB = 1, onerpctriggermultiplicity++;
                                            }
                                            
                                            if ( (refcanale>=64+reflimit) && (refcanale<96-reflimit) ) {
                                                if ( ( (trig1-rpctrig) > trg_windows_min ) && ( (trig1-rpctrig) < trg_windows_max) ) nicehitC = 1, onerpctriggermultiplicity++;
                                            }
                                        }
                                    }
                                    
                                    // Printing info
                                    {
                                        //if ( (nicehitA == 1) && (i_station == 1) ) cout << "1-RPC-A: Event " << jentry << " triggered!" << endl;
                                        //if ( nicehitB == 1 ) cout << "1-RPC-B: Event " << jentry << " triggered!" << endl;
                                        //if ( nicehitC == 1 ) cout << "1-RPC-C: Event " << jentry << " triggered!" << endl;
                                    }
                                }
                                
                                // Discarding events with high multiplicity
                                if ( onerpctriggermultiplicity < onerpctriggermultiplicity_max) {
                                    if ( (nicehitA == 1) && (i_station == 1) ) counterA ++;
                                    if ( (nicehitB == 1) && (i_station == 1) ) counterB ++;
                                    if ( (nicehitC == 1) && (i_station == 1) ) counterC ++;
                                }
                                else {
                                    nicehitA = 0;
                                    nicehitB = 0;
                                    nicehitC = 0;
                                }
                            }
                            
                            // Initialization of 2-RPC-based trigger mode and Large SCINT counters
                            if ( (etapartition == "ALL") && (largeacceptance == 0) && (twotrackingrpc == 1) && (threetrackingrpc == 0) ) {
                                
                                
                                // Two HvScan Analyses at same time 4/2 and 4/3 stack
                                // implementation to be done in the trigger event counters!!!!!!!!!!!!!!!
                                /*
                                 size_t pos42, pos43;
                                 pos42 = name_set_tn[i_station].find("RE4-2");
                                 pos43 = name_set_tn[i_station].find("RE4-3");
                                 if (pos42!=std::string::npos) reference_module = reference_module;
                                 if (pos42!=std::string::npos) reference_map_module1 = reference_map_module1;
                                 if (pos42!=std::string::npos) reference_module_t2 = reference_module_t2;
                                 if (pos42!=std::string::npos) reference_map_module1_t2 = reference_map_module1_t2;
                                 
                                 if (pos43!=std::string::npos) reference_module = reference_module_t3;
                                 if (pos43!=std::string::npos) reference_map_module1 = reference_map_module1_t3;
                                 if (pos43!=std::string::npos) reference_module_t2 = reference_module_t4;
                                 if (pos43!=std::string::npos) reference_map_module1_t2 = reference_map_module1_t4;
                                 */
                                
                                // Number of iterations per trigger station first
                                {
                                    if ( reference_map_module1 > 0 ) {
                                        refcycle_max = 2;
                                    }
                                    else {
                                        refcycle_max = 1;
                                    }
                                }
                                
                                // Looping over all channels of the station first
                                for ( int cycle = 1; cycle <= refcycle_max; cycle++ ) {
                                    
                                    if (cycle == 1) rpctrigger_start = reference_map_start, rpctrigger_finish = reference_map_finish, rpctrigger_module = reference_module;
                                    if (cycle == 2) rpctrigger_start = reference_map_start1, rpctrigger_finish = reference_map_finish1, rpctrigger_module = reference_map_module1;
                                    
                                    vector<unsigned int> *RPCHITS_PER_CHANNEL = HitsPerChannel[ rpctrigger_module ];
                                    long trig1 = TRIG->at( 0 );
                                    
                                    // Looping on channels...
                                    for ( rpctrigger_bit = rpctrigger_start; rpctrigger_bit < rpctrigger_finish; rpctrigger_bit++ ) {
                                        
                                        // Mapping
                                        refcanale = rpctrigger_bit - rpctrigger_start;
                                        if ( (rpctrigger_start == 0) && (rpctrigger_finish == 64) ) refcanale = rpctrigger_bit + 32;
                                        if ( (rpctrigger_start == 0) && (rpctrigger_finish == 32) ) refcanale = rpctrigger_bit + 64;
                                        
                                        if ( RPCHITS_PER_CHANNEL->at(rpctrigger_bit)>=1 && (refcanale>=reflimit) && (refcanale<32-reflimit) ) {
                                            vector<unsigned int> *TRIGRPC = _Data[ rpctrigger_module ][ rpctrigger_bit ];
                                            long rpctrig = TRIGRPC->at( 0 );
                                            if ( ( (trig1-rpctrig) > trg_windows_min ) && ( (trig1-rpctrig) < trg_windows_max) ) nicehitA = 1, onerpctriggermultiplicity++;
                                        }
                                        
                                        if ( RPCHITS_PER_CHANNEL->at(rpctrigger_bit)>=1 && (refcanale>=32+reflimit) && (refcanale<64-reflimit) ) {
                                            vector<unsigned int> *TRIGRPC = _Data[ rpctrigger_module ][ rpctrigger_bit ];
                                            long rpctrig = TRIGRPC->at( 0 );
                                            if ( ( (trig1-rpctrig) > trg_windows_min ) && ( (trig1-rpctrig) < trg_windows_max) ) nicehitB = 1, onerpctriggermultiplicity++;
                                        }
                                        
                                        if ( RPCHITS_PER_CHANNEL->at(rpctrigger_bit)>=1 && (refcanale>=64+reflimit) && (refcanale<96-reflimit) ) {
                                            vector<unsigned int> *TRIGRPC = _Data[ rpctrigger_module ][ rpctrigger_bit ];
                                            long rpctrig = TRIGRPC->at( 0 );
                                            if ( ( (trig1-rpctrig) > trg_windows_min ) && ( (trig1-rpctrig) < trg_windows_max) ) nicehitC = 1, onerpctriggermultiplicity++;
                                        }
                                    }
                                }
                                
                                // Number of iterations per trigger station second
                                {
                                    if ( reference_map_module1_t2 > 0 ) {
                                        refcycle_max = 2;
                                    }
                                    else {
                                        refcycle_max = 1;
                                    }
                                }
                                
                                // Looping over all channels of the station second
                                for ( int cycle = 1; cycle <= refcycle_max; cycle++ ) {
                                    
                                    if (cycle == 1) rpctrigger_start = reference_map_start_t2, rpctrigger_finish = reference_map_finish_t2, rpctrigger_module = reference_module_t2;
                                    if (cycle == 2) rpctrigger_start = reference_map_start1_t2, rpctrigger_finish = reference_map_finish1_t2, rpctrigger_module = reference_map_module1_t2;
                                    
                                    vector<unsigned int> *RPCHITS_PER_CHANNEL = HitsPerChannel[ rpctrigger_module ];
                                    long trig1 = TRIG->at( 0 );
                                    
                                    // Looping on channels...
                                    for ( rpctrigger_bit = rpctrigger_start; rpctrigger_bit < rpctrigger_finish; rpctrigger_bit++ ) {
                                        
                                        // Mapping
                                        refcanale = rpctrigger_bit - rpctrigger_start;
                                        if ( (rpctrigger_start == 0) && (rpctrigger_finish == 64) ) refcanale = rpctrigger_bit + 32;
                                        if ( (rpctrigger_start == 0) && (rpctrigger_finish == 32) ) refcanale = rpctrigger_bit + 64;
                                        
                                        if ( RPCHITS_PER_CHANNEL->at(rpctrigger_bit)>=1 && (refcanale>=reflimit) && (refcanale<32-reflimit) ) {
                                            vector<unsigned int> *TRIGRPC = _Data[ rpctrigger_module ][ rpctrigger_bit ];
                                            long rpctrig = TRIGRPC->at( 0 );
                                            if ( ( (trig1-rpctrig) > trg_windows_min ) && ( (trig1-rpctrig) < trg_windows_max) ) nicehitA_T2 = 1, tworpctriggermultiplicity++;
                                        }
                                        
                                        if ( RPCHITS_PER_CHANNEL->at(rpctrigger_bit)>=1 && (refcanale>=32+reflimit) && (refcanale<64-reflimit) ) {
                                            vector<unsigned int> *TRIGRPC = _Data[ rpctrigger_module ][ rpctrigger_bit ];
                                            long rpctrig = TRIGRPC->at( 0 );
                                            if ( ( (trig1-rpctrig) > trg_windows_min ) && ( (trig1-rpctrig) < trg_windows_max) ) nicehitB_T2 = 1, tworpctriggermultiplicity++;
                                        }
                                        
                                        if ( RPCHITS_PER_CHANNEL->at(rpctrigger_bit)>=1 && (refcanale>=64+reflimit) && (refcanale<96-reflimit) ) {
                                            vector<unsigned int> *TRIGRPC = _Data[ rpctrigger_module ][ rpctrigger_bit ];
                                            long rpctrig = TRIGRPC->at( 0 );
                                            if ( ( (trig1-rpctrig) > trg_windows_min ) && ( (trig1-rpctrig) < trg_windows_max) ) nicehitC_T2 = 1, tworpctriggermultiplicity++;
                                        }
                                    }
                                }
                                
                                // Discarding events with high multiplicity
                                if ( ( tworpctriggermultiplicity < tworpctriggermultiplicity_max ) && (onerpctriggermultiplicity < onerpctriggermultiplicity_max) ) {
                                    if ( (nicehitA == 1) && (nicehitA_T2 == 1) && (i_station == 1) ) counterA ++;
                                    if ( (nicehitB == 1) && (nicehitB_T2 == 1) && (i_station == 1) ) counterB ++;
                                    if ( (nicehitC == 1) && (nicehitC_T2 == 1) && (i_station == 1) ) counterC ++;
                                    
                                    
                                    if ( (nicehitA == 1) && (nicehitA_T2 == 1) ) {
                                        nicehitA = 1;
                                    }
                                    else {
                                        nicehitA = 0;
                                    }
                                    
                                    if ( (nicehitB == 1) && (nicehitB_T2 == 1) ) {
                                        nicehitB = 1;
                                    }
                                    else {
                                        nicehitB = 0;
                                    }
                                    
                                    if ( (nicehitC == 1) && (nicehitC_T2 == 1) ) {
                                        nicehitC = 1;
                                    }
                                    else {
                                        nicehitC = 0;
                                    }
                                    
                                    
                                }
                                else {
                                    nicehitA = 0;
                                    nicehitB = 0;
                                    nicehitC = 0;
                                }
                                
                                nicehit=0;                                
                            }
                            
                            // Initialization of 3-RPC-based trigger mode and Large SCINT counters
                            if ( (etapartition == "ALL") && (largeacceptance == 0) && (threetrackingrpc == 1) && (trackingreconstruction == 0) ) {
                                
                                // Number of iterations per trigger station first
                                {
                                    if ( reference_map_module1 > 0 ) {
                                        refcycle_max = 2;
                                    }
                                    else {
                                        refcycle_max = 1;
                                    }
                                }
                                // Looping over all channels of the station first
                                for ( int cycle = 1; cycle <= refcycle_max; cycle++ ) {
                                    
                                    if (cycle == 1) rpctrigger_start = reference_map_start, rpctrigger_finish = reference_map_finish, rpctrigger_module = reference_module;
                                    if (cycle == 2) rpctrigger_start = reference_map_start1, rpctrigger_finish = reference_map_finish1, rpctrigger_module = reference_map_module1;
                                    
                                    vector<unsigned int> *RPCHITS_PER_CHANNEL = HitsPerChannel[ rpctrigger_module ];
                                    long trig1 = TRIG->at( 0 );
                                    
                                    // Looping on channels...
                                    for ( rpctrigger_bit = rpctrigger_start; rpctrigger_bit < rpctrigger_finish; rpctrigger_bit++ ) {
                                        
                                        // Mapping
                                        refcanale = rpctrigger_bit - rpctrigger_start;
                                        if ( (rpctrigger_start == 0) && (rpctrigger_finish == 64) ) refcanale = rpctrigger_bit + 32;
                                        if ( (rpctrigger_start == 0) && (rpctrigger_finish == 32) ) refcanale = rpctrigger_bit + 64;
                                        
                                        if ( RPCHITS_PER_CHANNEL->at(rpctrigger_bit)>=1 ) {
                                            
                                            vector<unsigned int> *TRIGRPC = _Data[ rpctrigger_module ][ rpctrigger_bit ];
                                            long rpctrig = TRIGRPC->at( 0 );
                                            if ( ( (trig1-rpctrig) > trg_windows_min ) && ( (trig1-rpctrig) < trg_windows_max) ) {
                                                
                                                if ( (refcanale>=0) && (refcanale<32) ) {
                                                    nicehitA = 1;
                                                    onerpctriggermultiplicity++;
                                                    triggerhit++;
                                                }
                                                if ( (refcanale>=32) && (refcanale<64) ) {
                                                    nicehitB = 1;
                                                    onerpctriggermultiplicity++;
                                                    triggerhit++;
                                                }
                                                if ( (refcanale>=64) && (refcanale<96) ) {
                                                    nicehitC = 1;
                                                    onerpctriggermultiplicity++;
                                                    triggerhit++;
                                                }
                                                //if (i_station==1) cout << " E = " << jentry << " Found hit at " << refcanale << endl;
                                            }
                                        }
                                    }
                                }
                                
                                
                                
                                // Number of iterations per trigger station second
                                {
                                    if ( reference_map_module1_t2 > 0 ) {
                                        refcycle_max = 2;
                                    }
                                    else {
                                        refcycle_max = 1;
                                    }
                                }
                                // Looping over all channels of the station second
                                for ( int cycle = 1; cycle <= refcycle_max; cycle++ ) {
                                    
                                    if (cycle == 1) rpctrigger_start = reference_map_start_t2, rpctrigger_finish = reference_map_finish_t2, rpctrigger_module = reference_module_t2;
                                    if (cycle == 2) rpctrigger_start = reference_map_start1_t2, rpctrigger_finish = reference_map_finish1_t2, rpctrigger_module = reference_map_module1_t2;
                                    
                                    
                                    vector<unsigned int> *RPCHITS_PER_CHANNEL = HitsPerChannel[ rpctrigger_module ];
                                    long trig1 = TRIG->at( 0 );
                                    
                                    
                                    // Looping on channels...
                                    for ( rpctrigger_bit = rpctrigger_start; rpctrigger_bit < rpctrigger_finish; rpctrigger_bit++ ) {
                                        
                                        // Mapping
                                        refcanale = rpctrigger_bit - rpctrigger_start;
                                        
                                        if ( (rpctrigger_start == 0) && (rpctrigger_finish == 64) ) refcanale = rpctrigger_bit + 32;
                                        if ( (rpctrigger_start == 0) && (rpctrigger_finish == 32) ) refcanale = rpctrigger_bit + 64;
                                        
                                        
                                        if ( RPCHITS_PER_CHANNEL->at(rpctrigger_bit)>=1 ) {
                                            
                                            vector<unsigned int> *TRIGRPC = _Data[ rpctrigger_module ][ rpctrigger_bit ];
                                            long rpctrig = TRIGRPC->at( 0 );
                                            if ( ( (trig1-rpctrig) > trg_windows_min ) && ( (trig1-rpctrig) < trg_windows_max) ) {
                                                
                                                if ( (refcanale>=0) && (refcanale<32) ) {
                                                    nicehitA_T2 = 1;
                                                    tworpctriggermultiplicity++;
                                                    triggerhit2++;
                                                }
                                                if ( (refcanale>=32) && (refcanale<64) ) {
                                                    nicehitB_T2 = 1;
                                                    tworpctriggermultiplicity++;
                                                    triggerhit2++;
                                                }
                                                if ( (refcanale>=64) && (refcanale<96) ) {
                                                    nicehitC_T2 = 1;
                                                    tworpctriggermultiplicity++;
                                                    triggerhit2++;
                                                }
                                                //if (i_station==1) cout << " E = " << jentry << " Found hit at " << refcanale << endl;
                                            }
                                        }
                                    }
                                }
                                
                                
                                
                                // Number of iterations per trigger station third
                                {
                                    if ( reference_map_module1_t3 > 0 ) {
                                        refcycle_max = 2;
                                    }
                                    else {
                                        refcycle_max = 1;
                                    }
                                }
                                // Looping over all channels of the station third
                                for ( int cycle = 1; cycle <= refcycle_max; cycle++ ) {
                                    
                                    if (cycle == 1) rpctrigger_start = reference_map_start_t3, rpctrigger_finish = reference_map_finish_t3, rpctrigger_module = reference_module_t3;
                                    if (cycle == 2) rpctrigger_start = reference_map_start1_t3, rpctrigger_finish = reference_map_finish1_t3, rpctrigger_module = reference_map_module1_t3;
                                    
                                    vector<unsigned int> *RPCHITS_PER_CHANNEL = HitsPerChannel[ rpctrigger_module ];
                                    long trig1 = TRIG->at( 0 );
                                    
                                    // Looping on channels...
                                    for ( rpctrigger_bit = rpctrigger_start; rpctrigger_bit < rpctrigger_finish; rpctrigger_bit++ ) {
                                        
                                        // Mapping
                                        refcanale = rpctrigger_bit - rpctrigger_start;
                                        if ( (rpctrigger_start == 0) && (rpctrigger_finish == 64) ) refcanale = rpctrigger_bit + 32;
                                        if ( (rpctrigger_start == 0) && (rpctrigger_finish == 32) ) refcanale = rpctrigger_bit + 64;
                                        
                                        
                                        if ( RPCHITS_PER_CHANNEL->at(rpctrigger_bit)>=1 ) {
                                            
                                            vector<unsigned int> *TRIGRPC = _Data[ rpctrigger_module ][ rpctrigger_bit ];
                                            long rpctrig = TRIGRPC->at( 0 );
                                            if ( ( (trig1-rpctrig) > trg_windows_min ) && ( (trig1-rpctrig) < trg_windows_max) ) {
                                                
                                                if ( (refcanale>=0) && (refcanale<32) ) {
                                                    nicehitA_T3 = 1;
                                                    threerpctriggermultiplicity++;
                                                    triggerhit3++;
                                                }
                                                if ( (refcanale>=32) && (refcanale<64) ) {
                                                    nicehitB_T3 = 1;
                                                    threerpctriggermultiplicity++;
                                                    triggerhit3++;
                                                }
                                                if ( (refcanale>=64) && (refcanale<96) ) {
                                                    nicehitC_T3 = 1;
                                                    threerpctriggermultiplicity++;
                                                    triggerhit3++;
                                                }
                                                //if (i_station==1) cout << " E = " << jentry << " Found hit at " << refcanale << endl;
                                            }
                                        }
                                    }
                                    
                                }
                                
                                
                                // Discarding events with high multiplicity
                                if ( (threerpctriggermultiplicity < threerpctriggermultiplicity_max ) || ( tworpctriggermultiplicity < tworpctriggermultiplicity_max ) || (onerpctriggermultiplicity < onerpctriggermultiplicity_max)) {
                                        if ( (nicehitA == 1) && (nicehitA_T2 == 1) && (nicehitA_T3 == 1) && (i_station == 1) ) counterA ++;
                                        if ( (nicehitB == 1) && (nicehitB_T2 == 1) && (nicehitB_T3 == 1) && (i_station == 1) ) counterB ++;
                                        if ( (nicehitC == 1) && (nicehitC_T2 == 1) && (nicehitC_T3 == 1) && (i_station == 1) ) counterC ++;
                                        
                                        
                                        if ( (nicehitA == 1) && (nicehitA_T2 == 1) && (nicehitA_T3 == 1) ) {
                                            nicehitA = 1;
                                        }
                                        else {
                                            nicehitA = 0;
                                        }
                                        
                                        if ( (nicehitB == 1) && (nicehitB_T2 == 1) && (nicehitB_T3 == 1) ) {
                                            nicehitB = 1;
                                        }
                                        else {
                                            nicehitB = 0;
                                        }
                                        
                                        if ( (nicehitC == 1) && (nicehitC_T2 == 1) && (nicehitC_T3 == 1) ) {
                                            nicehitC = 1;
                                        }
                                        else {
                                            nicehitC = 0;
                                        }
                                        
                                        
                                    }
                                else {
                                        nicehitA = 0;
                                        nicehitB = 0;
                                        nicehitC = 0;
                                    }
                                    
                                nicehit=0;
                                //cout << " Info-event " << jentry << " nicehitsA,B,C " << nicehitA << " " << nicehitB << " "<< nicehitC << endl;
                            }
                                                        
                            // Initialization of 3-RPC-based trigger mode and Large SCINT counters and TRACKING
                            if ( (etapartition == "ALL") && (largeacceptance == 0) && (threetrackingrpc == 1) && (trackingreconstruction == 1) ) {
                                
                                // Number of iterations per trigger station first
                                {
                                    if ( reference_map_module1 > 0 ) {
                                        refcycle_max = 2;
                                    }
                                    else {
                                        refcycle_max = 1;
                                    }
                                }
                                // Looping over all channels of the station first
                                for ( int cycle = 1; cycle <= refcycle_max; cycle++ ) {
                                    
                                    if (cycle == 1) rpctrigger_start = reference_map_start, rpctrigger_finish = reference_map_finish, rpctrigger_module = reference_module;
                                    if (cycle == 2) rpctrigger_start = reference_map_start1, rpctrigger_finish = reference_map_finish1, rpctrigger_module = reference_map_module1;
                                    
                                    vector<unsigned int> *RPCHITS_PER_CHANNEL = HitsPerChannel[ rpctrigger_module ];
                                    long trig1 = TRIG->at( 0 );
                                    
                                    // Looping on channels...
                                    for ( rpctrigger_bit = rpctrigger_start; rpctrigger_bit < rpctrigger_finish; rpctrigger_bit++ ) {
                                        
                                        // Mapping
                                        refcanale = rpctrigger_bit - rpctrigger_start;
                                        if ( (rpctrigger_start == 0) && (rpctrigger_finish == 64) ) refcanale = rpctrigger_bit + 32;
                                        if ( (rpctrigger_start == 0) && (rpctrigger_finish == 32) ) refcanale = rpctrigger_bit + 64;
                                        
                                        if ( RPCHITS_PER_CHANNEL->at(rpctrigger_bit)>=1 ) {
                                            
                                            vector<unsigned int> *TRIGRPC = _Data[ rpctrigger_module ][ rpctrigger_bit ];
                                            long rpctrig = TRIGRPC->at( 0 );
                                         
                                            correct_timing = ( ( trig1 - rpctrig ) - (bunchIDmod[rpctrigger_module]*250) );
                                            
                                            if ( ( correct_timing > trg_windows_min ) && ( correct_timing < trg_windows_max) ) {
                                                
                                                if ( (refcanale>=0) && (refcanale<32) ) {
                                                    //nicehitA = 1;
                                                    onerpctriggermultiplicity++;
                                                    reco_hit1[triggerhit] = refcanale;
                                                    triggerhit++;
                                                }
                                                if ( (refcanale>=32) && (refcanale<64) ) {
                                                    //nicehitB = 1;
                                                    onerpctriggermultiplicity++;
                                                    reco_hit1[triggerhit] = refcanale;
                                                    triggerhit++;
                                                }
                                                if ( (refcanale>=64) && (refcanale<96) ) {
                                                    //nicehitC = 1;
                                                    onerpctriggermultiplicity++;
                                                    reco_hit1[triggerhit] = refcanale;
                                                    triggerhit++;
                                                }
                                                //if (i_station==1) cout << " E = " << jentry << " Found hit at " << refcanale << endl;
                                            }
                                        }
                                    }
                                }
                                
                                
                                
                                // Number of iterations per trigger station second
                                {
                                    if ( reference_map_module1_t2 > 0 ) {
                                        refcycle_max = 2;
                                    }
                                    else {
                                        refcycle_max = 1;
                                    }
                                }
                                // Looping over all channels of the station second
                                for ( int cycle = 1; cycle <= refcycle_max; cycle++ ) {
                                    
                                    if (cycle == 1) rpctrigger_start = reference_map_start_t2, rpctrigger_finish = reference_map_finish_t2, rpctrigger_module = reference_module_t2;
                                    if (cycle == 2) rpctrigger_start = reference_map_start1_t2, rpctrigger_finish = reference_map_finish1_t2, rpctrigger_module = reference_map_module1_t2;

                                    
                                    vector<unsigned int> *RPCHITS_PER_CHANNEL = HitsPerChannel[ rpctrigger_module ];
                                    long trig1 = TRIG->at( 0 );

                                    
                                    // Looping on channels...
                                    for ( rpctrigger_bit = rpctrigger_start; rpctrigger_bit < rpctrigger_finish; rpctrigger_bit++ ) {
                                        
                                        // Mapping
                                        refcanale = rpctrigger_bit - rpctrigger_start;
                                        
                                        if ( (rpctrigger_start == 0) && (rpctrigger_finish == 64) ) refcanale = rpctrigger_bit + 32;
                                        if ( (rpctrigger_start == 0) && (rpctrigger_finish == 32) ) refcanale = rpctrigger_bit + 64;

                                    
                                        if ( RPCHITS_PER_CHANNEL->at(rpctrigger_bit)>=1 ) {
                                        
                                            vector<unsigned int> *TRIGRPC = _Data[ rpctrigger_module ][ rpctrigger_bit ];
                                            long rpctrig = TRIGRPC->at( 0 );
                                            correct_timing = ( ( trig1 - rpctrig ) - (bunchIDmod[rpctrigger_module]*250) ) ;
                                            
                                            if ( ( correct_timing > trg_windows_min ) && ( correct_timing < trg_windows_max) ) {
                                                
                                                if ( (refcanale>=0) && (refcanale<32) ) {
                                                    //nicehitA_T2 = 1;
                                                    tworpctriggermultiplicity++;
                                                    reco_hit2[triggerhit2] = refcanale;
                                                    triggerhit2++;
                                                }
                                                if ( (refcanale>=32) && (refcanale<64) ) {
                                                    //nicehitB_T2 = 1;
                                                    tworpctriggermultiplicity++;
                                                    reco_hit2[triggerhit2] = refcanale;
                                                    triggerhit2++;
                                                }
                                                if ( (refcanale>=64) && (refcanale<96) ) {
                                                    //nicehitC_T2 = 1;
                                                    tworpctriggermultiplicity++;
                                                    reco_hit2[triggerhit2] = refcanale;
                                                    triggerhit2++;
                                                }
                                                //if (i_station==1) cout << " E = " << jentry << " Found hit at " << refcanale << endl;
                                            }
                                        }
                                    }
                                }
                                
                                
                                
                                // Number of iterations per trigger station third
                                {
                                    if ( reference_map_module1_t3 > 0 ) {
                                        refcycle_max = 2;
                                    }
                                    else {
                                        refcycle_max = 1;
                                    }
                                }
                                // Looping over all channels of the station third
                                for ( int cycle = 1; cycle <= refcycle_max; cycle++ ) {
                                    
                                    if (cycle == 1) rpctrigger_start = reference_map_start_t3, rpctrigger_finish = reference_map_finish_t3, rpctrigger_module = reference_module_t3;
                                    if (cycle == 2) rpctrigger_start = reference_map_start1_t3, rpctrigger_finish = reference_map_finish1_t3, rpctrigger_module = reference_map_module1_t3;
                                    
                                    vector<unsigned int> *RPCHITS_PER_CHANNEL = HitsPerChannel[ rpctrigger_module ];
                                    long trig1 = TRIG->at( 0 );
                                    
                                    // Looping on channels...
                                    for ( rpctrigger_bit = rpctrigger_start; rpctrigger_bit < rpctrigger_finish; rpctrigger_bit++ ) {
                                        
                                        // Mapping
                                        refcanale = rpctrigger_bit - rpctrigger_start;
                                        if ( (rpctrigger_start == 0) && (rpctrigger_finish == 64) ) refcanale = rpctrigger_bit + 32;
                                        if ( (rpctrigger_start == 0) && (rpctrigger_finish == 32) ) refcanale = rpctrigger_bit + 64;
                                        

                                        if ( RPCHITS_PER_CHANNEL->at(rpctrigger_bit)>=1 ) {
                                            
                                            vector<unsigned int> *TRIGRPC = _Data[ rpctrigger_module ][ rpctrigger_bit ];
                                            long rpctrig = TRIGRPC->at( 0 );
                                            correct_timing = ( ( trig1 - rpctrig ) - (bunchIDmod[rpctrigger_module]*250) ) ;

                                            if ( ( correct_timing > trg_windows_min ) && ( correct_timing < trg_windows_max) ) {
                                                
                                                if ( (refcanale>=0) && (refcanale<32) ) {
                                                    //nicehitA_T3 = 1;
                                                    threerpctriggermultiplicity++;
                                                    reco_hit3[triggerhit3] = refcanale;
                                                    triggerhit3++;
                                                }
                                                if ( (refcanale>=32) && (refcanale<64) ) {
                                                    //nicehitB_T3 = 1;
                                                    threerpctriggermultiplicity++;
                                                    reco_hit3[triggerhit3] = refcanale;
                                                    triggerhit3++;
                                                }
                                                if ( (refcanale>=64) && (refcanale<96) ) {
                                                    //nicehitC_T3 = 1;
                                                    threerpctriggermultiplicity++;
                                                    reco_hit3[triggerhit3] = refcanale;
                                                    triggerhit3++;
                                                }
                                                //if (i_station==1) cout << " E = " << jentry << " Found hit at " << refcanale << endl;
                                            }
                                        }
                                    }
                                    
                                }

                                // this was inside the previous cycle...
                                //cout << " Threerpctrig multipl. " << threerpctriggermultiplicity << endl;                                
                                // Discarding events with high multiplicity
                                if ( (triggerhit3 > 0 ) && ( triggerhit2 > 0 ) && (triggerhit > 0) ) {
                                   if ( (threerpctriggermultiplicity < threerpctriggermultiplicity_max ) && ( tworpctriggermultiplicity < tworpctriggermultiplicity_max ) && (onerpctriggermultiplicity < onerpctriggermultiplicity_max)) {
                                      proceditracking = 1;
                                   }
                                   else {
                                      proceditracking = 0;
                                   }
                                }
                                else {
                                   proceditracking = 0;
                                }    
                                nicehit=0;
                                

                                // Tracking algorithm
                                if ( ( trackingreconstruction == 1) && ( proceditracking == 1)  ) {
                                    
                                    // Verboses settings
                                    bool verbose_print_cluster = 0;
                                    bool verbose_print_event_in_cluster = 0;
                                    bool verbose_probable_track = 1;
                                    bool verbose_track = 0;
                                    bool verbose_print_m = 0;
                                    
                                    if ( i_station == 999 ) verbose_print_cluster = 1, verbose_print_event_in_cluster = 1, verbose_probable_track = 1, verbose_track = 1, verbose_print_m = 1;
                                    
                                    
                                    if (verbose_print_event_in_cluster) cout << endl << " EVENT = " << jentry << endl;
                                   
                                    // Cluster calculation for ref1
                                    {
                                        int finish, scan;
                                        if (jentry > 0) {
                                            
                                            for ( scan = 0; scan <= 90; scan++ ) {
                                                
                                                int reco_start = reco_hit1[scan];
                                                int reco_temp = reco_start;
                                                finish = reco_start;
                                                
                                                if ( reco_start > 0 ) {
                                                    
                                                    for ( nn = scan + 1; nn <= 90; nn++ ){
                                                        
                                                        if ( (reco_hit1[nn]>0) && (reco_hit1[nn]<32) && (reco_start < 32) ) {
                                                            temp2 = reco_hit1[ nn ];
                                                        }
                                                        if ( (reco_hit1[nn]>=32) && (reco_hit1[nn]<64) && (reco_start >= 32) && (reco_start < 64) ) {
                                                            temp2 = reco_hit1[ nn ];
                                                        }
                                                        if ( (reco_hit1[nn]>=64) && (reco_start > 64) ) {
                                                            temp2 = reco_hit1[ nn ];
                                                        }
                                                        if ( reco_hit1[nn]==0) {
                                                            scan = nn - 1;
                                                            nn = 96;
                                                        }
                                                        
                                                        
                                                        // Increasing size of the found cluster
                                                        if ( (temp2-reco_temp) == 1 ) {
                                                            
                                                            finish = temp2;
                                                            reco_temp = temp2;
                                                        }
                                                        else {
                                                            
                                                            float mean_cluster = reco_start;
                                                            if (finish>0) mean_cluster = ( finish + reco_start ) / 2;
                                                            
                                                            if (verbose_print_cluster) cout << "1) Entry="<<jentry << " , found starting cluster at strip = " << reco_start << " and finishing at " << finish << " with average " << setprecision(2) << mean_cluster << endl;
                                                            scan = nn - 1;
                                                            nn = 96;
                                                            
                                                            reco_cluster1[realcluster] = mean_cluster;
                                                            realcluster++;
                                                            
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    
                                    // Cluster calculation for ref2
                                    {
                                        int finish, scan;
                                        if (jentry > 0) {
                                            
                                            for ( scan = 0; scan <= 90; scan++ ) {
                                                
                                                int reco_start = reco_hit2[scan];
                                                int reco_temp = reco_start;
                                                finish = reco_start;

                                                if ( reco_start > 0 ) {
                                                    
                                                    for ( nn = scan + 1; nn <= 90; nn++ ){
                                                        
                                                        if ( (reco_hit2[nn]>0) && (reco_hit2[nn]<32) && (reco_start < 32) ) {
                                                            temp2 = reco_hit2[ nn ];

                                                        }
                                                        if ( (reco_hit2[nn]>=32) && (reco_hit2[nn]<64) && (reco_start >= 32) && (reco_start < 64) ) {
                                                            temp2 = reco_hit2[ nn ];
                                                        }
                                                        if ( (reco_hit2[nn]>=64) && (reco_start > 64) ) {
                                                            temp2 = reco_hit2[ nn ];
                                                        }
                                                        if ( reco_hit2[nn]==0) {
                                                            scan = nn - 1;
                                                            nn = 96;
                                                        }
                                                        
                                                        
                                                        // Increasing size of the found cluster
                                                        if ( (temp2-reco_temp) == 1 ) {
                                                            
                                                            finish = temp2;
                                                            reco_temp = temp2;
                                                        }
                                                        else {
                                                            float mean_cluster = reco_start;
                                                            if (finish>0) mean_cluster = ( finish + reco_start ) / 2;
                                                            
                                                            if (verbose_print_cluster) cout << "2) Entry="<<jentry << " , found starting cluster at strip = " << reco_start << " and finishing at " << finish << " with average " << setprecision(2) << mean_cluster << endl;
                                                            scan = nn - 1;
                                                            nn = 96;
                                                            
                                                            reco_cluster2[realcluster2] = mean_cluster;
                                                            realcluster2++;
                                                            
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    
                                    // Cluster calculation for ref3
                                    {
                                        int finish, scan;
                                        if (jentry > 0) {
                                            
                                            for ( scan = 0; scan <= 90; scan++ ) {
                                                
                                                int reco_start = reco_hit3[scan];
                                                int reco_temp = reco_start;
                                                finish = reco_start;

                                                if ( reco_start > 0 ) {
                                                    
                                                    for ( nn = scan + 1; nn <= 90; nn++ ){
                                                        
                                                        if ( (reco_hit3[nn]>0) && (reco_hit3[nn]<32) && (reco_start < 32) ) {
                                                            temp2 = reco_hit3[ nn ];
                                                        }
                                                        if ( (reco_hit3[nn]>=32) && (reco_hit3[nn]<64) && (reco_start >= 32) && (reco_start < 64) ) {
                                                            temp2 = reco_hit3[ nn ];
                                                        }
                                                        if ( (reco_hit3[nn]>=64) && (reco_start > 64) ) {
                                                            temp2 = reco_hit3[ nn ];
                                                        }
                                                        if ( reco_hit3[nn]==0) {
                                                            scan = nn - 1;
                                                            nn = 96;
                                                        }
                                                        
                                                        
                                                        // Increasing size of the found cluster
                                                        if ( (temp2-reco_temp) == 1 ) {
                                                            
                                                            finish = temp2;
                                                            reco_temp = temp2;
                                                        }
                                                        else {
                                                            float mean_cluster = reco_start;
                                                            if (finish>0) mean_cluster = ( finish + reco_start ) / 2;
                                                            
                                                            if (verbose_print_cluster) cout << "3) Entry="<<jentry << " , found starting cluster at strip = " << reco_start << " and finishing at " << finish << " with average " << setprecision(2) << mean_cluster << endl;
                                                            scan = nn - 1;
                                                            nn = 96;
                                                            
                                                            reco_cluster3[realcluster3] = mean_cluster;
                                                            realcluster3++;
                                                            
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                     
                                    // Track reconstruction
                                    if ( ( realcluster > 0) && ( realcluster2 > 0) && ( realcluster3 > 0) )
                                    {
                                        // Initialization
                                        float deltazeta2 = vertical_space * reference_station_t2;
                                        float deltazeta3 = vertical_space * reference_station_t3;
                                        /*
                                        if ( alreadywritten == 0 ) {
                                           cout << " " << endl;
                                           cout << " Track reconstruction info " << endl;
                                           cout << " distance 1-2 = " << deltazeta2 << endl;
                                           cout << " distance 1-3 = " << deltazeta3 << endl;
                                           alreadywritten = 1;
                                        }
                                        */
                                        
                                        float angular_coeff_towards1, angular_coeff_towards3;
                                        float min;
                                        int pmin = -1, aa, bb, cc, ppmax, pp = 0, hh = 0;
                                        float emme_vector[20], qu_vector[20], chi2_vector[20];
                                        
                                        for ( int hh = 0; hh < 20; hh++ ) {
                                            emme_vector[hh] = 999;
                                            qu_vector[hh] = 999;
                                            chi2_vector[hh] = 999;
                                        }

                                
                                        // Performing FITs
                                        {
                                            
                                            for ( aa = 0; aa < 95; aa++ ) {
                                                for ( bb = 0; bb < 95; bb++ ) {
                                                    
                                                    if ( reco_cluster2[aa] == 0 ) {
                                                        aa = 95;
                                                    }
                                                    if ( reco_cluster1[bb] == 0 ) {
                                                        bb = 95;
                                                    }
                                                    
                                                    
                                                    if ( ( reco_cluster2[aa] > 0 ) && ( reco_cluster1[bb] > 0 ) ) {
                                                        

                                                        for ( cc = 0; cc < 95; cc++ ) {
                                                            if ( reco_cluster3[cc] == 0 ) {
                                                                cc = 95;
                                                            }
                                                            else {

                                                                float x[3];
                                                                //const float   z[3] = {0 , 220., 660.};
                                                                const float   z[3] = {0. , deltazeta2, deltazeta3};
                                                                const float zer[3] = {1.,   1.,   1.};
                                                                
                                                                x[0] = reco_cluster1[bb];
                                                                x[1] = reco_cluster2[aa];
                                                                x[2] = reco_cluster3[cc];
                                                                
                                                                const float xer[3] = {1, 1, 1};
                                                                
                                                                TGraphErrors fitgraph(3, z, x, zer, xer);
                                                                if (verbose_fit) {
                                                                    fitgraph.Fit("pol1");
                                                                }
                                                                else {
                                                                    fitgraph.Fit("pol1", "Q");
                                                                }
                                                                
                                                                TF1 *fitfunc = fitgraph.GetFunction("pol1");
                                                                
                                                                double param[2];
                                                                
                                                                fitfunc->GetParameters(param);
                                                                double chisq = fitfunc->GetChisquare();
                                                                double ndf = fitfunc->GetNDF();
                                                                
                                                                int procedi=0;
                                                                if ( (reco_cluster1[bb]<32) && (reco_cluster2[aa]<32) && (reco_cluster3[cc]<32) ) procedi=1;
                                                                
                                                                if ( (reco_cluster1[bb]>=32) && (reco_cluster2[aa]>=32) && (reco_cluster3[cc]>=32) && (reco_cluster1[bb]<64) && (reco_cluster2[aa]<64) && (reco_cluster3[cc]<64) ) procedi=1;
                                                                
                                                                if ( (reco_cluster1[bb]>=64) && (reco_cluster2[aa]>=64) && (reco_cluster3[cc]>=64) ) procedi=1;
                                                                
                                                                if ( procedi == 1 ) {
                                                                    rpc_track = 1;
                                                                    int nice_track_hit = 0;
                                                                    if  ( ( chisq < 20 ) && ( hh < 15 ) && ( chisq > 0.01 )  ) {
                                                                        onetrackatleast = 1;
                                                                        nice_track_hit = 1;
                                                                        emme_vector[hh] = param[1];
                                                                        qu_vector[hh] = param[0];
                                                                        chi2_vector[hh] = chisq;
                                                                        hh++;
                                                                    }
                                                                
                                                                    if ( (i_station==1) && (nice_track_hit == 1) ) {
                                                                        if (verbose_track == 1) cout << "-) Found track! chi2 = " << chisq << " ndf = " << ndf << "  m = " << param[1] << "    q = " << param[0] << " using " << reco_cluster1[bb] << " " << reco_cluster2[aa] << " " << reco_cluster3[cc] << endl;
                                                                    }
                                                                }
                                                                
                                                                
                                                                // Calculate residuals for each point and the squared root of
                                                                // the squared sum of them
                                                                //residual[1] = x[0] - param[1]*z[0] - param[0];
                                                                //residual[2] = x[1] - param[1]*z[1] - param[0];
                                                                //residual[3] = x[2] - param[1]*z[2] - param[0];
                                                                
                                                                //residualX[0] = TMath::Sqrt;(residualX[1]*residualX[1] + residualX[2]*residualX[2] + residualX[3]*residualX[3]);                                                                
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        
                                        // Choosing best FIT
                                        if (onetrackatleast==1) {
                                            float perfectchi2 = chi2_vector[0];
                                            emme = emme_vector[0];
                                            qu = qu_vector[0];
                                            
                                            for ( hh = 0; hh < 20; hh++ ) {
                                                if ( ( abs(chi2_vector[hh]-1) < perfectchi2 )  )  {
                                                    perfectchi2 = chi2_vector[hh];
                                                    emme = emme_vector[hh];
                                                    qu = qu_vector[hh];
                                                }
                                            }

/*
                                            // dumping into text file
                                            trackfileout = fopen("/home/user/temp/dump_tracks.txt","a+");
                                            fprintf(trackfileout,"%f %f\n", qu, emme);
                                            fclose(trackfileout);
*/

                                            if ( i_station == 1) {
                                                h_chi2->Fill( perfectchi2 );
                                                h_m->Fill( emme );
                                                h_qu->Fill( qu );
					    }

/*
                                            // Filling histos and setting right counters
                                            if ( i_station == 1) {
                                               
                                                sprintf(c, "%05d", jentry);
                                                TString stringa = "Event"+ string(c);
                                                TDirectoryFile *hpx = new TDirectoryFile(stringa,"", "", hfile2);
                                                //cout << stringa << " ." << endl;
                                                
                                                hfile2->cd(stringa);
                                                TTree *tree = new TTree("RecTracks","Reconstructed tracks");
                                                tree->Branch("R", "TEveRecTrack", &trackeve, 16000, 2);
                                                
                                                float fillmetemp;
                                                if ( qu >= 64 ) {
                                                    fillmetemp = qu-64;
                                                    trackeve->fV.fX = fillmetemp;
                                                    trackeve->fV.fY = 100;
                                                    trackeve->fV.fZ = 0;
                                                }
                                                if ( qu >= 32 ) {
                                                    fillmetemp = qu-32;
                                                    trackeve->fV.fX = fillmetemp;
                                                    trackeve->fV.fY = 50;
                                                    trackeve->fV.fZ = 0;
                                                }
                                                else {
                                                    trackeve->fV.fX = qu;
                                                    trackeve->fV.fY = 0;
                                                    trackeve->fV.fZ = 0;
                                                }

                                                trackeve->fP.fX = emme;
                                                trackeve->fP.fY = 0;
                                                trackeve->fP.fZ = 1;
                                                
                                                trackeve->fIndex = -1;
                                                tree->Fill();
                                                tree->Write();
                                            }
*/

                                            if ( ( trackselected == 0 ) && ( qu >= 0 ) && ( qu < 32 ) ) {
                                                trackselected = 1;
                                                if ( i_station == 1) {
                                                   //cout << "Event " << jentry << " Increasing A << " << tracked_sector_isA << endl;
                                                   tracked_sector_isA++;
                                                }
                                            }
                                            
                                            if ( ( trackselected == 0 ) && ( qu >= 32 ) && ( qu < 64 ) ) {
                                                trackselected = 1;
                                                if ( i_station == 1) {
                                     	             //cout << "Event " << jentry << " Increasing B << " << tracked_sector_isB << endl; 
	                                             tracked_sector_isB++;
                                                }
                                            }
                                            
                                            if ( ( trackselected == 0 ) && ( qu >= 64 ) && ( qu < 96 ) ) {
                                                trackselected = 1;
                                                if ( i_station == 1) {
                                                    //cout << "Event " << jentry << " Increasing C << " << tracked_sector_isC << endl; 
                                                    tracked_sector_isC++;
                                                }
                                            }
                                            
                                            if (verbose_track == 1) cout << "Event " << jentry << " Choosing track with chi2 = " << perfectchi2 << " and m = " << emme << " and q = " << qu << " track is A = " << tracked_sector_isA << " B = " << tracked_sector_isB << " C = " << tracked_sector_isC << endl;
                                        }
                                        

                                        
                                        // Setting right track flag
                                        nicehit = 0, nicehitA = 0, nicehitB = 0, nicehitC = 0, nicehitA_T2 = 0, nicehitA_T3 = 0, nicehitB_T2 = 0, nicehitB_T3 = 0, nicehitC_T2 = 0, nicehitC_T3 = 0;
                                        if ( trackselected == 1 ) nicehit = 1;
                                        
                                        /*
                                        // Calculating angular coefficients (starting from detector2 in the middle)
                                        {
                                         
                                            for ( aa = 0; aa < 95; aa++ ) {
                                                for ( bb = 0; bb < 95; bb++ ) {
                                                    
                                                    if ( reco_cluster2[aa] == 0 ) {
                                                        aa = 95;
                                                    }
                                                    if ( reco_cluster1[bb] == 0 ) {
                                                        bb = 95;
                                                    }
                                                    
                                                    
                                                    if ( ( reco_cluster2[aa] > 0 ) && ( reco_cluster1[bb] > 0 ) ) {

                                                        angular_coeff_towards1=deltazeta2 / ( reco_cluster2[aa] - reco_cluster1[bb] );
                                                        if ( ((reco_cluster2[aa] - reco_cluster1[bb])<=1) && (reco_cluster2[aa] - reco_cluster1[bb])>=0 ) angular_coeff_towards1=9999;
                                                        
                                                        for ( cc = 0; cc < 95; cc++ ) {
                                                            if ( reco_cluster3[cc] == 0 ) {
                                                                cc = 95;
                                                            }
                                                            else {
                                                                angular_coeff_towards3=deltazeta3 / (reco_cluster3[cc]-reco_cluster2[aa]);
                                                                if ( ((reco_cluster2[aa] - reco_cluster3[cc])<=1) && (reco_cluster2[aa] - reco_cluster3[cc])>=0 ) angular_coeff_towards3=9999;

                                                                float divider=angular_coeff_towards3;
                                                                if ( angular_coeff_towards1 < angular_coeff_towards3 ) divider = angular_coeff_towards1;
                                                                float angular_diff = abs( ( angular_coeff_towards3 - angular_coeff_towards1 ) / divider );
                                                                
                                                                table[1][pp] = angular_diff;
                                                                table[2][pp] = reco_cluster1[bb];
                                                                table[3][pp] = reco_cluster2[aa];
                                                                table[4][pp] = reco_cluster3[cc];
                                                                if (pp<1000) pp++;
                                                                ppmax = pp;
                                                                //if (verbose_print_m) cout << "ENTRY= " << jentry << " m% = " << angular_diff << " given: " << " DUT2: " << reco_cluster2[aa] << " DUT1: " << reco_cluster1[bb] << " DUT3: " << reco_cluster3[cc] << " while angular coeff toward1 is " << angular_coeff_towards1 << " and angular coff toward3 is " << angular_coeff_towards3 << endl;
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        
                                        // Looking for similar angular coefficient
                                        {
                                            min = table[1][0];
                                            for ( int i = 0; i <= ppmax; i++ ) {
                                                if (verbose_probable_track) cout << "...Looking for min angular coeff, found: " << table[1][i] << " at pos " << i << " (min=" << min << ")" << endl;
                                                
                                                if ( ( table[1][i] <= min ) && ( table[1][i] < 999 ) ) {
                                                    min = table[1][i];
                                                    pmin = i;
                                                    if (verbose_probable_track) cout << "...FOUND minimum at position " << pmin << " " << table[1][pmin] << " ( " << table[2][pmin] << " " << table[3][pmin] << " " << table[4][pmin] << " )" << endl;
                                                }
                                            }
                                            if ( pmin >= 0 ) {
                                                reco1 = table[2][pmin];
                                                reco2 = table[3][pmin];
                                                reco3 = table[4][pmin];
                                                if (verbose_probable_track) cout << " Found a probable track: DUT1 strip = " << reco1 << " DUT2 strip = " << reco2 << " DUT3 strip = " << reco3 << endl;
                                            }
                                        }
                                        
                                        // Fitting candidate track
                                        if ( pmin >= 0) {
                                            float x[3];
                                            const float   z[3] = {0. , deltazeta2, deltazeta3};
                                            const float zer[3] = {5.,   5.,   5.};
                                            
                                            x[0] = reco1;
                                            x[1] = reco2;
                                            x[2] = reco3;
                                            
                                            const float xer[3] = {1, 1, 1};
                                            
                                            TGraphErrors fitgraph(3, z, x, zer, xer);
                                            if (verbose_fit) {
                                                fitgraph.Fit("pol1");
                                            }
                                            else {
                                                fitgraph.Fit("pol1", "Q");
                                            }
                                            
                                            TF1 *fitfunc = fitgraph.GetFunction("pol1");
                                            
                                            double param[2];
                                            
                                            fitfunc->GetParameters(param);
                                            double chisq = fitfunc->GetChisquare();
                                            double ndf = fitfunc->GetNDF();
                                            
                                            rpc_track = 1;
                                            nicehit = 1;
                                            emme = param[1];
                                            qu = param[0];
                                            
                                            if (i_station==1) {
                                                if (verbose_track == 1) cout << "-) Found track! chi2 = " << chisq << " ndf = " << ndf << "  m = " << emme << "    q = " << qu << endl;
                                            }
                                            // Calculate residuals for each point and the squared root of
                                            // the squared sum of them
                                            //residual[1] = x[0] - param[1]*z[0] - param[0];
                                            //residual[2] = x[1] - param[1]*z[1] - param[0];
                                            //residual[3] = x[2] - param[1]*z[2] - param[0];
                                            
                                            //residualX[0] = TMath::Sqrt(residualX[1]*residualX[1] + residualX[2]*residualX[2] + residualX[3]*residualX[3]);                                            
                                        }
                                        */
                                    } 
                                }
                            }
                        }
                        
                        // Printing informations (event by event)
                        {
                            if (master_trigger) {
                                //cout << "CounterA = " <<  CounterA << "CounterB = " << counterB << "CounterC = " << counterC << "CounterSMALL = " << counter << endl;
                                if (verbose_filling) cout << " " << endl << "Event: " << jentry << "  " << endl << "TRG -> at(0) = " << TRIG->at( 0 ) << endl;
                                if (verbose_filling) cout << "HITS_PER_CHANNEL -> at(" << trigger_bit << ") = " << HITS_PER_CHANNEL->at( trigger_bit ) << endl << " " << endl;
                            }
                            else {
                                if (verbose_filling) cout << " " << endl << "Event: " << jentry << "  " << endl << " NO TRG!!!!!!!! " << endl;
                            }
                        }
                        
                        // Number of iterations per station
                        {
                            if ( aModule1 > 0 ) {
                                cycle_max = 2;
                            }
                            else {
                                cycle_max = 1;
                            }
                        }
                        
                        // Looping over all channels of the station
                        for ( int cycle = 1; cycle <= cycle_max; cycle++ ) {
                            
                            if (cycle == 1) channel_start = map_start, channel_finish = map_finish, channel_module = aModule;
                            if (cycle == 2) channel_start = map_start1, channel_finish = map_finish1, channel_module = aModule1;
                            
                            for ( unsigned aChannel = channel_start; aChannel < channel_finish; aChannel++ ) {
                                
                                // Vector Hit Initialization
                                vector<unsigned int> *HITS_PER_CHANNELC = HitsPerChannel[ channel_module ];
                                vector<unsigned int> *CH = _Data[ channel_module ][ aChannel ];
                                
                                // Printing informations inside the event
                                {
                                    if (verbose_filling2) cout << " " << endl << "***Event: " << jentry << "  " << "Module: " << aModule << "   " << "Station: " << i_station << "   " << "Channel: " << aChannel << endl;
                                    if (verbose_filling2) cout << "***Trigger: " << TRIG->at( 0 ) << endl;
                                    if (verbose_filling2) cout << "***Hits per channel: " << HITS_PER_CHANNELC->at( aChannel ) << endl << " " << endl;
                                }
                                

                                
                                // Event analyzing
                                multievents = HITS_PER_CHANNELC->at( aChannel );
                                if (multievents>=50) multievents=50;
                                for ( int muev = 1; muev <= multievents ; muev++) {
                                    canale_ev[muev] = CH->at( muev-1 );
                                    //cout << " FOUND MULTIEVENT = " << multievents << endl;
                                }

                                for ( int muev = 1; muev <= multievents ; muev++) {
                                    long ch = canale_ev[muev];
                                //}                                    
                                    
                                //if( multievents > 0 ) {

                                    //long ch = CH->at( 0 );
                                    
                                    // Readout Mapping
                                    canale = aChannel - channel_start;
                                    if ( (channel_start == 0) && (channel_finish == 64) ) canale = aChannel + 32;
                                    if ( (channel_start == 0) && (channel_finish == 32) ) canale = aChannel + 64;

                                    if (canale<64) timing_strip[i_station][canale][ muev ] = ch - bunchIDmod[channel_module]*250;
                                    //cout << " " << muev << " " << canale_ev[muev] << endl;

                                    // Cutting chamber edges
                                    {
                                        if ( ( etapartition == "A" ) && ( canale < canale_cut ) && ( canale > 0 ) ) cut = 1;
                                        if ( ( etapartition == "A" ) && ( canale > 32-canale_cut ) && ( canale < 32 ) ) cut = 1;
                                        if ( ( etapartition == "B" ) && ( canale < 32+canale_cut ) && ( canale > 32 ) && (canale<64) ) cut = 1;
                                        if ( ( etapartition == "B" ) && ( canale > 64-canale_cut ) && ( canale > 32 ) && (canale<64) ) cut = 1;
                                        if ( ( etapartition == "C" ) && ( canale < 64+canale_cut ) && ( canale > 64 ) && (canale <96) ) cut = 1;
                                        if ( ( etapartition == "C" ) && ( canale > 96-canale_cut ) && ( canale > 64 ) && (canale <96) ) cut = 1;
                                        
                                        if ( ( etapartition == "ALL" ) && ( canale<canale_cut ) && (canale > 0 ) ) cut = 1;
                                        if ( ( etapartition == "ALL" ) && ( canale>32-canale_cut ) && (canale < 32 ) ) cut = 1;
                                        if ( ( etapartition == "ALL" ) && ( canale<32+canale_cut ) && (canale > 32 ) && (canale<64) ) cut = 1;
                                        if ( ( etapartition == "ALL" ) && ( canale>64-canale_cut ) && (canale > 32 ) && (canale<64) ) cut = 1;
                                        if ( ( etapartition == "ALL" ) && ( canale<64+canale_cut ) && (canale > 64 ) && (canale <96) ) cut = 1;
                                        if ( ( etapartition == "ALL" ) && ( canale>96-canale_cut ) && (canale > 64 ) && (canale <96) ) cut = 1;
                                    }
                                    cut = 0;
                                    
                                    if (cut == 0) {
                                        
                                        // Filling the strip, noise profiles, 2D-histo and cluster noise
                                        {
                                            if (  (nicehit == 1) || (nicehitA == 1) || (nicehitB == 1) || (nicehitC == 1)  ) {
                                                correct_timing = ( ( trig - ch ) - (bunchIDmod[channel_module]*250) ) ;
                                                h_Strip2d->Fill( aChannel + (128*channel_module) , correct_timing/5 );
                                                
                                                //cout << "Station " << i_station << " Module " << aModule << " event: " << jentry << " Canale = " << canale << " Trig - timingCh = " << (trig-ch)/10 << endl;
                                                
                                                if ( ( correct_timing > trg_windows_min) &&  ( correct_timing < trg_windows_max) ) {
                                                    
                                                    if ( canale<32 ) h_Strip[i_station]->Fill(31-canale);
                                                    if ( (canale>=32) && (canale<64) ) h_Strip[i_station]->Fill(63-canale+32);
                                                    if ( canale>=64) h_Strip[i_station]->Fill(95-canale+64);                                                    
                                                    
                                                    if (verbose_filling_detailed) cout << "Module " << channel_module << " event: " << jentry << " found fired channel at strip at: " << ch << " aChannel = " << aChannel << "    Trig - Ch = " << trig-ch << endl << " " << endl;
                                                }
                                                else {
                                                    if ( ( correct_timing < (trg_windows_min-delaytrg*10) ) || ( correct_timing > (trg_windows_max+delaytrg*10) ) ) {

                                                        if ( canale<32 ) h_Stripnoise[i_station]->Fill(31-canale);
                                                        if ( (canale>=32) && (canale<64) ) h_Stripnoise[i_station]->Fill(63-canale+32);
                                                        if ( canale>=64) h_Stripnoise[i_station]->Fill(95-canale+64);

                                                        if ( canale<32 ) noiserateA[i_station]++;
                                                        if ( (canale>=32) && (canale<64) ) noiserateB[i_station]++;
                                                        if ( canale>=64 ) noiserateC[i_station]++;
                                                    
                                                        if (verbose_filling_eff_noise) cout << "-->Event: " << jentry << " is NOISY! found fired channel at strip at: " << ch << " aChannel = " << aChannel << "       Trig - Ch = " << trig-ch << endl;                                                        
                                                        
                                                        // Noise Clusterization calculation
                                                        {
                                                            if ( canale < 32) {
                                                                clusterizationAnoise[kka_noise] = canale;
                                                                kka_noise++;
                                                            }
                                                            
                                                            if ( (canale >=32) && (canale < 64) ) {
                                                                clusterizationBnoise[kkb_noise] = canale;
                                                                kkb_noise++;
                                                            }
                                                            
                                                            if ( canale >= 64 ) {
                                                                clusterizationCnoise[kkc_noise] = canale;
                                                                kkc_noise++;
                                                            }
                                                            
                                                        }
                                                    }
                                                }
                                            }
                                            else {

                                                if ( canale<32 ) h_Stripnoise[i_station]->Fill(31-canale);
                                                if ( (canale>=32) && (canale<64) ) h_Stripnoise[i_station]->Fill(63-canale+32);
                                                if ( canale>=64) h_Stripnoise[i_station]->Fill(95-canale+64);

                                                if ( canale<32 ) noiserateA[i_station]++;
                                                if ( (canale>=32) && (canale<64) ) noiserateB[i_station]++;
                                                if ( (canale>=64) ) noiserateC[i_station]++;
                                                
                                                if (verbose_filling_eff_noise) cout << "-->Event: " << jentry << " is NOISY! found fired channel at strip at: " << ch << " aChannel = " << aChannel << "       Trig - Ch = " << trig-ch << endl;
                                                
                                                // Noise Clusterization calculation
                                                {
                                                    if ( canale < 32) {
                                                        clusterizationAnoise[kka_noise] = canale;
                                                        kka_noise++;
                                                    }
                                                    
                                                    if ( (canale >=32) && (canale < 64) ) {
                                                        clusterizationBnoise[kkb_noise] = canale;
                                                        kkb_noise++;
                                                    }
                                                    
                                                    if ( canale >= 64 ) {
                                                        clusterizationCnoise[kkc_noise] = canale;
                                                        kkc_noise++;
                                                    }
                                                    
                                                }

                                            }
                                        }
                                        
                                        // Efficiency and cluster calculation
                                        if ( (nicehit == 1) || (nicehitA == 1) || (nicehitB == 1) || (nicehitC == 1) ) {
                                            correct_timing = ( ( trig - ch ) - (bunchIDmod[channel_module]*250) ) ;
                                            if ( ( correct_timing > trg_windows_min ) && ( correct_timing < trg_windows_max) ) {
                                                
                                                // Printing info
                                                {
                                                    if (verbose_filling_eff_counts) cout << "-->Event: " << jentry << " is EFFICIENT! found fired channel at strip: " << canale << " Station = " << i_station << ", Trig - Ch = " << (trig-ch) << endl;
                                                }
                                                
                                                // Clusterization calculation
                                                {
                                                    if ( canale < 32) {
                                                        clusterizationA[kka] = canale;
                                                        kka++;
                                                    }
                                                        
                                                    if ( (canale >=32) && (canale < 64) ) {
                                                        clusterizationB[kkb] = canale;
                                                        kkb++;
                                                    }
                                                        
                                                    if ( canale >= 64 ) {
                                                        clusterizationC[kkc] = canale;
                                                        kkc++;
                                                    }
                                                        
                                                    //if (verbose_clustering1) cout << " Clustering: kk=" << kk << " canale=" << canale << " clusterization[" << kk << "] " << clusterizationA[kk] << endl;
                                                }
                                                
                                                // Calculate efficiencies for station i in both cases "eta = ALL" && "eta = A||B||C"
                                                if (trackingreconstruction == 0) {
                                                    if ( (etapartition == "ALL" ) && (largeacceptance == 0) ) {
                                                        
                                                        if ( (firedchannelA == 0) && (nicehitA == 1) && (canale >=0) && (canale < 32) ) {
                                                            chamber_efficiencyA[i_station] ++;
                                                            firedchannelA = 1;
                                                            
                                                            
                                                            // Printing info
                                                            {
                                                                if (detailed_printout) cout << "A Efficient Event: " << jentry << " found fired channel at strip: " << canale << " Station = " << i_station << ", Trig - Ch = " << (trig-ch) << endl;
                                                            }
                                                            //cout << "A) Chamber_efficiency[" << i_station << "] = " << chamber_efficiencyA[i_station] << endl;
                                                        }
                                                        if ( (firedchannelB == 0) && (nicehitB == 1) && (canale >=32) && (canale < 64) ) {
                                                            chamber_efficiencyB[i_station] ++;
                                                            firedchannelB = 1;
                                                            
                                                            
                                                            // Printing info
                                                            {
                                                                if (detailed_printout) cout << "B Efficient Event: " << jentry << " found fired channel at strip: " << canale << " Station = " << i_station << ", Trig - Ch = " << (trig-ch) << endl;
                                                            }
                                                            //cout << "B) Chamber_efficiency[" << i_station << "] = " << chamber_efficiencyB[i_station] << endl;
                                                        }
                                                        if ( (firedchannelC == 0) && (nicehitC == 1) && (canale >=64) && (canale < 96) )  {
                                                            chamber_efficiencyC[i_station] ++;
                                                            firedchannelC = 1;
                                                            
                                                            // Printing info
                                                            {
                                                                if (detailed_printout) cout << "C Efficient Event: " << jentry << " found fired channel at strip: " << canale << " Station = " << i_station << ", Trig - Ch = " << (trig-ch) << endl;
                                                            }
                                                            //cout << "C) Chamber_efficiency[" << i_station << "] = " << chamber_efficiencyC[i_station] << endl;
                                                        }
                                                    }
                                                    if ( (etapartition == "ALL" ) && (largeacceptance == 1) ) {
                                                        
                                                        if ( (firedchannel == 0) && (nicehit == 1)  ) {
                                                            chamber_efficiency[i_station] ++;
                                                            firedchannel = 1;
                                                            //cout << " Chamber_efficiency[" << i_station << "] = " << chamber_efficiency[i_station] << endl;
                                                        }
                                                    }
                                                    else {
                                                        if ( firedchannel == 0 ) {
                                                            //chamber_efficiency[i_station] ++;
                                                            if (etapartition=="A" && canale>=0 && canale<32) chamber_efficiency[i_station] ++;
                                                            
                                                            if (etapartition=="B" && canale>=32 && canale<64) chamber_efficiency[i_station] ++;
                                                            
                                                            if (etapartition=="C" && canale>=64 && canale<96) chamber_efficiency[i_station] ++;
                                                            
                                                            //cout << "chamber_efficiency[" << i_station << "]=" << chamber_efficiency[i_station] << " canale=" << canale << " aChannel=" << aChannel << " evento=" << jentry << " I should start at channel " << map_start << " and finish at " << map_finish << " of module " << aModule << endl;
                                                            firedchannel = 1;
                                                            if ( counterdecrease == 1 ) counterout[i_station]--;
                                                            //if ( counterdecrease == 1) cout << " event changed! " << jentry << endl;
                                                            //if (i_station == 4) cout << "efficient EVENT: " << jentry << endl;
                                                        }
                                                    }
                                                }
                                                else {
                                                    bool verbose_track_detailed = 0;
                                                    
                                                    float expected_hit = i_station * vertical_space * emme + qu;
                                                    if ( channel_by_channel_trg == 0 ) {
                                                        triggers_per_channels[i_station][ int(expected_hit) ]++;
                                                        channel_by_channel_trg = 1;
                                                    }
                                                    
                                                    float resolution = expected_hit - canale;
                                                    int resoint = int(expected_hit);
                                                    if ( (abs(resolution)<32  ) && (ch_uneff == 0) ) {
                                                        chamber_unefficiency[i_station][resoint]++;
                                                        ch_uneff = 1;
                                                    }
                                                
                                                    h_residual[i_station]->Fill( resolution );
                                                    
                                                    if ( ( i_station == reference_station) || ( i_station == reference_station_t2) || ( i_station == reference_station_t3) ) {
                                                        temp_residual = temp_residual + resolution*resolution;
                                                    }
                                                    
                                                        
                                                    if (verbose_track_detailed) cout << " -> I am expecting a hit on the detector " << i_station << " at strip " << expected_hit << " nicehits = " << nicehitA << " " << nicehitB << " " << nicehitC << endl;
                                                    
                                                    int goon=0;
                                                    if ( (qu<32) && (canale<32) ) goon=1;
                                                    if ( (qu>=32) && (canale>=32) && (qu<64) && (canale<64) ) goon=1;
                                                    if ( (qu>=64) && (canale>=64) ) goon=1;
                                                    
                                                    if ( abs(expected_hit-canale) < efficiency_radius ) {
                                                        
                                                        //if ( abs(expected_hit-canale) < 5 ) {
                                                        //timehit++;
                                                        //tree904->Fill();
                                                        //}
                                                        if ( (firedchannelA == 0) && (qu >=0) && (qu < 32) ) {
                                                            chamber_efficiencyA[i_station] ++;
                                                            firedchannelA = 1;
                                                            firedchannelB = 1;
                                                            firedchannelC = 1;
                                                            if (verbose_track_detailed) cout << " --> Detector " << i_station << " event " << jentry << " I just find a hit at strip " << canale << " (expecting at " << expected_hit << " )" << endl;
                                                        }
                                                            
                                                        if ( (firedchannelB == 0) && (qu >=32) && (qu < 64) ) {
                                                            chamber_efficiencyB[i_station] ++;
                                                            firedchannelA = 1;
                                                            firedchannelB = 1;
                                                            firedchannelC = 1;
                                                            if (verbose_track_detailed) cout << " --> Detector " << i_station << " event " << jentry << " I just find a hit at strip " << canale << " (expecting at " << expected_hit << " )" << endl;

                                                        }

                                                        if ( (firedchannelC == 0) && (qu >=64) ) {
                                                            chamber_efficiencyC[i_station] ++;
                                                            firedchannelA = 1;
                                                            firedchannelB = 1;
                                                            firedchannelC = 1;
                                                            if (verbose_track_detailed) cout << " --> Detector " << i_station << " event " << jentry << " I just find a hit at strip " << canale << " (expecting at " << expected_hit << " )" << endl;
                                                        }
                                                    }
                                                }                                                
                                            }
                                        }
                                        
                                        // CrossTalk studies
                                        {
                                            if (etapartition=="ALL") {
                                                if ( (firedchannelA == 1) && (firedchannelB == 0) && (firedchannelC == 0) ) {
                                                    if ( ( (trig - ch) > trg_windows_min ) && ( (trig - ch) < trg_windows_max) ) {
                                                        if ( (canale >=32) && (canale < 96 ) ) xtalk++;
                                                    }
                                                    //cout << " Found xtalk!" << endl;
                                                }
                                                
                                                if ( (firedchannelA == 0) && (firedchannelB == 1) && (firedchannelC == 0) ) {
                                                    if ( ( (trig - ch) > trg_windows_min ) && ( (trig - ch) < trg_windows_max) ) {
                                                        if ( (canale >=0) && (canale < 32 ) ) xtalk++;
                                                        if ( (canale >=64) && (canale < 96 ) ) xtalk++;
                                                    }
                                                    //cout << " Found xtalk!" << endl;
                                                }
                                                
                                                if ( (firedchannelA == 0) && (firedchannelB == 0) && (firedchannelC == 1) ) {
                                                    if ( ( (trig - ch) > trg_windows_min ) && ( (trig - ch) < trg_windows_max) ) {
                                                        if ( (canale >=0) && (canale < 64 ) ) xtalk++;
                                                    }
                                                    //cout << " Found xtalk!" << endl;
                                                }
                                            }
                                            else {
                                                
                                                if ( (firedchannel == 1) && (etapartition=="A") ) {
                                                    if ( ( (trig - ch) > trg_windows_min ) && ( (trig - ch) < trg_windows_max) ) {
                                                        if ( (canale >=32) && (canale < 64 ) ) xtalk++;
                                                        //cout << " Found xtalk in B and C" << endl;
                                                    }
                                                }
                                                
                                                if ( (firedchannel == 1) && (etapartition=="B") ) {
                                                    if ( ( (trig - ch) > trg_windows_min ) && ( (trig - ch) < trg_windows_max) ) {
                                                        if ( (canale >=0) && (canale < 32 ) ) xtalk++;
                                                        if ( (canale >=64) && (canale < 96 ) ) xtalk++;
                                                        //cout << " Found xtalk in A and C" << endl;
                                                    }
                                                }
                                                
                                                if ( (firedchannel == 1) && (etapartition=="C") ) {
                                                    if ( ( (trig - ch) > trg_windows_min ) && ( (trig - ch) < trg_windows_max) ) {
                                                        if ( (canale >=0) && (canale < 64 ) ) xtalk++;
                                                        //cout << " Found xtalk in A and B" << endl;
                                                    }
                                                }
                                                
                                            }
                                            
                                        }
                                    }
                                }
                            }
                        }
                        
                        // Calculating cluster size, cluster&hit multiplicity, CrossTalk
                        {
                            multihit = 0;
                            if (twotrackingrpc == 1) {
                                nicehit=0;
                                if ( nicehitA_T2 == 0 ) nicehitA = 0;
                                if ( nicehitB_T2 == 0 ) nicehitB = 0;
                                if ( nicehitC_T2 == 0 ) nicehitC = 0;
                            }
                            
                            if ( (threetrackingrpc == 1) && (trackingreconstruction == 0) ) {
                                nicehit=0;
                                if ( nicehitA_T2 == 0 ) nicehitA = 0;
                                if ( nicehitB_T2 == 0 ) nicehitB = 0;
                                if ( nicehitC_T2 == 0 ) nicehitC = 0;
                                if ( nicehitA_T3 == 0 ) nicehitA = 0;
                                if ( nicehitB_T3 == 0 ) nicehitB = 0;
                                if ( nicehitC_T3 == 0 ) nicehitC = 0;
                                
                            }
                            
                            if ( (nicehit == 1) || (nicehitA == 1) || (nicehitB == 1) || (nicehitC == 1) ) {

                                multihit = 1;
                                
                                // Clusterization A
                                for ( kk = 1; kk <= 96; kk++ ) {
                                    
                                    clusterA=0;
                                    if (clusterizationA[kk]>0) {
                                        
                                        temp1 = clusterizationA[kk];
                                        clusterA++;
                                        hitmultiplicity++;
                                        
                                        //cout << jentry << " Found hit at: " << clusterizationA[kk] << endl;
                                        for ( nn = kk + 1; nn <= 96; nn++ ){
                                            
                                            if (clusterizationA[nn]>0) {
                                                //cout << jentry << " Found next hit at: " << clusterizationA[nn] << endl;

                                                temp2 = clusterizationA[ nn ];
                                                hitmultiplicity++;
                                                if ( (temp2-temp1) == 1 ) {
                                                    clusterA++;
                                                    temp1 = temp2;
                                                    //cout << jentry << " Increasing csize to " << clusterA << endl;
                                                }
                                                else {
                                                    //cout << jentry << " Filling Cluster A: "  << clusterA << endl;
                                
                                                    h_clustersizeA[i_station]->Fill( clusterA );
                                                    temp2 = 999999;
                                                    kk = nn - 1;
                                                    nn = 96;
                                                    clustermultiplicity++;
                                                }                                                
                                            }
                                            else {
                                                //cout << jentry << " Filling Cluster A: "  << clusterA << endl;

                                                h_clustersizeA[i_station]->Fill( clusterA );
                                                temp2 = 999999;
                                                kk = nn - 1;
                                                nn = 96;
                                                clustermultiplicity++;

                                            }
                                        }
                                    }
                                    
                                }
                                
                                // Clusterization B
                                for ( kk = 1; kk <= 96; kk++ ) {
                                    
                                    clusterB=0;
                                    if (clusterizationB[kk]>0) {
                                        
                                        temp1 = clusterizationB[kk];
                                        clusterB++;
                                        hitmultiplicity++;
                                        
                                        //cout << jentry << " Found hit at: " << clusterizationB[kk] << endl;
                                        for ( nn = kk + 1; nn <= 96; nn++ ){
                                            
                                            if (clusterizationB[nn]>0) {
                                                //cout << jentry << " Found next hit at: " << clusterizationB[nn] << endl;
                                                
                                                temp2 = clusterizationB[ nn ];
                                                hitmultiplicity++;
                                                if ( (temp2-temp1) == 1 ) {
                                                    clusterB++;
                                                    temp1 = temp2;
                                                    //cout << jentry << " Increasing csize to " << clusterB << endl;
                                                }
                                                else {
                                                    //cout << jentry << " Filling Cluster A: "  << clusterB << endl;
                                                    
                                                    h_clustersizeB[i_station]->Fill( clusterB );
                                                    temp2 = 999999;
                                                    kk = nn - 1;
                                                    nn = 96;
                                                    clustermultiplicity++;
                                                }
                                            }
                                            else {
                                                //cout << jentry << " Filling Cluster A: "  << clusterA << endl;
                                                
                                                h_clustersizeB[i_station]->Fill( clusterB );
                                                temp2 = 999999;
                                                kk = nn - 1;
                                                nn = 96;
                                                clustermultiplicity++;                                                
                                            }
                                        }
                                    }                                    
                                }
                                
                                // Clusterization C
                                for ( kk = 1; kk <= 96; kk++ ) {
                                    
                                    
                                    clusterC=0;
                                    if (clusterizationC[kk]>=1) {
                                    
                                        //debugqui
                                        clusterC++;

                                        
                                        //Look for the fastest strip and time
                                        if ( clusterizationC[kk] <= 32 ) {
                                            channel_module = aModule;
                                            channeltosee = clusterizationC[kk] - map_start;
                                        }
                                        if ( ( clusterizationC[kk] < 64 ) && (clusterizationC[kk] > 32 ) ) {
                                            if ( (map_finish-map_start) == 96 ) {
                                                channel_module = aModule;
                                                channeltosee = clusterizationC[kk] + 32;
                                            }                                            
                                            if ( (map_finish-map_start) == 32 ) {
                                                channel_module = aModule1;
                                                channeltosee = clusterizationC[kk] - 32;
                                            }
                                            if ( (map_finish-map_start) == 64 ) {
                                                channel_module = aModule;
                                                channeltosee = clusterizationC[kk] + 64;
                                            }
                                        }
                                        if ( clusterizationC[kk] >= 64 ) {
                                            if ( (map_finish-map_start) == 96 ) {
                                                if (map_start==32) {
                                                    channeltosee = clusterizationC[kk] + 32;
                                                }
                                                else {
                                                    channeltosee = clusterizationC[kk];
                                                }
                                                channel_module = aModule;
                                            }
                                            if ( (map_finish-map_start) == 64 ) {
                                                channel_module = aModule1;
                                                channeltosee = clusterizationC[kk] - 64 ;
                                            }
                                            if ( (map_finish-map_start) == 32 ) {
                                                channel_module = aModule1;
                                                channeltosee = clusterizationC[kk] - 32;
                                            }
                                        }

                                        vector<unsigned int> *CHT = _Data[ channel_module ][ channeltosee ];
                                        
                                        long timingclusterC1 = CHT->at( 0 );

                                        if (kk==1) {
                                            fastest_time = timingclusterC1;
                                            fastest_strip = clusterizationC[kk];
                                            slowest_time = timingclusterC1;
                                        }
                                        if (clusterC>1) {
                                            if (timingclusterC1<fastest_time) {
                                                fastest_time = timingclusterC1;
                                                fastest_strip = clusterizationC[kk];
                                            }
                                            if (timingclusterC1>slowest_time) {
                                                slowest_time = timingclusterC1;
                                            }
                                            
                                            
                                        }

                                        
                                        temp1 = clusterizationC[kk];
                                        hitmultiplicity++;
                                        
                                        
                                        //cout << jentry << " Station " << i_station << " Found hit at: " << clusterizationC[kk] << " " << " Module= " << channel_module << " channel=" << channeltosee << " time = " << timingclusterC1 << " fastest is " << fastest_time << endl;


                                        for ( nn = kk + 1; nn <= 96; nn++ ){
                                            
                                            if (clusterizationC[nn]>=1) {
                                                
                                                if ( clusterizationC[nn] <= 32 ) {
                                                    channel_module = aModule;
                                                    channeltosee = clusterizationC[nn] - map_start;
                                                }
                                                if ( ( clusterizationC[nn] < 64 ) && (clusterizationC[nn] > 32 ) ) {
                                                    if ( (map_finish-map_start) == 96 ) {
                                                        channel_module = aModule;
                                                        channeltosee = clusterizationC[nn] + 32;
                                                    }
                                                    if ( (map_finish-map_start) == 32 ) {
                                                        channel_module = aModule1;
                                                        channeltosee = clusterizationC[nn] - 32;
                                                    }
                                                    if ( (map_finish-map_start) == 64 ) {
                                                        channel_module = aModule;
                                                        channeltosee = clusterizationC[nn] + 64;
                                                    }
                                                }
                                                if ( clusterizationC[nn] >= 64 ) {
                                                    if ( (map_finish-map_start) == 96 ) {
                                                        if (map_start==32) {
                                                            channeltosee = clusterizationC[nn] + 32;
                                                        }
                                                        else {
                                                            channeltosee = clusterizationC[nn];
                                                        }
                                                        channel_module = aModule;
                                                    }
                                                    if ( (map_finish-map_start) == 64 ) {
                                                        channel_module = aModule1;
                                                        channeltosee = clusterizationC[nn] - 64 ;
                                                    }
                                                    if ( (map_finish-map_start) == 32 ) {
                                                        channel_module = aModule1;
                                                        channeltosee = clusterizationC[nn] - 32;
                                                    }
                                                }

                                                vector<unsigned int> *CHT2 = _Data[ channel_module ][ channeltosee ];
                                                
                                                long timingclusterC2 = CHT2->at( 0 );

                                                if (timingclusterC2<fastest_time) {
                                                    fastest_strip = clusterizationC[nn];
                                                    fastest_time = timingclusterC2;
                                                }
                                                
                                                //cout << jentry << " --> Found next hit at " << clusterizationC[nn] << " " << " Module= " << channel_module << " channel=" << channeltosee << " time = " << timingclusterC2 << " fastest is " << fastest_time << endl;

                                                temp2 = clusterizationC[ nn ];

                                                
                                                hitmultiplicity++;
                                                if ( (temp2-temp1) == 1 ) {
                                                    clusterC++;
                                                    temp1 = temp2;
                                                    
                                                    //cout << jentry << " Increasing csize to " << clusterC << endl;
                                                }
                                                else {
                                                    //cout << jentry << " Filling Cluster C: "  << clusterC << endl;
                                                    
                                                    h_clustersizeC[i_station]->Fill( clusterC );
                                                    //if ( jentry<500 ) cout << " DEBUG EVENT = " << jentry << " station = " << i_station << " cisze = " << clusterC << endl; 
                                                    clustermultiplicity++;
                                                    
                                                    /*
                                                    for ( int nnn = kk; nnn <= nn; nnn++ ){
                                                        
                                                        if (clusterizationC[nnn]>=1) {
                                                            
                                                            if ( clusterizationC[nnn] <= 32 ) {
                                                                channel_module = aModule;
                                                                channeltosee = clusterizationC[nnn] - map_start;
                                                            }
                                                            if ( ( clusterizationC[nn] < 64 ) && (clusterizationC[nnn] > 32 ) ) {
                                                                if ( (map_finish-map_start) == 96 ) {
                                                                    channel_module = aModule;
                                                                    channeltosee = clusterizationC[nnn] + 32;
                                                                }
                                                                if ( (map_finish-map_start) == 32 ) {
                                                                    channel_module = aModule1;
                                                                    channeltosee = clusterizationC[nnn] - 32;
                                                                }
                                                                if ( (map_finish-map_start) == 64 ) {
                                                                    channel_module = aModule;
                                                                    channeltosee = clusterizationC[nnn] + 64;
                                                                }
                                                            }
                                                            if ( clusterizationC[nnn] >= 64 ) {
                                                                if ( (map_finish-map_start) == 96 ) {
                                                                    if (map_start==32) {
                                                                        channeltosee = clusterizationC[nnn] + 32;
                                                                    }
                                                                    else {
                                                                        channeltosee = clusterizationC[nnn];
                                                                    }
                                                                    channel_module = aModule;
                                                                }
                                                                if ( (map_finish-map_start) == 64 ) {
                                                                    channel_module = aModule1;
                                                                    channeltosee = clusterizationC[nnn] - 64 ;
                                                                }
                                                                if ( (map_finish-map_start) == 32 ) {
                                                                    channel_module = aModule1;
                                                                    channeltosee = clusterizationC[nnn] - 32;
                                                                }
                                                            }
                                                            
                                                            vector<unsigned int> *CHT = _Data[ channel_module ][ channeltosee ];
                                                            
                                                            long timingcluster_fill = CHT->at( 0 );
                                                            
                                                            deltatfastime = fastest_time - timingcluster_fill ;
                                                            deltastripfast = fastest_strip - clusterizationC[nnn] ;
                                                            
                                                            tree904->Fill();

//                                                            //cout << jentry << " --> Filling deltachannel=" << deltastripfast << " fastest = " << fastest_strip << " fastest_time = " << deltatfastime << " other= " << clusterizationC[nnn] << endl;

                                                        }
                                                    }
                                                    */
                                                    temp2 = 999999;
                                                    kk = nn - 1;
                                                    nn = 96;
                                                    
                                                }
                                            }
                                            else {
                                                //cout << jentry << " Filling Cluster C: "  << clusterC << endl;
                                                
                                                h_clustersizeC[i_station]->Fill( clusterC );
                                                //if (jentry<500 ) cout << " DEBUG EVENT = " << jentry << " station = " << i_station << " cisze = " << clusterC << endl;
                                                /*
                                                for ( int nnn = kk; nnn <= nn; nnn++ ){
                                                    
                                                    if (clusterizationC[nnn]>=1) {
                                                        
                                                        if ( clusterizationC[nnn] <= 32 ) {
                                                            channel_module = aModule;
                                                            channeltosee = clusterizationC[nnn] - map_start;
                                                        }
                                                        if ( ( clusterizationC[nn] < 64 ) && (clusterizationC[nnn] > 32 ) ) {
                                                            if ( (map_finish-map_start) == 96 ) {
                                                                channel_module = aModule;
                                                                channeltosee = clusterizationC[nnn] + 32;
                                                            }
                                                            if ( (map_finish-map_start) == 32 ) {
                                                                channel_module = aModule1;
                                                                channeltosee = clusterizationC[nnn] - 32;
                                                            }
                                                            if ( (map_finish-map_start) == 64 ) {
                                                                channel_module = aModule;
                                                                channeltosee = clusterizationC[nnn] + 64;
                                                            }
                                                        }
                                                        if ( clusterizationC[nnn] >= 64 ) {
                                                            if ( (map_finish-map_start) == 96 ) {
                                                                if (map_start==32) {
                                                                    channeltosee = clusterizationC[nnn] + 32;
                                                                }
                                                                else {
                                                                    channeltosee = clusterizationC[nnn];
                                                                }
                                                                channel_module = aModule;
                                                            }
                                                            if ( (map_finish-map_start) == 64 ) {
                                                                channel_module = aModule1;
                                                                channeltosee = clusterizationC[nnn] - 64 ;
                                                            }
                                                            if ( (map_finish-map_start) == 32 ) {
                                                                channel_module = aModule1;
                                                                channeltosee = clusterizationC[nnn] - 32;
                                                            }
                                                        }
                                                        
                                                        vector<unsigned int> *CHT = _Data[ channel_module ][ channeltosee ];
                                                        
                                                        long timingcluster_fill = CHT->at( 0 );
                                                        
                                                        deltatfastime = fastest_time - timingcluster_fill ;
                                                        deltastripfast = fastest_strip - clusterizationC[nnn] ;
                                                        
                                                        tree904->Fill();

                                                        cout << jentry << " --> Filling deltachannel=" << deltastripfast << " fastest = " << fastest_time << " other= " << clusterizationC[nnn] << endl;

                                                    }
                                                }
                                                */
                                                
                                                temp2 = 999999;
                                                kk = nn - 1;
                                                nn = 96;
                                                clustermultiplicity++;
                                                
                                            }
                                        }
                                    }

                                }
                                
  
                                // Fill all strip within the cluster with their time
                                for ( int nnn = 1; nnn <= 96; nnn++ ){
                                    
                                    if (clusterizationC[nnn]>=1) {

                                        if ( clusterizationC[nnn] <= 32 ) {
                                        channel_module = aModule;
                                        channeltosee = clusterizationC[nnn] - map_start;
                                    }
                                        if ( ( clusterizationC[nn] < 64 ) && (clusterizationC[nnn] > 32 ) ) {
                                        if ( (map_finish-map_start) == 96 ) {
                                            channel_module = aModule;
                                            channeltosee = clusterizationC[nnn] + 32;
                                        }
                                        if ( (map_finish-map_start) == 32 ) {
                                            channel_module = aModule1;
                                            channeltosee = clusterizationC[nnn] - 32;
                                        }
                                        if ( (map_finish-map_start) == 64 ) {
                                            channel_module = aModule;
                                            channeltosee = clusterizationC[nnn] + 64;
                                        }
                                    }
                                        if ( clusterizationC[nnn] >= 64 ) {
                                        if ( (map_finish-map_start) == 96 ) {
                                            if (map_start==32) {
                                                channeltosee = clusterizationC[nnn] + 32;
                                            }
                                            else {
                                                channeltosee = clusterizationC[nnn];
                                            }
                                            channel_module = aModule;
                                        }
                                        if ( (map_finish-map_start) == 64 ) {
                                            channel_module = aModule1;
                                            channeltosee = clusterizationC[nnn] - 64 ;
                                        }
                                        if ( (map_finish-map_start) == 32 ) {
                                            channel_module = aModule1;
                                            channeltosee = clusterizationC[nnn] - 32;
                                        }
                                    }
                                    
                                        vector<unsigned int> *CHT = _Data[ channel_module ][ channeltosee ];
                                    
                                        long timingcluster_fill = CHT->at( 0 );
                                    
                                        deltatfastime = -fastest_time + timingcluster_fill ;
                                        deltastripfast = fastest_strip - clusterizationC[nnn] ;
                                        deltatfastslowtime = fastest_time - slowest_time;
                                        tree904->Fill();

                                        //cout << jentry << " --> Filling deltachannel=" << deltastripfast << " fastest = " << fastest_strip << " other= " << clusterizationC[nnn] << endl;

                                    }
                                }

                                if (multihit) h_clustermultiplicity[i_station]->Fill( clustermultiplicity );
                                if (multihit) h_hitmultiplicity[i_station]->Fill( hitmultiplicity );
                                if ( xtalk > 0 ) h_xtalk[i_station]->Fill( xtalk );
                                //cout << "Number of CrossTalks: " << xtalk << endl;

                                //mean_csizeA = clusterA;
                                //m_ang = emme;
                                //tree904->Fill();
                            }
                        }
  
                        // Calculating cluster size, cluster&hit multiplicity for noise
                        {
                            // Clusterization A
                            for ( kk = 1; kk <= 96; kk++ ) {
                                    
                                clusterAnoise=0;
                                if (clusterizationAnoise[kk]>0) {
                                        
                                    temp1noise = clusterizationAnoise[kk];
                                    clusterAnoise++;
                                    hitmultiplicitynoise++;
                                        
                                    //cout << jentry << " Found hit at: " << clusterizationA[kk] << endl;
                                    for ( nn = kk + 1; nn <= 96; nn++ ){
                                            
                                            if (clusterizationAnoise[nn]>0) {
                                                //cout << jentry << " Found next hit at: " << clusterizationA[nn] << endl;
                                               
                                                temp2noise = clusterizationAnoise[ nn ];
                                                hitmultiplicitynoise++;
                                                if ( (temp2noise-temp1noise) == 1 ) {
                                                    clusterAnoise++;
                                                    temp1noise = temp2noise;
                                                    //cout << jentry << " Increasing csize to " << clusterA << endl;
                                                }
                                                else {
                                                    //cout << jentry << " Filling Cluster A: "  << clusterA << endl;
                                                    
                                                    h_clustersizeAnoise[i_station]->Fill( clusterAnoise );
                                                    temp2noise = 999999;
                                                    kk = nn - 1;
                                                    nn = 96;
                                                    clustermultiplicitynoise++;
                                                }
                                                
                                            }
                                            else {
                                                //cout << jentry << " Filling Cluster A: "  << clusterA << endl;
                                                
                                                h_clustersizeAnoise[i_station]->Fill( clusterAnoise );
                                                temp2noise = 999999;
                                                kk = nn - 1;
                                                nn = 96;
                                                clustermultiplicitynoise++;
                                                
                                            }
                                        }
                                }
                                    
                            }

                            // Clusterization B
                            for ( kk = 1; kk <= 96; kk++ ) {
                                    
                                    clusterBnoise=0;
                                    if (clusterizationBnoise[kk]>0) {
                                        
                                        temp1noise = clusterizationBnoise[kk];
                                        clusterBnoise++;
                                        hitmultiplicitynoise++;
                                        
                                        //cout << jentry << " Found hit at: " << clusterizationB[kk] << endl;
                                        for ( nn = kk + 1; nn <= 96; nn++ ){
                                            
                                            if (clusterizationBnoise[nn]>0) {
                                                //cout << jentry << " Found next hit at: " << clusterizationB[nn] << endl;
                                                
                                                temp2noise = clusterizationBnoise[ nn ];
                                                hitmultiplicitynoise++;
                                                if ( (temp2noise-temp1noise) == 1 ) {
                                                    clusterBnoise++;
                                                    temp1noise = temp2noise;
                                                    //cout << jentry << " Increasing csize to " << clusterB << endl;
                                                }
                                                else {
                                                    //cout << jentry << " Filling Cluster A: "  << clusterB << endl;
                                                    
                                                    h_clustersizeBnoise[i_station]->Fill( clusterBnoise );
                                                    temp2noise = 999999;
                                                    kk = nn - 1;
                                                    nn = 96;
                                                    clustermultiplicitynoise++;
                                                }
                                            }
                                            else {
                                                //cout << jentry << " Filling Cluster A: "  << clusterA << endl;
                                                
                                                h_clustersizeBnoise[i_station]->Fill( clusterBnoise );
                                                temp2noise = 999999;
                                                kk = nn - 1;
                                                nn = 96;
                                                clustermultiplicitynoise++;
                                            }
                                        }
                                    }
                                }
                            
                            // Clusterization C
                            for ( kk = 1; kk <= 96; kk++ ) {
                                
                                clusterCnoise=0;
                                if (clusterizationCnoise[kk]>0) {
                                    
                                    temp1noise = clusterizationCnoise[kk];
                                    clusterCnoise++;
                                    hitmultiplicitynoise++;
                                    
                                    //cout << jentry << " Found hit at: " << clusterizationB[kk] << endl;
                                    for ( nn = kk + 1; nn <= 96; nn++ ){
                                        
                                        if (clusterizationCnoise[nn]>0) {
                                            //cout << jentry << " Found next hit at: " << clusterizationB[nn] << endl;
                                            
                                            temp2noise = clusterizationCnoise[ nn ];
                                            hitmultiplicitynoise++;
                                            if ( (temp2noise-temp1noise) == 1 ) {
                                                clusterCnoise++;
                                                temp1noise = temp2noise;
                                                //cout << jentry << " Increasing csize to " << clusterB << endl;
                                            }
                                            else {
                                                //cout << jentry << " Filling Cluster A: "  << clusterB << endl;
                                                
                                                h_clustersizeCnoise[i_station]->Fill( clusterCnoise );
                                                temp2noise = 999999;
                                                kk = nn - 1;
                                                nn = 96;
                                                clustermultiplicitynoise++;
                                            }
                                        }
                                        else {
                                            //cout << jentry << " Filling Cluster A: "  << clusterA << endl;
                                            
                                            h_clustersizeCnoise[i_station]->Fill( clusterCnoise );
                                            temp2noise = 999999;
                                            kk = nn - 1;
                                            nn = 96;
                                            clustermultiplicitynoise++;
                                        }
                                    }
                                }
                            }
                            
                            if (multihit) h_clustermultiplicitynoise[i_station]->Fill( clustermultiplicitynoise );
                            if (multihit) h_hitmultiplicitynoise[i_station]->Fill( hitmultiplicitynoise );
 
                        }

                      
                        // Calculating strip timing
                        {
                            float check_my_time, difference;
                            for ( kk = 1; kk <= 96; kk++ ) {

                                for (int kkk = 1; kkk <= 50; kkk++ ) {
                                    
                                    //if ( (timing_strip[i_station][kk][kkk] > 0) && ( kkk >= 2 ) ) cout << " Found canale = " << kk << " hit at " << timing_strip[i_station][kk][kkk] << " Multievent = " << kkk << endl;

                                    if ( timing_strip[i_station][kk][kkk] > 0 ) {

                                        check_my_time = timing_strip[i_station][kk][kkk];
                                        if ( kkk == 1 ) fastest_striptime[i_station][kk] = timing_strip[i_station][kk][kkk];
                                        if ( check_my_time <= fastest_striptime[i_station][kk]) fastest_striptime[i_station][kk] = check_my_time;
                                        
                                       // if ( kkk >= 2 ) cout << " Found fastest canale = " << kk << " hit at " << fastest_striptime[i_station][kk] << " slow hits at " << timing_strip[i_station][kk][kkk] << " " << kkk << endl;
                                      //  if ( kkk >=2 ) cout << timing_strip[i_station][kk][1] << " " << timing_strip[i_station][kk][2] << endl;
                                    }
                                }
                            }
                         
                            // Filling histo
                            for ( kk = 1; kk <= 96; kk++ ) {
                                
                                for (int kkk = 1; kkk <= 50; kkk++ ) {
                                                                        
                                    if ( timing_strip[i_station][kk][kkk] > 0 ) {

                                        difference = timing_strip[i_station][kk][kkk] - fastest_striptime[i_station][kk];
                                        if (difference>0) h_Striptiming[i_station]->Fill( difference/5 );
                                        //if (difference>0) cout << " STAT " << i_station << " Event " << jentry << " Difference " << difference << endl;
                                    }
                                }
                                
                            }
                            
                        }


                        // Filling squared residuals
                        if ( ( nicehit == 1 ) && ( i_station == 1 ) ) h_residuals->Fill( TMath::Sqrt( temp_residual ) );

                    }

                }

            }
        }
        
        map_file.close();
        tree904->Write();
        
        // Filling strip by strip efficiency
        for ( i_station = 1; i_station <= max_station; i_station++ ) {

            for ( int canal = 0; canal <= 95; canal++ ) {
                //if ( i_station == 4) cout << " Dump canale " << canal << " " << chamber_unefficiency[i_station][canal] << endl;
                int channel_effi = int(100 * chamber_unefficiency[i_station][canal] / triggers_per_channels[i_station][canal]);
                
                for ( int kkk = 0; kkk <= channel_effi; kkk++ ) {
                    h_Stripeffi[i_station]->Fill ( canal );
                }
            }
        }
        
        //hfile->Close();
        cout << "Data Processing ended. " << endl;
    }
    
    // Writing plots for each station
    {
        for (int i_station = 1; i_station <= max_station; i_station++){
            
            //cout << " Processing station: " << i_station << endl;
            // Drawing Canvases and frames
            gStyle->SetOptStat(kFALSE);
            TCanvas *c2 = new TCanvas("c2","c2",1000,600);
            
            clustersizedistrA="";
            clustersizedistrB="";
            clustersizedistrC="";
            
            // Adjusting dynamically the vertical axes and labels
            {
                int proposed_maxbins = (h_Strip[i_station]->GetBinContent( h_Strip[i_station]->GetMaximumBin() ))*2;
                if ( proposed_maxbins > 0) maxbins = proposed_maxbins;
                
                int proposed_noisemaxbins = (h_Stripnoise[i_station]->GetBinContent( h_Stripnoise[i_station]->GetMaximumBin() ))*4;
                if ( proposed_noisemaxbins > 0) noisemaxbins = proposed_noisemaxbins;
                
                int proposed_csize_maxA = (h_clustersizeA[i_station]->GetBinContent( h_clustersizeA[i_station]->GetMaximumBin() ))*1.5;
                int proposed_csize_maxB = (h_clustersizeB[i_station]->GetBinContent( h_clustersizeB[i_station]->GetMaximumBin() ))*1.5;
                int proposed_csize_maxC = (h_clustersizeC[i_station]->GetBinContent( h_clustersizeC[i_station]->GetMaximumBin() ))*1.5;

                int proposed_csize_max;
                
                if ( (proposed_csize_maxA > proposed_csize_maxB) && (proposed_csize_maxA > proposed_csize_maxC) ) csize_max = proposed_csize_maxA;
                if ( (proposed_csize_maxB > proposed_csize_maxA) && (proposed_csize_maxB > proposed_csize_maxC) ) csize_max = proposed_csize_maxB;
                if ( (proposed_csize_maxC > proposed_csize_maxA) && (proposed_csize_maxC > proposed_csize_maxB) ) csize_max = proposed_csize_maxC;                

                chi2max = (h_chi2->GetBinContent( h_chi2->GetMaximumBin() ))*2;
                qumax = (h_qu->GetBinContent( h_qu->GetMaximumBin() ))*2;
                mmax = (h_m->GetBinContent( h_m->GetMaximumBin() ))*2;
                resmax = (h_residual[i_station]->GetBinContent( h_residual[i_station]->GetMaximumBin() ))*2;

                
                int proposed_cmulti_max = (h_clustermultiplicity[i_station]->GetBinContent( h_clustermultiplicity[i_station]->GetMaximumBin() ))*2;
                if ( proposed_cmulti_max > 0) cmulti_max = proposed_cmulti_max;
                
                int proposed_hit_max = (h_hitmultiplicity[i_station]->GetBinContent( h_hitmultiplicity[i_station]->GetMaximumBin() ))*2;
                if ( proposed_hit_max > 0) hit_max = proposed_hit_max;
                
                text_ymin = (maxbins - maxbins/3.0);
                text_ymax = maxbins;
                text_yminn = (int)TMath::Power(10, ( ( log10(noisemaxbins) ) - (log10(noisemaxbins)/3) ));
                text_ymaxn = noisemaxbins;
                text_ymincs = (csize_max - csize_max/7);
                text_ymaxcs = csize_max;
                text_ymincm = (cmulti_max - cmulti_max/7);
                text_ymaxcm = cmulti_max;
                text_yminh = (hit_max - hit_max/7);
                text_ymaxh = hit_max;

                text_ymin_res = (resmax - resmax/7);
                text_ymax_res = resmax;

                
                int sottrai = (chi2max/4);
                if ( sottrai == 0 ) sottrai=1;
                text_yminchi2 = (chi2max - sottrai);
                text_ymaxchi2 = chi2max;
                
                sottrai = (mmax/4);
                if ( sottrai == 0 ) sottrai=1;
                text_yminm = (mmax - sottrai);
                text_ymaxm = mmax;

                sottrai = (qumax/4);
                if ( sottrai == 0 ) sottrai=1;
                text_yminqu = (qumax - sottrai);
                text_ymaxqu = qumax;

            }
            
            // Declaring the frames
            TH2F *frame = new  TH2F("frame", "", strip_bin, strip_start, strip_finish, maxbins, 0, maxbins);
            TH2F *frametiming = new  TH2F("frametiming", "", 1000, -100, 1000, 1000, 0, 80);
            TH2F *frame_effihits = new  TH2F("frame_effihits", "", strip_bin, strip_start, strip_finish, 1200, 0, 120);
            TH2F *framecluster = new  TH2F("framecluster", "", 11, 0, 11, csize_max, 0, csize_max);
            TH2F *frameclusternoise = new  TH2F("frameclusternoise", "", 32, 0, 32, csize_max, 0, csize_max);
            TH2F *framehitmulti = new  TH2F("framehitmulti", "", 96, 0, 96, hit_max, 0, hit_max);
            TH2F *frameclustermulti = new  TH2F("frameclustermulti", "", 96, 0, 96, cmulti_max, 0, cmulti_max);
            TH2F *framenoise = new  TH2F("framenoise", "", strip_bin, strip_start, strip_finish, noisemaxbins, 0, noisemaxbins);
            TH2F *framext = new  TH2F("framext", "", framext_bin, framext_start, framext_finish, xtbins, 0, xtmax);
            TH2F *framechi2 = new  TH2F("framechi2", "", 100, 0, 20, chi2max, 0, chi2max);
            TH2F *framem = new  TH2F("framem", "", 100, -0.2, 0.2, mmax, 0, mmax);
            TH2F *frameq = new  TH2F("frameq", "", 96, 0, 96, qumax, 0, qumax);
            TH2F *frameres = new  TH2F("frameres", "", 300, -16, 16, resmax, 0, resmax);
            TH2F *frameresiduals = new  TH2F("frameresiduals", "", 300, 0, 16, resmax, 0, resmax);
            
            //cout << " Frame initialized. " << endl;
            
            // STRIP & NOISE PROFILES
            {
                // Forming a nice histogram title for strip profile
                titlestring = "Strips Chamber ID: ";
                titlestring = titlestring + name_set_tn[i_station];
                titlestring = titlestring + " at station: ";
                stringstream sstm4;
                sstm4 << titlestring << i_station;
                titlestring = sstm4.str();
                titlestring = titlestring + " TRG: ";
                titlestring = titlestring + Trigger_layer;
                titlestring = titlestring + " (eta=";
                titlestring = titlestring + etapartition;
                titlestring = titlestring + ")";
                strcpy( htitle1, titlestring.c_str() );
                
                
                // Drawing frame
                frame->SetTitle(htitle1);
                frame->GetXaxis()->SetTitleOffset(1.1);
                frame->GetXaxis()->SetLabelSize(0.025);
                frame->GetXaxis()->SetTitle("Strip channels");
                TAxis *axis = frame->GetXaxis();
                axis->SetNdivisions(6,16,1,kFALSE);
                axis->SetDecimals(kFALSE);
                frame->GetYaxis()->SetTitleOffset(1.1);
                frame->GetYaxis()->SetLabelSize(0.025);
                frame->GetYaxis()->SetTitle("total counts / channel");
                frame->Draw();
                
                
                
                // Adding environmental conditions | Adding FEB values
                snprintf(infovalue, sizeof(infovalue), "T= %4.1f #circC,   RH= %2.0f,   P= %3.0fmbar", temperature, humidity, pressure);
                snprintf(infovalue0, sizeof(infovalue0), "THR: %i %i %i %i | %i %i %i %i | %i %i %i %i", FEBS[1][i_station],FEBS[2][i_station],FEBS[3][i_station],FEBS[4][i_station], FEBS[5][i_station],FEBS[6][i_station],FEBS[7][i_station],FEBS[8][i_station], FEBS[9][i_station],FEBS[10][i_station],FEBS[11][i_station],FEBS[12][i_station]);
                
                
                // Adding histrogram info for strip profile
                if ( (etapartition == "ALL") && ( largeacceptance == 0 ) ) { 
                    int entriesA = h_Strip[i_station]->Integral(0,32);
                    int entriesB = h_Strip[i_station]->Integral(32,64);
                    int entriesC = h_Strip[i_station]->Integral(64,96);
                    snprintf(infovalue1, sizeof(infovalue1), "EntriesA: %i (%i), EntriesB: %i (%i), EntriesC: %i (%i)", entriesA,counterA,entriesB, counterB, entriesC , counterC);
                    snprintf(infovalue11, sizeof(infovalue11), "Total Entries: %i, Triggered events: %i (%i)", entriesA + entriesB +entriesC, counterA + counterB + counterC, nentries);
                }
                else {
                    int entriesAA = h_Strip[i_station]->Integral(0,32);
                    int entriesBB = h_Strip[i_station]->Integral(32,64);
                    int entriesCC = h_Strip[i_station]->Integral(64,96);
                    int counterAA = 0, counterBB = 0, counterCC = 0;
                    if (etapartition == "A") counterAA = counter;
                    if (etapartition == "B") counterBB = counter;
                    if (etapartition == "C") counterCC = counter;
                    snprintf(infovalue1, sizeof(infovalue1), "EntriesA: %i (%i), EntriesB: %i (%i), EntriesC: %i (%i)", entriesAA,counterAA,entriesBB, counterBB, entriesCC , counterCC);
                    snprintf(infovalue11, sizeof(infovalue11), "Total Entries: %i, Triggered events: %i (%i)", entriesAA + entriesBB +entriesCC, counter, nentries);
                }
                
                
                
                // Adding histrogram info for noise profile
                if ( (etapartition == "ALL") && ( largeacceptance == 0 ) ) {
                    int entriesA = h_Stripnoise[i_station]->Integral(0,32);
                    int entriesB = h_Stripnoise[i_station]->Integral(32,64);
                    int entriesC = h_Stripnoise[i_station]->Integral(64,96);
                    snprintf(infovalue12, sizeof(infovalue12), "EntriesA: %i (%i), EntriesB: %i (%i), EntriesC: %i (%i)", entriesA,counterA,entriesB, counterB, entriesC , counterC);
                    snprintf(infovalue13, sizeof(infovalue13), "Total Entries: %i, Triggered events: %i (%i)", entriesA + entriesB +entriesC, counterA + counterB + counterC, nentries);
                }
                else {
                    int entriesAA = h_Stripnoise[i_station]->Integral(0,32);
                    int entriesBB = h_Stripnoise[i_station]->Integral(32,64);
                    int entriesCC = h_Stripnoise[i_station]->Integral(64,96);
                    int counterAA = 0, counterBB = 0, counterCC = 0;
                    if (etapartition == "A") counterAA = counter;
                    if (etapartition == "B") counterBB = counter;
                    if (etapartition == "C") counterCC = counter;
                    snprintf(infovalue12, sizeof(infovalue12), "EntriesA: %i (%i), EntriesB: %i (%i), EntriesC: %i (%i)", entriesAA,counterAA,entriesBB, counterBB, entriesCC , counterCC);
                    snprintf(infovalue13, sizeof(infovalue13), "Total Entries: %i, Triggered events: %i (%i)", entriesAA + entriesBB +entriesCC, counter, nentries);
                }
                
                
                
                
                // Adding V /uA conditions
                snprintf(infovalue2, sizeof(infovalue2), "TN: HV= %5.0f V,    I= %4.2f #mu A", hv_set_tn[i_station], i_set_tn[i_station]);
                snprintf(infovalue3, sizeof(infovalue3), "TW: HV= %5.0f V,    I= %4.2f #mu A", hv_set_tn[i_station], i_set_tn[i_station]);
                snprintf(infovalue4, sizeof(infovalue4), " B: HV= %5.0f V,    I= %4.2f #mu A", hv_set_tn[i_station], i_set_tn[i_station]);
                
                // Additional calculations
                chamber_clustersizeA = h_clustersizeA[i_station]->GetMean();
                chamber_clustersizeB = h_clustersizeB[i_station]->GetMean();
                chamber_clustersizeC = h_clustersizeC[i_station]->GetMean();

                int noiseratetempA = noiserateA[i_station];
                int noiseratetempB = noiserateB[i_station];
                int noiseratetempC = noiserateC[i_station];
                
                if ( kind[i_station] == 1 ) {
                    surfaceA = 4 * 55 * 32;
                    surfaceB = 3.5 * 77 * 32;
                    surfaceC = 3 * 66 * 32;

                }
                else {
                    surfaceA = 2.75 * 62.0 * 32;
                    surfaceB = 2.5 * 55.0 * 32;
                    surfaceC = 1.8 * 49.0 * 32;
                }
                
                noiserateA[i_station] = (noiserateA[i_station] / (nentries * timewindow ) / surfaceA ) ;
                err_noiseA = ( sqrt(noiseratetempA) / (nentries * timewindow ) / surfaceA ) ;

                noiserateB[i_station] = (noiserateB[i_station] / (nentries * timewindow ) / surfaceB ) ;
                err_noiseB = ( sqrt(noiseratetempB) / (nentries * timewindow ) / surfaceB ) ;

                noiserateC[i_station] = (noiserateC[i_station] / (nentries * timewindow ) / surfaceC ) ;
                err_noiseC = ( sqrt(noiseratetempC) / (nentries * timewindow ) / surfaceC ) ;

                if (  (etapartition == "ALL") && ( largeacceptance == 0 ) ) {
                    if ( trackingreconstruction == 0 ) {
                        chamber_efficiencyA[i_station] = 100* chamber_efficiencyA[i_station] / counterA ;
                        chamber_efficiencyB[i_station] = 100* chamber_efficiencyB[i_station] / counterB ;
                        chamber_efficiencyC[i_station] = 100* chamber_efficiencyC[i_station] / counterC ;
                    }
                    else if ( trackingreconstruction == 1 ) {
                        chamber_efficiencyA[i_station] = 100* chamber_efficiencyA[i_station] / tracked_sector_isA ;
                        chamber_efficiencyB[i_station] = 100* chamber_efficiencyB[i_station] / tracked_sector_isB ;
                        chamber_efficiencyC[i_station] = 100* chamber_efficiencyC[i_station] / tracked_sector_isC ;
                    }
                    
                    
                    err_effA = 100 * sqrt (( chamber_efficiencyA[i_station]/100 * (1-chamber_efficiencyA[i_station]/100)) / counterA  );
                    err_effB = 100 * sqrt (( chamber_efficiencyB[i_station]/100 * (1-chamber_efficiencyB[i_station]/100)) / counterB  );
                    err_effC = 100 * sqrt (( chamber_efficiencyC[i_station]/100 * (1-chamber_efficiencyC[i_station]/100)) / counterC  );
                    snprintf(infovalue5, sizeof(infovalue5), "#eta_{A} = %5.2f#pm%5.2f %, #eta_{B} = %5.2f#pm%5.2f, #eta_{C} = %5.2f#pm%5.2f", chamber_efficiencyA[i_station], err_effA, chamber_efficiencyB[i_station], err_effB, chamber_efficiencyC[i_station], err_effC );
                    snprintf(infovalue55, sizeof(infovalue55), "c.s._{A} = %4.2f, c.s._{B} = %4.2f, c.s._{C} = %4.2f, noise = %4.2f#pm%4.2f %4.2f#pm%4.2f %4.2f#pm%4.2f Hz/cm2", chamber_clustersizeA, chamber_clustersizeB, chamber_clustersizeC, noiserateA[i_station],err_noiseA,noiserateB[i_station],err_noiseB,noiserateC[i_station],err_noiseC);
                }
                else {
                    chamber_efficiency[i_station] = 100* chamber_efficiency[i_station] / ( counter-counterout[i_station] ) ;
                    chamber_clustersize = ( chamber_clustersizeA + chamber_clustersizeB + chamber_clustersizeC ) / 3;
                    noiserate[i_station] = ( noiserateA[i_station] + noiserateB[i_station] + noiserateC[i_station] ) / 3;
                    if (chamber_efficiency[i_station] == 100 ) chamber_efficiency[i_station] = 99.5;
                    err_eff = 100 * sqrt (( chamber_efficiency[i_station]/100 * (1-chamber_efficiency[i_station]/100)) / ( counter-counterout[i_station] )  );
                    snprintf(infovalue5, sizeof(infovalue5), "#eta = %5.2f#pm%5.2f %, c.s. = %4.2f, noise = %7.5f#pm%7.5f Hz/cm2", chamber_efficiency[i_station], err_eff, chamber_clustersize, noiserate[i_station],err_noise );
                }
                
                
                // Adding text to the plot for strip profile
                TPaveText *pt2 = new TPaveText(text_xmin, text_ymin, text_xmax, text_ymax);
                pt2->AddText(infovalue1);
                pt2->AddText(infovalue11);
                pt2->AddText(infovalue2);
                pt2->AddText(infovalue3);
                pt2->AddText(infovalue4);
                pt2->AddText(infovalue5);
                if ( (etapartition == "ALL") && ( largeacceptance==0) ) pt2->AddText(infovalue55);
                pt2->AddText(infovalue);
                pt2->AddText(infovalue0);
                
                
                // Adding text to the plot for noise profile
                TPaveText *pt5 = new TPaveText(text_xminn, text_yminn, text_xmaxn, text_ymaxn);
                pt5->AddText(infovalue12);
                pt5->AddText(infovalue13);
                pt5->AddText(infovalue2);
                pt5->AddText(infovalue3);
                pt5->AddText(infovalue4);
                pt5->AddText(infovalue5);
                if (etapartition == "ALL") pt5->AddText(infovalue55);
                pt5->AddText(infovalue);
                pt5->AddText(infovalue0);
                
                
                // Drawing strip profiles
                h_Strip[i_station]->Draw("same");
                pt2->Draw();
                
                // Saving plots on files
                snprintf(hname, sizeof(hname), "/srv/www/htdocs/dqm/plot/%istripprofile%i.gif", run, i_station);
                //snprintf(hname, sizeof(hname), "%istripprofile%i.gif", run, i_station);
                gPad->Update();
                gPad->Print(hname);
                
                
                // NOISE PROFILE--------------------------------------------------------------
                
                // Drawing frame noise
                framenoise->GetXaxis()->SetTitleOffset(1.1);
                framenoise->GetXaxis()->SetLabelSize(0.025);
                framenoise->GetXaxis()->SetTitle("Strip channels");
                TAxis *axis2 = framenoise->GetXaxis();
                axis2->SetNdivisions(6,16,1,kFALSE);
                axis2->SetDecimals(kFALSE);
                framenoise->GetYaxis()->SetTitleOffset(1.1);
                framenoise->GetYaxis()->SetLabelSize(0.025);
                framenoise->GetYaxis()->SetTitle("total counts / channel");
                
                
                // Forming a nice histogram title for noise profile
                titlestringnoise = "Noisy strips Chamber ID: ";
                titlestringnoise = titlestringnoise + name_set_tn[i_station];
                titlestringnoise = titlestringnoise + " at station: ";
                stringstream sstm3;
                sstm3 << titlestringnoise << i_station;
                titlestringnoise = sstm3.str();
                titlestringnoise = titlestringnoise + " TRG: ";
                titlestringnoise = titlestringnoise + Trigger_layer;
                titlestringnoise = titlestringnoise + " (eta=";
                titlestringnoise = titlestringnoise + etapartition;
                titlestringnoise = titlestringnoise + ")";
                strcpy( htitle2, titlestringnoise.c_str() );
                
                // Drawing noisy strip profiles
                c2->SetLogy(1);
                framenoise->SetTitle(htitle2);
                framenoise->Draw();
                h_Stripnoise[i_station]->Draw("same");
                pt5->Draw();
                
                
                // Saving plots on files
                snprintf(hname, sizeof(hname), "/srv/www/htdocs/dqm/plot/%inoisestripprofile%i.gif", run, i_station);
                gPad->Update();
                gPad->Print(hname);
                c2->SetLogy(0);
                //cout << " Strip/noise profiles initialized. " << endl;
            }
            
            
            // Strip timings
            {
                // Drawing Canvases, frames and saving plots
                TCanvas *canvasforstriptiming = new TCanvas("canvasforstriptiming","canvasforstriptiming",1000,600);
                frametiming->SetTitle("Strip timings");
                frametiming->GetXaxis()->SetTitleOffset(1.1);
                frametiming->GetXaxis()->SetLabelSize(0.025);
                frametiming->GetXaxis()->SetTitle("Timing");
                frametiming->GetYaxis()->SetTitleOffset(1.1);
                frametiming->GetYaxis()->SetLabelSize(0.025);
                frametiming->GetYaxis()->SetTitle("Counts");
                frametiming->GetYaxis()->SetDecimals();
                frametiming->SetTitle(htitle1);
                frametiming->Draw();
                h_Striptiming[i_station]->Draw("same");
                
                snprintf(hname, sizeof(hname), "/srv/www/htdocs/dqm/plot/%istripprofile_timing%i.gif", run, i_station);
                gPad->Update();
                gPad->Print(hname);
                
            }
            
            
            // Efficient and un-efficient hits
            {
                // Drawing Canvases, frames and saving plots
                TCanvas *canvasforstripeffi = new TCanvas("canvasforstripeffi","canvasforstripeffi",1000,600);
                frame_effihits->SetTitle("Efficiency strips");
                frame_effihits->GetXaxis()->SetTitleOffset(1.1);
                frame_effihits->GetXaxis()->SetLabelSize(0.025);
                frame_effihits->GetXaxis()->SetTitle("Efficiency strips");
                frame_effihits->GetYaxis()->SetTitleOffset(1.1);
                frame_effihits->GetYaxis()->SetLabelSize(0.025);
                frame_effihits->GetYaxis()->SetTitle("Efficiency");
                frame_effihits->GetYaxis()->SetDecimals();
                frame_effihits->SetTitle(htitle1);
                frame_effihits->Draw();
                h_Stripeffi[i_station]->Draw("same");
                
                snprintf(hname, sizeof(hname), "/srv/www/htdocs/dqm/plot/%istripprofile_effi%i.gif", run, i_station);
                gPad->Update();
                gPad->Print(hname);
                
            }
            
            // CLUSTER SIZE
            {
                
                // Forming a nice histogram title for strip profile
                titlestring = "Cluster size Chamber ID: ";
                titlestring = titlestring + name_set_tn[i_station];
                titlestring = titlestring + " at station: ";
                stringstream sstm34;
                sstm34 << titlestring << i_station;
                titlestring = sstm34.str();
                titlestring = titlestring + " TRG: ";
                titlestring = titlestring + Trigger_layer;
                titlestring = titlestring + " (eta=A)";
                strcpy( htitle3, titlestring.c_str() );
                
                
                // Preparing text for cluster size legend
                int entriescsA = h_clustersizeA[i_station]->GetEntries();
                float meanclusterA = h_clustersizeA[i_station]->GetMean();
                snprintf(infovalue98, sizeof(infovalue98), "Entries: %i", entriescsA);
                snprintf(infovalue99, sizeof(infovalue99), "Mean value: %4.2f", meanclusterA);
                int multicsizeA = h_clustersizeA[i_station]->GetBinContent( h_clustersizeA[i_station]->GetXaxis()->GetNbins()+1 );
                snprintf(infovalue97, sizeof(infovalue97), "Overflow: %i", multicsizeA );
                TPaveText *pt3 = new TPaveText(text_xmincs,text_ymincs,text_xmaxcs,text_ymaxcs);
                pt3->AddText(infovalue98);
                pt3->AddText(infovalue99);
                pt3->AddText(infovalue97);
                
                
                // Drawing frame for cluster size
                framecluster->SetTitle("Cluster size");
                framecluster->GetXaxis()->SetTitleOffset(1.1);
                framecluster->GetXaxis()->SetLabelSize(0.025);
                framecluster->GetXaxis()->SetTitle("Cluster size");
                framecluster->GetYaxis()->SetTitleOffset(1.1);
                framecluster->GetYaxis()->SetLabelSize(0.025);
                framecluster->GetYaxis()->SetTitle("counts");
                framecluster->SetTitle(htitle3);
                framecluster->Draw();
                h_clustersizeA[i_station]->Draw("same");
                pt3->Draw();
                
                // Saving plots on files
                snprintf(hname, sizeof(hname), "/srv/www/htdocs/dqm/plot/%iclustersizeA%i.gif", run, i_station);
                gPad->Update();
                gPad->Print(hname);
                //cout << " Cluster size initialized. " << endl;
            }
            {
                
                // Forming a nice histogram title for strip profile
                titlestring = "Cluster size Chamber ID: ";
                titlestring = titlestring + name_set_tn[i_station];
                titlestring = titlestring + " at station: ";
                stringstream sstm34;
                sstm34 << titlestring << i_station;
                titlestring = sstm34.str();
                titlestring = titlestring + " TRG: ";
                titlestring = titlestring + Trigger_layer;
                titlestring = titlestring + " (eta=B)";
                strcpy( htitle3, titlestring.c_str() );
                
                
                // Preparing text for cluster size legend
                int entriescsB = h_clustersizeB[i_station]->GetEntries();
                float meanclusterB = h_clustersizeB[i_station]->GetMean();
                snprintf(infovalue98, sizeof(infovalue98), "Entries: %i", entriescsB);
                snprintf(infovalue99, sizeof(infovalue99), "Mean value: %4.2f", meanclusterB);
                int multicsizeB = h_clustersizeB[i_station]->GetBinContent( h_clustersizeB[i_station]->GetXaxis()->GetNbins()+1 );
                snprintf(infovalue97, sizeof(infovalue97), "Overflow: %i", multicsizeB );
                TPaveText *pt3 = new TPaveText(text_xmincs,text_ymincs,text_xmaxcs,text_ymaxcs);
                pt3->AddText(infovalue98);
                pt3->AddText(infovalue99);
                pt3->AddText(infovalue97);
                
                
                // Drawing frame for cluster size
                framecluster->SetTitle("Cluster size");
                framecluster->GetXaxis()->SetTitleOffset(1.1);
                framecluster->GetXaxis()->SetLabelSize(0.025);
                framecluster->GetXaxis()->SetTitle("Cluster size");
                framecluster->GetYaxis()->SetTitleOffset(1.1);
                framecluster->GetYaxis()->SetLabelSize(0.025);
                framecluster->GetYaxis()->SetTitle("counts");
                framecluster->SetTitle(htitle3);
                framecluster->Draw();
                h_clustersizeB[i_station]->Draw("same");
                pt3->Draw();
                
                // Saving plots on files
                snprintf(hname, sizeof(hname), "/srv/www/htdocs/dqm/plot/%iclustersizeB%i.gif", run, i_station);
                gPad->Update();
                gPad->Print(hname);
                //cout << " Cluster size initialized. " << endl;
            }
            {
                
                // Forming a nice histogram title for strip profile
                titlestring = "Cluster size Chamber ID: ";
                titlestring = titlestring + name_set_tn[i_station];
                titlestring = titlestring + " at station: ";
                stringstream sstm34;
                sstm34 << titlestring << i_station;
                titlestring = sstm34.str();
                titlestring = titlestring + " TRG: ";
                titlestring = titlestring + Trigger_layer;
                titlestring = titlestring + " (eta=C)";
                strcpy( htitle3, titlestring.c_str() );
                
                
                // Preparing text for cluster size legend
                int entriescsC = h_clustersizeC[i_station]->GetEntries();
                float meanclusterC = h_clustersizeC[i_station]->GetMean();
                snprintf(infovalue98, sizeof(infovalue98), "Entries: %i", entriescsC);
                snprintf(infovalue99, sizeof(infovalue99), "Mean value: %4.2f", meanclusterC);
                int multicsizeC = h_clustersizeA[i_station]->GetBinContent( h_clustersizeC[i_station]->GetXaxis()->GetNbins()+1 );
                snprintf(infovalue97, sizeof(infovalue97), "Overflow: %i", multicsizeC );
                TPaveText *pt3 = new TPaveText(text_xmincs,text_ymincs,text_xmaxcs,text_ymaxcs);
                pt3->AddText(infovalue98);
                pt3->AddText(infovalue99);
                pt3->AddText(infovalue97);
                
                
                // Drawing frame for cluster size
                framecluster->SetTitle("Cluster size");
                framecluster->GetXaxis()->SetTitleOffset(1.1);
                framecluster->GetXaxis()->SetLabelSize(0.025);
                framecluster->GetXaxis()->SetTitle("Cluster size");
                framecluster->GetYaxis()->SetTitleOffset(1.1);
                framecluster->GetYaxis()->SetLabelSize(0.025);
                framecluster->GetYaxis()->SetTitle("counts");
                framecluster->SetTitle(htitle3);
                framecluster->Draw();
                h_clustersizeC[i_station]->Draw("same");
                pt3->Draw();
                
                // Saving plots on files
                snprintf(hname, sizeof(hname), "/srv/www/htdocs/dqm/plot/%iclustersizeC%i.gif", run, i_station);
                gPad->Update();
                gPad->Print(hname);
                //cout << " Cluster size initialized. " << endl;
            }

            // CLUSTER SIZE (noise)
            {
                // Drawing frame for cluster size
                frameclusternoise->SetTitle("Cluster size");
                frameclusternoise->GetXaxis()->SetTitleOffset(1.1);
                frameclusternoise->GetXaxis()->SetLabelSize(0.025);
                frameclusternoise->GetXaxis()->SetTitle("Cluster size");
                frameclusternoise->GetYaxis()->SetTitleOffset(1.1);
                frameclusternoise->GetYaxis()->SetLabelSize(0.025);
                frameclusternoise->GetYaxis()->SetTitle("counts");
                frameclusternoise->SetTitle(htitle3);
                frameclusternoise->Draw();
                h_clustersizeAnoise[i_station]->Draw("same");
                
                // Saving plots on files
                snprintf(hname, sizeof(hname), "/srv/www/htdocs/dqm/plot/%iclustersizeA%inoise.gif", run, i_station);
                gPad->Update();
                gPad->Print(hname);
            }
            {
                // Drawing frame for cluster size
                frameclusternoise->SetTitle("Cluster size");
                frameclusternoise->GetXaxis()->SetTitleOffset(1.1);
                frameclusternoise->GetXaxis()->SetLabelSize(0.025);
                frameclusternoise->GetXaxis()->SetTitle("Cluster size");
                frameclusternoise->GetYaxis()->SetTitleOffset(1.1);
                frameclusternoise->GetYaxis()->SetLabelSize(0.025);
                frameclusternoise->GetYaxis()->SetTitle("counts");
                frameclusternoise->SetTitle(htitle3);
                frameclusternoise->Draw();
                h_clustersizeBnoise[i_station]->Draw("same");
                
                // Saving plots on files
                snprintf(hname, sizeof(hname), "/srv/www/htdocs/dqm/plot/%iclustersizeB%inoise.gif", run, i_station);
                gPad->Update();
                gPad->Print(hname);
            }
            {
                // Drawing frame for cluster size
                frameclusternoise->SetTitle("Cluster size");
                frameclusternoise->GetXaxis()->SetTitleOffset(1.1);
                frameclusternoise->GetXaxis()->SetLabelSize(0.025);
                frameclusternoise->GetXaxis()->SetTitle("Cluster size");
                frameclusternoise->GetYaxis()->SetTitleOffset(1.1);
                frameclusternoise->GetYaxis()->SetLabelSize(0.025);
                frameclusternoise->GetYaxis()->SetTitle("counts");
                frameclusternoise->SetTitle(htitle3);
                frameclusternoise->Draw();
                h_clustersizeCnoise[i_station]->Draw("same");
                
                // Saving plots on files
                snprintf(hname, sizeof(hname), "/srv/www/htdocs/dqm/plot/%iclustersizeC%inoise.gif", run, i_station);
                gPad->Update();
                gPad->Print(hname);
            }
            
            
            
            // CLUSTER MULTIPLICITY
            {
                // Forming a nice histogram title for strip profile
                titlestring = "Cluster multiplicity Chamber ID: ";
                titlestring = titlestring + name_set_tn[i_station];
                titlestring = titlestring + " at station: ";
                stringstream sstm35;
                sstm35 << titlestring << i_station;
                titlestring = sstm35.str();
                titlestring = titlestring + " TRG: ";
                titlestring = titlestring + Trigger_layer;
                titlestring = titlestring + " (eta=";
                titlestring = titlestring + etapartition;
                titlestring = titlestring + ")";
                strcpy( htitle4, titlestring.c_str() );
                
                
                // Preparing text for cluster size legend
                int entriescs = h_clustermultiplicity[i_station]->GetEntries();
                float meanclustermulti = h_clustermultiplicity[i_station]->GetMean();
                snprintf(infovalue98, sizeof(infovalue98), "Entries: %i", entriescs);
                snprintf(infovalue99, sizeof(infovalue99), "Mean value: %4.2f", meanclustermulti);
                int multioverflow = h_clustermultiplicity[i_station]->GetBinContent( h_clustersizeA[i_station]->GetXaxis()->GetNbins()+1 );
                snprintf(infovalue97, sizeof(infovalue97), "Overflow: %i", multioverflow );
                TPaveText *pt4 = new TPaveText(text_xmincm,text_ymincm,text_xmaxcm,text_ymaxcm);
                pt4->AddText(infovalue98);
                pt4->AddText(infovalue99);
                pt4->AddText(infovalue97);
                
                
                // Drawing frame for cluster multiplicity
                frameclustermulti->SetTitle("Number of Clusters");
                frameclustermulti->GetXaxis()->SetTitleOffset(1.1);
                frameclustermulti->GetXaxis()->SetLabelSize(0.025);
                frameclustermulti->GetXaxis()->SetTitle("Number of Clusters");
                frameclustermulti->GetYaxis()->SetTitleOffset(1.1);
                frameclustermulti->GetYaxis()->SetLabelSize(0.025);
                frameclustermulti->GetYaxis()->SetTitle("Counts");
                frameclustermulti->SetTitle(htitle4);
                frameclustermulti->Draw();
                h_clustermultiplicity[i_station]->Draw("same");
                pt4->Draw();
                
                // Saving plots on files
                snprintf(hname, sizeof(hname), "/srv/www/htdocs/dqm/plot/%iclustermultiplicity%i.gif", run, i_station);
                gPad->Update();
                gPad->Print(hname);
                //cout << " Cluster mult. initialized. " << endl;
            }

            // CLUSTER MULTIPLICITY (noise)
            {
            // Drawing frame for cluster multiplicity
            frameclustermulti->SetTitle("Number of Clusters");
            frameclustermulti->GetXaxis()->SetTitleOffset(1.1);
            frameclustermulti->GetXaxis()->SetLabelSize(0.025);
            frameclustermulti->GetXaxis()->SetTitle("Number of Clusters");
            frameclustermulti->GetYaxis()->SetTitleOffset(1.1);
            frameclustermulti->GetYaxis()->SetLabelSize(0.025);
            frameclustermulti->GetYaxis()->SetTitle("Counts");
            frameclustermulti->SetTitle(htitle4);
            frameclustermulti->Draw();
            h_clustermultiplicity[i_station]->Draw("same");
            
            // Saving plots on files
            snprintf(hname, sizeof(hname), "/srv/www/htdocs/dqm/plot/%iclustermultiplicity%inoise.gif", run, i_station);
            gPad->Update();
            gPad->Print(hname);
            //cout << " Cluster mult. initialized. " << endl;
            }

            
            // HIT MULTIPLICITY
            {
                // Forming a nice histogram title for strip profile
                titlestring = "Hit multiplicity Chamber ID: ";
                titlestring = titlestring + name_set_tn[i_station];
                titlestring = titlestring + " at station: ";
                stringstream sstm36;
                sstm36 << titlestring << i_station;
                titlestring = sstm36.str();
                titlestring = titlestring + " TRG: ";
                titlestring = titlestring + Trigger_layer;
                titlestring = titlestring + " (eta=";
                titlestring = titlestring + etapartition;
                titlestring = titlestring + ")";
                strcpy( htitle5, titlestring.c_str() );
                
                
                // Preparing text for cluster size legend
                int entriescs = h_hitmultiplicity[i_station]->GetEntries();
                float meanhitmulti = h_hitmultiplicity[i_station]->GetMean();
                snprintf(infovalue91, sizeof(infovalue91), "Entries: %i", entriescs);
                snprintf(infovalue92, sizeof(infovalue92), "Mean value: %4.2f", meanhitmulti);
                int multihitoverflow = h_hitmultiplicity[i_station]->GetBinContent( h_hitmultiplicity[i_station]->GetXaxis()->GetNbins()+1 );
                snprintf(infovalue93, sizeof(infovalue93), "Overflow: %i", multihitoverflow );
                TPaveText *pt7 = new TPaveText(text_xminh,text_yminh,text_xmaxh,text_ymaxh);
                pt7->AddText(infovalue91);
                pt7->AddText(infovalue92);
                pt7->AddText(infovalue93);
                
                
                // Drawing frame for hit multiplicity
                framehitmulti->SetTitle("Number of hits");
                framehitmulti->GetXaxis()->SetTitleOffset(1.1);
                framehitmulti->GetXaxis()->SetLabelSize(0.025);
                framehitmulti->GetXaxis()->SetTitle("Hits multiplicity");
                framehitmulti->GetYaxis()->SetTitleOffset(1.1);
                framehitmulti->GetYaxis()->SetLabelSize(0.025);
                framehitmulti->GetYaxis()->SetTitle("Counts");
                framehitmulti->SetTitle(htitle5);
                framehitmulti->Draw();
                h_hitmultiplicity[i_station]->Draw("same");
                pt7->Draw();
                
                // Saving plots on files
                snprintf(hname, sizeof(hname), "/srv/www/htdocs/dqm/plot/%ihitmultiplicity%i.gif", run, i_station);
                gPad->Update();
                gPad->Print(hname);
                //cout << " Hit mult. initialized. " << endl;
            }

            // HIT MULTIPLICITY (noise)
            {
                // Drawing frame for hit multiplicity
                framehitmulti->SetTitle("Number of hits");
                framehitmulti->GetXaxis()->SetTitleOffset(1.1);
                framehitmulti->GetXaxis()->SetLabelSize(0.025);
                framehitmulti->GetXaxis()->SetTitle("Hits multiplicity");
                framehitmulti->GetYaxis()->SetTitleOffset(1.1);
                framehitmulti->GetYaxis()->SetLabelSize(0.025);
                framehitmulti->GetYaxis()->SetTitle("Counts");
                framehitmulti->SetTitle(htitle5);
                framehitmulti->Draw();
                h_hitmultiplicity[i_station]->Draw("same");
                
                // Saving plots on files
                snprintf(hname, sizeof(hname), "/srv/www/htdocs/dqm/plot/%ihitmultiplicity%inoise.gif", run, i_station);
                gPad->Update();
                gPad->Print(hname);
                //cout << " Hit mult. initialized. " << endl;
            }
            
            // CROSSTALK
            {
                // Forming a nice histogram title for xtalk profile
                titlestring = "CrossTalk Chamber ID: ";
                titlestring = titlestring + name_set_tn[i_station];
                titlestring = titlestring + " at station: ";
                stringstream sstm39;
                sstm39 << titlestring << i_station;
                titlestring = sstm39.str();
                titlestring = titlestring + " TRG: ";
                titlestring = titlestring + Trigger_layer;
                titlestring = titlestring + " (eta=";
                titlestring = titlestring + etapartition;
                titlestring = titlestring + ")";
                strcpy( htitle6, titlestring.c_str() );
                
                // Preparing text for xtalk size legend
                int entriesxt = h_xtalk[i_station]->GetEntries();
                float meanxt = h_xtalk[i_station]->GetMean();
                
                snprintf(infovalue51, sizeof(infovalue51), "Entries: %i", entriesxt);
                float percentagextalk;
                int counterABC = counterA + counterB + counterC;
                if ( (etapartition == "ALL") && (largeacceptance==0) && (counterABC > 0) ) percentagextalk = ( 100 * (entriesxt) / ( counterABC ) );
                if ( (etapartition == "ALL") && (largeacceptance==1) && (counter > 0) ) percentagextalk = ( 100 * (entriesxt) / ( counter ) );
                if ( (etapartition != "ALL") && (counter > 0) ) percentagextalk = ( 100 * (entriesxt) / ( counter ) );
                
                snprintf(infovalue54, sizeof(infovalue54), "CrossTalk [%]: %5.3f", percentagextalk );
                snprintf(infovalue52, sizeof(infovalue52), "Mean value: %4.2f", meanxt);
                int overflowxt = h_xtalk[i_station]->GetBinContent( h_xtalk[i_station]->GetXaxis()->GetNbins()+1 );
                snprintf(infovalue53, sizeof(infovalue53), "Overflow: %i", overflowxt );
                
                TPaveText *pt8 = new TPaveText(text_xminxt,text_yminxt,text_xmaxxt,text_ymaxxt);
                pt8->AddText(infovalue51);
                pt8->AddText(infovalue54);
                pt8->AddText(infovalue52);
                pt8->AddText(infovalue53);
                
                // Drawing frame for xtalk
                framext->SetTitle("CrossTalk");
                framext->GetXaxis()->SetTitleOffset(1.1);
                framext->GetXaxis()->SetLabelSize(0.025);
                framext->GetXaxis()->SetTitle("Hits in neighbour partitions [strips]");
                framext->GetYaxis()->SetTitleOffset(1.1);
                framext->GetYaxis()->SetLabelSize(0.025);
                framext->GetYaxis()->SetTitle("Counts");
                framext->SetTitle(htitle6);
                framext->Draw();
                h_xtalk[i_station]->Draw("same");
                pt8->Draw();
                // Saving plots on files
                snprintf(hname, sizeof(hname), "/srv/www/htdocs/dqm/plot/%ixtalk%i.gif", run, i_station);
                gPad->Update();
                gPad->Print(hname);
                //cout << " CrossTalk initialized. " << endl;
            }

            // FIT results
            {
                // Drawing Canvases, frames and saving plots
                TCanvas *canvasforchi2 = new TCanvas("canvasforchi2","canvasforchi2",1000,600);
                framechi2->SetTitle("chi2 distribution");
                framechi2->GetXaxis()->SetTitleOffset(1.1);
                framechi2->GetXaxis()->SetLabelSize(0.025);
                framechi2->GetXaxis()->SetTitle("chi2 distribution");
                framechi2->GetYaxis()->SetTitleOffset(1.1);
                framechi2->GetYaxis()->SetLabelSize(0.025);
                framechi2->GetYaxis()->SetTitle("Counts");
                framechi2->GetYaxis()->SetDecimals();
                framechi2->SetTitle(htitle7);
                framechi2->Draw();
                h_chi2->Draw("same");
                
                
                int chi2entries = h_chi2->GetEntries();
                float chi2mean = h_chi2->GetMean();
                float chi2rms = h_chi2->GetRMS();
                snprintf(infovalue111, sizeof(infovalue111), "Entries: %i", chi2entries);
                snprintf(infovalue112, sizeof(infovalue112), "Mean %f", chi2mean );
                snprintf(infovalue113, sizeof(infovalue113), "RMS %f", chi2rms );
                TPaveText *pt9 = new TPaveText(text_xminchi2,text_yminchi2,text_xmaxchi2,text_ymaxchi2);
                pt9->AddText(infovalue111);
                pt9->AddText(infovalue112);
                pt9->AddText(infovalue113);
                pt9->Draw();
                
                
                
                snprintf(hname, sizeof(hname), "/srv/www/htdocs/dqm/plot/%ichi2.gif", run);
                gPad->Update();
                gPad->Print(hname);

                
                // Drawing Canvases, frames and saving plots
                TCanvas *canvasform = new TCanvas("canvasform","canvasform",1000,600);
                framem->SetTitle("FIT Angular coefficient distribution");
                framem->GetXaxis()->SetTitleOffset(1.1);
                framem->GetXaxis()->SetLabelSize(0.025);
                framem->GetXaxis()->SetTitle("FIT Angular coefficient distribution");
                framem->GetYaxis()->SetTitleOffset(1.1);
                framem->GetYaxis()->SetLabelSize(0.025);
                framem->GetYaxis()->SetTitle("Counts");
                framem->SetTitle(htitle8);
                framem->Draw();
                h_m->Draw("same");
                
                int mentries = h_m->GetEntries();
                float mmean = h_m->GetMean();
                float mrms = h_m->GetRMS();
                snprintf(infovalue114, sizeof(infovalue114), "Entries: %i", mentries);
                snprintf(infovalue115, sizeof(infovalue115), "Mean %f", mmean );
                snprintf(infovalue116, sizeof(infovalue116), "RMS %f", mrms );
                TPaveText *pt10 = new TPaveText(text_xminm,text_yminm,text_xmaxm,text_ymaxm);
                pt10->AddText(infovalue114);
                pt10->AddText(infovalue115);
                pt10->AddText(infovalue116);
                pt10->Draw();

                snprintf(hname, sizeof(hname), "/srv/www/htdocs/dqm/plot/%im.gif", run);
                gPad->Update();
                gPad->Print(hname);


                // Drawing Canvases, frames and saving plots
                TCanvas *canvasforqu = new TCanvas("canvasforqu","canvasforqu",1000,600);
                frameq->SetTitle("FIT q distribution");
                frameq->GetXaxis()->SetTitleOffset(1.1);
                frameq->GetXaxis()->SetLabelSize(0.025);
                frameq->GetXaxis()->SetTitle("FIT q distribution");
                frameq->GetYaxis()->SetTitleOffset(1.1);
                frameq->GetYaxis()->SetLabelSize(0.025);
                frameq->GetYaxis()->SetTitle("Counts");
                frameq->SetTitle(htitle9);
                frameq->Draw();
                h_qu->Draw("same");

                int quentries = h_qu->GetEntries();
                float qumean = h_qu->GetMean();
                float qurms = h_qu->GetRMS();
                snprintf(infovalue117, sizeof(infovalue117), "Entries: %i", quentries);
                snprintf(infovalue118, sizeof(infovalue118), "Mean %f", qumean );
                snprintf(infovalue119, sizeof(infovalue119), "RMS %f", qurms );
                TPaveText *pt11 = new TPaveText(text_xminqu,text_yminqu,text_xmaxqu,text_ymaxqu);
                pt11->AddText(infovalue117);
                pt11->AddText(infovalue118);
                pt11->AddText(infovalue119);
                pt11->Draw();

                snprintf(hname, sizeof(hname), "/srv/www/htdocs/dqm/plot/%iqu.gif", run);
                gPad->Update();
                gPad->Print(hname);
                
                
                // Drawing Canvases, frames and saving plots
                TCanvas *canvasforres = new TCanvas("canvasforres","canvasforres",1000,600);
                frameres->SetTitle("Residuals distribution");
                frameres->GetXaxis()->SetTitleOffset(1.1);
                frameres->GetXaxis()->SetLabelSize(0.025);
                frameres->GetXaxis()->SetTitle("Residuals distribution (strips)");
                frameres->GetYaxis()->SetTitleOffset(1.1);
                frameres->GetYaxis()->SetLabelSize(0.025);
                frameres->GetYaxis()->SetTitle("Counts");
                frameres->GetYaxis()->SetDecimals();
                frameres->SetTitle(htitle19);
                frameres->Draw();
                h_residual[i_station]->Draw("same");
                
                
                int res_entries = h_residual[i_station]->GetEntries();
                float res_mean = h_residual[i_station]->GetMean();
                float res_rms = h_residual[i_station]->GetRMS();
                snprintf(infovalue211, sizeof(infovalue211), "Entries: %i", res_entries);
                snprintf(infovalue212, sizeof(infovalue212), "Mean %f", res_mean );
                snprintf(infovalue213, sizeof(infovalue213), "RMS %f", res_rms );
                TPaveText *pt12 = new TPaveText(text_xmin_res,text_ymin_res,text_xmax_res,text_ymax_res);
                pt12->AddText(infovalue211);
                pt12->AddText(infovalue212);
                pt12->AddText(infovalue213);
                pt12->Draw();
                
                snprintf(hname, sizeof(hname), "/srv/www/htdocs/dqm/plot/%iresolution%i.gif", run, i_station);                
                gPad->Update();
                gPad->Print(hname);
                
                
                
                if (i_station==1) {
                // Drawing Canvases, frames and saving plots
                TCanvas *canvasforresiduals = new TCanvas("canvasforresiduals","canvasforresiduals",1000,600);
                frameresiduals->SetTitle("Residuals distribution");
                frameresiduals->GetXaxis()->SetTitleOffset(1.1);
                frameresiduals->GetXaxis()->SetLabelSize(0.025);
                frameresiduals->GetXaxis()->SetTitle("Residuals distribution (strips)");
                frameresiduals->GetYaxis()->SetTitleOffset(1.1);
                frameresiduals->GetYaxis()->SetLabelSize(0.025);
                frameresiduals->GetYaxis()->SetTitle("Counts");
                frameresiduals->GetYaxis()->SetDecimals();
                frameresiduals->SetTitle(htitle19);
                frameresiduals->Draw();
                h_residuals->Draw("same");

                int resid_entries = h_residuals->GetEntries();
                float resid_mean = h_residuals->GetMean();
                float resid_rms = h_residuals->GetRMS();
                snprintf(infovalue311, sizeof(infovalue311), "Entries: %i", resid_entries);
                snprintf(infovalue312, sizeof(infovalue312), "Mean %f", resid_mean );
                snprintf(infovalue313, sizeof(infovalue313), "RMS %f", resid_rms );
                TPaveText *pt13 = new TPaveText(text_xmin_res,text_ymin_res,text_xmax_res,text_ymax_res);
                pt13->AddText(infovalue311);
                pt13->AddText(infovalue312);
                pt13->AddText(infovalue313);
                pt13->Draw();

                
                snprintf(hname, sizeof(hname), "/srv/www/htdocs/dqm/plot/%iresiduals.gif", run);
                gPad->Update();
                gPad->Print(hname);
                }
                
            }
            
            // TDC COMPLETE MAP
            {
                // Drawing Canvase and frames
                TCanvas *tdcanvas = new TCanvas("tdcanvas","tdcanvas",1000,600);
                
                // Drawing strip profiles
                h_Strip2d->GetXaxis()->SetTitle("TDC channels");
                h_Strip2d->GetYaxis()->SetTitle("TIMING (ns)");
                
                // Drawing first axis
                TAxis *axis = h_Strip2d->GetXaxis();
                axis->SetNdivisions(max_modules,128,1,kFALSE);
                axis->SetDecimals(kFALSE);
                
                h_Strip2d->SetFillColor(2);
                h_Strip2d->SetMarkerStyle(7);
                h_Strip2d->Draw();
                
                // Draw second axis (on the right side)
                int latestchannel = ( (total_tdc_channels-32)%96);
                TGaxis *axis2 = new TGaxis(32, timemin, total_tdc_channels-latestchannel, timemin, 1, maxstationreal+1, maxstationreal+1,"-SM");
                axis2->SetLineColor(kRed);
                axis2->SetLabelColor(kRed);
                axis2->SetLabelOffset(-0.6);
                axis2->SetTickSize(-1);
                axis2->SetDecimals(kFALSE);
                axis2->Draw("same");
                
                // Preparing text for 2D histo legend
                int entries_station1=0, entries_station2=0, entries_station3=0, entries_station4=0, entries_station5=0, entries_station6=0, entries_station7=0, entries_station8=0, entries_station9=0, entries_station10=0;
                if (max_modules >= 1 ) entries_station1 = h_Strip2d->Integral(32,128,1,total_time_bins);
                if (max_modules >= 2 ) entries_station2 = h_Strip2d->Integral(128,224,1,total_time_bins);
                if (max_modules >= 3 ) entries_station3 = h_Strip2d->Integral(224,320,1,total_time_bins);
                if (max_modules >= 4 ) entries_station4 = h_Strip2d->Integral(320,416,1,total_time_bins);
                if (max_modules >= 5 ) entries_station5 = h_Strip2d->Integral(416,512,1,total_time_bins);
                if (max_modules >= 6 ) entries_station6 = h_Strip2d->Integral(512,608,1,total_time_bins);
                if (max_modules >= 7 ) entries_station7 = h_Strip2d->Integral(608,704,1,total_time_bins);
                if (max_modules >= 8 ) entries_station8 = h_Strip2d->Integral(704,800,1,total_time_bins);
                if (max_modules >= 9 ) entries_station9 = h_Strip2d->Integral(800,896,1,total_time_bins);
                if (max_modules >= 10 ) entries_station10 = h_Strip2d->Integral(896,992,1,total_time_bins);
                
                // Trigger mode label and counts
                {
                    // Initialization of SmallCounter-based trigger mode
                    if (etapartition != "ALL") {
                        snprintf(infovalue81, sizeof(infovalue81), "SMALL TRG MODE %i", counter);
                    }
                    
                    
                    // Large SCINT counters trigger mode
                    if (  (etapartition == "ALL") && (largeacceptance == 1) && (twotrackingrpc == 0)  ) {
                        snprintf(infovalue81, sizeof(infovalue81), "LARGE TRG MODE %i", counter);
                    }
                    
                    
                    // Initialization of 1-RPC-based trigger mode and Large SCINT counters
                    if (  (etapartition == "ALL") && (largeacceptance == 0) && (twotrackingrpc == 0) ) {
                        snprintf(infovalue81, sizeof(infovalue81), "1-RPC TRG MODE %i %i %i", counterA, counterB, counterC);
                    }
                    
                    
                    // Initialization of 2-RPC-based trigger mode and Large SCINT counters
                    if (  (etapartition == "ALL") && (largeacceptance == 0) && (twotrackingrpc == 1) ) {
                        snprintf(infovalue81, sizeof(infovalue81), "2-RPC TRG MODE");
                        snprintf(infovalue811, sizeof(infovalue811), "%i %i %i", counterA, counterB, counterC);
                    }
                    
                    // Initialization of 3-RPC-based trigger mode and Large SCINT counters
                    if (  (etapartition == "ALL") && (largeacceptance == 0) && (threetrackingrpc == 1) ) {
                        snprintf(infovalue81, sizeof(infovalue81), "3-RPC TRG MODE");
                        snprintf(infovalue811, sizeof(infovalue811), "%i %i %i", counterA, counterB, counterC);
                    }
                    
                }
                
                snprintf(infovalue83, sizeof(infovalue83), "Station1: %i", entries_station1 );
                snprintf(infovalue84, sizeof(infovalue84), "Station2: %i", entries_station2 );
                snprintf(infovalue85, sizeof(infovalue85), "Station3: %i", entries_station3 );
                snprintf(infovalue86, sizeof(infovalue86), "Station4: %i", entries_station4 );
                snprintf(infovalue87, sizeof(infovalue87), "Station5: %i", entries_station5 );
                snprintf(infovalue88, sizeof(infovalue88), "Station6: %i", entries_station6 );
                snprintf(infovalue881, sizeof(infovalue881), "Station7: %i", entries_station7 );
                snprintf(infovalue882, sizeof(infovalue882), "Station8: %i", entries_station8 );
                snprintf(infovalue883, sizeof(infovalue883), "Station9: %i", entries_station9 );
                snprintf(infovalue884, sizeof(infovalue884), "Station10: %i", entries_station10 );
                
                TPaveText *pt8 = new TPaveText(text_xminh2,text_yminh2,text_xmaxh2,text_ymaxh2);
                pt8->AddText(infovalue81);
                pt8->AddText(infovalue811);
                pt8->AddText(infovalue83);
                pt8->AddText(infovalue84);
                pt8->AddText(infovalue85);
                pt8->AddText(infovalue86);
                pt8->AddText(infovalue87);
                pt8->AddText(infovalue88);
                pt8->AddText(infovalue881);
                pt8->AddText(infovalue882);
                pt8->AddText(infovalue883);
                pt8->AddText(infovalue884);
                pt8->Draw();
                
                // Saving plots on files
                snprintf(hname, sizeof(hname), "/srv/www/htdocs/dqm/plot/%istripprofile2d.gif", run);
                gPad->Update();
                gPad->Print(hname);
                //cout << " TDC 2D map initialized. " << endl;
            }
            
            // Extracting bin contents
            stringstream sstm6;
            char numstr[10];
            strip_prof_string = "";
            strip_prof_noise_string="";
            clustersizedistr="";
            for (int binscan = 1; binscan <= 96; binscan++) {
                int binContent = h_Strip[i_station]->GetBinContent(binscan);
                int binContent_noise = h_Stripnoise[i_station]->GetBinContent(binscan);
                float fbinContent_noise;
                if (kind[i_station]==1) {
                    if (binscan<=32) {
                        strip_surf = 3.5*54; // strip dimension
                        fbinContent_noise = ( binContent_noise / strip_surf) / (nentries * timewindow );
                    }
                    if ( (binscan<=64) && (binscan>32) ) {
                        strip_surf = 3*76.5; // strip dimension
                        fbinContent_noise = ( binContent_noise / strip_surf) / (nentries * timewindow );
                    }
                    if (binscan>64) {
                        strip_surf = 2.75*61.5; // strip dimension
                        fbinContent_noise = ( binContent_noise / strip_surf) / (nentries * timewindow );
                    }
                }
                else {
                    
                    if (binscan<=32) {
                        strip_surf = 2.25 * 62; // strip dimension
                        fbinContent_noise = ( binContent_noise / strip_surf) / (nentries * timewindow );
                    }
                    if ( (binscan<=64) && (binscan>32) ) {
                        strip_surf = 2.0 * 54; // strip dimension
                        fbinContent_noise = ( binContent_noise / strip_surf) / (nentries * timewindow );
                    }
                    if (binscan>64) {
                        strip_surf = 1.75 * 48; // strip dimension
                        fbinContent_noise = ( binContent_noise / strip_surf) / (nentries * timewindow );
                    }
                    
                }
                
                sprintf(numstr, "%d", binContent);
                strip_prof_string = strip_prof_string + numstr + ",";
                
                sprintf(numstr, "%4.2f", fbinContent_noise);
                if ( binscan == 96) {
			strip_prof_noise_string = strip_prof_noise_string + numstr;
		}
		else {
		    strip_prof_noise_string = strip_prof_noise_string + numstr + ",";
		}


            }
            if (verbose_dump) cout << "Strip profile: " << strip_prof_string << endl;
            if (verbose_dump) cout << "Noise Strip profile: " << strip_prof_noise_string << endl;
            
            
            for (int binscan = 1; binscan <= 11; binscan++) {
                int binContent = h_clustersizeA[i_station]->GetBinContent(binscan);
                sprintf(numstr, "%d", binContent);
                clustersizedistrA = clustersizedistrA + numstr + ",";
                //cout << " dump " << clustersizedistrA << endl;
            }

            for (int binscan = 1; binscan <= 11; binscan++) {
                int binContent = h_clustersizeB[i_station]->GetBinContent(binscan);
                sprintf(numstr, "%d", binContent);
                clustersizedistrB = clustersizedistrB + numstr + ",";
                //cout << " dump " << clustersizedistrB << endl;
            }

            for (int binscan = 1; binscan <= 11; binscan++) {
                int binContent = h_clustersizeC[i_station]->GetBinContent(binscan);
                sprintf(numstr, "%d", binContent);
                clustersizedistrC = clustersizedistrC + numstr + ",";
                //cout << " dump " << clustersizedistrC << endl;
            }

            // Taking the timestamp
            char time_buf[21];
            time_t now;
            time(&now);
            strftime(time_buf, 21, "%d.%m.%Y %H:%M:%S", gmtime(&now));
            
            
            // Taking the run number
            sprintf(string_run, "%i", run);
            
            
            // Taking environmental conditions
            sprintf(string_temperature, "%4.1f", temperature);
            sprintf(string_humidity, "%2.0f", humidity);
            sprintf(string_pressure, "%3.0f", pressure);
            
            
            // Dumping data into files
            snprintf(hname, sizeof(hname), "/home/user/re4data/TDC/dump/run%i_station%i.csv", run, i_station);
            
            if ( ( etapartition == "ALL" ) && ( largeacceptance == 0 ) ) {
                for (int toprint = 1; toprint<4; toprint++) {
                    if (toprint == 1) {
                        noiserate[i_station] = noiserateA[i_station];
                        chamber_efficiency[i_station] = chamber_efficiencyA[i_station];
                        err_eff = err_effA;
                        etapartitiontowrite = "A";
                        clustersizedistr = clustersizedistrA;
                        chamber_clustersize = chamber_clustersizeA;
                    }
                    if (toprint == 2) {
                        noiserate[i_station] = noiserateB[i_station];
                        chamber_efficiency[i_station] = chamber_efficiencyB[i_station];
                        err_eff = err_effB;
                        etapartitiontowrite = "B";
                        clustersizedistr = clustersizedistrB;
                        chamber_clustersize = chamber_clustersizeB;

                    }
                    if (toprint == 3) {
                        noiserate[i_station] = noiserateC[i_station];
                        chamber_efficiency[i_station] = chamber_efficiencyC[i_station];
                        err_eff = err_effC;
                        etapartitiontowrite = "C";
                        clustersizedistr = clustersizedistrC;
                        chamber_clustersize = chamber_clustersizeC;
                    }
                    
                    ofstream dumpfile;
                    dumpfile.open (hname, fstream::app);
                    dumpfile << string_run << "," << time_buf << "," << site << "," << string_temperature << "," << string_pressure << "," << string_humidity << "," << name_set_tn[i_station] << "," << etapartitiontowrite << "," << Trigger_layer << "," << i_station << ",";
                    dumpfile << hv_set_tn[i_station] << ","  << hv_mon_tn[i_station] << "," << hv_mon_tw[i_station] << "," << hv_mon_b[i_station] << "," << i_set_tn[i_station] << "," << i_set_tw[i_station]  << "," << i_set_b[i_station]  << ",";
                    
                    if (err_eff>=0) {
                        dumpfile << nentries << "," << chamber_efficiency[i_station]  << "," << err_eff  << "," << chamber_clustersize   << "," << noiserate[i_station] << "," << clustersizedistr;
                    }
                    else {
                        err_eff=999;
                        dumpfile << nentries << "," << chamber_efficiency[i_station]  << "," << err_eff  << "," << chamber_clustersize   << "," << noiserate[i_station] << "," << clustersizedistr;
                    }
                    
                    for (int mm = 1; mm <= 12; mm++){
                        sprintf(string_feb, "%i", FEBS[mm][i_station]);
                        dumpfile << string_feb << ",";
                    }
                    
                    dumpfile << strip_prof_string << strip_prof_noise_string << endl;
                    dumpfile.close();
                }
            }
            
            else {
                ofstream dumpfile;
                dumpfile.open (hname, fstream::app);
                dumpfile << string_run << "," << time_buf << "," << site << "," << string_temperature << "," << string_pressure << "," << string_humidity << "," << name_set_tn[i_station] << "," << etapartition << "," << Trigger_layer << "," << i_station << ",";
                dumpfile << hv_set_tn[i_station] << ","  << hv_mon_tn[i_station] << "," << hv_mon_tw[i_station] << "," << hv_mon_b[i_station] << "," << i_set_tn[i_station] << "," << i_set_tw[i_station]  << "," << i_set_b[i_station]  << ",";
                
                if (err_eff>=0) {
                    dumpfile << nentries << "," << chamber_efficiency[i_station]  << "," << err_eff  << "," << chamber_clustersize   << "," << noiserate[i_station] << "," << clustersizedistr;
                }
                else {
                    err_eff=999;
                    dumpfile << nentries << "," << chamber_efficiency[i_station]  << "," << err_eff  << "," << chamber_clustersize   << "," << noiserate[i_station] << "," << clustersizedistr;
                }
                
                for (int mm = 1; mm <= 12; mm++){
                    sprintf(string_feb, "%i", FEBS[mm][i_station]);
                    dumpfile << string_feb << ",";
                }
                
                dumpfile << strip_prof_string << strip_prof_noise_string << endl;
                dumpfile.close();
            }
            
            // Printing summary informations of the chamber
            {
                cout << setprecision(5);
                if ( trackingreconstruction == 1 ) counterA = tracked_sector_isA;
                if ( trackingreconstruction == 1 ) counterB = tracked_sector_isB;
                if ( trackingreconstruction == 1 ) counterC = tracked_sector_isC;
                
                if (i_station==1) cout <<" Total entries (large pads) = " << nentries << endl;
                if ( (i_station==1) && (etapartition!="ALL") ) cout << " Triggered entries = \033[1;34m" << counter << "\033[0m" << endl;
                if ( (i_station==1) && (etapartition=="ALL") && (largeacceptance==0) ) cout << " Triggered entries ->   A = \033[1;34m " << counterA << "\033[0m     B = \033[1;34m " << counterB << "\033[0m     C = \033[1;34m " << counterC << "\033[0m" << endl;
                if ( (i_station==1) && (etapartition=="ALL") && (largeacceptance==1) ) cout << " Triggered entries = \033[1;34m" << counter << "\033[0m" << endl;
                
                cout << "  " << endl;
                cout << "Stat. " << i_station << " noisy rate (hz/cm2) = " << noiserate[i_station] << endl;
                
                if ( (etapartition == "ALL" ) && ( largeacceptance == 0) ) {
                    cout << "Stat. " << i_station << " efficiencies :  A = " << "\033[1;31m" << chamber_efficiencyA[i_station] << "\033[0m" << " +- " << err_effA << " (\033[1;34m" << chamber_efficiencyA[i_station]*counterA/100  << "\033[0m)" << " B =  " << "\033[1;31m" << chamber_efficiencyB[i_station] << "\033[0m" << " +- " << err_effB << " (\033[1;34m" << chamber_efficiencyB[i_station]*counterB/100  << "\033[0m)" << " C =  " << "\033[1;31m" << chamber_efficiencyC[i_station] << "\033[0m" << " +- " << err_effC << " (\033[1;34m" << chamber_efficiencyC[i_station]*counterC/100  << "\033[0m)" <<  endl;
                }
                
                if ( (etapartition == "ALL" ) && ( largeacceptance == 1) ) {
                    cout << "Stat. " << i_station << " efficiency:  " << etapartition << " = " << "\033[1;31m" << '\t' << chamber_efficiency[i_station] << "\033[0m" << " +- " << err_eff << " (\033[1;34m " << chamber_efficiency[i_station]*counter/100  << "\033[0m)" << endl;
                }
                
                if ( etapartition != "ALL" )  {
                    cout << "Stat. " << i_station << " efficiency:  " << etapartition << " = " << "\033[1;31m" << '\t' << chamber_efficiency[i_station] << "\033[0m" << " +- " << err_eff << " (\033[1;34m " << chamber_efficiency[i_station]*counter/100  << "\033[0m)" << endl;
                }
            }

            // Freeing some memory..
            delete c2;
            delete frame;
            delete frametiming;
            delete frame_effihits;
            delete framecluster;
            delete frameclusternoise;
            delete framehitmulti;
            delete frameclustermulti;
            delete framenoise;
            delete framext;
            delete framechi2;
            delete framem;
            delete frameq;
            delete frameres;
            delete frameresiduals;
        }
        cout << "Plot written. " << endl;
    }
    
    // Printing general informations
    {
        cout << "  " << endl;
        cout << "  " << endl;
        cout << " ------SHOWER COUNTER-------------------- " << endl;
        cout << " 1 Events: TOP = " << shower_count1top << " BOT = " << shower_count1bot << endl;
        cout << " 2 Events: TOP = " << shower_count2top << " BOT = " << shower_count2bot << endl;
        cout << " 3 Events: TOP = " << shower_count3top << " BOT = " << shower_count3bot << endl;
        cout << " 4 Events: TOP = " << shower_count4top << " BOT = " << shower_count4bot << endl;
        cout << " 5 Events: TOP = " << shower_count5top << " BOT = " << shower_count5bot << endl;
        cout << " -------------------------- " << endl;
        cout << "  " << endl;
        cout << "  " << endl;
    }
}
