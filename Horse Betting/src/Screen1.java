import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.image.ImageObserver;
import java.text.AttributedCharacterIterator;
import gameStuff.*;
import javax.swing.*;
class Background extends JPanel
{
	Image bg = new ImageIcon("screen1.jpg").getImage();
	public void paintComponent(Graphics g) 
	{
		g.drawImage(bg, 0, 0, getWidth(), getHeight(), this);
	}
	

}
class information extends JPanel
{
	String names[];
	int balance;
	information(String names[], int balance)
	{
		setOpaque(false);
		this.names=names;
		this.balance=balance;
		
	}
	protected void paintComponent(Graphics g)
	{
		
	    super.paintComponent(g);
	    g.setFont(new Font("TimesRoman", Font.PLAIN, 35));
	    g.setColor(Color.WHITE);
	    g.drawString(names[0],100,55);
	    g.drawString(names[1],370,55); 
	    g.drawString(names[2],640,55);
	    g.drawString(names[3],190,170);
	    g.drawString(names[4],570,170);
	    String balanceS=String.valueOf(balance);
	    g.drawString(balanceS,680,365);
	    
	}
	  
	
}

public class Screen1 extends JFrame implements ItemListener,ActionListener
{
	static String names[]= {"Roach","Lucky","Speed Boy","Red","Dusty"};
	static String selectedHorse;
	static JComboBox c1;
	static JPanel Screen ;
	static Screen1 t;
	boolean gameDone=false;
	Player p;
	public Screen1()
	{
		p=new Player();
		selectedHorse=names[0];
		Screen = new Background();
		Screen.setLayout(new BorderLayout());
		Screen.add(new information(names,p.getMoney()));
	    c1 = new JComboBox(names);
	    c1.setBounds(400, 400, 20, 20);
	    //t = new Screen1();
		c1.addItemListener(this);
		JPanel newJP= new JPanel();
		c1.setOpaque(false);
		newJP.add(c1,BorderLayout.CENTER);
		newJP.setOpaque(false);
		Button submit= new Button("Submit");
		submit.addActionListener(this);
		newJP.add(submit);
		Screen.add(newJP,BorderLayout.SOUTH);
		add(Screen);
		setDefaultCloseOperation(EXIT_ON_CLOSE);
		setSize(849, 477);
		setVisible(true);
		this.setResizable(false);
	}
	public Screen1(Player p)
	{
		
		this.p=p;
		selectedHorse=names[0];
		Screen = new Background();
		Screen.setLayout(new BorderLayout());
		Screen.add(new information(names,p.getMoney()));
	    c1 = new JComboBox(names);
	    c1.setBounds(400, 400, 20, 20);
	    //t = new Screen1();
		c1.addItemListener(this);
		JPanel newJP= new JPanel();
		c1.setOpaque(false);
		newJP.add(c1,BorderLayout.CENTER);
		newJP.setOpaque(false);
		Button submit= new Button("Submit");
		submit.addActionListener(this);
		newJP.add(submit);
		Screen.add(newJP,BorderLayout.SOUTH);
		add(Screen);
		setDefaultCloseOperation(EXIT_ON_CLOSE);
		setSize(849, 477);
		setVisible(true);
		this.setResizable(false);
	}
	public Player getPlayer()
	{
		return p;
	}
	public boolean done()
	{
		return gameDone;
	}
	public void itemStateChanged(ItemEvent e)
    {
		
		if (e.getSource() == c1) 
		{
			selectedHorse=(String) c1.getSelectedItem();
			System.out.println(selectedHorse);
		}
       
    }
	boolean buttonPressed=false;;
	public boolean getButtonPressed()
	{
		return buttonPressed;
	}
	public void actionPerformed(ActionEvent e)  
	{
		buttonPressed=true;
		String balanceS = JOptionPane.showInputDialog(null, "How much are you betting?");
		int balance=Integer.parseInt(balanceS);
		Player ptest = new Player(selectedHorse, balance);
		while(balance > p.getMoney())
		{
			balanceS = JOptionPane.showInputDialog(null, "Not a valid amount, Please re-enter");
			balance=Integer.parseInt(balanceS);;
		}
		p.setHorse(selectedHorse);
		p.setBet(balance);
		//p.setMoney2(balance);
		setVisible(false);
		gameDone=true;
		dispose();
			
	}
	
	

}



