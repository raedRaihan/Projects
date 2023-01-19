package gameStuff;

import java.util.concurrent.ThreadLocalRandom;

public class Horse 
{
	String name;
	int position;
	int postionX=185;
	public Horse(String name,int position)
	{
		this.name=name;
		this.position=position;
	}
	public int getPosition()
	{
		return position;
	}
	public int getPositionX()
	{
		return postionX;
	}
	public boolean WonGame()
	{
		if(postionX>1180)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	public void changePosition()
	{
		int min=5;
		int max=40;
		int randomNum = ThreadLocalRandom.current().nextInt(min, max + 1);
		postionX+=randomNum;
	}
	
	

}
