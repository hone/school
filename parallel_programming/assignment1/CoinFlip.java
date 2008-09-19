import java.util.Random;
import java.util.ArrayList;
import java.util.Iterator;

/**
 * Flips coins in parallel
 */
public class CoinFlip implements Runnable {
	private Random random;
	private int heads;
	private int totalRuns;
	private int runs;
	final static int HEADS = 0;
	final static int TAILS = 1;

	/**
	 * Constructor.
	 * @param runs number of times to do a flip.
	 */
	public CoinFlip( int runs ) {
		this.runs = runs;
		this.heads = 0;
		this.totalRuns = 0;
		random = new Random();
	}

	public int getHeads() {
		return this.heads;
	}

	public int getTotalRuns() {
		return this.totalRuns;
	}

	public void run() {
		for( int i = 0; i < runs; i++ ) {
			if( random.nextInt() % 2 == HEADS ) {
				this.heads++;
			}

			this.totalRuns++;
		}
	}

	public static void main( String[] args ) {

		int numOfThreads = 0,
			numOfIterations = 0,
			totalRuns = 0,
			totalHeads = 0;
		long startTime = System.currentTimeMillis(),
			 endTime = 0;

		if( args.length < 2 ) {
			System.err.println( "Need 2 arguments: #threads #iterations" );
			System.exit( 0 );
		}

		try {
			numOfThreads = Integer.parseInt( args[0] );
			numOfIterations = Integer.parseInt( args[1] );
		} catch( NumberFormatException e ) {
			System.err.println( "Not a proper number: " + e.getMessage() );
			System.exit( 0 );
		}

		ArrayList<Thread> threads = new ArrayList<Thread>( numOfThreads );
		ArrayList<CoinFlip> coins = new ArrayList<CoinFlip>( numOfThreads );
		for( int i = 0; i < numOfThreads; i++ )
		{
			CoinFlip coin = new CoinFlip( numOfIterations );
			Thread thread = new Thread( coin );

			coins.add( coin );
			threads.add( thread );

			thread.start();
		}

		for( int i = 0; i < numOfThreads; i++ )
		{
			Thread thread = threads.get( i );
			CoinFlip coin = coins.get( i );
			try {
				thread.join();
			} catch( InterruptedException e )
			{
				// should not get this error
				System.err.println( "Thread got interrupted: " + e.getMessage() );
			}
			totalHeads += coin.getHeads();
			totalRuns += coin.getTotalRuns();
		}
		endTime = System.currentTimeMillis();

		System.out.println( totalHeads + " heads in " + totalRuns + " coin tosses." );
		System.out.println( "Elapsed time: " + ( endTime - startTime ) + "ms" );
	}
}
