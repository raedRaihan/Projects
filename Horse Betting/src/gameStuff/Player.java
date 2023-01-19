package gameStuff;

public class Player 
{
	String pickedHorse;
	int Money=300;
	int bet;
	long time;
	public Player()
	{
		
	}
	public Player(String pickedHorse, int bet)
	{
		 this.pickedHorse=pickedHorse;
		 this.bet=bet;
	} 
	public String getPickedHorse()
	{
		return pickedHorse;
	}
	public void setMoney(int newMoney)
	{
		Money+=newMoney;
	}
	public void setMoney2(int newMoney)
	{
		Money=newMoney;
	}
	public void setHorse(String name)
	{
		pickedHorse=name;
	}
	public void setBet(int bet)
	{
		this.bet=bet;
	}
	public int getMoney()
	{
		return Money;
	}
	public int getBet()
	{
		return bet;
	}
	public void setTime(long time){this.time = time;}
	public long getTime(){return time;}
}
