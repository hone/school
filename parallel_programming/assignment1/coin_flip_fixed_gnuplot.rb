require 'rubygems'
require 'gnuplot'
require 'coin_flip_data_parser'

parser = CoinFlipDataParser.new( ARGV[0] )

# graph data with gnuplot
Gnuplot.open do |gp|
  Gnuplot::Plot.new( gp ) do |plot|
    x = parser.data.keys.sort
    y = x.collect {|iterations| parser.data[iterations][iterations / 10 ** 6][0] }
    plot.title "Coin Flip Fixed Startup Cost"
    plot.xlabel "Iterations"
    plot.ylabel "Elapsed Time (ms)"
    plot.xrange "[0:#{ x.max + x.min }]"
    plot.yrange "[0:#{ y.max + y.min }]"
    plot.terminal "png"
    plot.output "coin_flip_per_thread_startup_cost.png"

    plot.data << Gnuplot::DataSet.new( [x,y] ) do |ds|
      ds.with = "linespoints"
      ds.notitle
    end
  end
end
