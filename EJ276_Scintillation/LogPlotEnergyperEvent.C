#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TPad.h>


void LogPlotEnergyperEvent() {
    // 1. Open your file and get the Ntuple
    TFile *f = new TFile("/home/rohit-kumar/softwares/EJ200-build/test.root", "READ");
    TTree *tree = (TTree*)f->Get("Hits"); // "Hits" is the name from your RunAction

    // 2. Your Log Binning Logic
    const Int_t nbins = 1000;
    Double_t xmin = 1e-5, xmax = 1e1; // Adjusted for MeV or keV range
    Double_t logxmin = TMath::Log10(xmin);
    Double_t logxmax = TMath::Log10(xmax);
    Double_t binwidth = (logxmax-logxmin)/nbins;
    Double_t xbins[nbins+1];
    
    for (Int_t i=0; i<=nbins; i++) {
       xbins[i] = TMath::Power(10, logxmin + i*binwidth);
    }

    // 3. Create the histogram using the custom bins
    TH1F *h = new TH1F("h", "Energy Distribution per Event;Energy (MeV);Counts", nbins, xbins);

    // 4. Fill the histogram from the Ntuple branch
    // Assuming your branch name is "TotalEnergy"
    tree->Draw("TotalEnergyperEvent >> h", "", "goff"); 

    // 5. Draw and Format
    TCanvas *c1 = new TCanvas("c1", "Log Plot", 800, 600);
    h->SetLineColor(kAzure+1);
    h->SetFillColorAlpha(kAzure+1, 0.3);
    h->Draw("HIST");

    gPad->SetLogx(); // Apply log scale to the X-axis
//    gPad->SetLogy(); // Log scale on Y is also common for flux spectrums
}
