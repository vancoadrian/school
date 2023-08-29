package gallery.building;

import java.util.ArrayList;

import gallery.persons.Artist;
import gallery.persons.Person;
import gallery.persons.staff.Employee;

public final class Gallery {
	private ArrayList<Room> rooms = new ArrayList<Room>();	//kompozícia
	protected ArrayList<Employee> employees = new ArrayList<Employee>();	//agregacia
	protected ArrayList<Artist> artists = new ArrayList<Artist>();	//agregacia
	private  String title;
	
	private static Gallery gallery = new Gallery("Galeria Umenia"); //singleton	pridane pri zadani 3
	private Gallery(String title) {
		this.title = title;
	}	
	
	
	//add upravene pri zadani 3
	public void addRoom(String title) {
		this.rooms.add(new Room(title));
		System.out.println("Bola vytvorena miestnost [" + title + "] v Gallery [" + this.title + "].\n");
	}
	

			//downcasting	pridane pri zadani 3
	public void addPersonToRightArray(Person person) {
		if(person instanceof Employee) {
			employees.add( (Employee) person);
			System.out.println("Employee [" + person.getName() + "] bol pridany do Gallery [" + this.title + "].\n");
		}
		if(person instanceof Artist) {
			artists.add( (Artist) person);
			System.out.println("Artist [" + person.getName() + "] bol pridany do Gallery [" + this.title + "].\n");
		}
	}	
	
	
	
	//getery	
	public ArrayList<Room> getRooms() {
		return rooms;
	}
	
	public ArrayList<Employee> getEmployees() {
		return employees;
	}
	
	protected ArrayList<Artist> getArtists() {
		return artists;
	}
	
	public String getTitle() {
		return title;
	}
	
	
	
	//setery
	protected void setTitle(String title) {
		this.title = title;
	}
	//upravene pri zadani 3
	protected void setEmployees(ArrayList<Employee> employees) {
		this.employees = employees;
		System.out.println("Do Gallery [" + this.title + "] sa nahrali zamestnanci.\n");
	}
	//upravene pri zadani 3
	protected void setArtists(ArrayList<Artist> artists) {
		this.artists = artists;
		System.out.println("Do Gallery [" + this.title + "] sa nahrali umelci.\n");
	}

	
	
	//remove	pridane pri zadani 3
	public void removeRoom(String title) {
		for (Room i : rooms) {
			if( title == i.getTitle() )
				rooms.remove(i);
			System.out.println("Room [" + title + "] bola odstranena z Gallery [" + this.title + "].\n");
			break;
		}
		System.out.println("Room [" + title + "] sa nenachádza v Gallery ["+  this.title + "].\n");
	}

	
	
	// vypitanie singletonu	pridane pri zadani 3
	public static Gallery getGallery() {
		return gallery;
	}
}
