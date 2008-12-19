require 'rubygems'
require 'gnuplot'
require 'coin_flip_data_parser'

parser = CoinFlipDataParser.new( ARGV[0] )
MAX_THREADS = parser.data[parser.data.keys.first].size - 1

# graph data with gnuplot
Gnuplot.open do |gp|
  Gnuplot::Plot.new( gp ) do |plot|
    x = parser.data[parser.data.keys.first].keys.sort
    y = x.collect {|thread| parser.data[parser.data.keys.first][thread][0] }
    plot.title "Coin Flip Per Thread Startup Cost"
    plot.xlabel "Threads"
    plot.ylabel "Elapsed Time (ms)"
    plot.xrange "[0:#{ x.max + x.min }]"
    plot.yrange "[0:#{ y.max + y.min }]"
    plot.terminal "png"
    plot.output "coin_flip_per_thread_startup_cost.png"

    x.each_with_index do |x_value, index|
      puts "#{x_value}, #{y[index]}"
    end

    plot.data << Gnuplot::DataSet.new( [x,y] ) do |ds|
      ds.with = "linespoints"
      ds.notitle
    end
  end
end
