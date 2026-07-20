#define MakingPlots_cxx
#include "MakingPlots.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>
#include "TMath.h"


using namespace std;

void MakingPlots::Loop()
{
//   In a ROOT session, you can do:
//      root> .L MakingPlots.C
//      root> MakingPlots t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch



   // 2. Your Log Binning Logic
    const Int_t nbins = 100;
    Double_t xmin = 1e-6, xmax = 2; // Adjusted for MeV or keV range
    Double_t logxmin = TMath::Log10(xmin);
    Double_t logxmax = TMath::Log10(xmax);
    Double_t binwidth = (logxmax-logxmin)/nbins;
    Double_t xbins[nbins+1];
    
    for (Int_t i=0; i<=nbins; i++) {
       xbins[i] = TMath::Power(10, logxmin + i*binwidth);
    }

    // 3. Create the histogram using the custom bins
    TFile *plots = new TFile("EnergyPlotHistogramsEJ200_n_100events.root","RECREATE");
    plots->cd();
    TH1F *h_NonZero  = new TH1F("h_NonZero", "Energy Distribution per Event;Energy (MeV);Counts", nbins, xbins);
    TH1F *h_WithZero = new TH1F("h_WithZero", "Energy Distribution per Event;Energy (MeV);Counts", nbins, xbins);


   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      if(jentry%1000==0)cout << "Processed: " << jentry << endl;
      // if (Cut(ientry) < 0) continue;
      if(HitCount>0) h_NonZero->Fill(TotalEnergyperEvent);
      h_WithZero->Fill(TotalEnergyperEvent);
   }
   plots->Write();
}
