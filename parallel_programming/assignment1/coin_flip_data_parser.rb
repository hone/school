# Parses .dat files produced by CoinFlip.java
class CoinFlipDataParser
  MAX_REPEATS = 20
  AVERAGE = 0

  attr_reader :data

  # constructor that takes in a filename
  def initialize( filename )
    @data = Array.new
    @data.push Array.new # data[0] will hold the averages
    parse( filename )
  end

  def parse( filename )
    # parse data files and get elapsed times
    File.open( filename ) do |file|
      index = 0
      file.each_with_index do |line, i|
        # starting a repetition
        if i % MAX_REPEATS == 0
          # if not first item (empty), calculate averages
          if i != 0
            average( index )
          end
          @data.push Array.new
          index += 1
        end

        @data[index].push line.split.last.to_i
      end
    end

    average( @data.size - 1 )
  end

  def average( index )
    sum = @data[index].inject( 0 ) { |sum, elapsed_time| sum + elapsed_time }
    average = sum / MAX_REPEATS.to_f
    @data[AVERAGE].push average
  end
end
