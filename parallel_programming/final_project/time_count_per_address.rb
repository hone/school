require 'mail_loader'
require 'base_job'

class MailEntry < Struct.new( :from, :time ); end
class TimeRange < Struct.new( :start, :end, :index ); end

class TimeCountPerAdress < BaseJob
  EARLY_MORNING = TimeRange.new( 0, 7, 1 )
  BUSINESS_MORNING = TimeRange.new( 8, 12, 2 )
  BUSINESS_AFTERNOON = TimeRange.new( 13, 17, 3 )
  EVENING = TimeRange.new( 18, 21, 4 )
  LATE_EVENING = TimeRange.new( 22, 24, 5 )

  def self.run
    inbox_mail_loader = MailLoader.new( INBOX_FILE )
    old_messages_mail_loader = MailLoader.new( OLD_MESSAGES_FILE )
    data = build_data( inbox_mail_loader.mails + old_messages_mail_loader.mails )

    results = run_tests( 3, 3, data )
    results.collect {|result| result[1] }
  end

  def self.map( mails )
    result = Array.new
    mails.each do |mail|
      case mail.time
      when EARLY_MORNING.start .. EARLY_MORNING.end
        result << ["#{mail.from} | #{EARLY_MORNING.index}", 1]
      when BUSINESS_MORNING.start .. BUSINESS_MORNING.end
        result << ["#{mail.from} | #{BUSINESS_MORNING.index}", 1]
      when BUSINESS_AFTERNOON.start .. BUSINESS_AFTERNOON.end
        result << ["#{mail.from} | #{BUSINESS_AFTERNOON.index}", 1]
      when EVENING.start .. EVENING.end
        result << ["#{mail.from} | #{EVENING.index}", 1]
      when LATE_EVENING.start .. LATE_EVENING.end
        result << ["#{mail.from} | #{LATE_EVENING.index}", 1]
      else
        raise "Invalid hour parameter: #{mail}"
      end
    end

    result
  end

  def self.reduce(pairs)
    totals = Hash.new
    pairs.each do |pair|
      index, count = pair[0], pair[1]
      totals[index] ||= 0
      totals[index] += count
    end

    totals
  end

  def self.build_data( mails )
    mails.collect {|mail| MailEntry.new( mail.from, mail.date.hour ) }.flatten
  end
end
