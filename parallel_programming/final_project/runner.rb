require 'mail_loader'

puts "ARG0: #{ARGV[0]}"
puts "ARG1: #{ARGV[1]}"
puts "ARG2: #{ARGV[2]}"
mail_loader = MailLoader.new( ARGV[0], ARGV[1].to_i, ARGV[2] )
