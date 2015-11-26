import java.awt.Color;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.util.StringTokenizer;

import javax.swing.BorderFactory;
import javax.swing.Icon;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;

class MineButton extends MouseAdapter{
	private static String message;
	private static boolean enableSelect = true;
	private static MineSweeper boardMaster;
	private static Icon[] buttonIcon = new ImageIcon[11];

	private JButton button;
	private int x;
	private int y;
	private boolean flag = false;
	

	MineButton(int x, int y, int size){
		message = null;
		button = new JButton();
		this.x = x;
		this.y = y;
		
		
		button.addActionListener(new mineButtonSetListener());
		button.addMouseListener(this);
		button.setSize(size,size);
		
	}

	private class mineButtonSetListener implements ActionListener{
		public void actionPerformed(ActionEvent e){
			if(enableSelect && !button.isSelected() && !flag){
				clickButton();
			}
		}
	}
	public void mouseClicked(MouseEvent e){
		if(e.getModifiers() == MouseEvent.BUTTON3_MASK){
			if(flag){
				button.setIcon(null);
				flag = false;
			}
			else{
				button.setIcon(buttonIcon[10]);
				flag = true;
			}
		}
	}
	
	public static void setEnableSelect(boolean b){
		enableSelect = b;
	}
	public static void initMessage(){
		message = null;
	}
	public static String getMessage(){
		return message;
	}
	public JButton getButton(){
		return button;
	}
	public void clickButtonAuto(int num){
		button.setIcon(buttonIcon[num]);
		button.setSelected(true);
	}
	public void clickButton(){
		message = " "+x+" "+y;
		System.out.println(message);
		button.setIcon(buttonIcon[0]);
		
		enableSelect = false;
		boardMaster.setTurn(-1);
	}
	public static void setMaster(MineSweeper m){
		boardMaster = m;
	}
	public static void setIcon(){
		for(int i=0;i<buttonIcon.length;i++){
			buttonIcon[i] = new ImageIcon(Integer.toString(i)+".png");
		}
	}
}
class MineSweeper extends JFrame{
	private int currentTurn = -1; // turn : 0~3, -1 : nobody have turn.
	private int myNum;       // identifier. -1 : no have identifier.
	private JPanel buttonSetPanel = new JPanel();
	private JPanel readyPanel = new JPanel();
	private JButton readyButton;
	private boolean ready = false;
	private boolean start = false;
	public MineButton[][] mineButtonSet;
	private String readyMessage;
	
	
	MineSweeper(int num){
		myNum = num;
		readyMessage = "";
		
		int buttonSize = 30;
		int numOfButtonX = 18;
		int numOfButtonY = 18;
		int boardX = 200;
		int boardY = 10;
		int readyButtonSizeX = 200;
		int readyButtonSizeY = 60;
		
		int boardSizeX = numOfButtonX * buttonSize;
		int boardSizeY = numOfButtonY * buttonSize;
		int frameSizeX = 400 + boardSizeX;
		int frameSizeY = 150 + boardSizeY;
		int readyPanelSizeX = boardSizeX / 3 * 2;
		int readyPanelSizeY = (frameSizeY - boardSizeY) / 3 * 2;
		
		this.setTitle("MultiMineSweeper");
		this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		this.setLayout(null);
		setSize(frameSizeX,frameSizeY);
		mineButtonSet = new MineButton[numOfButtonX][];
		
		buttonSetPanel.setLayout(null);
		buttonSetPanel.setSize(boardSizeX, boardSizeY);
		buttonSetPanel.setLocation(200, 10);
		
		for(int i=0;i<numOfButtonX;i++){
			mineButtonSet[i] = new MineButton[numOfButtonY];
			for(int j=0;j<mineButtonSet[i].length;j++){
				mineButtonSet[i][j] = new MineButton(i,j,buttonSize);
				mineButtonSet[i][j].getButton().setLocation(i*buttonSize, j*buttonSize);
				buttonSetPanel.add(mineButtonSet[i][j].getButton());
			}
		}
		MineButton.setMaster(this);
		MineButton.setIcon();
		
		readyPanel.setLayout(null);
		readyPanel.setSize(readyPanelSizeX, readyPanelSizeY);
		readyPanel.setLocation(boardX + boardSizeX / 2 - readyPanelSizeX / 2, boardY + boardSizeY + 10);
		readyPanel.setBorder(BorderFactory.createLineBorder(Color.black));
		
		readyButton = new JButton("READY");
		readyButton.setSize(readyButtonSizeX,readyButtonSizeY);
		readyButton.setLocation((readyPanelSizeX - readyButtonSizeX) / 2, (readyPanelSizeY - readyButtonSizeY) / 2);
		readyButton.addActionListener(new ReadyButtonListener());
		readyPanel.add(readyButton);
		
		
		setResizable(false);
		
		
		this.add(buttonSetPanel);
		this.add(readyPanel);
		this.setVisible(true);
	}
	
	private class ReadyButtonListener implements ActionListener{
		public void actionPerformed(ActionEvent e){
			if(!start){
				if(ready){
					ready = false;
					readyMessage = "\0"+myNum + "__F\0";
					
				}
				else{
					ready = true;
					readyMessage = "\0"+myNum + "__T\0";
				}
			}
		}
	}
	public String getReadyMessage(){
		return readyMessage;
	}
	public void initReadyMessage(){
		readyMessage = "";
	}
	public void gameStart(){
		start = true;
		MineButton.setEnableSelect(true);
	}
	public void setReady(boolean b){
		ready = b;
	}
	public void setStart(boolean b){
		start = b;
	}
	public boolean getStart(){
		return start;
	}
	public void clickButtonAuto(String data){
		//  parsing 후 버튼 자동 누르기
		int x;
		int y;
		int num;
		
		StringTokenizer s = new StringTokenizer(data,"(),",false);
		x = Integer.parseInt(s.nextToken());
		y = Integer.parseInt(s.nextToken());
		num = Integer.parseInt(s.nextToken());
		
		mineButtonSet[x][y].clickButtonAuto(num);
	}
	public void setTurn(int turn){
		currentTurn = turn;
	}
	public void setIdentifier(int num){
		myNum = num;
	}
	public void checkTurn(){
		if(currentTurn == myNum){
			MineButton.setEnableSelect(true);
		}
		else{
			MineButton.setEnableSelect(false);
		}
	}
}
public class MineSweeperGUI {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		MineSweeper m = new MineSweeper(1);

		m.clickButtonAuto("(3,3,8)");
	}
}
