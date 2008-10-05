require 'rubygems'
require 'gnuplot'
require 'coin_flip_data_parser'

parser = CoinFlipDataParser.new( ARGV[0] )
MAX_THREADS = parser.data[parser.data.keys.first].size - 1

# graph data with gnuplot
Gnuplot.open do |gp|
  Gnuplot::Plot.new( gp ) do |plot|
    averages = ( 1..MAX_THREADS ).collect {|thread| parser.data[parser.data.keys.first][thread][0] }
    x = ( 1..MAX_THREADS ).to_a
    y = ( 1..MAX_THREADS ).collect {|thread| averages[0] / averages[thread - 1] }

    plot.title "Number of Threads vs. Elapsed Time"
    plot.xlabel "Threads"
    plot.ylabel "Elapsed Time (ms)"
    plot.xrange "[0:#{( MAX_THREADS + 1 )}]"
    plot.yrange "[0:#{ y.max + y.min }]"
    #plot.terminal "png"
    #plot.output "coin_flip_speedup.png"

    plot.data << Gnuplot::DataSet.new( [x,y] ) do |ds|
      ds.with = "linespoints"
      ds.notitle
    end
  end
end
