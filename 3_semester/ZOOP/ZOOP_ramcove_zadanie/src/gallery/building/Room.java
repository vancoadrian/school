package gallery.building;

import java.util.ArrayList;

import gallery.persons.staff.Employee;

public class Room{
	private String title;
	private ArrayList<ExposurePlace> places = new ArrayList<ExposurePlace>(); //kompozicia
	private Employee responsibleEmployee;
	private boolean haveResponsibleEmployee = false;	//pridane pri zadani 3
	
	public Room(String title){
		this.setTitle(title);
	}

	//upravene pri zadani 3
	//add
	public void addPlace(String type, int price,int id) {
		this.places.add(new ExposurePlace(type,price,id));
	}
	
	
	
	//remove
	public boolean removePlace(ExposurePlace place) {
		return this.places.remove(place);
	}
	
	
	
	//getery
	public String getTitle() {
		return title;
	}
	
	public boolean isHaveResponsibleEmployee() {
		return haveResponsibleEmployee;
	}

	public ArrayList<ExposurePlace> getPlaces() {
		return places;
	}

	
	public Employee getResponsibleEmployee() {
		return responsibleEmployee;
	}
	
	
	
	//setery
	public void setTitle(String title) {
		this.title = title;
	}
	
	public void setResponsibleEmployee(Employee employee) {
		this.responsibleEmployee = employee;
	}

	public void setHaveResponsibleEmployee(boolean haveResponsibleEmployee) {
		this.haveResponsibleEmployee = haveResponsibleEmployee;
	}	
}
