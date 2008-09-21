import java.util.Random;

class CoinFlipThread implements Runnable {
	private Random random;
	private int heads;
	private int runs;
	final static int HEADS = 0;
	final static int TAILS = 1;

	/**
	 * Constructor.
	 * @param runs number of times to do a flip.
	 */
	public CoinFlipThread( int runs ) {
		this.runs = runs;
		this.heads = 0;
		random = new Random();
	}

	/**
	 * method that is run when start() is called by Thread
	 */
	public void run() {
		for( int i = 0; i < runs; i++ ) {
			if( random.nextInt() % 2 == HEADS ) {
				this.heads++;
			}
		}
	}

	public int getHeads() {
		return this.heads;
	}
}
