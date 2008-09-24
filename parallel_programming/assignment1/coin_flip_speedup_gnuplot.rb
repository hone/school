require 'rubygems'
require 'gnuplot'
require 'coin_flip_data_parser'

parser = CoinFlipDataParser.new( ARGV[0] )
MAX_THREADS = parser.data.size - 1
puts parser.data[0].inspect

# graph data with gnuplot
Gnuplot.open do |gp|
  Gnuplot::Plot.new( gp ) do |plot|
    plot.title "Number of Threads vs. Elapsed Time"
    plot.ylabel "Threads"
    plot.xlabel "Elapsed Time (ms)"
    plot.xrange "[0:#{( MAX_THREADS + 1 )}]"
    plot.yrange "[0:#{parser.data[0].max + 20}]"
#    plot.terminal "png"
#    plot.output "graph.png"

    x = ( 1..MAX_THREADS ).to_a
    y = parser.data[0]

    plot.data << Gnuplot::DataSet.new( [x,y] ) do |ds|
      ds.with = "linespoints"
      ds.notitle
    end
  end
end
