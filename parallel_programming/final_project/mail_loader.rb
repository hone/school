require 'tmail'

# Utility to parse all the e-mail
class MailLoader
  SERIALIZED_FILE = "mail.dump"
  attr_reader :mails

  def initialize( serialized_file, num_threads = 1, path = nil )
    @mails = Array.new

    if File.exist?( serialized_file )
      File.open( serialized_file ) do |file|
        @mails = Marshal.load( file )
      end
    else
      start_time = Time.now
      parse_all_mail( path, num_threads )
      end_time = Time.now

      puts end_time - start_time
      File.open( serialized_file, 'w' ) do |file|
        Marshal.dump( @mails, file )
      end
    end

    puts "Loaded: #{@mails.size} e-mails."
  end

  private
  # parses all the e-mail, currently based off of evolution filenaming scheme
  def parse_all_mail( path, num_threads )
    filenames = Dir.glob( "#{path}/*." )
    threads = Array.new
    threads_count = Array.new( num_threads ) { Array.new }
    num_files_per_thread = filenames.size / num_threads
    extra_files = filenames.size - num_files_per_thread * num_threads

    1.upto( num_threads ) do |thread_num|
      start_index = (thread_num - 1) * num_files_per_thread
      filenames_per_thread =
        if num_threads == thread_num
          filenames[start_index .. filenames.size - 1]
        else
          filenames[start_index .. (thread_num * num_files_per_thread - 1)]
        end

      threads << Thread.new( filenames_per_thread ) do |files|
        files.each do |filename|
          mail = TMail::Mail.load_from( filename )
          threads_count[thread_num - 1].push( mail )
          #@mails.push( mail )
        end
      end
    end

    threads.each_with_index do |thread, i|
      thread.join
      @mails += threads_count[i]
    end

    @mails
  end
end
