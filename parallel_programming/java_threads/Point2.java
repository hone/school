public class Point2 {
	private int x, y;

	public void setXY( int x, int y ) {
		synchronized( this ) {
			this.x = x;
			this.y = y;
		}
	}
}
