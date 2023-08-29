package gallery.finance;

import gallery.persons.Artist;
//Class nedokoncena[implementovana] ale zatial nema vplyv (TODO) 
public class Fee {
	//pridany konstruktor a isPaid v zadani 3
	protected boolean isPaid = false;
	Artist payer;
	int howLong;
	int price;
	Account account;
	
	public Fee(int price) {
		this.price = price;
	}
}
