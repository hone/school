require 'rubygems'
require 'gnuplot'
require 'brute_force_des_data_parser'

BruteForceDESDataParser

parser = BruteForceDESDataParser.new( ARGV[0] )
MAX_THREADS = parser.data[parser.data.keys.first].size

# graph data with gnuplot
Gnuplot.open do |gp|
  Gnuplot::Plot.new( gp ) do |plot|
    x = ( 1..MAX_THREADS ).to_a
    averages = x.collect {|thread| parser.data[parser.data.keys.first][thread][0] }
    y = ( 1..MAX_THREADS ).collect {|thread| averages[0] / averages[thread - 1] }

    plot.title "Number of Threads vs. Speedup"
    plot.xlabel "Threads"
    plot.ylabel "Speedup"
    plot.xrange "[0:#{( MAX_THREADS + 1 )}]"
    plot.yrange "[0:#{ y.max + y.min }]"
    plot.terminal "png"
    plot.output "brute_force_des_speedup.png"

    plot.data << Gnuplot::DataSet.new( [x,y] ) do |ds|
      ds.with = "linespoints"
      ds.notitle
    end
  end
end
