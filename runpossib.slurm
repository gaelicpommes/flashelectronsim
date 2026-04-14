#!/bin/bash
#SBATCH --job-name=pddgrid
#SBATCH --output=logs/pddgrid_%A_%a.out
#SBATCH --error=logs/pddgrid_%A_%a.err
#SBATCH --time=12:00:00
#SBATCH --cpus-per-task=15
#SBATCH --mem=8G
#SBATCH --array=1-7%1
#SBATCH --requeue

set -euo pipefail

module purge
module load Geant4/11.3.0-GCC-13.3.0
module load Geant4-data/11.3

cd /scratch/brussel/113/vsc11383/projects/third_sim/FlashElectronSim/build
export G4FORCENUMBEROFTHREADS=${SLURM_CPUS_PER_TASK}

CSV="../macros/1404paramgridv5.csv"
APPLICATOR_CM=10

# -------------------------
# Human-readable beamOn tag
# -------------------------
format_beamon_tag() {
    local n="$1"
    if (( n >= 1000000 )); then
        if (( n % 1000000 == 0 )); then
            echo "$((n/1000000))M"
        else
            awk -v x="$n" 'BEGIN{printf "%.1fM", x/1000000.0}'
        fi
    elif (( n >= 1000 )); then
        if (( n % 1000 == 0 )); then
            echo "$((n/1000))k"
        else
            awk -v x="$n" 'BEGIN{printf "%.1fk", x/1000.0}'
        fi
    else
        echo "${n}"
    fi
}

# -------------------------
# Grab Nth data row
# Skips header, blank lines, and comment lines starting with #
# -------------------------
LINE=$(awk -F',' '
    BEGIN {count=0}
    /^[[:space:]]*#/ {next}
    /^[[:space:]]*$/ {next}
    $1 == "run_id" {next}
    {count++}
    count == ENVIRON["SLURM_ARRAY_TASK_ID"] {print; exit}
' "${CSV}")

if [[ -z "${LINE}" ]]; then
  echo "No valid row for task ${SLURM_ARRAY_TASK_ID} in ${CSV}"
  exit 1
fi

# CSV columns:
# run_id,beamOn,Emean_MeV,sigmaE_MeV,sigmaTheta_deg,sigma_x_mm,sigma_y_mm
IFS=',' read -r RUNID BEAMON EMEAN SIGE SIGTH SX SY <<< "${LINE}"

# Trim whitespace just in case
RUNID=$(echo "${RUNID}" | xargs)
BEAMON=$(echo "${BEAMON}" | xargs)
EMEAN=$(echo "${EMEAN}" | xargs)
SIGE=$(echo "${SIGE}" | xargs)
SIGTH=$(echo "${SIGTH}" | xargs)
SX=$(echo "${SX}" | xargs)
SY=$(echo "${SY}" | xargs)

BEAMTAG=$(format_beamon_tag "${BEAMON}")

echo "Task ${SLURM_ARRAY_TASK_ID}: run_id=${RUNID} beamOn=${BEAMON} (${BEAMTAG}) E=${EMEAN} sigE=${SIGE} sigTh=${SIGTH} SX=${SX} SY=${SY}"

# Make a short tag for filenames
TAG="r${RUNID}_E${EMEAN}_sE${SIGE}_sTh${SIGTH}"

# Clean tag: replace '.' with 'p'
TAG_CLEAN=$(echo "${TAG}" | sed 's/\./p/g')

# -------------------------
# Generate source macro
# -------------------------
SRCMAC="src_${TAG_CLEAN}_J${SLURM_JOB_ID}_T${SLURM_ARRAY_TASK_ID}.mac"

sed -e "s/EMEAN/${EMEAN}/g" \
    -e "s/SIGE/${SIGE}/g" \
    -e "s/SIGTH/${SIGTH}/g" \
    -e "s/SX/${SX}/g" \
    -e "s/SY/${SY}/g" \
    ../macros/sourcepossib.mac > "${SRCMAC}"

# -------------------------
# Output files
# -------------------------
PDDTXT="pdd_${TAG_CLEAN}_N${BEAMTAG}_J${SLURM_JOB_ID}_T${SLURM_ARRAY_TASK_ID}.txt"
LAT_DMAX="latDmax_${TAG_CLEAN}_N${BEAMTAG}_J${SLURM_JOB_ID}_T${SLURM_ARRAY_TASK_ID}.txt"
LAT_R50="latR50_${TAG_CLEAN}_N${BEAMTAG}_J${SLURM_JOB_ID}_T${SLURM_ARRAY_TASK_ID}.txt"
LAT_MID="latMid_${TAG_CLEAN}_N${BEAMTAG}_J${SLURM_JOB_ID}_T${SLURM_ARRAY_TASK_ID}.txt"

RUNMAC="$(mktemp run_${TAG_CLEAN}_XXXX.mac)"
trap 'rm -f "$RUNMAC" "$SRCMAC"' EXIT

# -------------------------
# Replace placeholders in run template
# -------------------------
sed -e "s|SOURCE_MAC|${SRCMAC}|g" \
    -e "s|BEAMON|${BEAMON}|g" \
    -e "s|PDD_TXT|${PDDTXT}|g" \
    -e "s|LAT_DMAX_DOSE_TXT|${LAT_DMAX}|g" \
    -e "s|LAT_R50_DOSE_TXT|${LAT_R50}|g" \
    -e "s|LAT_MID_DOSE_TXT|${LAT_MID}|g" \
    -e "s|APPLICATOR_CM|${APPLICATOR_CM}|g" \
    ../macros/pddpossib.mac > "${RUNMAC}"

./FlashElectronSim "${RUNMAC}"

echo "Wrote: ${PDDTXT}"
echo "Wrote: ${LAT_DMAX}"
echo "Wrote: ${LAT_R50}"
echo "Wrote: ${LAT_MID}"
