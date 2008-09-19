public class Counter {
	private int counter = 0;

	public int get() {
		return counter;
	}
	
	public void set( int n ) {
		counter = n;
	}
	
	public void increment() {
		set( get() + 1 );
	}
}
