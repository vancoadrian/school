package gallery.persons;

import java.util.ArrayList;

import gallery.building.ExposurePlace;
import gallery.building.Gallery;
import gallery.building.Room;
import gallery.things.Artwork;
//zadanie 3 pridany interface upravene metody
public class Artist extends Person implements canRentExposure{
	private int exhibitionFee = 0;
	private String styleOfArt;
	private ArrayList<Artwork> works = new ArrayList<Artwork>();	//kompozicia
	private String aboutTheAuthor;
	private ArrayList<ExposurePlace> expositions = new ArrayList<ExposurePlace>();	//agregacia
	private Gallery gallery;	//asociacia
	
	public Artist(String name, String idCardNumber, String styleOfArt, Gallery gallery) {
		super(name, idCardNumber);
		this.styleOfArt = styleOfArt;
		System.out.println("Bol vytvoreny umelec [" + name + "].\n");
		this.gallery = gallery; 
	}
	
	
	public Artist(String name, String idCardNumber, String gender, String styleOfArt, Gallery gallery) {
		super(name, idCardNumber,gender);
		this.styleOfArt = styleOfArt;
		System.out.println("Bol vytvoreny umelec [" + name + "].\n");
		this.gallery = gallery;
	}
	
	public Artist(String name, String idCardNumber, int age, String styleOfArt, Gallery gallery) {
		super(name, idCardNumber,age);
		this.styleOfArt = styleOfArt;
		System.out.println("Bol vytvoreny umelec [" + name + "].\n");
		this.gallery = gallery;
	}

	
	//predstavenie (overriding)
	public void infoAboutMe() {
		System.out.println("Umelec [" + this.getName() + "] štýl: [" +this.getStyleOfArt() + "].\n");
	}
	
	
	//zadanie 3
	//vypis volnych miest na vystavenie v Gallery
	public void freeExposurePlaces() {
		boolean notExistFree = true;
		for( Room i : this.gallery.getRooms() ) {
			System.out.println("Volné miesta v Room ["+i.getTitle()+"]: ");
			for( ExposurePlace j : i.getPlaces() ) {
				if(j.isRented() == false) {
					notExistFree = false;
					System.out.println("ID: [" + j.getID() + "] cena [" + j.getPrice() + "].");
				}
			}
			if(notExistFree) {
				System.out.println("Nie su ziadne.\n");
			}
			notExistFree = true;
		}
	}
	
	
	//zadanie 3
	//ziadost o vystavenie 
	public void askForExposurePlaceForArt(String roomTitle, int exposureId, String artworkTitle, int lengthOfLease){
		for(Room i : this.gallery.getRooms()) {
			if( roomTitle == i.getTitle() ) {
				if(i.getResponsibleEmployee() != null) {
				i.getResponsibleEmployee().editExposurePlace(exposureId, this, artworkTitle, lengthOfLease);
				} else {
					System.out.println("Miestnost este nema prideleneho zamestnanca.");
				}
			}
		}
	}
	
	
	//zadanie 3
	//add
	public void addArtwork(String title, String style, String autor, String dimensions) {
		this.works.add(new Artwork(title, style, autor, dimensions));
	}
	
	public void addExposition(ExposurePlace exposition) {
		this.expositions.add(exposition);
	}
	
	
	//getery
	public int getExhibitionFee() {
		return exhibitionFee;
	}

	public String getStyleOfArt() {
		return styleOfArt;
	}

	public ArrayList<Artwork> getWorks() {
		return works;
	}

	public String getAboutTheAuthor() {
		return aboutTheAuthor;
	}
	
	public ArrayList<ExposurePlace> getExpositions() {
		return expositions;
	}
	
	
	
	//setery
	public void setAboutTheAuthor(String aboutTheAuthor) {
		this.aboutTheAuthor = aboutTheAuthor;
	}

	protected void setExpositions(ArrayList<ExposurePlace> expositions) {
		this.expositions = expositions;
	}


	@Override
	public void increaseFee(int fee) {
		this.exhibitionFee += fee;
	}
	
}
