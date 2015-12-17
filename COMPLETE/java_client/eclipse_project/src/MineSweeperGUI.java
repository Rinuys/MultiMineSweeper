import java.awt.Color;
import java.awt.Font;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.util.NoSuchElementException;
import java.util.StringTokenizer;

import javax.swing.BorderFactory;
import javax.swing.Icon;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;

class MineButton extends MouseAdapter{
	private MineSweeper board;
	public int num = 10;
	
	private static Icon[] buttonIcon = new ImageIcon[11];

	private JButton button;
	private int x;
	private int y;
	private boolean flag = false;
	

	MineButton(int x, int y, int size, MineSweeper board){		
		this.board = board;
		button = new JButton();
		this.x = x;
		this.y = y;
		
		
		button.addActionListener(new mineButtonSetListener());
		button.addMouseListener(this);
		button.setSize(size,size);
		
	}
	
	private class mineButtonSetListener implements ActionListener{
		public void actionPerformed(ActionEvent e){
			if(board.getEnableSelect() && !button.isSelected() && !flag){
				clickButton();
			}
		}
	}
	public void mouseClicked(MouseEvent e){
		if(e.getModifiers() == MouseEvent.BUTTON3_MASK && !button.isSelected()){
			if(flag){
				setFlag(false);
			}
			else{
				setFlag(true);
			}
		}
	}
	public void setFlag(boolean b){
		flag = b;
		if(!b){
			button.setIcon(null);
			board.setMineIndex(-1);
		}
		else{
			button.setIcon(buttonIcon[10]);
			board.setMineIndex(1);
		}
		
	}
	public boolean getFlag(){
		return flag;
	}

	public JButton getButton(){
		return button;
	}
	public void clickButtonAuto(int num){
		if(this.num != num){
			this.num = num;
			if(num != 10){
				button.setIcon(buttonIcon[num]);
				button.setSelected(true);
			}
			else{
				button.setIcon(null);
				button.setSelected(false);
			}
		}
	}
	
	public void clickButton(){
		board.setButtonMessage((x+1)+","+(y+1));
		button.setIcon(buttonIcon[0]);
		num = 0;
		
		board.setEnableSelect(false);
	}
	public static void setIcon(){
		for(int i=0;i<buttonIcon.length;i++){
			buttonIcon[i] = new ImageIcon(Integer.toString(i)+".png");
		}
	}
	
}

class MineSweeper extends JFrame{	
	private JPanel buttonSetPanel = new JPanel();
	private JPanel messagePanel = new JPanel();
	private JScrollPane messageScroll;
	private JTextArea message = new JTextArea();
	private JPanel mineIndexPanel = new JPanel();
	private JTextField totalMineIndex = new JTextField();
	private JTextField remainsMineIndex = new JTextField();
	private JTextField mineIndexPan = new JTextField();
	
	public MineButton[][] mineButtonSet;
	private String startMessage;
	private int numOfConnect = 0;
	
	
	private DataParser parse;
	
	private int totalMine;
	private int remainsMine;
	
	
	private String buttonMessage = null;
	private boolean enableSelect = true;	
	
	MineSweeper(int screenX, int screenY){
		startMessage = "";
		
		int buttonSize = 30;
		int numOfButtonX = 20;
		int numOfButtonY = 20;
		int boardX = 200;
		int boardY = 10;
		
		int boardSizeX = numOfButtonX * buttonSize;
		int boardSizeY = numOfButtonY * buttonSize;
		int mineIndexPanelSizeX = boardSizeX;
		int mineIndexPanelSizeY = 75;
		int frameSizeX = boardSizeX + 25;
		int frameSizeY = 240 + boardSizeY;
		int messagePanelSizeX = boardSizeX;
		int messagePanelSizeY = frameSizeY - boardSizeY - mineIndexPanelSizeY - 70;
		
		totalMine = numOfButtonX * numOfButtonY / 3;
		remainsMine = totalMine;
		
		this.setTitle("MultiMineSweeper");
		this.setLocation(screenX / 2 - this.getWidth(), screenY / 2 - this.getHeight());
		this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		this.setLayout(null);
		setSize(frameSizeX,frameSizeY);
		mineButtonSet = new MineButton[numOfButtonX][];
		
		/* BoardSetting */
		buttonSetPanel.setLayout(null);
		buttonSetPanel.setSize(boardSizeX, boardSizeY);
		buttonSetPanel.setLocation(10, 10);
		for(int i=0;i<numOfButtonX;i++){
			mineButtonSet[i] = new MineButton[numOfButtonY];
			for(int j=0;j<mineButtonSet[i].length;j++){
				mineButtonSet[i][j] = new MineButton(i,j,buttonSize,this);
				mineButtonSet[i][j].getButton().setLocation(i*buttonSize, j*buttonSize);
				buttonSetPanel.add(mineButtonSet[i][j].getButton());
			}
		}
		MineButton.setIcon();
		
		
		/* MineIndexSetting */
		mineIndexPanel.setLayout(new GridLayout(1,4));
		mineIndexPanel.setSize(mineIndexPanelSizeX, mineIndexPanelSizeY);
		totalMineIndex.setText(totalMine+"");
		totalMineIndex.setEditable(false);
		totalMineIndex.setBorder(null);
		totalMineIndex.setFont(totalMineIndex.getFont().deriveFont(Font.BOLD,25.0f));
		remainsMineIndex.setText(remainsMine+"");
		remainsMineIndex.setEditable(false);
		remainsMineIndex.setBorder(null);
		remainsMineIndex.setFont(remainsMineIndex.getFont().deriveFont(Font.BOLD,25.0f));
		mineIndexPanel.add(new JLabel(new ImageIcon("9.png")));
		mineIndexPanel.add(totalMineIndex);
		mineIndexPanel.add(new JLabel(new ImageIcon("10.png")));
		mineIndexPanel.add(remainsMineIndex);
		mineIndexPanel.setLocation(10,boardSizeY + 20);
		mineIndexPanel.setBorder(BorderFactory.createLineBorder(Color.black));
		
		/* MessageIndexSetingt */
		messagePanel.setLayout(null);
		messagePanel.setSize(messagePanelSizeX, messagePanelSizeY);
		messagePanel.setLocation(10, mineIndexPanel.getY() + mineIndexPanelSizeY + 10);
		messagePanel.setBorder(BorderFactory.createLineBorder(Color.black));
		message.setEditable(false);
		messageScroll = new JScrollPane(message);
		messageScroll.setSize(messagePanelSizeX, messagePanelSizeY);
		messageScroll.setLocation(1,1);
		messagePanel.add(messageScroll);

		
		this.setResizable(false);
		this.addWindowListener(w);
		this.add(mineIndexPanel);
		this.add(buttonSetPanel);
		this.add(messagePanel);
		this.setVisible(true);
	}

