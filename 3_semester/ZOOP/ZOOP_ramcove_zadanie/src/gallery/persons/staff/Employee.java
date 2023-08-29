package gallery.persons.staff;

//zadanie 3  importy pridane pre ziskanie casu a prevedenie na string
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;

import gallery.building.ExposurePlace;
import gallery.building.Room;
import gallery.persons.Artist;
import gallery.persons.Person;
import gallery.things.Artwork;
//zadanie 3 implementovany interface Responsible
public class Employee extends Person implements Responsible{
	//zadanie 3 ID a STARTDATE na final pridany boolean responsible 
	private final int ID;
	private int salary;
	private int money = 0;
	private final int STARTDATE;
	private String employmentRelationship;
	private Artist artist;	//asociacia
	private Artwork artworkToAdd;	//asociacia
	private Room managedRoom;	//asociacia
	private boolean responsible = false;
	
	
	//ma pridelenu miestnost ktoru spravuje
	public Employee(String name, String idCardNumber, int id, int salary, int startDate, String employmentRelationship, Room managedRoom) {
		super(name, idCardNumber);
		this.ID = id;
		this.setSalary(salary);
		this.STARTDATE = startDate;
		this.employmentRelationship = employmentRelationship;
		this.managedRoom = managedRoom;
		this.responsible = true;
		managedRoom.setHaveResponsibleEmployee(true);
		System.out.println("Bol vytvoreny zamestnanec [" + name + "].\n");
	}
	//ma pridelenu miestnost ktoru spravuje a udava pohlavie
	public Employee(String name, String idCardNumber,String gender, int id, int salary, int startDate, String employmentRelationship, Room managedRoom) {
		super(name, idCardNumber,gender);
		this.ID = id;
		this.setSalary(salary);
		this.STARTDATE = startDate;
		this.employmentRelationship = employmentRelationship;
		this.managedRoom = managedRoom;
		this.responsible = true;
		managedRoom.setHaveResponsibleEmployee(true);
		System.out.println("Bol vytvoreny zamestnanec [" + name + "].\n");
	}

	//zadanie 3
	public void createExposurePlace(String type, int price, int id){
		if(this.responsible == true) {
			this.managedRoom.addPlace(type, price, id);
			System.out.println("Zamestnanec [" + this.getName() + "] vytvoril v Room [" + this.managedRoom.getTitle() +"] ExposurePlace [" + type + "] s cenou " + price +".\n");
		} else {
		System.out.println("Zamestanec [" + this.getName() + "] nemoze vytvarat pretoze nema pridelenu Room.\n");
		}
	}
	//zadanie 3
	public void deleteExposurePlace(int id) {
		if(this.responsible == true) {
			for(ExposurePlace i : this.managedRoom.getPlaces()) {
				if(id == i.getID()) {
					this.managedRoom.removePlace(i);
					System.out.println("Zamestanec [" + this.getName() + "] odstranil ExposurePlace id: ["+ id +"] z Room [" + this.managedRoom.getTitle() +"].\n");
					break;
				}
				System.out.println("Zamestanec [" + this.getName() + "] nenasiel ExposurePlace id: ["+ id +"] v Room [" + this.managedRoom.getTitle() +"].\n");
			}			
		} else {
		System.out.println("Zamestanec [" + this.getName() + "] nemoze odstranit ExposurePlace pretoze nema pridelenu Room.\n");
		}
	}
	//zadanie 3
	public void editExposurePlace(int id, int price) {
		if(this.responsible) {
			for(ExposurePlace i : this.managedRoom.getPlaces() ) {
				if( id == i.getID() ) {
					i.setPrice(price);
					break;
				}
				System.out.println("Zamestanec [" + this.getName() + "] nenasiel ExposurePlace ["+ id +"] na edit v Room [" + this.managedRoom.getTitle() +"].\n");
			}
		}else {
			System.out.println("Zamestanec [" + this.getName() + "] nemoze editovat ExposurePlace pretoze nema pridelenu Room.\n");
		}
	}
	
	//zadanie 3
	public void editExposurePlace(int exposureId, Artist artist, String artworkTitle, int lengthOfLease) {
		boolean badId = true, badTitle = true;
		for( ExposurePlace i : this.managedRoom.getPlaces( )) {
			if( exposureId == i.getID() ) {
				badId = false;
				for( Artwork j : artist.getWorks() ) {
					if( artworkTitle == j.getTitle() ) {
						badTitle = false;
						i.setRenter(artist);
						i.setArtwork(j);
						i.setRented(true);
						String pattern = "MM/dd/yyyy";
						DateFormat df = new SimpleDateFormat(pattern);
						Date today = Calendar.getInstance().getTime();
						i.setStartOfLease(df.format(today));
						i.setLengthOfLease(lengthOfLease);
						this.wasExposed(artist,i.getPrice());
						artist.addExposition(i);
					}
				}
				if(badTitle) {
					System.out.println("Zle zadany nazov diela.\n");
				}
			}
		}
		if(badId) {
			System.out.println("Zle zadané ID expozicneho miesta.\n");
		}	
	}
	
	//zadanie 3 upravene
	//predstavenie (overriding)
		@Override
		public void infoAboutMe() {
				System.out.println("Zamestanec [" + this.getName() + "] id: [" + this.ID + "] plat: [" + this.salary + "].\n");
		}
		//zadanie 3 upravene
		@Override
		public void wasExposed(Artist artist,int price) {
			artist.increaseFee(price);
			System.out.println("Prave bolo vystavené dielo zamestnancom [" + this.getName() + "].");
		}
	
	//add
	/*TODO
	private void addMoney(int money) {
		this.money += money;
	}
	*/
	
	
	//getery
	public int getId() {
		return ID;
	}

	public int getSalary() {
		return salary;
	}
	
	public int getMoney() {
		return money;
	}
	
	public int getStartDate() {
		return STARTDATE;
	}
	
	public Artwork getArtworkToAdd() {
		return artworkToAdd;
	}
	
	public String getEmploymentRelationship() {
		return employmentRelationship;
	}

	public Artist getArtist() {
		return artist;
	}
	
	public Room getManagedRoom() {
		return managedRoom;
	}
	
	
	
	//setery
	protected void setSalary(int salary) {
		this.salary = salary;
	}

	public void setArtist(Artist artist) {
		this.artist = artist;
	}

	public void setArtworkToAdd(Artwork artworkToAdd) {
		this.artworkToAdd = artworkToAdd;
	}

	public void setManagedRoom(Room managedRoom) {
		this.managedRoom = managedRoom;
	}
}
