import java.io.IOException;
import java.util.Iterator;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import sun.misc.BASE64Encoder;

import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.JobClient;
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.mapred.TextInputFormat;
import org.apache.hadoop.mapred.TextOutputFormat;
import org.apache.hadoop.mapred.FileInputFormat;
import org.apache.hadoop.mapred.FileOutputFormat;
import org.apache.hadoop.mapred.Reporter;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.Reducer;

public class LineCount {
	public static class Map extends MapReduceBase implements Mapper<LongWritable, Text, Text, IntWritable> {
		private final static String HASH_ALGORITHM = "SHA1";
		private final static IntWritable ONE = new IntWritable( 1 );
		private Text text = new Text();
		private MessageDigest md_sha = null;

		public void map( LongWritable position, Text line, OutputCollector<Text, IntWritable> output, Reporter reporter ) throws IOException {
			String line_string = line.toString();
			int line_size = line_string.length();
			String line_hash = HashSHA1( line_string ) + " " + line_size;
			this.text.set( line_hash );
			output.collect( this.text, ONE );
		}

		// Return a string that is Base64 encoded version of the SHA-1 hash of the input String
		private String HashSHA1( String input ) {
			// Initialize the secure hash functions
			if( null == this.md_sha ) {
				try {
					this.md_sha = MessageDigest.getInstance( HASH_ALGORITHM );
				} catch( NoSuchAlgorithmException e ) {
					System.out.println( "Should never get here." );
					assert( false );
				}
			}

			this.md_sha.reset();
			this.md_sha.update( input.getBytes() );
			byte[] digestbytes = md_sha.digest();
			String digeststr = new BASE64Encoder().encode( digestbytes );

			return digeststr;
		}
	}

	public static class Reduce extends MapReduceBase implements Reducer<Text, IntWritable, Text, IntWritable> {
		public void reduce( Text key, Iterator<IntWritable> values, OutputCollector<Text, IntWritable> output, Reporter reporter) throws IOException {
			int sum = 0;
			while( values.hasNext() ) {
				sum += values.next().get();
			}
			output.collect( key, new IntWritable( sum ) );
		}
	}

	public static void main( String[] args ) throws IOException {
		JobConf conf = new JobConf( LineCount.class );
		conf.setJobName( "wordcount" );

		conf.setOutputKeyClass( Text.class );
		conf.setOutputValueClass( IntWritable.class );

		conf.setMapperClass( Map.class );
		conf.setCombinerClass( Reduce.class );
		conf.setReducerClass( Reduce.class );

		conf.setInputFormat( TextInputFormat.class );
		conf.setOutputFormat( TextOutputFormat.class );

		FileInputFormat.setInputPaths( conf, new Path( args[0] ) );
		FileOutputFormat.setOutputPath( conf, new Path( args[1] ) );

		JobClient.runJob( conf );
	}
}
