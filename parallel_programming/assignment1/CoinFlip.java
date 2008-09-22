import java.util.ArrayList;
import java.util.Iterator;

/**
 * Flips coins in parallel
 */
public class CoinFlip {
	static int numOfThreads;
	static int numOfIterations;
	static int numOfIterationsPerThread;
	static int numOfExtraIterations;
	static ArrayList<Thread> threads;
	static ArrayList<CoinFlipThread> coins;

	/**
	 * parse the command line arguments
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
		long startTime = System.nanoTime(),
			 endTime = 0;

		parseArguments( args );
		setupArrayLists();
		calculateIterationsPerThread();
		long fixedStartupCostEndTime = System.nanoTime();
		long fixedStartupCost = fixedStartupCostEndTime - startTime;
		createThreads();
		long startupCostTotalThreads = System.nanoTime() - fixedStartupCostEndTime;
		System.out.println( "Fixed Startup Cost: " + fixedStartupCost );
		System.out.println( "Per Thread Startup Cost: " + startupCostTotalThreads );
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
		endTime = System.nanoTime();

		System.out.println( totalHeads + " heads in " + numOfIterations	+ " coin tosses." );
		System.out.println( "Elapsed time: " + ( endTime - startTime ) + "ns" );
	}
}
