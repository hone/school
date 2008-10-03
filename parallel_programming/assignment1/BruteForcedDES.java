import java.util.ArrayList;
import java.util.Iterator;
import java.util.Random;
import javax.crypto.SealedObject;

/**
 * Brute forces a DES encryption
 */
class BruteForcedDES implements Runnable {
	final static int PRINT_TIME = 100000;
	final static int DEFAULT = 0;
	final static int PLOT = 1;

	private int id;
	private SealedObject sealedObject;
	private ArrayList<Long> keys;
	private SealedDES deccipher;
	private long runStart;

	/**
	 * Constructor.
	 * @param id id of the thread
	 * @param sealedObject object to be broken.
	 * @param keys keys to brute force through
	 * @param runStart starting run time
	 */
	public BruteForcedDES( int id, SealedObject sealedObject, ArrayList<Long> keys, long runStart ) {
		this.id = id;
		this.sealedObject = sealedObject;
		this.keys = keys;
		this.deccipher = new SealedDES();
		this.runStart = runStart;
	}

	public void run() {
		Iterator<Long> iterator = keys.iterator();
		while( iterator.hasNext() )
		{
			long key = iterator.next().longValue();
			// Set the key and decipher the object
			this.deccipher.setKey( key );
			String decryptstr = deccipher.decrypt( sealedObject );

			// Does the object contain the known plaintext
			if (( decryptstr != null ) && ( decryptstr.indexOf ( "Hopkins" ) != -1 ))
			{
				//  Remote printlns if running for time.
				//System.out.println (  "Thread " + this.id + " Found decrypt key " + key + " producing message: " + decryptstr );
			}

			// Update progress every once in awhile.
			//  Remote printlns if running for time.
			if ( key % PRINT_TIME == 0 )
			{ 
				long elapsed = System.currentTimeMillis() - runStart;
				//System.out.println ( "Thread " + this.id + " Searched key number " + key + " at " + elapsed + " milliseconds.");
			}
		}
	}

	// Program demonstrating how to create a random key and then search for the key value.
	public static void main( String[] args ) {
		if ( args.length < 2 )
		{
			System.out.println ("Usage: java SealedDES threads key_size_in_bits outputFormat iterationNumber");
			return;
		}

		// Get the argument
		int numOfThreads = Integer.parseInt( args[0] );
		long keybits = Long.parseLong( args[1] );
		int outputFormat = DEFAULT;
		int iterationNumber = 0;
		
		if( args.length > 2 ) {
			outputFormat = Integer.parseInt( args[3] );
			iterationNumber = Integer.parseInt( args[2] );
		}

		// Hideous weirdness to deal with the absence of unsigned ints
		//   and my desire to avoid an unnecessary exponentiation.
		//  This loop initialize max_keys to 0xFF when i=8
		long maxkey = 0;
		for ( int i=1; i<= keybits; i++ )
		{
			maxkey = maxkey << 1 | 0x01;
		}

		// Create a simple cipher
		SealedDES enccipher = new SealedDES();

		// Get a number between 0 and 2^32
		Random generator = new Random();
		long key =  generator.nextLong();

		// Mask off the high bits so we get a short key
		key = key & maxkey;

		// Set up a key
		enccipher.setKey ( key ); 

		// Generate a sample string
		String plainstr = "Johns Hopkins afraid of the big bad wolf?";

		// Encrypt
		SealedObject sldObj = enccipher.encrypt( plainstr );

		// Here ends the set-up.  Pretending like we know nothing except sldObj,
		// discover what key was used to encrypt the message.

		// Get and store the current time -- for timing
		long runstart;
		runstart = System.currentTimeMillis();

		// divide keyspace among threads
		ArrayList<ArrayList<Long>> keyRanges = new ArrayList<ArrayList<Long>>( numOfThreads );
		for( int i = 0; i < numOfThreads; i++ ) {
			keyRanges.add( new ArrayList<Long>() );
		}
		for( int i = 0; i < maxkey; i++ ) {
			int threadBin = ( int )i % numOfThreads;
			keyRanges.get( threadBin ).add( ( long )i );
		}

		// Search for the right key
		ArrayList<Thread> threads = new ArrayList<Thread>( numOfThreads );
		ArrayList<BruteForcedDES> crackers = new ArrayList<BruteForcedDES>( numOfThreads );
		for( int i = 0; i < numOfThreads; i ++ ) {

			BruteForcedDES cracker = new BruteForcedDES( i, sldObj, keyRanges.get( i ), runstart );
			Thread thread = new Thread( cracker );
			crackers.add( cracker );
			threads.add( thread );

			thread.start();
		}

		// Sync threads
		Iterator<Thread> iterator = threads.iterator();
		while( iterator.hasNext() ) {
			Thread thread = iterator.next();
			try {
				thread.join();
			}
			catch( InterruptedException e ) {
				System.err.println( "Thread interrupted: " + e.getMessage() );
			}
		}

		// Output search time
		long elapsed = System.currentTimeMillis() - runstart;
		long keys = maxkey + 1;

		switch( outputFormat ) {
			case PLOT:
				System.out.println( iterationNumber + "\t" + numOfThreads + "\t" + keys + "\t" + elapsed );
                break;
			default:
				System.out.println( "Completed search of " + keys + " keys at " + elapsed + " milliseconds.");
				System.out.println( "Final elapsed time: " + elapsed );
				break;
		}
	}
}
