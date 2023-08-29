package gallery.things;

public class Artwork extends Thing{

	private String style;
	private String autor;
	private String dimensions;
	
	public Artwork(String title, String style, String autor, String dimensions) {
		super(title);
		this.style = style;
		this.autor = autor;
		this.dimensions = dimensions;
	}

	
	//getery
	public String getStyle() {
		return style;
	}

	public String getDimensions() {
		return dimensions;
	}

	public String getAutor() {
		return autor;
	}
	
}
