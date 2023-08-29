package gallery.persons.staff;

import gallery.persons.Artist;
//zadanie 3 cely interface
public interface Responsible {
	public void  wasExposed(Artist artist,int price);
	static void howMuchTotalRentPrice(int price, int months) {
		if(price <= 0 || months <= 0) {
			System.out.println("Vsetko musi byt kladne a vacsie ako nula\n");
		} else {
			int discount = 0;
			if(months>2 && months<=4) { discount = price / 40;}
			else if(months>4) { discount = price / 25;}
			System.out.println("Vypocitana cena pre prenajom s cenou [" + price + "] na pocet mesiacov ["+months+"] je [" + (price*(months-discount)) + "].\n");
		}
	}
}
