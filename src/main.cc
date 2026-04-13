// main.cc  (simple, robust UI+macro handling)
// main.cc
#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"

#include "FTFP_BERT.hh"
#include "G4EmStandardPhysics_option4.hh"

#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"
#include "G4ScoringManager.hh"
#include "G4String.hh"
#include <algorithm>
#include <cstdlib>
#if defined(G4MULTITHREADED)
#include "G4MTRunManager.hh"
#endif

int main(int argc, char** argv)
{
  // Use Geant4 default run manager type (MT when Geant4 is built with MT support)
  auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);

#if defined(G4MULTITHREADED)
  if (auto* mtRM = dynamic_cast<G4MTRunManager*>(runManager)) {
    G4int nThreads = 0; // 0 => keep Geant4 default/autoconfig

    if (const char* envThreads = std::getenv("G4FORCENUMBEROFTHREADS")) {
      nThreads = std::max(1, std::atoi(envThreads));
    }

    // Optional CLI override: ./FlashElectronSim <macro.mac> <threads>
    if (argc > 2) {
      nThreads = std::max(1, std::atoi(argv[2]));
    }

    if (nThreads > 0) {
      mtRM->SetNumberOfThreads(nThreads);
    }
  }
#endif

  G4ScoringManager::GetScoringManager();

  runManager->SetUserInitialization(new DetectorConstruction());

  auto* physics = new FTFP_BERT;
  physics->ReplacePhysics(new G4EmStandardPhysics_option4());
  runManager->SetUserInitialization(physics);

  runManager->SetUserInitialization(new ActionInitialization());

  auto* visManager = new G4VisExecutive();
  visManager->Initialize();

  auto* uiManager = G4UImanager::GetUIpointer();

  if (argc > 1) {
    uiManager->ApplyCommand("/control/execute " + G4String(argv[1]));
  } else {
    auto* ui = new G4UIExecutive(argc, argv);
    ui->SessionStart();
    delete ui;
  }

  delete visManager;
  delete runManager;
  return 0;
}
