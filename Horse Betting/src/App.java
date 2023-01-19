import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.image.ImageObserver;
import java.io.*;
import java.io.InputStream.*;
import java.text.AttributedCharacterIterator;
import javax.swing.*;
import gameStuff.*;
import javax.sound.sampled.*;

public class App extends JFrame 
{
	static String answer = "null";
	static boolean musicGo = true;

	public static void main(String[] args) throws UnsupportedAudioFileException, IOException, LineUnavailableException
	{
		String names[]= {"Roach","Lucky","Speed Boy","Red","Dusty"};
		JFrame frame = new JFrame("Pick Your Mode"); 
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setSize(300,300); 
		 while(musicGo) 
		 {
			 JPanel panel=new JPanel();
			 JButton button = new JButton("Normal");
			 JButton buttonTwo=new JButton("Classic");
			 panel.add(button);
			 panel.add(buttonTwo);
			 frame.add(panel);
			 frame.setVisible(true);
			 
			 button.addActionListener(new ActionListener() {
			 public void actionPerformed(ActionEvent e) {
			 answer = "Normal.wav";
			 musicGo = false;
			 }
			 });
			 buttonTwo.addActionListener(new ActionListener() {
			 public void actionPerformed(ActionEvent e) {
			 answer = "Classic.wav";
			 musicGo = false;
			 }
			 });
		}
		frame.setVisible(false);
		frame.dispose();
		
		File file = new File(answer);
		AudioInputStream audioStream= AudioSystem.getAudioInputStream(file);
		Clip clip= AudioSystem.getClip();
		clip.open(audioStream);
		clip.start();
		

		Screen1 S1= new Screen1();
		while(!S1.done())
		{
			System.out.println();
		}
		Player p=S1.getPlayer();
		boolean gameGo=true;
		while(gameGo)
		{
			String selectedHorse = p.getPickedHorse();
			int i = 0;
			String firstName = names[0];
			while(!selectedHorse.equals(names[i]))
			{
				i++;
			}
			names[0] = selectedHorse;
			names[i] = firstName;
			mainScreen ms= new mainScreen(names,p);
			gameGo=ms.playAgain();
			if(gameGo)
			{
				S1= new Screen1(p);
				while(!S1.done())
				{
					System.out.println();
				}
			}		
		    
		}
		System.exit(0);
		
		
	}
	
	

}
