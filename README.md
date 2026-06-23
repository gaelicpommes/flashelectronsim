# flashelectronsim

## Applicator selection (10 cm / 5 cm / 2 cm)

You can now choose applicator inner diameter from a macro before `/run/initialize`:

```tcl
/flash/setApplicatorIDcm 10   # allowed: 10, 5, 2
```

For Slurm runs, `runpossib.slurm` defaults to the 10 cm applicator. Override it
without editing the script like this:

```bash
APPLICATOR_CM=5 sbatch runpossib.slurm
APPLICATOR_CM=2 sbatch runpossib.slurm
```

The geometry uses these fixed specs:

- 10 cm applicator: ID=100 mm, OD=116 mm, L=428 mm
- 5 cm applicator:  ID=50 mm,  OD=65 mm,  L=428 mm
- 2 cm applicator:  ID=20 mm,  OD=35 mm,  L=428 mm

`macros/beamdiag_template.mac`, `macros/pddpossib.mac`, and `macros/runpossib.mac`
support `APPLICATOR_CM` placeholder replacement for batch jobs.

## Batch / Slurm usage

This project is set up for macro-driven batch runs. From the build directory,
run with an explicit macro file:

```bash
cd build
./FlashElectronSim macros/run.mac
```

For the CSV-driven PDD/profile workflow, submit the Slurm script from the
project root:

```bash
sbatch runpossib.slurm
```

Running `./FlashElectronSim` without a macro no longer auto-starts visualization;
it prints the batch usage message instead. Optional visualization is still available
explicitly if needed:

```bash
cd build
./FlashElectronSim macros/vis.mac
```

A lowercase convenience launcher is also created after a successful build:

```bash
cd build
./flashelectronsim macros/run.mac
```
