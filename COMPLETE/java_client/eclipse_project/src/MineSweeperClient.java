import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import javax.swing.*;

class RoomSelect extends JFrame{
	private Client c;
	
	private JPanel roomPanel = new JPanel();
	private JTextField[] roomButtonText = new JTextField[4];
	private JButton[] roomButton = new JButton[4];
	private int[] roomPlayer = new int[4];
	
	private DataParser parser;
	
	RoomSelect(Client c,int screenX, int screenY){
		this.c = c;
		this.setSize(250,300);
		this.setLocation(screenX,screenY);
		this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		this.setTitle("MultiMineSweeper");
		
		roomPanel.setLayout(new GridLayout(4,1));
		for(int i=0;i<roomButton.length;i++){
			roomButton[i] = new JButton();
			roomButton[i].setBorder(BorderFactory.createLineBorder(Color.red,5));
			roomButton[i].setText("ROOM"+(i+1)+", 연결없음");
			roomButton[i].addActionListener(new roomButtonSetListener());
			roomPanel.add(roomButton[i]);
		}
		this.add(roomPanel);
		for(int i=0;i<roomButton.length;i++){
			
		}
		this.addWindowListener(w);
		this.setVisible(true);
	}
	WindowListener w = new WindowAdapter(){
		public void windowClosing(WindowEvent e){
			c.socketClose(0);
			c.socketClose(1);
			c.socketClose(2);
			c.socketClose(3);
			System.exit(0);
		}
	};
	private class roomButtonSetListener implements ActionListener{
		public void actionPerformed(ActionEvent e){
			if(e.getSource() == roomButton[0] && c.connection[0]){
				c.gameStart(0);
			}
			if(e.getSource() == roomButton[1] && c.connection[1]){
				c.gameStart(1);
			}
			if(e.getSource() == roomButton[2] && c.connection[2]){
				c.gameStart(2);
			}
			if(e.getSource() == roomButton[3] && c.connection[3]){
				c.gameStart(3);
			}

		}
	}
	
	public void setRoom(int num,String s){
		parser = new DataParser(s);
		roomPlayer[num] = parser.connectionParse() / 2;
		setButton();
	}
	public void setButton(){
		for(int i=0;i<roomButton.length;i++){
			if(c.connection[i]){
				roomButton[i].setBorder(BorderFactory.createLineBorder(Color.green,5));
				roomButton[i].setText("ROOM"+(i+1)+", "+roomPlayer[i]+"명");
			}
			else{
				roomButton[i].setBorder(BorderFactory.createLineBorder(Color.red,5));
				roomButton[i].setText("ROOM"+(i+1)+", 연결없음");
			}
		}
		
	}
}
class Client extends Thread{
	private Socket[] client = new Socket[4];
	
	private Reader[] reader = new Reader[4];
	private BufferedReader[] input = new BufferedReader[4];
	private String[] inputData = new String[4];
	
	private RoomSelect r;
	private MineSweeper m;
	boolean abc = false;
	
	public boolean[] connection = new boolean[4];
	private int[] connectionCount = new int[4];
	public int selected = -1;
	PrintWriter output;
	
	private boolean START = false;
	private boolean STARTING = false;
	
	
	Client(){
		Toolkit tk = Toolkit.getDefaultToolkit();
		Dimension screenSize = tk.getScreenSize();
		
		r = new RoomSelect(this,screenSize.width / 2,screenSize.height / 2);
		
		for(int i=0;i<client.length;i++)
			reader[i] = new Reader(i);
		for(int i=0;i<inputData.length;i++){
			inputData[i] = null;
		}
		
		start();
		while(true){
			if(selected == -1 && !START){
				for(int i=0;i<client.length;i++){
					if(!connection[i]){
						setClient(i);
					}
					if(connection[i]){
						if(!reader[i].start){
							System.out.println(i+" 쓰레드시작");
							reader[i].startRead();
						}
					}
				}
			}
			else if(START && (!STARTING)){
				try {
					input[selected] = new BufferedReader(new InputStreamReader(client[selected].getInputStream()));
					
					inputData[selected] = input[selected].readLine();
				} catch (IOException e) {
				}
				
				if(inputData[selected] != null){
					m.clickButtonAuto(inputData[selected]);
					inputData[selected] = null;
				}
			}
		}		   
	}
	private void setDisconnect(int num){
		connection[num] = false;
		reader[num].interrupt();
	}
	private class Reader extends Thread{
		int num;
		boolean start = false;
		
		Reader(int num){
			this.num = num;
		}
		public void startRead(){
			start = true;
			System.out.println(num+" 쓰레드시작");
			try {
				input[num] = new BufferedReader(new InputStreamReader(client[num].getInputStream()));
			} catch (IOException e) {
			}
			start();
		}
		public void run(){
			while(start && !Thread.interrupted()){
				try {
					if(selected == -1){
//						input[num] = new BufferedReader(new InputStreamReader(client[num].getInputStream()));
						inputData[num] = input[num].readLine();
						if(inputData[num] != null && selected == -1){
							r.setRoom(num,inputData[num]);
							inputData[num] = null;
						}
					}					
				} catch (IOException e) {
					setDisconnect(num);
					break;
				}
			}
		}
	}
	public void run(){
		while(true){
			output = null;
			if(selected == -1){
				int i=-1;
				try {
					for(i=0;i<client.length;i++){
						if(connection[i]){
							sleep(100);
							output = new PrintWriter(client[i].getOutputStream(),true);
							output.write("#query");
							output.flush();
						}
					}
					
				} catch (IOException e) {
				} catch (InterruptedException e) {
					
				}
			}
			else if(START && (!STARTING)){
				try {
					if(m.getButtonMessage() != null){
						output = new PrintWriter(client[selected].getOutputStream(),true);
						output.write(m.getButtonMessage());
						m.initButtonMessage();
						output.flush();
						
					}
					else{
						sleep(400);
						output = new PrintWriter(client[selected].getOutputStream(),true);
						output.write("refresh");
						output.flush();
					}
					
				} catch (IOException e1) {
				} catch (InterruptedException e1) {
				}
			}
		}
	}
	private void setClient(int num){
		try {
			Thread.sleep(100);
			System.out.println(num+"번째 방, 연결 시도중...");
		    client[num] = new Socket("vpn.jeonghan.wo.tc", 9000+num);
		    connection[num] = true;
		    System.out.println(num+"번째 방, 연결 성공");
		}
		catch (IOException e) {
			connectionCount[num] += 1;
			if(connectionCount[num] > 10){
				setClient(num);
			}
			else{
				// 연결 실패
				connection[num] = false;
			}
		} 
		catch (InterruptedException e) {
		}
	}
	public void gameStart(int num){
		STARTING = true;
		selected = num;
		START = true;
		for(int i=0;i<inputData.length;i++){
			inputData[i] = null;
		}
		for(int i=0;i<client.length;i++){
			setDisconnect(i);
		}
		
		output = null;
		m = new MineSweeper(r.getX(),r.getY());
		r.setVisible(false);
		r = null;
		STARTING = false;
	}
	public void socketClose(int num){
		try {
			client[num].close();
		} catch (IOException e) {
		}
		catch (NullPointerException e){
			
		}
	}
}
public class MineSweeperClient {
	public static void main(String[] args){
		new Client();
	}
}

