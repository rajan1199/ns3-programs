set terminal pdf
set output "acn_compare.pdf"
set xlabel "Time (in seconds)"
set ylabel "Throughput (in Mbit/sec)"
set title "Comparison of throughputs for 2 different models"
plot "acn_proj_graph.data" using 1:2 title "FriisPropagationLossModel" with lines,\
"acn_proj_graph.data" using 1:3 title "FixedRSSLossModel" with lines
