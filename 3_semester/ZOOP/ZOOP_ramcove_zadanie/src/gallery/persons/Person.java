package gallery.persons;
//zadanie 3 abstract class
public abstract class Person {
	private String name;
	private String gender;
	private String idCardNumber;
	private int age;
	
	//uviedol minimum udajov
	public Person(String name, String idCardNumber){
		this.name = name;
		this.idCardNumber = idCardNumber;
	}
	
	//uviedol minimum udajov a pohlavie 
	public Person(String name, String idCardNumber, String gender){
		this.name = name;
		this.idCardNumber = idCardNumber;
		this.gender = gender;
	}
	//uviedol minimum udajov a rok (overloading s predchadzajucim)
	public Person(String name, String idCardNumber, int age){
		this.name = name;
		this.idCardNumber = idCardNumber;
		this.age = age;
	}
	//uviedol vsetky osobne udaje a rok
	public Person(String name, String idCardNumber, String gender, int age){
		this.name = name;
		this.idCardNumber = idCardNumber;
		this.gender = gender;
		this.age = age;
	}
	
	//upravene pri zadani 3
	//predstavenie
	public abstract void infoAboutMe();

	
	//getery
	public String getName() {
		return name;
	}

	public String getIdCardNumber() {
		return idCardNumber;
	}

	public String getGender() {
		return gender;
	}

	public int getAge() {
		return age;
	}
	
}
