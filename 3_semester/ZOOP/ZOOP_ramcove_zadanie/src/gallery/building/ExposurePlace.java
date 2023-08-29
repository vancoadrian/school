package gallery.building;

import java.util.ArrayList;

import gallery.finance.Fee;
import gallery.persons.Artist;
import gallery.things.Artwork;
import gallery.things.Thing;

public class ExposurePlace{
	//upravene pri zadani 3 pridane isRented,Fee, final ID, doplnene getery setery
	private Artist renter;	//agregacia
	private boolean isRented = false;
	private int price;
	private final int ID;
	//TODO: ArrayList<Fee> fees;
	public String type;
	public ArrayList<Thing> equipment = new ArrayList<Thing>(); //agregacia
	private Artwork artwork;	//agregacia
	private String startOfLease;
	private int lengthOfLease;
	private Fee fee;	//kompozícia
	//TODO: int lastPayment;
	
	public ExposurePlace(String type, int price,int id) {
		this.fee = new Fee(price);
		this.type = type;
		this.price = price;
		this.ID = id;
	}
	
	
	//getery
	public int getID(){
		return this.ID;
	}
	
	public int getPrice(){
		return this.price;
	}

	public int getLengthOfLease() {
		return lengthOfLease;
	}
	
	public String getStartOfLease() {
		return startOfLease;
	}
	
	public boolean isRented() {
		return isRented;
	}

	public Artwork getArtwork() {
		return artwork;
	}

	public Artist getRenter() {
		return renter;
	}
	
	public void setRenter(Artist renter) {
		this.renter = renter;
	}

	
	
	//setery
	public void setRented(boolean isRented) {
		this.isRented = isRented;
	}

	public void setArtwork(Artwork artwork) {
		this.artwork = artwork;
	}


	public void setStartOfLease(String startOfLease) {
		this.startOfLease = startOfLease;
	}

	public void setPrice(int price) {
		this.price = price;
	}
	

	public void setLengthOfLease(int lenghtOfLease) {
		this.lengthOfLease = lenghtOfLease;
	}	
}
