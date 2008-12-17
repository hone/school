class BaseJob
  include SkynetDebugger # provides logging methods

  INBOX_FILE = "inbox.dump"
  SENT_FILE = "sent.dump"
  OLD_MESSAGES_FILE = "old_messages.dump"

  def self.run_job( mappers, reducers, data )
    start_time = Time.now
    job = Skynet::Job.new(
      :mappers => mappers,
      :reducers => reducers,
      :map_reduce_class => self,
      :map_data => data
    )
    results = job.run
    end_time = Time.now

    [results, end_time - start_time]
  end

  def self.run_tests( max_mapper, max_reducer, data )
    results = Array.new

    1.upto( max_mapper ) do |m|
      1.upto( max_reducer ) do |r|
        result = run_job( m, r, data )
        puts "#{m} #{r} #{result[1]}"
        results << result
      end
    end

    results
  end
end
