require 'mail_entry'

class MailUtility
  def initialize( mails )
    # array of all the mail
    @mails = mails
    # key: from address, value: mail_entry
    @from_hash = Hash.new do |hash, key|
      hash[key] = MailEntry.new( 0, Array.new )
    end
  end
  
  def parse_mail
    @mails.each do |mail|
      mail.from.each do |from_address|
        mail_entry = @from_hash[from_address.downcase.to_sym]
        mail_entry.count += 1
        mail_entry.dates.push( mail.date )
      end
    end

    @from_hash.keys.sort {|a, b| a.to_s <=> b.to_s }.each do |from|
      puts "#{from} : #{from_hash[from].count}"
    end
  end
end
