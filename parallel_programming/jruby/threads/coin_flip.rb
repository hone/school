# Handles each CoinFlip, executes per thread
class CoinFlip
  HEADS = 0
  TAILS = 1

  attr_reader :heads_count

  def initialize( runs )
    @runs = runs
    @heads_count = 0
  end

  # runs the iterations
  def run
    1.upto( @runs ) do |i|
      # coin flip
      if rand( 2 ) == HEADS
        @heads_count += 1
      end
    end
  end
end

# calculates the iterations per thread and the extra iterations for the last thread
def calculate_iterations( num_of_iterations, num_of_threads )
  iterations_per_thread = num_of_iterations / num_of_threads
  extra_iterations =
    if( num_of_iterations % num_of_threads != 0 )
      num_of_iterations - num_of_threads * iterations_per_thread;
    else
      0
    end

  [iterations_per_thread, extra_iterations]
end

if $0 == __FILE__
  if ARGV.length < 2
    $stderr.puts "need to specify number of iterations and number of threads."
    exit
  end

  threads = Array.new
  coins = Array.new
  num_of_iterations = ARGV.shift.to_i
  num_of_threads = ARGV.shift.to_i
  iterations_per_thread, extra_iterations = calculate_iterations( num_of_iterations, num_of_threads )
  start_time = Time.now

  1.upto( num_of_threads ) do |thread_num|
    iterations = iterations_per_thread
    iterations += extra_iterations if thread_num == num_of_iterations
    threads << Thread.new( iterations ) do |runs|
      coin_flip = CoinFlip.new( runs )
      coins << coin_flip
      coin_flip.run
    end
  end

  threads.each {|thread| thread.join }
  total_heads = coins.inject( 0 ) {|sum, coin| coin.heads_count + sum }

  end_time = Time.now

  puts "#{total_heads} heads in #{num_of_iterations} coin tosses."
  puts "Start time: #{start_time}"
  puts "End time: #{end_time}"
  puts "Elapsed time: #{end_time - start_time} s"
end
