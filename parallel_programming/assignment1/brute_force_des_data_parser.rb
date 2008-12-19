# Parses .dat files produced by BruteForcedDES.java
class BruteForceDESDataParser
  AVERAGE = 0

  attr_reader :data

  # constructor that takes in a filename
  def initialize( filename )
    @data = Hash.new
    parse( filename )
  end

  def parse( filename )
    # parse data files and get elapsed times
    File.open( filename ) do |file|
      file.each_with_index do |line, i|
        # parse line
        elements = line.split
        iteration = elements.shift.to_i
        thread = elements.shift.to_i
        keys = elements.shift.to_i
        time = elements.shift.to_i

        @data[keys] = Hash.new if @data[keys].nil?
        @data[keys][thread] = [0] if @data[keys][thread].nil? # this will hold the averages

        @data[keys][thread][iteration] = time
      end
    end

    @data.each do |keys, threads|

      threads.each do |thread, values|
        values[AVERAGE] = average( values ) unless values.empty?
      end
    end
  end

  def average( iterations )
    sum = iterations.inject( 0 ) {|sum, elapsed_time| sum + elapsed_time }
    average = sum / iterations.size.to_f

    average
  end
end
