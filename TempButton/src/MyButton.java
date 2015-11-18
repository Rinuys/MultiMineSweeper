import java.awt.event.*;

import javax.swing.*;

class OhMyButton extends JFrame{
	private JPanel myPanel = new JPanel();
	private JButton myButton;
	
	OhMyButton(){
		this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		myButton = new JButton("Test");
		myButton.addActionListener(new MyButtonListener());
		
		setSize(100,80);
		setResizable(false);
		
		myPanel.add(myButton);
		this.add(myPanel);
		this.setVisible(true);
	}
	
	private class MyButtonListener implements ActionListener{
		public void actionPerformed(ActionEvent e){
			if(e.getSource() == myButton){
				// 버튼을 누르면,
				System.out.println("Press!");
			}
		}
	}
}
public class MyButton {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		new OhMyButton();
		System.out.println("Press!");
	}

}