	WindowListener w = new WindowAdapter(){
		public void windowClosing(WindowEvent e){
			System.exit(0);
		}
	};
	public String getStartMessage(){
		return startMessage;
	}
	public void initStartMessage(){
		startMessage = "";
	}
	public void setEnableSelect(boolean b){
		enableSelect = b;
	}
	boolean getEnableSelect(){
		return enableSelect;
	}
	public void clickButtonAuto(String data){
		//  parsing 후 버튼 자동 누르기
		int x;
		int y;
		int num;
		int flagCount = 0;
		
		parse = new DataParser(data);
		for(x=0;x<20;x++){
			for(y=0;y<20;y++){
				try{
					num = parse.buttonParse();
					mineButtonSet[x][y].clickButtonAuto(num);
					if(num == 10){
						flagCount += 1;
					}
				}
				catch(NoSuchElementException e){
					System.exit(1);
				}
			}
		}
		if(flagCount >= 400){
			initFlag();
		}
		setEnableSelect(true);
	}
	public void initFlag(){
		for(int x=0;x<20;x++){
			for(int y=0;y<20;y++){
				if(mineButtonSet[x][y].getFlag())
					mineButtonSet[x][y].setFlag(false);
			}
		}
	}
	public void printMessage(String message){
		this.message.insert(message+"\n", this.message.getText().length());
	}
	public void setMineIndex(int num){
		remainsMine -= num;
		remainsMineIndex.setText(remainsMine+"");
	}
	public void setTotalMine(int num){
		totalMine = num;
		totalMineIndex.setText(totalMine+"");
	}
	public void initButtonMessage(){
		buttonMessage = null;
	}
	public String getButtonMessage(){
		return buttonMessage;
	}
	public void setButtonMessage(String s){
		buttonMessage = s;
	}
}
class DataParser{
	private StringTokenizer s;
	private String str;
	private int parseNum;
	private String mode;
	
	DataParser(String str){
		this.str = str;
		parseNum = 0;
	}
	
	public void initParser(String str){
		this.str = str;
		parseNum = 0;
	}
	public void initParser(){
		parseNum = 0;
	}
	public int bootParse(){
		if(parseNum == 0){
			s = new StringTokenizer(str,"_\r\n ",false);
		}
		parseNum += 1;
		return Integer.parseInt(s.nextToken());
	}
	public int connectionParse(){
		if(parseNum == 0){
			s = new StringTokenizer(str," \r\n\0",false);
		}
		parseNum += 1;
		return Integer.parseInt(s.nextToken());
	}
	public int buttonParse(){
		if(parseNum == 0){
			s = new StringTokenizer(str," (),\r\n\t",false);
		}
		parseNum += 1;
		int tmp = Integer.parseInt(s.nextToken().trim());
		try{
			if(tmp > 10){
				return 10;
			}
			return tmp;
		}
		catch(Exception e){
			return 10;
		}
		
	}
	public boolean more(){
		return s.hasMoreTokens();
	}
}
public class MineSweeperGUI {
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		MineSweeper m = new MineSweeper(0,0);

		m.clickButtonAuto("(3,3,8)");
		for(int i=0;i<50;i++){
			m.printMessage("안녕하세요"+i+i);
		}
		
	}
}
