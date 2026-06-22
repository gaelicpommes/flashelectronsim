# flashelectronsim

## Applicator selection (10 cm / 5 cm / 2 cm)

You can now choose applicator inner diameter from a macro before `/run/initialize`:

```tcl
/flash/setApplicatorIDcm 10   # allowed: 10, 5, 2
```

The geometry uses these fixed specs:

- 10 cm applicator: ID=100 mm, OD=116 mm, L=428 mm
- 5 cm applicator:  ID=50 mm,  OD=66 mm,  L=428 mm
- 2 cm applicator:  ID=20 mm,  OD=36 mm,  L=428 mm

`macros/beamdiag_template.mac`, `macros/pddpossib.mac`, and `macros/runpossib.mac`
support `APPLICATOR_CM` placeholder replacement for batch jobs.

## Visual interactive GUI

After building, start the visual Geant4 session from the build directory:

```bash
cd build
./FlashElectronSim
```

A lowercase convenience launcher is also created, so this also works after a successful build:

```bash
cd build
./flashelectronsim
```

When no macro file is passed, the program now automatically runs `macros/vis.mac` and
`macros/source.mac` before opening the interactive session. The Qt/OpenGL viewer should
show the geometry, and you can type Geant4 commands in the GUI command prompt, for example:

```tcl
/run/beamOn 10
/vis/viewer/refresh
/control/execute macros/run.mac
```

For batch mode without the GUI, pass a macro explicitly:

```bash
./FlashElectronSim macros/run.mac
```
