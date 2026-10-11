#!/usr/bin/env bash
# Integration test: producer and monitor as separate processes connected only by shared memory.
# Usage: run_two_processes.sh <stage_bin> <monitor_bin> <model_dir>
set -euo pipefail
stage_bin=$1
monitor_bin=$2
model_dir=$3
channel="/ea_it_$$"

"$monitor_bin" --channel "$channel" --duration-ms 1500 --period-ms 10 &
monitor_pid=$!
"$stage_bin" --channel "$channel" --model-dir "$model_dir" --frames 40 --period-ms 20
wait "$monitor_pid"
