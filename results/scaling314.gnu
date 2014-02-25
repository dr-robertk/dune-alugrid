#set xrange [256:16384]
#set yrange [3:300]

set terminal postscript eps color "Helvetica" 22

set output "scaling314.eps"
set title "ALUGrid main_ball"

set logscale 

set key bottom left
set grid

set pointsize 2

set xtics ( "32" 32, "64" 64, "128" 128, "256" 256, "512" 512, "1024" 1024, "2048" 2048, "4096" 4096 )
# , "8192" 8192, "16224" 16224 )

set xlabel "#cores"
set ylabel "run time"

c = 3 
p "mb_alusfc_314/optimal.dat" title "optimal" w l lw c, \
  "mb_bresen_314/scaling.dat" title "bresenham" w lp lw c, \
  "mb_alusfc_314/scaling.dat" title "alusfc" w lp lw c, \
  "mb_alusfc_314_linkage/scaling.dat" title "alusfc_l" w lp lw c, \
  "mb_hsfc_314/scaling.dat" title "hsfc" w lp lw c, \
  "mb_zgraph_314/scaling.dat" title "zgraph" w lp lw c, \
  "mb_kway_314/scaling.dat" title "kway" w lp lw c