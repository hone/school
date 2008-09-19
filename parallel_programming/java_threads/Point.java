public class Point {
	private int x, y;

	public synchronized void setXY( int x, int y ) {
		this.x = x;
		this.y = y;
	}
}
