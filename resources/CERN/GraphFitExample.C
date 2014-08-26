#include "../../interface/ROOT.h"

using namespace std;

void GraphFitExample(){
   
   TCanvas *c1 = new TCanvas("c1","A Simple Graph Example",200,10,700,500);
   TCanvas *c2 = new TCanvas("c1","A Simple Graph Example - second ",200,10,700,500);
   TGraph * emptyGraph = new TGraph();
   
   int varsize = 10;
   
   int * x = new int[varsize];
   int * y = new int[varsize];
   
   for (int i = 0 ; i < 10 ; i++){
     x[i] = i;
     y[i] = i*2;
  }
   
   TGraph * gr = new TGraph(10,x,y);
   gr->SetLineColor(0);
   
   TF1 * fitfunc = new TF1("FitTrack","[0]+x*[1]",2,9);
   fitfunc->SetLineColor(kBlue);
   gr->Fit(fitfunc,"RQ");
   
   std::cout << gr->GetCorrelationFactor() << endl;
   cout << fitfunc->Eval(11) << endl;
   cout << fitfunc->Eval(-5) << endl;
   
   //gr->Draw("AC*");
   
   /** example empty constructor*/
   
   c2->cd();
   
   for (int i = 0 ; i < 10 ; i++){
     emptyGraph->SetPoint(i,i,i*2);
  }
   
   emptyGraph->SetLineColor(0);
   emptyGraph->Fit(fitfunc,"RQ");
   emptyGraph->Draw("AC*");
   
   fitfunc->GetParameter(0);
   
   c2->Draw();
   
   delete x; delete y;
   
}   