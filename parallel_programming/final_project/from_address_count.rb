require 'mail_loader'
require 'base_job'

class FromAddressCount < BaseJob

  def self.run
    inbox_mail_loader = MailLoader.new( INBOX_FILE )
    old_messages_mail_loader = MailLoader.new( OLD_MESSAGES_FILE )
    data = build_data( inbox_mail_loader.mails + old_messages_mail_loader.mails )

    run_tests( 3, 3, data )
  end

  def self.map( mails )
    result = Array.new
    mails.each do |mail|
      result << [mail, 1]
    end

    result
  end

  def self.reduce( pairs )
    totals = Hash.new
    pairs.each do |pair|
      from, count = pair[0], pair[1]
      totals[from] ||= 0
      totals[from] += count
    end

    totals
  end

  def self.build_data( mails )
    mails.collect {|mail| mail.from }.flatten
  end
end
