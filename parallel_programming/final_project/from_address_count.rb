require 'mail_loader'

class FromAddressCount
  include SkynetDebugger # provides logging methods

  def self.run
    mail_loader = MailLoader.new( MailLoader::SERIALIZED_FILE )
    from_addresses = mail_loader.mails.collect {|mail| mail.from }.flatten

    job = Skynet::Job.new(
      :mappers => 2,
      :reducers => 1,
      :map_reduce_class => self,
      :map_data => from_addresses
    )

    results = job.run
  end

  def self.map(mails)
    result = Array.new
    mails.each do |mail|
      result << [mail, 1]
    end

    result
  end

  def self.reduce(pairs)
    totals = Hash.new
    pairs.each do |pair|
      from, count = pair[0], pair[1]
      totals[from] ||= 0
      totals[from] += count
    end

    totals
  end
end
