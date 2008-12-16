require 'mail_loader'

puts "ARG0: #{ARGV[0]}"
puts "ARG1: #{ARGV[1]}"
mail_loader = MailLoader.new( ARGV[0], ARGV[1] )
