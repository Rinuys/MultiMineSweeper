import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.StringTokenizer;

import javax.swing.Icon;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;

class MineButton{
	private static String message;
	private static boolean enableSelect = true;
	private static MineSweeper boardMaster;
	private static Icon[] buttonIcon = new ImageIcon[10];

	private JButton button;
	private int x;
	private int y;
	private boolean flag;
	private boolean selected;
	

	MineButton(int x, int y, int size){
		message = null;
		button = new JButton();
		this.x = x;
		this.y = y;
		
		
		button.addActionListener(new mineButtonSetListener());
		button.setSize(size,size);
		
	}

	private class mineButtonSetListener implements ActionListener{
		public void actionPerformed(ActionEvent e){
			if(enableSelect && !button.isSelected() && !flag){
				clickButton();
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
	private int myNum = -1;       // identifier. -1 : no have identifier.
	private JPanel myPanel = new JPanel();
	public MineButton[][] mineButtonSet;
	
	MineSweeper(){
		int mineSweeperBoardX = 200;
		int mineSweeperBoardY = 10;
		int buttonSize = 30;
		this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		mineButtonSet = new MineButton[20][];
		
		myPanel.setLayout(null);
		
		for(int i=0;i<20;i++){
			mineButtonSet[i] = new MineButton[20];
			for(int j=0;j<mineButtonSet[i].length;j++){
				mineButtonSet[i][j] = new MineButton(i,j,buttonSize);
				mineButtonSet[i][j].getButton().setLocation(mineSweeperBoardX+i*buttonSize, mineSweeperBoardY+j*buttonSize);
				myPanel.add(mineButtonSet[i][j].getButton());
			}
		}
		MineButton.setMaster(this);
		MineButton.setIcon();
		
		setSize(1000,800);
		setResizable(false);
		
		
		this.add(myPanel);
		this.setVisible(true);
	}

	public void clickButtonAuto(String data){
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
		if(myNum != -1 && currentTurn == myNum){
			MineButton.setEnableSelect(true);
		}
	}
}
public class MineSweeperGUI {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		MineSweeper m = new MineSweeper();

		m.clickButtonAuto("(3,3,8)");
	}
}
