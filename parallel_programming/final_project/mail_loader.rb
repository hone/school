require 'tmail'

# Utility to parse all the e-mail
class MailLoader
  SERIALIZED_FILE = "mail.dump"
  attr_reader :mails

  def initialize( serialized_file, path = nil )
    @mails = Array.new

    if File.exist?( serialized_file )
      File.open( serialized_file ) do |file|
        @mails = Marshal.load( file )
      end
    else
      parse_all_mail( path )
      File.open( serialized_file, 'w' ) do |file|
        Marshal.dump( @mails, file )
      end
    end

    puts "Loaded: #{@mails.size} e-mails."
  end

  private
  # parses all the e-mail, currently based off of evolution filenaming scheme
  def parse_all_mail( path )
    filenames = Dir.glob( "#{path}/*." )
    filenames.each do |filename|
      mail = TMail::Mail.load_from( filename )
      @mails.push( mail )
    end
  end
end
