require 'rubygems'
require 'gnuplot'

# graph data with gnuplot
Gnuplot.open do |gp|
  Gnuplot::Plot.new( gp ) do |plot|
    MAX_THREADS = 3
    x = (1..3).to_a
    y1 = 5.06
    y2 = 166.56
    y3 = 300
    y = [ y1/y1, y1/y2, y1/y3 ]

    plot.title "Scaleup"
    plot.xlabel "Size"
    plot.ylabel "Scaleup"
    plot.xrange "[0:#{( MAX_THREADS + 1 )}]"
    plot.yrange "[0:#{ y.max + y.min }]"
    plot.terminal "png"
    plot.output "scaleup.png"

    plot.data << Gnuplot::DataSet.new( [x,y] ) do |ds|
      ds.with = "linespoints"
      ds.notitle
    end
  end
end
