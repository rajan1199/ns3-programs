set terminal pdf
set output "acn_1.pdf"
set xlabel "Time (in seconds)"
set ylabel "Throughput (in Mbit/sec)"
plot "acn_proj_graph.data" using 1:2 with lines title "Throughput v/s Time for FriisPropagationLossModel"
