Float_t x[10], y[10], error[10];
int number_of_points;

void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag) {

    //calculate chisquare
    Double_t chisq = 0;
    Double_t delta;
    int i;
    
    for ( i = 0; i < number_of_points; i++ ) {
        delta  = (y[i]-func(x[i],par))/error[i];
        chisq += delta*delta;
    }
    f = chisq;
}

Double_t func(float x, Double_t *par) {
    Double_t value = par[0]/(1 + exp( par[1] * (par[2] - x) ));
    return value;
}

void re4summary() {
    
    // Setting plots range and style
    char filename[80], outputtempfile[80], titletowrite[80];
    gStyle->SetOptStat(0.1);
    gStyle->SetMarkerSize(1);
    gStyle->SetMarkerStyle(20);
    gStyle->SetMarkerColor(1);
    int volt_bin = 100, volt_min = 8400, volt_max = 10200;
    int effi_bin = 10000, effi_min = 0, effi_max = 110;
    
    
    // Text positioning
    float label_font=0.025;
    float axis_offset=0.9;
    float label_size=0.03;
    
    
    // Input file variables
    string      date, time, trg, chambervector[200], sitevector[200], typevector[200], chambername, buttonpress;
    Float_t     efficiency, efficiencyerr, csize, noise, Volt, hvtn, hvtw, hvb, var1;
    Int_t       run, numberofevents;
    Char_t      trigger_mode[20], eta[20], name[200], site[80], type[80], trigger_mode_out[200], eta_out[200], name_out[20], site_out[200], type_out[200];
    
    
    // Output file variables
    Float_t fit_efficiency, fit_efficiency_err, slope, slope_err, hvat50, hvat50_err, chi2, hv50_diff_ab, hv50_diff_ac, hv50_diff_bc, hv50_diff_tb, hv95_diff_ab, hv95_diff_ac, hv95_diff_bc, hv95_diff_tb, hv9010_diff_ab, hv9010_diff_ac, hv9010_diff_bc, hv9010_diff_tb, hv7525_diff_ab, hv7525_diff_ac, hv7525_diff_bc, hv7525_diff_tb, effi_diff_ab, effi_diff_ac, effi_diff_bc, effi_diff_tb;
    
    Double_t hv9010, hv7525, hv10, hv25, hv75, hv90, hv95, hv10_a, hv10_b, hv10_c, hv25_a, hv25_b, hv25_c, hv75_a, hv75_b, hv75_c, hv90_a, hv90_b, hv90_c, hv95_a, hv95_b, hv95_c, hv50_a, hv50_b, hv50_c, hv7525_a, hv7525_b, hv7525_c, hv9010_a, hv9010_b, hv9010_c, effi_a, effi_b, effi_c,;
    
    TString     etamode_out, trgmode_out;
    Double_t    par[3], err_par[3];


    
    // Constant Cuts
    const char s_namecut[80];
    const char s_doublecut[80] = "trigger_mode==\"DOUBLE\"";
    const char s_topcut[80] = "trigger_mode==\"TOP\"";
    const char s_bottomcut[80] = "trigger_mode==\"BOTTOM\"";
    const char s_etacutA[80] = "eta==\"A\"";
    const char s_etacutB[80] = "eta==\"B\"";
    const char s_etacutC[80] = "eta==\"C\"";
    TCut trgcut, etacut, namecut;
    
    int j = 1, i, k, l, m, sect;
    TFile *hfile = 0;
    hfile = TFile::Open("re4summary.root","RECREATE");
    
    TTree *rpclab = new TTree("rpclab","904 RPC Lab - PLOTS");
    rpclab->Branch("Volt",&Volt,"Volt/F");
    rpclab->Branch("efficiency",&efficiency,"efficiency/F");
    rpclab->Branch("efficiencyerr",&efficiencyerr,"efficiencyerr/F");
    rpclab->Branch("eta",eta,"eta/C");
    rpclab->Branch("trigger_mode",trigger_mode,"trigger_mode/C");
    rpclab->Branch("name",name,"name/C");
    rpclab->Branch("site",site,"site/C");
    rpclab->Branch("type",type,"site/C");
    
    TTree *rpclab_performance = new TTree("rpclab_performance","904 RPC Lab - performance");
    rpclab_performance->Branch("csize",&csize,"csize/F");
    rpclab_performance->Branch("fit_efficiency",&fit_efficiency,"fit_efficiency/F");
    rpclab_performance->Branch("slope",&slope,"slope/F");
    rpclab_performance->Branch("hvat50",&hvat50,"hvat50/F");
    rpclab_performance->Branch("chi2",&chi2,"chi2/F");
    rpclab_performance->Branch("hv95",&hv95,"hv95/D");
    rpclab_performance->Branch("hv9010",&hv9010,"hv9010/D");
    rpclab_performance->Branch("hv7525",&hv7525,"hv7525/D");
    rpclab_performance->Branch("eta_out",eta_out,"eta_out/C");
    rpclab_performance->Branch("name_out",name_out,"name_out/C");
    rpclab_performance->Branch("site_out",site_out,"site_out/C");
    rpclab_performance->Branch("type_out",type_out,"type_out/C");
    rpclab_performance->Branch("trigger_mode_out",trigger_mode_out,"trigger_mode_out/C");
    rpclab_performance->Branch("effi_diff_ab",&effi_diff_ab,"effi_diff_ab/F");
    rpclab_performance->Branch("effi_diff_ac",&effi_diff_ac,"effi_diff_ac/F");
    rpclab_performance->Branch("effi_diff_bc",&effi_diff_bc,"effi_diff_bc/F");
    rpclab_performance->Branch("effi_diff_tb",&effi_diff_tb,"effi_diff_tb/F");
    rpclab_performance->Branch("hv50_diff_ab",&hv50_diff_ab,"hv50_diff_ab/F");
    rpclab_performance->Branch("hv50_diff_ac",&hv50_diff_ac,"hv50_diff_ac/F");
    rpclab_performance->Branch("hv50_diff_bc",&hv50_diff_bc,"hv50_diff_bc/F");
    rpclab_performance->Branch("hv50_diff_tb",&hv50_diff_tb,"hv50_diff_tb/F");
    rpclab_performance->Branch("hv95_diff_ab",&hv95_diff_ab,"hv95_diff_ab/F");
    rpclab_performance->Branch("hv95_diff_ac",&hv95_diff_ac,"hv95_diff_ac/F");
    rpclab_performance->Branch("hv95_diff_bc",&hv95_diff_bc,"hv95_diff_bc/F");
    rpclab_performance->Branch("hv95_diff_tb",&hv95_diff_tb,"hv95_diff_tb/F");
    rpclab_performance->Branch("hv9010_diff_ab",&hv9010_diff_ab,"hv9010_diff_ab/F");
    rpclab_performance->Branch("hv9010_diff_ac",&hv9010_diff_ac,"hv9010_diff_ac/F");
    rpclab_performance->Branch("hv9010_diff_bc",&hv9010_diff_bc,"hv9010_diff_bc/F");
    rpclab_performance->Branch("hv9010_diff_tb",&hv9010_diff_tb,"hv9010_diff_tb/F");
    rpclab_performance->Branch("hv7525_diff_ab",&hv7525_diff_ab,"hv7525_diff_ab/F");
    rpclab_performance->Branch("hv7525_diff_ac",&hv7525_diff_ac,"hv7525_diff_ac/F");
    rpclab_performance->Branch("hv7525_diff_bc",&hv7525_diff_bc,"hv7525_diff_bc/F");
    rpclab_performance->Branch("hv7525_diff_tb",&hv7525_diff_tb,"hv7525_diff_tb/F");
    
    
    
    // Creating Canvas
    {
        TCanvas *Frameperf = new TCanvas("Frameperf","Chamber performance",0,0,1500,900);
        //TPad *pad1 = new TPad("pad1","",0,0,1,1); //1st pad for T
        //TPad *pad2 = new TPad("pad2","",0,0,1,1); //2nd pad for P
        //pad2->SetFillStyle(4000); //will be transparent
        //pad1->Draw();
        //pad1->cd();
        
        TH2F *sens1 = new  TH2F("sens1","sens1",volt_bin,volt_min,volt_max,effi_bin,effi_min,effi_max);
        sens1->SetMarkerColor(1);
        sens1->SetMarkerStyle(20);
        sens1->SetMarkerSize(0.9);
        strcpy(titletowrite,"Summary performance ");
        sens1->SetTitle(titletowrite);
        sens1->GetXaxis()->SetTitleOffset(axis_offset);
        sens1->GetXaxis()->SetLabelSize(label_size);
        sens1->GetYaxis()->SetTitleSize(0.04);
        sens1->GetXaxis()->SetTitle("HV_{effective}");
        sens1->GetYaxis()->SetTitleOffset(axis_offset);
        sens1->GetYaxis()->SetLabelSize(label_size);
        sens1->GetYaxis()->SetTitleSize(0.04);
        sens1->GetYaxis()->SetTitle("Efficiency");
        //gPad->SetGrid();
        sens1->Draw("");
        //cout << " Frame initialized" << endl;
        
    }
    
    // Verboses printouts
    int verbose_filling_chamber = 1, verbose_loop_chamber = 0;
    
    
    // File opening
    strcpy(filename,"cosmictestRE4.txt");
    ifstream myfile (filename);
    //cout << " Opening " << filename << endl;
    
    
    // Sigmoit Fit
    {
        TF1 *sigmoid = new TF1("sigmoid"," [0]/(1 + exp( [1] * ([2] - x) ) )");
        sigmoid->SetParNames("Efficiency","Slope","HV @ 50 efficiency");
        sigmoid->SetParameter(0, 94);
        sigmoid->SetParLimits(0, 80, 99.9);
        sigmoid->SetParameter(1, 0.001);
        sigmoid->SetParLimits(1, 0.00001, 0.09);
        sigmoid->SetParameter(2, 8900);
        sigmoid->SetParLimits(2, 8700, 9400);
        sigmoid->SetLineColor(1);
        //cout << " Fit initialized" << endl;
    }
    
    // Looping over the input file
    if (myfile.is_open()) {
        while ( myfile.good() ) {
            
            // Reading input file
            myfile >> site >> date >> time >> var1 >> run >> name >> type >> eta >> trigger_mode >> numberofevents >> Volt >> hvtn >> hvtw >> hvb >> efficiency >> efficiencyerr >> csize;
            //cout << "reading file..." << endl;

            // Solving DB ghent bug
            if (strcmp(site,"Ghent")==0) {
                efficiency = efficiency*100;
                efficiencyerr = efficiencyerr*100;
            }
            rpclab->Fill();
            
            
            // Filling chamber, site, type vector names
            {
                stringstream ss;
                ss << name;
                ss >> chambername;
                
                if ( j == 1 ) {
                    chambervector[j] = chambername;
                    
                    stringstream ss;
                    ss << site;
                    ss >> sitevector[j];
                    
                    stringstream ss;
                    ss << type;
                    ss >> typevector[j];
                    
                    if (verbose_filling_chamber) cout << " I have found the chamber " << chambervector[j] << " " << j << " site = " << sitevector[j] << " type = " << type << endl;
                    j++;
                }
                
                if ( j > 1) {
                    //cout << " Chambervector[" << j << "] = " << chambervector[j] << "[" << j << "]" << " and name= " << name << endl;
                    
                    stringstream ss;
                    ss << site;
                    ss >> sitevector[j];
                    
                    stringstream ss;
                    ss << type;
                    ss >> typevector[j];
                    
                    if ( chambervector[j-1] != chambername ) {
                        chambervector[j] = chambername;
                        if (verbose_filling_chamber) cout << " I have found the chamber " << chambervector[j] << " " << j << " site = " << sitevector[j] << " type = " << typevector[j] << endl;
                        j++;
                    }
                    
                }
            }
            
        }
        rpclab->Write();
        myfile.close();
    }
    cout << " Finished reading the file." << endl;
    
    // Loop over chamber name and eta partition and trg mode to measure performance
    for (i = 1; i < j; i++ ) {
        if (verbose_loop_chamber) cout << "Working on chamber " << i << " " << chambervector[i] << endl;

        eta_a = 0;
        eta_b = 0;
        eta_c = 0;
        
        for (l = 1; l <= 3; l++ ) {
            
            sect = 0;

            for (k = 1; k <= 3; k++ ) {
                
                
                // Printouts verbose
                {
                    cout << " Now I am working on this chamber " << chambervector[i] << " and this partition " << eta << " (site = " << sitevector[i] << " and type = " << typevector[i] << " )" << endl;
                    //cin >> buttonpress;
                }
                
                
                // Cuts
                {
                    // Forming correct namecut
                    {
                        strcpy(s_namecut,"name==\"");
                        strcat(s_namecut,chambervector[i].c_str());
                        strcat(s_namecut,"\"");
                        namecut = s_namecut;
                    }
                    
                    // Forming correct trgcut
                    {
                        if ( l == 1 ) strcpy(trigger_mode_out,"DOUBLE");
                        if ( l == 2 ) strcpy(trigger_mode_out,"TOP");
                        if ( l == 3 ) strcpy(trigger_mode_out,"BOTTOM");
                        
                        if ( l == 1 ) trgcut = s_doublecut;
                        if ( l == 2 ) trgcut = s_topcut;
                        if ( l == 3 ) trgcut = s_bottomcut;
                    }
                    
                    // Forming correct etacut
                    {
                        if ( k == 1 ) strcpy(eta_out,"A");
                        if ( k == 2 ) strcpy(eta_out,"B");
                        if ( k == 3 ) strcpy(eta_out,"C");

                        if ( k == 1 ) etacut = s_etacutA;
                        if ( k == 2 ) etacut = s_etacutB;
                        if ( k == 3 ) etacut = s_etacutC;
                    }
                }

          
                // Elaborating name, type and site
                {
                    strcpy(outputtempfile,"Plots/");
                    strcat(outputtempfile,"temp.txt");
                    ofstream tempfile (outputtempfile);
                    tempfile << chambervector[i] << " " << typevector[i] << " " << sitevector[i] << endl;
                    tempfile.close();
                    
                    ifstream tempfiler (outputtempfile);
                    if (tempfiler.is_open()) {
                        while ( tempfiler.good() ) {
                            
                            // Reading temp file
                            tempfiler >> name_out >> type_out >> site_out;
                        }
                    }
                    tempfiler.close();
                    //cout << " Reading " << name_out << " " << type_out << " " << site_out << " " << eta << endl;
                }
                
                
                // Calculate number of hv points
                {
                    rpclab->Draw("efficiency:Volt>>hvpoints",namecut&&trgcut&&etacut);
                    number_of_points = hvpoints->Integral();
                    sens1->Draw("");
                    rpclab->Draw("efficiency:Volt",namecut&&trgcut&&etacut,"same");
                    //cout << " Number of HV points = " << number_of_points << endl;
                }
                

                // Perform the fits
                {
                    rpclab->Draw("Volt:efficiency:efficiencyerr",namecut&&trgcut&&etacut,"goff");
                    TGraphErrors *rpc_graph = new TGraphErrors(number_of_points,rpclab->GetV1(),rpclab->GetV2(),0,rpclab->GetV3());
                    rpc_graph->SetMarkerColor(1);
                    rpc_graph->SetMarkerStyle(20);
                    rpc_graph->SetMarkerSize(0.9);
                    rpc_graph->SetLineColor(1);

                    /*
                    // -------------------------------------------
                    // MINUIT minimitation
                     Double_t *x = rpclab->GetV1();
                     Double_t *y = rpclab->GetV2();
                     Double_t *error = rpclab->GetV3();
                     
                     for ( int p = 0; p < number_of_points; ++p ) {
                     cout << " MINUIT vectors: " << x[p] << " " << y[p] << " " << error[p] << endl;
                     }

                     cout << " Calling MINUIT... " << endl;
                    TMinuit *gMinuit = new TMinuit(3);
                    gMinuit->SetFCN(fcn);
                    cout << " MINUIT -> Function initialized. " << endl;
                    
                    Double_t arglist[10];
                    Int_t ierflg = 0;
                    
                    arglist[0] = 1;
                    gMinuit->mnexcm("SET ERR", arglist ,1, ierflg);
                    
                    // Set starting values and step sizes for parameters
                    Double_t vstart[3] = {94, 0.01, 8900};
                    Double_t step[3] = {0.1 , 0.0001 , 5};
                    Double_t min[3] = {80 , 0.001 , 8600};
                    Double_t max[3] = {99.9 , 0.1 , 9300};

                    gMinuit->mnparm(0, "Efficiency", vstart[0], step[0], min[0], max[0], ierflg);
                    gMinuit->mnparm(1, "Slope", vstart[1], step[1], min[1], max[1], ierflg);
                    gMinuit->mnparm(2, "HV50", vstart[2], step[2], min[2], max[2], ierflg);

                    // Now ready for minimization step
                    arglist[0] = 10000;
                    arglist[1] = 0.1.;
                    gMinuit->mnexcm("MIGRAD", arglist ,2, ierflg);
                    
                    // Print results
                    Double_t amin,edm,errdef;
                    Int_t nvpar,nparx,icstat;
                    gMinuit->mnstat(amin,edm,errdef,nvpar,nparx,icstat);
                    gMinuit->GetParameter(0, par[0], err_par[0]);
                    gMinuit->GetParameter(1, par[1], err_par[1]);
                    gMinuit->GetParameter(2, par[2], err_par[2]);
                    cout << " MINUIT parameters: " << par[0] << " " << par[1] << " " << par[2] << endl;
                    
                    cout << " MINUIT ended correctly " << endl;
                    // -------------------------------------------
                     */

                    // CHI2 minimitation
                    rpc_graph->Fit("sigmoid","EM");
                    TF1 *fit = rpc_graph->GetFunction("sigmoid");

                    if ( fit->GetNDF() > 0 ) chi2 = fit->GetChisquare() / fit->GetNDF();
                    
                    fit_efficiency = fit->GetParameter(0);
                    fit_efficiency_err = fit->GetParError(0);
                    
                    slope = fit->GetParameter(1);
                    slope_err = fit->GetParError(1);
                    
                    hvat50 = fit->GetParameter(2);
                    hvat50_err = fit->GetParError(2);
                    
                    hv95 = -( 1 / slope ) * TMath::Log( 1/0.95 - 1 ) + hvat50;
                    hv90 = -( 1 / slope ) * TMath::Log( 1/0.90 - 1 ) + hvat50;
                    hv75 = -( 1 / slope ) * TMath::Log( 1/0.75 - 1 ) + hvat50;
                    hv25 = -( 1 / slope ) * TMath::Log( 1/0.25 - 1 ) + hvat50;
                    hv10 = -( 1 / slope ) * TMath::Log( 1/0.10 - 1 ) + hvat50;

                    hv9010 = hv90 - hv10;
                    hv7525 = hv75 - hv25;
                    
                    //rpclab_performance->Fill();

                    cout << " -> Fit results is: efficiency_max = " << fit_efficiency << " +- " << fit_efficiency_err << " slope = " << slope << " +- " << slope_err << " HV at 50 = " << hvat50 << " +- " << hvat50_err << "  Chi2 = " << chi2 << endl;
                    
                    // Saving the canvas into images
                    {
                        stringstream ss;
                        ss << name;
                        ss >> chambername;
                        
                        sens1->Draw("");
                        rpc_graph->Draw("ep");
                        
                        TString output = string(chambervector[i]);

                        if ( k == 1 ) etamode_out = "_double";
                        if ( k == 2 ) etamode_out = "_top";
                        if ( k == 3 ) etamode_out = "_bottom";

                        if ( l == 1 ) trgmode_out = "_a";
                        if ( l == 2 ) trgmode_out = "_b";
                        if ( l == 3 ) trgmode_out = "_c";

                        //TF1 *sigmoid_toshow = new TF1("sigmoid_toshow"," [0]/(1 + exp( [1] * ([2] - x) ) )",8500,10000);
                        //sigmoid_toshow->SetParameter(0,par[0]);
                        //sigmoid_toshow->SetParameter(1,par[1]);
                        //sigmoid_toshow->SetParameter(2,par[2]);
                        //sigmoid_toshow->Draw();
                        Frameperf->Update();
                        Frameperf->SaveAs( "Plots/" + output + etamode_out + trgmode_out + ".gif" );
                    }

                }

                
                // Differences between a//b//c
                {                    
                    
                    if ( k == 1 ) {
                        effi_a = fit_efficiency;
                        hv50_a = hvat50;
                        hv95_a = hv95;
                        hv7525_a = hv7525;
                        hv9010_a = hv9010;
                        sect++;
                    }
                    if ( k == 2 ) {
                        effi_b = fit_efficiency;
                        hv50_b = hvat50;
                        hv95_b = hv95;
                        hv7525_b = hv7525;
                        hv9010_b = hv9010;
                        sect++;
                    }
                    if ( k == 3 ) {
                        effi_c = fit_efficiency;
                        hv50_c = hvat50;
                        hv95_c = hv95;
                        hv7525_c = hv7525;
                        hv9010_c = hv9010;
                        sect++;
                    }
                    
                    if ( sect == 3) {
                        effi_diff_ab = effi_a - effi_b;
                        effi_diff_ac = effi_a - effi_c;
                        effi_diff_bc = effi_b - effi_c;

                        hv50_diff_ab = hv50_a - hv50_b;
                        hv50_diff_ac = hv50_a - hv50_c;
                        hv50_diff_bc = hv50_b - hv50_c;
                        
                        hv95_diff_ab = hv95_a - hv95_b;
                        hv95_diff_ac = hv95_a - hv95_c;
                        hv95_diff_bc = hv95_b - hv95_c;
                        
                        hv7525_diff_ab = hv7525_a - hv7525_b;
                        hv7525_diff_ac = hv7525_a - hv7525_c;
                        hv7525_diff_bc = hv7525_b - hv7525_c;
                        
                        hv9010_diff_ab = hv9010_a - hv9010_b;
                        hv9010_diff_ac = hv9010_a - hv9010_c;
                        hv9010_diff_bc = hv9010_b - hv9010_c;
                        
                    }
                }
                
                
                // Differences between top//bottom
                {
                    if ( (l == 2) && (k == 1) ) {
                        eta_a++;
                        effi_ta = fit_efficiency;
                        hvat50_ta = hvat50;
                        hv95_ta = hv95;
                        hv7525_ta = hv7525;
                        hv9010_ta = hv9010;
                    }
                    if ( (l == 3) && (k == 1) ) {
                        eta_a++;
                        effi_ba = fit_efficiency;
                        hvat50_ba = hvat50;
                        hv95_ba = hv95;
                        hv7525_ba = hv7525;
                        hv9010_ba = hv9010;
                    }
                    
                    if ( (l == 2) && (k == 2) ) {
                        eta_b++;
                        effi_tb = fit_efficiency;
                        hvat50_tb = hvat50;
                        hv95_tb = hv95;
                        hv7525_tb = hv7525;
                        hv9010_tb = hv9010;
                    }
                    if ( (l == 3) && (k == 2) ) {
                        eta_b++;
                        effi_bb = fit_efficiency;
                        hvat50_bb = hvat50;
                        hv95_bb = hv95;
                        hv7525_bb = hv7525;
                        hv9010_bb = hv9010;
                    }
                    
                    if ( (l == 2) && (k == 3) ) {
                        eta_c++;
                        effi_tc = fit_efficiency;
                        hvat50_tc = hvat50;
                        hv95_tc = hv95;
                        hv7525_tc = hv7525;
                        hv9010_tc = hv9010;
                    }
                    if ( (l == 3) && (k == 3) ) {
                        eta_c++;
                        effi_bc = fit_efficiency;
                        hvat50_bc = hvat50;
                        hv95_bc = hv95;
                        hv7525_bc = hv7525;
                        hv9010_bc = hv9010;
                    }
                    
                    if ( eta_a == 2 ) {
                        effi_diff_tb = effi_ta - effi_ba;
                        hv50_diff_tb = hvat50_ta - hvat50_ba;
                        hv95_diff_tb = hvat50_ta - hvat50_ba;
                        hv7525_diff_tb = hvat50_ta - hvat50_ba;
                        hv9010_diff_tb = hvat50_ta - hvat50_ba;
                    }
                    
                    if ( eta_b == 2 ) {
                        effi_diff_tb = effi_tb - effi_bb;
                        hv50_diff_tb = hvat50_tb - hvat50_bb;
                        hv95_diff_tb = hvat50_tb - hvat50_bb;
                        hv7525_diff_tb = hvat50_tb - hvat50_bb;
                        hv9010_diff_tb = hvat50_tb - hvat50_bb;
                    }
                    
                    if ( eta_c == 2 ) {
                        effi_diff_tb = effi_tc - effi_bc;
                        hv50_diff_tb = hvat50_tc - hvat50_bc;
                        hv95_diff_tb = hvat50_tc - hvat50_bc;
                        hv7525_diff_tb = hvat50_tc - hvat50_bc;
                        hv9010_diff_tb = hvat50_tc - hvat50_bc;
                    }
                }

                rpclab_performance->Fill();
            }
        }
    }
    rpclab_performance->Write();
}
