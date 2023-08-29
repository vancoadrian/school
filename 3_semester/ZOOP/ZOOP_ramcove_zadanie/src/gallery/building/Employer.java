package gallery.building;

import gallery.persons.Artist;
import gallery.persons.Person;
import gallery.persons.staff.Employee;

public class Employer extends Person {
	//upravene pri zadani 3 na finals
	private final int ORGANIZATIONIDENTIFICATIONNUMBER;	//IÈO (OIN)
	private final String TAXIDENTIFICATIONNUMBER;	//DIÈ (TIN)
	private final String VALUEADDEDTAXIDENTIFICATIONNUMBER;	//IÈ DPH (IN_VAT)
	public final Gallery GALLERY;	//agregacia
	//TODO: private Account account;
		
	public Employer(String name, String idCardNumber, String gender, int age, int OIN, String TIN, String IN_VAT, Gallery gallery){
		super(name,idCardNumber,gender,age);
		this.GALLERY = gallery;
		this.ORGANIZATIONIDENTIFICATIONNUMBER = OIN;
		this.TAXIDENTIFICATIONNUMBER = TIN;
		this.VALUEADDEDTAXIDENTIFICATIONNUMBER = IN_VAT;
	}
	
	
	
	//upravene pri zadani 3
	//predstavenie (overriding)
	@Override
	public void infoAboutMe() {
		System.out.println("Zamestnávatel  [" + this.getName() + "], galéria [" + this.GALLERY.getTitle() + "]");
		System.out.println("	OIN: "+ this.getOrganizationIdentificationNumber());
		System.out.println("	TIN: "+ this.getTaxIdentificationNumber());
		System.out.println("	IN_VAT: "+ this.getValueAddedTaxIdentificationNumber()+"\n");
	}
	
	public void fireEmployee(Employee employee) {
		if( this.GALLERY.employees.remove(employee) ) {
		System.out.println("Employee [" + employee.getName() + "] bol odstraneny z Gallery.\n");
		}
		else {
			System.out.println("Employee [" + employee.getName() + "] sa nenasiel v Gallery.\n");
		} 
	}
	
	//upravene pri zadani 3
	public void fireEmployee(String name) {
		for(Employee i : this.GALLERY.employees) {
			if(name == i.getName()) {
				this.GALLERY.employees.remove(i);
				System.out.println("Employee [" + name + "] bol odstraneny z Gallery.\n");
				break;
			}
		}
		System.out.println("Employee [" + name + "] sa nenasiel v Gallery.\n");
	}
	
	
	
	//pridane pri zadani 3
	//vytvorenie Room v Gallery
	public void createRoom(String title) {
		this.GALLERY.addRoom(title);
	}
	
	//pridane pri zadani 3
	//zmazanie Room v Gallery
	public void deleteRoom(String title) {
		this.GALLERY.removeRoom(title);
	}
	
	//pridane pri zadani 3
	public  void editRoom(String oldTitle, String newTitle) {
		boolean notExist = true;
		for(Room i: this.GALLERY.getRooms() ) {
			if( oldTitle == i.getTitle() ) {
				System.out.println("Room [" + oldTitle + "] bola premenovana na [" + newTitle + "].\n");
				i.setTitle(newTitle);
				notExist = false;
				break;
			}
		}
		if(notExist) {
		System.out.println("Room [" + oldTitle + "] sa nenachadza v Gallery.\n");
		}
	}
	
	//pridane pri zadani 3
	public void editRoom(String title, Employee employee) {
		for(Room i: this.GALLERY.getRooms() ) {
			if( title == i.getTitle() ) {
				if(i.isHaveResponsibleEmployee()) {
					i.setResponsibleEmployee(employee);
					System.out.println("V Room [" + title + "] bol zmeneny zodpovedny zamestnanec na [" + employee.getName() +"].");
				} else {
					i.setHaveResponsibleEmployee(true);
					i.setResponsibleEmployee(employee);
					System.out.println("Room [" + title + "] bola  pridelena zamestnancovi [" + employee.getName() +"].");
				}
				break;
			}
		}
	}
	
	//pridane pri zadani 3
	public void findArtist(String name) {
		boolean isNotInGallery = true;
		for( Artist i : this.GALLERY.getArtists() ) {
			if( name == i.getName() ) {
				System.out.println("Artist " + name + " sa nachádza v Gallery.\n");
				isNotInGallery = false;
				break;
				//dalej rozsiritelne o rozne informacie napr. ci umelec vystavuje
			}
		}
		if(isNotInGallery) {
			System.out.println("Artist " + name + " sa nenachádza v Gallery.\n");
		}
	}
	
	//pridane pri zadani 3
	public void findArtwork(String title) {
		boolean isNotExposed = true;
		for( Room i : this.GALLERY.getRooms() ) {
			for( ExposurePlace j : i.getPlaces() ) {
				if( j.getArtwork().getTitle() == title ) {
					System.out.println("Obraz ["+ title +"] sa vystavuje v Room [" + i.getTitle() + "].\n");
					isNotExposed = false;
					break;
				}
			}
		}
		if(isNotExposed) {
			System.out.println("Obraz ["+ title +"] sa momentalne nevystavuje.\n");
		}
	}
	
	//TODO: public void stats() {};
	
	//upravene pri zadani 3
	//upcasting 
	public void addPersonToGallery(Person person){
		this.GALLERY.addPersonToRightArray(person);
	}


	public void removeArtist(Artist artist) {
		this.GALLERY.artists.remove(artist);
	}
	
	//getery
	public int getOrganizationIdentificationNumber() {
		return ORGANIZATIONIDENTIFICATIONNUMBER;
	}

	public String getValueAddedTaxIdentificationNumber() {
		return VALUEADDEDTAXIDENTIFICATIONNUMBER;
	}

	public String getTaxIdentificationNumber() {
		return TAXIDENTIFICATIONNUMBER;
	}

	public Gallery getGallery() {
		return GALLERY;
	}
	
}
