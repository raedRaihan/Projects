import java.awt.*;
import gameStuff.*;
import java.awt.image.ImageObserver;
import java.io.File;
import java.io.IOException;
import java.text.AttributedCharacterIterator;

import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.Clip;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.UnsupportedAudioFileException;
import javax.swing.*;
import java.util.concurrent.ThreadLocalRandom;
class BackgroundG extends JPanel
{

	Image bg = new ImageIcon("concept3.jpg").getImage();
	@Override
	public void paintComponent(Graphics g) 
	{
		g.drawImage(bg, 0, 0, getWidth(), getHeight(), this);
	}
	

}

class Everything extends JPanel
{
	String imageName="doom_head.gif";
	Image bg = new ImageIcon("horse.png").getImage();
	Image player = new ImageIcon(imageName).getImage();
	Image text=new ImageIcon("live_doom_guy_reacts.png").getImage();
	int x1=0;
	int x2=0;
	int x3=0;
	int x4=0;
	int x5=0;
	
	int y1=0;
	int y2=0;
	int y3=0;
	int y4=0;
	int y5=0;
	
	Player p;
	String [] names;
	void PlayerWins()
	{
		imageName="doom_guy_win.png";
		player = new ImageIcon(imageName).getImage();
	}
	void PlayerLoses()
	{
		imageName="doom_guy_dies.gif";
		player = new ImageIcon(imageName).getImage();
	}
	int getx()
	{
		return x1;
	}
	
	Everything(String [] names, int x1, int y1, int y2, int y3,int y4,int y5,Player p)
	{
		
		this.p=p;
		this.names=names;
		this.x1=x1;
		x2=x1;
		x3=x1;
		x4=x1;
		x5=x1;
		this.y1=y1;
		this.y2=y2;
		
		this.y3=y3;
		this.y4=y4;
		this.y5=y5;
		setOpaque(false);
		setLayout(new FlowLayout());
		
		
	}
	void setValues(int x1,int x2, int x3, int x4, int x5)
	{
		this.x1=x1;
		this.x2=x2;
		this.x3=x3;
		this.x4=x4;
		this.x5=x5;
		
	}
	protected void paintComponent(Graphics g)
	{


		super.paintComponent(g); 
	    g.drawImage(bg, x1, y1, 65, 65, this);
	    g.drawImage(player,480, 560, 120, 100, this);
	    g.drawImage(text,455, 650, 160, 55, this);
	    g.drawImage(bg, x2, y2, 65, 65, this);
	    g.drawImage(bg, x3, y3, 65, 65, this);
	    g.drawImage(bg, x4, y4, 65, 65, this);
	    g.drawImage(bg, x5, y5, 65, 65, this);
	    g.setFont(new Font("TimesRoman", Font.PLAIN, 18));
	    g.setColor(Color.WHITE);
	    g.drawString(names[0],90,100);
	    g.drawString(names[1],90,200);
	    g.drawString(names[2],90,300);
	    g.drawString(names[3],90,400);
	    g.drawString(names[4],90,500);
	    g.setFont(new Font("TimesRoman", Font.PLAIN, 35));
		g.drawString(String.valueOf(p.getTime()),300, 640);
	    g.drawString(p.getPickedHorse(),630,640);
	    g.drawString(String.valueOf(p.getBet()),1000,640);
	    g.drawString(String.valueOf(p.getMoney()),1180,640);
	    super.paintComponent(g);
		
	}
	  
	
}

