import java.util.ArrayList;
import java.util.Iterator;

/**
 * Flips coins in parallel
 */
public class CoinFlip {
	static final int DEFAULT = 0;
	static final int PLOT = 1;

	static int numOfThreads;
	static int numOfIterations;
	static int numOfIterationsPerThread;
	static int numOfExtraIterations;
	static int outputFormat;
	static ArrayList<Thread> threads;
	static ArrayList<CoinFlipThread> coins;

	/**
	 * parse the command line arguments.  input1 is number of threads, input2 is the number of coinflips, input3 is the iteration run number.
	 * @param args command line arguments
	 */
	private static void parseArguments( String[] args ) {
		if( args.length < 2 ) {
			System.err.println( "Need 2 arguments: #threads #iterations" );
			System.exit( 0 );
		}

		// parse arguments from command line
		try {
			numOfThreads = Integer.parseInt( args[0] );
			numOfIterations = Integer.parseInt( args[1] );
			
			// parse output specifier
			if( args.length > 2 ) {
				outputFormat = Integer.parseInt( args[2] );
			} else {
				outputFormat = DEFAULT;
			}
		} catch( NumberFormatException e ) {
			System.err.println( "Not a proper number: " + e.getMessage() );
			System.exit( 0 );
		}
	}

	/**
	 * setup two ArrayLists.
	 */
	private static void setupArrayLists() {
		threads = new ArrayList<Thread>( numOfThreads );
		coins = new ArrayList<CoinFlipThread>( numOfThreads );
	}

	/**
	 * calculates the number of iterations per thread.
	 */
	private static void calculateIterationsPerThread() {
		numOfIterationsPerThread = numOfIterations / numOfThreads;
		if( numOfIterations % numOfThreads != 0 ) {
			numOfExtraIterations = numOfIterations - numOfThreads * numOfIterationsPerThread;
		}
	}

	/**
	 * create/setup threads.
	 */
	private static void createThreads() {
		for( int i = 0; i < numOfThreads; i++ )
		{
			// handle adding remaining extra threads to last thread
			if( numOfExtraIterations > 0 && i == numOfThreads - 1 ) {
				numOfIterationsPerThread += numOfExtraIterations;
			}
			CoinFlipThread coin = new CoinFlipThread( numOfIterationsPerThread );
			Thread thread = new Thread( coin );

			coins.add( coin );
			threads.add( thread );
		}
	}

	/**
	 * run threads
	 */
	public static void runThreads() {
		Iterator<Thread> iterator = threads.iterator();
		while( iterator.hasNext() ) {
			Thread thread = iterator.next();
			thread.start();
		}
	}

	public static void main( String[] args ) {
		int totalHeads = 0;
		long startTime = System.currentTimeMillis(),
			 endTime = 0;

		parseArguments( args );
		setupArrayLists();
		calculateIterationsPerThread();
		createThreads();
		runThreads();

		// wait for threads to finish and calculate head count
		for( int i = 0; i < numOfThreads; i++ )
		{
			Thread thread = threads.get( i );
			CoinFlipThread coin = coins.get( i );
			try {
				thread.join();
			} catch( InterruptedException e )
			{
				// should not get this error
				System.err.println( "Thread got interrupted: " + e.getMessage() );
			}
			totalHeads += coin.getHeads();
		}
		endTime = System.currentTimeMillis();
		long elapsedTime = endTime - startTime;

		switch( outputFormat ) {
			case DEFAULT:
				System.out.println( totalHeads + " heads in " + numOfIterations	+ " coin tosses." );
				System.out.println( "Elapsed time: " + elapsedTime + "ms" );
				break;
            default:
                // Print the Iteration, Thread Count, Elapsed Time for 3-way tab delimited columns
				System.out.println( outputFormat  + "\t\t" + numOfThreads + "\t" + elapsedTime );
                break;
		}
	}
}
