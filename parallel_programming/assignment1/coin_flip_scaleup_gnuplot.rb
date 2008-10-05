require 'rubygems'
require 'gnuplot'
require 'coin_flip_data_parser'

parser = CoinFlipDataParser.new( ARGV[0] )
MAX_THREADS = parser.data[parser.data.keys.first].size - 1

# graph data with gnuplot
Gnuplot.open do |gp|
  Gnuplot::Plot.new( gp ) do |plot|
    x = parser.data.keys.sort
    y = x.collect do |flip_number|
      ts = parser.data[flip_number][1][0]
      puts "#{flip_number} #{flip_number / x.min }"
      tl = parser.data[flip_number][flip_number / x.min ][0]

      ts / tl.to_f
    end
    plot.title "Elapsed Time Small Machine / Elapsed Time N Machine vs. Problem Size"
    plot.xlabel "Flips"
    plot.ylabel "Elapsed Time (ms) [Ts/Tn]"
    plot.xrange "[0:#{ x.max + x.min }]"
    plot.yrange "[0:#{ y.max + y.min }]"
    #plot.terminal "png"
    #plot.output "coin_flip_scaleup_graph.png"


    plot.data << Gnuplot::DataSet.new( [x,y] ) do |ds|
      ds.with = "linespoints"
      ds.notitle
    end
  end
end
