require 'set'
require 'mail_loader'
require 'base_job'

class AddressConnection < BaseJob

  def self.run
    inbox_mail_loader = MailLoader.new( INBOX_FILE )
    old_messages_mail_loader = MailLoader.new( OLD_MESSAGES_FILE )
    data = build_data( mail_loader.mails )

    results = run_tests( 3, 3, data )
    results.collect {|result| result[1] }
  end

  def self.map( mails )
    result = Array.new
    mails.each do |mail|
      mail.each do |address1|
        mail.each do |address2|
          result << [address1, address2] if address1 != address2
        end
      end
    end

    result
  end

  def self.reduce( pairs )
    totals = Hash.new
    pairs.each do |pair|
      address1, address2 = pair[0], pair[1]
      totals[address1] ||= Set.new
      totals[address1] << address2
    end

    totals
  end

  def self.build_data( mails )
    addresses = mails.collect do |mail|
      address_list = Array.new

      [:from, :cc, :bcc, :to].each do |symbol|
        list = mail.send( symbol )
        address_list += list if list
      end

      address_list
    end
  end
end
