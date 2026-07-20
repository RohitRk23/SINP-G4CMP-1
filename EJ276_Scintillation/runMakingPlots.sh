#! /bin/bash
treeName=${1:-"Hits"}
fileName=${2:-"/home/ubuntu/RohitWork/Scintillation2-build/PerEventData_1GeV_withPhoton.root"}
root -l -b << EOF
TChain *t;
t = new TChain("$treeName");
t->Add("$fileName")
.L MakingPlots.C++
MakingPlots ta(t)
ta.Loop()
EOF
