
#!/bin/bash

# Number of times to repeat the each experiment
rounds=30

# Array containing the sizes
# init_size=216
# final_size=408
init_size=504
final_size=504
sizes=(280 376 504)
# Array containing the times
times=(0.5 1 1.5)

# Array containing the versions
versions1=("original" "send_recv" "isend_recv")

nvidia-smi --format=csv --loop-ms=10 --query-gpu=timestamp,name,uuid,pstate,memory.total,memory.used,memory.free,temperature.gpu,utilization.memory,utilization.gpu,power.management,power.draw >> gpu_power.csv &

# Nested for-loops to iterate over sizes, times, and versions
for round in $(seq 1 $((rounds))); do
  for time in "${times[@]}"; do
    for size in ${sizes[@]}; do
      for version in "${versions1[@]}"; do
        echo "Processing round: $round size: $size, time: $time, version: $version"
        # echo "./${version}/ModelagemFletcher.exe TTI $size $size $size 16 12.5 12.5 12.5 0.001 $time"
        case "$version" in
        "original")
          output=$(perf stat -e power/energy-pkg/ -x, -o /tmp/cpu_power.csv ./$version.exe TTI $size $size $size 16 12.5 12.5 12.5 0.001 $time | grep "$version")
          ;;
        "send_recv" | "isend_recv")
          output=$(perf stat -e power/energy-pkg/ -x, -o /tmp/cpu_power.csv mpirun -np 2 ./$version.exe TTI $size $size $size 16 12.5 12.5 12.5 0.001 $time | grep "$version")
          ;;
        *)
          echo "Unknown version: $version"
          ;;
        esac
        joules=$(tail -n +3 /tmp/cpu_power.csv | cut -d, -f1)
        output="$output,$joules"
        echo "$output"
        echo "$output" >> time.csv
        rm TTI*
      done
    done
  done
done


versions2=("spawn_all_at_once" "spawn_one_at_time")

# Nested for-loops to iterate over sizes, times, and versions
for round in $(seq 1 $((rounds))); do
  for time in "${times[@]}"; do
    for size in ${sizes[@]}; do
      for version in "${versions2[@]}"; do
	for processos in 2 4 8; do
          output=$(perf stat -e power/energy-pkg/ -x, -o /tmp/cpu_power.csv mpirun -np 1 ./$version.exe TTI $size $size $size 16 12.5 12.5 12.5 0.001 $time $processos  | grep "$version")
          joules=$(tail -n +3 /tmp/cpu_power.csv | cut -d, -f1)
          output="$output,$joules"
          echo "$output"
          echo "$output" >> time.csv
          rm TTI*
      	done
      done
    done
  done
done

killall nvidia-smi
