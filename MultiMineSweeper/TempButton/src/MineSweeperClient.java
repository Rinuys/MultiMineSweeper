import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
class Client extends Thread{
	private Socket MyClient;
	private byte[] inputMsg = new byte[7];
	private MineSweeper m = new MineSweeper();
	
	Client(){
		try {
		    MyClient = new Socket("lochuan.iptime.org", 9999);
		}
		catch (IOException e) {
		    System.out.println(e);
		}
		DataInputStream input;
		start();
		while(true){
			try {
				// Client to Server
		  		input = new DataInputStream(MyClient.getInputStream());
		  		input.readFully(inputMsg);

		  		m.clickButtonAuto(new String(inputMsg,0,7));
		   	}
		   	catch (IOException e) {
		   	}
		}		   
	}
	public void run(){
		while(true){
			String tmp = MineButton.getMessage();
			DataOutputStream output = null;
			System.out.println(tmp);
			
			if(tmp != null){
				// send
				// Server to Client
				try{
					output = new DataOutputStream(MyClient.getOutputStream());
					output.writeBytes(tmp);
					MineButton.initMessage();
				}
					catch (IOException e){					
				}
				
			}
			
		}
	}
}
public class MineSweeperClient {
	
	public static void main(String[] args){
		new Client();
	}
}

