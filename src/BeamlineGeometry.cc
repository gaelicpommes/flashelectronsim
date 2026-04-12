// src/BeamlineGeometry.cc
#include "BeamlineGeometry.hh"

#include "G4NistManager.hh"
#include "G4Tubs.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

namespace {
  void SetSolidVis(G4LogicalVolume* lv, const G4Colour& c) {
    auto vis = new G4VisAttributes(c);
    vis->SetForceSolid(true);
    lv->SetVisAttributes(vis);
    
  }
}

BeamlineHandles BeamlineGeometry::Build10cmBeamline(G4LogicalVolume* worldLV)
{
  BeamlineHandles h;

  auto nist = G4NistManager::Instance();
  auto air        = nist->FindOrBuildMaterial("G4_AIR");
  auto al         = nist->FindOrBuildMaterial("G4_Al");
  auto graphite   = nist->FindOrBuildMaterial("G4_GRAPHITE");   // placeholder collimator material
  auto plexiglass = nist->FindOrBuildMaterial("G4_PLEXIGLASS"); // PMMA

// -------------------------
// eWindow (REFERENCE PLANE ONLY) at z=0
// -------------------------
{
  h.zWindow = 0.0*mm;

  auto solid = new G4Tubs("eWindow",
                          0.0*mm,
                          58.0*mm,     // >= applicator OD/2 (58 mm)
                          0.1*mm/2,
                          0.*deg, 360.*deg);

  auto logic = new G4LogicalVolume(solid, air, "eWindowLV");
  new G4PVPlacement(nullptr, G4ThreeVector(0,0,h.zWindow), logic,
                    "eWindowPV", worldLV, false, 0, true);
  SetSolidVis(logic, G4Colour::White());

  // --- Source cutoff aperture at e-window (air, thin) ---
  {
    const auto halfZ = 0.05*mm;   // 0.1 mm thick
    const auto rCut  = 2.2*mm;    // CutoffX = CutoffY = 2.2 mm

    auto cutSolid = new G4Tubs("SourceCutoff",
                               0.0*mm, rCut, halfZ,
                               0.*deg, 360.*deg);

    auto cutLV = new G4LogicalVolume(cutSolid, air, "SourceCutoffLV");

    // place it at the electron window plane z=0
    new G4PVPlacement(nullptr, G4ThreeVector(0,0,0), cutLV,
                  "SourceCutoffPV", logic, false, 0, true);

    cutLV->SetVisAttributes(G4VisAttributes::GetInvisible()); // optional
  }
}

  // -------------------------
  // Aluminium foils (4x): 0.2 mm thick; 2.2 mm air gap; foil1 ~ 20 mm downstream
  // centre-to-centre spacing = 0.2 + 2.2 = 2.4 mm
  // -------------------------
  auto makeFoil = [&](const G4String& name, G4double zPos) {
    auto solid = new G4Tubs(name,
                            0.0*mm,
                            50.0*mm,     // placeholder radius (>= beam envelope)
                            0.2*mm/2,
                            0.*deg, 360.*deg);
    auto logic = new G4LogicalVolume(solid, al, name + "LV");
    new G4PVPlacement(nullptr, G4ThreeVector(0,0,zPos), logic,
                      name + "PV", worldLV, false, 0, true);
    SetSolidVis(logic, G4Colour::Yellow());
  };

  h.zFoil1 = 20.0*mm;
  const auto dzFoilCC = 2.4*mm;

  makeFoil("AlFoil1", h.zFoil1);
  makeFoil("AlFoil2", h.zFoil1 + 1*dzFoilCC);
  makeFoil("AlFoil3", h.zFoil1 + 2*dzFoilCC);
  h.zFoil4 = h.zFoil1 + 3*dzFoilCC;
  makeFoil("AlFoil4", h.zFoil4);

  // -------------------------
  // Fixed aperture collimator (manufacturer geometry)
  // Aperture diameter = 20 mm
  // Outer diameter    = 68 mm
  // Cylinder height   = 48.6 mm
  // -------------------------
  {
    const auto collOuterR    = 34.0*mm;     // OD 68 mm
    const auto collThickness = 48.6*mm;     // height
    const auto collHalfZ     = collThickness/2.0;

    const auto boreR         = 10.0*mm;     // aperture diameter 20 mm

    // Place it AFTER the last foil (avoid overlap)
    // Foil4 center = h.zFoil4, foil half-thickness = 0.1 mm
    const auto foilHalfZ = 0.1*mm;
    const auto gapAfterFoils = 1.0*mm;      // not specified; small air gap

    // Set collimator upstream face just after foil4 downstream face + gap
    const auto zCollIn = h.zFoil4 + foilHalfZ + gapAfterFoils;
    h.zCollCenter = zCollIn + collHalfZ;

    auto outer = new G4Tubs("FixColl_outer", 0.0*mm, collOuterR, collHalfZ, 0.*deg, 360.*deg);
    auto hole  = new G4Tubs("FixColl_hole",  0.0*mm, boreR,      collHalfZ + 1*um, 0.*deg, 360.*deg);
    auto solid = new G4SubtractionSolid("FixColl", outer, hole);

    // Material not provided → keep your placeholder unless they confirm it
    auto logic = new G4LogicalVolume(solid, graphite, "FixCollLV");
    new G4PVPlacement(nullptr, G4ThreeVector(0,0,h.zCollCenter), logic,
                      "FixCollPV", worldLV, false, 0, true);
    SetSolidVis(logic, G4Colour::Gray());

    h.zCollExit = h.zCollCenter + collHalfZ;


    // === DEBUG PRINTS ===
    G4cout << "\n[Beamline DEBUG]\n";
    G4cout << "Foil4 center z = " << h.zFoil4/mm << " mm\n";
    G4cout << "Coll upstream face z = "
           << (h.zCollCenter - collHalfZ)/mm << " mm\n";
    G4cout << "Coll exit z = " << h.zCollExit/mm << " mm\n\n";
    
  }

  // -------------------------
  // Applicator 10 cm (PMMA): ID=100 mm, OD=116 mm, L=428 mm
  // Applicator 5cm (PMMA): ID=50 mm, OD=66 mm, L=428 mm
  // Applicator 2cm (PMMA): ID=20 mm, OD=36 mm, L=428 mm
  // Placed flush to collimator downstream face
  // -------------------------
  {
    const auto appRin   = 25.0*mm;      // ID 100 mm-> bore radius 50 mm   
    const auto appRout  = 33.0*mm;      // OD 116 mm-> bore radius 58 mm
    const auto appL     = 428.0*mm;
    const auto appHalfZ = appL/2;

    // Small tolerance to avoid coincident surfaces (prevents overlap warnings)
    const auto epsR = 1.0*um;
    const auto epsZ = 1.0*um;

    h.zAppEntrance = h.zCollExit;               // flush to collimator exit
    const auto zAppCenter = h.zAppEntrance + appHalfZ;

    // PMMA tube (hollow by definition)
    auto appSolid = new G4Tubs("Applicator10cm",
                               appRin, appRout, appHalfZ,
                               0.*deg, 360.*deg);

    auto appLV = new G4LogicalVolume(appSolid, plexiglass, "Applicator10cmLV");

    new G4PVPlacement(nullptr, G4ThreeVector(0,0,zAppCenter), appLV,
                      "Applicator10cmPV", worldLV, false, 0, true);

    SetSolidVis(appLV, G4Colour::Blue());

    // AIR bore inside applicator (for filtering/stepping logic)
    // IMPORTANT: shrink slightly so it does NOT sit exactly on the mother boundary
    //auto boreSolid = new G4Tubs("ApplicatorBore10cm",
                                //0.0*mm,
                                //appRin - epsR,
                                //appHalfZ - epsZ,
                                //0.*deg, 360.*deg);

    //auto boreLV = new G4LogicalVolume(boreSolid, air, "ApplicatorBore10cmLV");

    // Bore centered in local coords of the applicator LV
    //new G4PVPlacement(nullptr, G4ThreeVector(0,0,0), boreLV,
                      //"ApplicatorBore10cmPV", appLV, false, 0, true);

    //boreLV->SetVisAttributes(G4VisAttributes::GetInvisible());

    //h.applicatorLV = appLV;
    //h.zAppExit     = h.zAppEntrance + appL;
  //}
  
    // -------------------------
  // Thin air exit plane at applicator exit (for debugging / scoring)
  // -------------------------
  //const auto exitThickness = 0.1*mm;
    //const auto exitHalfZ     = exitThickness/2;
    //const auto eps           = 0.1*mm;   // small gap to avoid overlap

    //const auto zExitCenter = h.zAppExit + exitHalfZ + eps;

    //auto exitSolid = new G4Tubs("ExitPlane",
                                //0.0*mm,
                                //60.0*mm,      // slightly larger than bore
                                //exitHalfZ,
                                //0.*deg, 360.*deg);

    //auto exitLV = new G4LogicalVolume(exitSolid, air, "ExitPlaneLV");

    //new G4PVPlacement(nullptr,
                      //G4ThreeVector(0,0,zExitCenter),
                      //exitLV,
                      //"ExitPlanePV",
                      //worldLV,
                      //false, 0, true);

    //SetSolidVis(exitLV, G4Colour::Red());

    //h.zExitPlane = zExitCenter;
  //}
    h.applicatorLV = appLV;
    h.zAppExit     = h.zAppEntrance + appL;
  }

  return h;
}
