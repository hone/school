public class UnsafeStack {
	public int top = 0;
	public int[] values = new int[1000];

	public void push( int n ) {
		values[top++] = n;
	}

	public int pop() {
		return values[--top];
	}
}