public class mainScreen extends JFrame 
{
	JLabel countDown=new JLabel("3",JLabel.CENTER);
	JPanel BackgroundExample = new BackgroundG();
	JPanel test= new JPanel();
	String names[];
	Player p;
	public boolean playAgain()
	{
		int result = JOptionPane.showConfirmDialog(null, "Do you want to play again?");
		boolean answer = true;
		switch (result)
		{
		 case JOptionPane.YES_OPTION:
			 setVisible(false);
			 dispose();
			 answer = true;
			 return answer;
		 case JOptionPane.NO_OPTION:
			 setVisible(false);
		 	 dispose();
		 	answer = false;
		 	return answer;
		 case JOptionPane.CANCEL_OPTION:
			 setVisible(false);
			 dispose();
			 answer = true;
			 return answer;
		 case JOptionPane.CLOSED_OPTION:
			 setVisible(false);
			 dispose();
			 answer = true;
			 return answer;
		}
		return answer;
	}
	public void countdownF()
	{
		countDown.setForeground(Color.white);
		try
		{
		    Thread.sleep(1000);
		}
		catch(InterruptedException ex)
		{
		    Thread.currentThread().interrupt();
		}
		countDown.setText("2");
		setVisible(true);
		try
		{
		    Thread.sleep(1000);
		}
		catch(InterruptedException ex)
		{
		    Thread.currentThread().interrupt();
		}
		countDown.setText("1");
		setVisible(true);
		try
		{
		    Thread.sleep(1000);
		}
		catch(InterruptedException ex)
		{
		    Thread.currentThread().interrupt();
		}
		countDown.setText("Go!");
		setVisible(true);
		try
		{
		    Thread.sleep(200);
		}
		catch(InterruptedException ex)
		{
		    Thread.currentThread().interrupt();
		}
		countDown.setText("");
		setVisible(true);
	}
	public mainScreen(String [] names,Player p)
	{
		this.p=p;
		this.names=names;
		BackgroundExample.setLayout(new BorderLayout());
		this.setResizable(false);
		
		countDown.setFont(new Font("TimesRoman", Font.PLAIN, 50));
		BackgroundExample.add(countDown);
		
		add(BackgroundExample);
		
		setDefaultCloseOperation(EXIT_ON_CLOSE);
		setSize(1290, 730);
		setVisible(true);
		countdownF();
		Horse h1= new Horse(names[0],65);
		Horse h2= new Horse(names[1],165);
		Horse h3= new Horse(names[2],270);
		Horse h4= new Horse(names[3],380);
		Horse h5= new Horse(names[4],475);
		
		setVisible(true);
		
		Everything ET=new Everything(names,185,h1.getPosition(),h2.getPosition(),h3.getPosition(),h4.getPosition(),h5.getPosition(),p);
		BackgroundExample.add(ET);
		setVisible(true);
		long startTime = System.currentTimeMillis();
		int limit=1180;
		int time=530;
		while(h1.getPositionX()<limit && h2.getPositionX()<limit && h3.getPositionX()<limit && h4.getPositionX()<limit && h5.getPositionX()<limit)
		{
			h1.changePosition();
			h2.changePosition();
			h3.changePosition();
			h4.changePosition();
			h5.changePosition();
			long elapsedTime = System.currentTimeMillis() - startTime;
			long elapsedSeconds = elapsedTime / 1000;
			long secondsDisplay = elapsedSeconds % 60;
			p.setTime(secondsDisplay);
			try
			{
			    Thread.sleep(time);
			}
			catch(InterruptedException ex)
			{
			    Thread.currentThread().interrupt();
			}
			
			ET.setValues(h1.getPositionX(),h2.getPositionX(),h3.getPositionX(),h4.getPositionX(),h5.getPositionX());
			setVisible(true);
		}
		
		if(h1.WonGame())
		{
			ET.PlayerWins();
			int money=p.getBet()*12;
			JOptionPane.showMessageDialog(this, "You Won " + money + " dollars!" );
			p.setMoney(money);
			
		}
		else
		{
			ET.PlayerLoses();
			File file = new File("dead.wav");
			AudioInputStream audioStream;
			try
			{
				audioStream = AudioSystem.getAudioInputStream(file);
				Clip clip= AudioSystem.getClip();
				clip.open(audioStream);
				clip.start();
			} catch (UnsupportedAudioFileException | IOException | LineUnavailableException e)
			{
				System.out.println(e);
			}
			int money=p.getBet();
			JOptionPane.showMessageDialog(this, "You Lost " + money + " dollars!");
			p.setMoney(-money);
			
			
		}
		setVisible(false);
		dispose();
	}
}
