require 'rubygems'
require 'gnuplot'
require 'brute_force_des_data_parser'

parser = BruteForceDESDataParser.new( ARGV[0] )
MAX_THREADS = parser.data[parser.data.keys.first].size - 1

# graph data with gnuplot
Gnuplot.open do |gp|
  Gnuplot::Plot.new( gp ) do |plot|
    x = parser.data.keys.sort
    x.shift
    y = x.collect do |flip_number|
      ts = parser.data[x.first][1][0]
      tl = parser.data[flip_number][flip_number / x.min ][0]
      puts "#{Math.log(flip_number)/Math.log(2)} #{flip_number / x.min} #{tl}"

      ts / tl.to_f
    end

    plot.title "Elapsed Time Small Machine / Elapsed Time N Machine vs. Problem Size"
    plot.xlabel "Keys"
    plot.ylabel "Scaleup"
    plot.xrange "[0:#{ x.max + x.min }]"
    plot.yrange "[0:#{ y.max + y.min }]"
    plot.terminal "png"
    plot.output "brute_force_des_scaleup.png"


    plot.data << Gnuplot::DataSet.new( [x,y] ) do |ds|
      ds.with = "linespoints"
      ds.notitle
    end
  end
end
