require 'rubygems'
require 'gnuplot'

# graph data with gnuplot
Gnuplot.open do |gp|
  Gnuplot::Plot.new( gp ) do |plot|
    MAX_THREADS = 3
    x = ( 1..MAX_THREADS ).to_a
    y1 = 138.44
    y2 = 166.5
    y3 = 158.3
    y = [ y1/y1, y1/y2, y1/y3 ]

    plot.title "Number of Threads vs. Speedup"
    plot.xlabel "Threads"
    plot.ylabel "Speedup"
    plot.xrange "[0:#{( MAX_THREADS + 1 )}]"
    plot.yrange "[0:#{ y.max + y.min }]"
    plot.terminal "png"
    plot.output "speedup.png"

    plot.data << Gnuplot::DataSet.new( [x,y] ) do |ds|
      ds.with = "linespoints"
      ds.notitle
    end
  end
end
