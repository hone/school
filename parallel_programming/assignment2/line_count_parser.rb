#!/usr/bin/env ruby

# summarizes the output of the hadoop output
def summary( input )
  unique_line_count = 0
  total_line_count = 0
  unique_line_size = 0
  total_line_size = 0

  input.each do |line|
    line_array = line.split
    hash = line_array.shift
    size = line_array.shift.to_i
    occurrences = line_array.shift.to_i

    unique_line_count += 1
    total_line_count += occurrences
    unique_line_size += size
    total_line_size += size * occurrences
  end

  puts <<OUTPUT
  Unique lines = #{unique_line_count}
  Total lines = #{total_line_count}
  Size of unique lines = #{unique_line_size}
  Size of all lines = #{total_line_size}
OUTPUT
end

# if run as a script
if $0 == __FILE__
  lines = Array.new
  while line = STDIN.gets do
    lines.push line
  end

  summary( lines )
end
