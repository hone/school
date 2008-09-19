import java.util.Map;
import java.util.HashMap;

public class SimpleCache {
	private final Map cache = new HashMap();

	public Object load( String objectName ) {
		return null;
		// load the object somehow
	}

	public void clearCache() {
		synchronized( cache ) {
			cache.clear();
		}
	}

	public Object getObject( String objectName ) {
		Object obj = null;
		synchronized( cache ) {
			Object o = cache.get( objectName );
			if( o == null ) {
				o = load( objectName );
				cache.put( objectName, o );
			}

			obj = o;
		}

		return obj;
	}
}
