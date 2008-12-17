require 'mail_loader'
require 'base_job'

class TimeRange < Struct.new( :start, :end, :index ); end

class TimeCount < BaseJob

  EARLY_MORNING = TimeRange.new( 0, 7, 1 )
  BUSINESS_MORNING = TimeRange.new( 8, 12, 2 )
  BUSINESS_AFTERNOON = TimeRange.new( 13, 17, 3 )
  EVENING = TimeRange.new( 18, 21, 4 )
  LATE_EVENING = TimeRange.new( 22, 24, 5 )

  def self.run
    old_mail_loader = MailLoader.new( OLD_MESSAGES_FILE )
    inbox_mail_loader = MailLoader.new( INBOX_FILE )
    data = build_data( old_mail_loader.mails + inbox_mail_loader.mails )

    run_tests( 3, 3, data )
  end

  def self.map( mails )
    result = Array.new
    mails.each do |mail|
      case mail
      when EARLY_MORNING.start .. EARLY_MORNING.end
        result << [EARLY_MORNING.index, 1]
      when BUSINESS_MORNING.start .. BUSINESS_MORNING.end
        result << [BUSINESS_MORNING.index, 1]
      when BUSINESS_AFTERNOON.start .. BUSINESS_AFTERNOON.end
        result << [BUSINESS_AFTERNOON.index, 1]
      when EVENING.start .. EVENING.end
        result << [EVENING.index, 1]
      when LATE_EVENING.start .. LATE_EVENING.end
        result << [LATE_EVENING.index, 1]
      else
        raise "Invalid hour parameter: #{mail}"
      end
    end

    result
  end

  def self.reduce(pairs)
    totals = Hash.new
    pairs.each do |pair|
      time, count = pair[0], pair[1]
      totals[time] ||= 0
      totals[time] += count
    end

    totals
  end

  def self.build_data( mails )
    mails.collect {|mail| mail.date.hour }
  end
end
