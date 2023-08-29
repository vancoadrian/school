package gallery;

import gallery.building.Employer;
import gallery.building.Gallery;
import gallery.persons.Artist;
import gallery.persons.staff.Employee;
//zadanie 3 vytvorena cela class a vytvorenie main na simulovanie.
public class GallerySystem {
	
private static GallerySystem GallerySystem = new GallerySystem(); //singleton
	private GallerySystem() {
	}
	
	public static void main(String[] args) {
		Gallery milanovaGallery = Gallery.getGallery();
		Employer Milan = new Employer("Milan Vehli", "AB75945", "Male", 34, 283355, "SK8282828", "DFC28173926",milanovaGallery);
		
		Milan.infoAboutMe();
		
		Milan.GALLERY.addRoom("Rage");
		Milan.GALLERY.addRoom("Delirium");
		Milan.editRoom("Rage","Rage2");
		Milan.editRoom("DETVA","DETVA2");
		
		System.out.println("Rooms v Gallery " + Milan.GALLERY.getRooms()+"\n");
		
		Employee Jozef = new Employee("Jozef Stoh", "CKD282873", 1, 500, 2, "Dve zmeny", Milan.GALLERY.getRooms().get(1));
		Employee Peter = new Employee("Peter Stoh", "CKD282872","Male" ,1, 500, 2, "Dve zmeny", Milan.GALLERY.getRooms().get(0));
		Milan.GALLERY.getRooms().get(0).setResponsibleEmployee(Peter);
		Milan.GALLERY.getRooms().get(1).setResponsibleEmployee(Jozef);
		
		Milan.addPersonToGallery(Jozef);
		Milan.addPersonToGallery(Peter);
		Milan.addPersonToGallery(Peter);
		
		Jozef.createExposurePlace("Jedlo" , 90, 625653);
		Peter.createExposurePlace("Total Emotions" , 11190, 625654);
		
		Peter.deleteExposurePlace(625654);
		
		
		Jozef.infoAboutMe();
		Peter.infoAboutMe();
		
		
		Artist DaVinci = new Artist("Leonardo DaVinci","GHF233","Cmaranice",milanovaGallery);
		Artist VanGogh = new Artist("Vincent VanGogh","GHF233","Olejomalby",milanovaGallery);
		
		Milan.addPersonToGallery(DaVinci);
		
		
		DaVinci.addArtwork("MonaLisa", "Portret", "DaVinci", "100x100x1");
		VanGogh.addArtwork("HviezdnaNoc", "Olejomalba", "VanGogh", "50x40x1");
		
		
		DaVinci.freeExposurePlaces();
		
		DaVinci.askForExposurePlaceForArt("Delirium", 625653, "MonaLisa", 3);
		
		
		System.out.println("employees" + Milan.GALLERY.getEmployees());
		Milan.fireEmployee(Jozef);
		Milan.fireEmployee("Peter");
		System.out.println("employees" + Milan.GALLERY.getEmployees());		
	}
}
