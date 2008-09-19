import java.util.Timer;
import java.util.TimerTask;

/**
 * CalculatePrimes2 -- calculate as many primes as we can in ten seconds, uses TimerTask
 */
public class CalculatePrimes2 extends Thread {
	public static final int MAX_PRIMES = 1000000;
	public static final int TEN_SECONDS = 10000;
	public static final int ONE_SECOND = 1000;

	public volatile boolean finished = false;

	public void run() {
		int[] primes = new int[MAX_PRIMES];
		int count = 0;

		for( int i = 2; count < MAX_PRIMES; i++ ) {
			// Check to see if the timer has expired
			if( finished ) {
				break;
			}

			boolean prime = true;
			for( int j = 0; j < count; j++ ) {
				if( i % primes[j] == 0 ) {
					prime = false;
					break;
				}
			}

			if( prime ) {
				primes[count++] = i;
				System.out.println( "Found prime: " + i );
			}
		}
	}

	public static void main( String[] args ) {
		Timer timer = new Timer();
		final CalculatePrimes calculator = new CalculatePrimes();
		calculator.start();

		timer.schedule(
				new TimerTask() {
					public void run()
					{
						calculator.finished = true;
					}
				}, TEN_SECONDS );
	}
}
