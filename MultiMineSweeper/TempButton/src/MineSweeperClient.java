import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.util.StringTokenizer;
class Client extends Thread{
	private Socket MyClient;
	private byte[] inputMsg = new byte[6];
	private String inputStr = null;
	private MineSweeper m;
	private boolean boot = false;
	private int roomNumber;
	private int seqNum;
	private StringTokenizer s;
	
	
	Client(){
		try {
		    MyClient = new Socket("vpn.jeonghan.wo.tc", 9034);
		}
		catch (IOException e) {
		    System.out.println(e);
		}
		DataInputStream input;
		start();
		while(true){
			try {
				// Server to Client
				input = new DataInputStream(MyClient.getInputStream());
		  		input.readFully(inputMsg);
		  		inputStr = new String(inputMsg,0,6);
		  		
				if(!boot){
					// Before Boot
					// 순서 정보를 받아야함.
					s = new StringTokenizer(inputStr,"_\r\n",false);
					
					roomNumber = Integer.parseInt(s.nextToken());
					seqNum = Integer.parseInt(s.nextToken());
					
					m = new MineSweeper(seqNum);
					boot = true;
				}
				else{
					if(m.getStart()){
						// After Start
						m.clickButtonAuto(new String(inputMsg,0,7));
					}
					else{
						// Before Start
					}
				}  		
		   	}
		   	catch (IOException e) {
		   	}
			catch (NullPointerException e){
			}
		}		   
	}
	public void run(){
		while(true){
			String tmp = "";
			DataOutputStream output = null;
			
			if(!boot){
			}
			else{
				if(m.getStart()){
					// After Start
					tmp = MineButton.getMessage();
				}
				else{
					// Before Start
					tmp = m.getReadyMessage();
					m.initReadyMessage();
				}
			}
			
			
			if(tmp != ""){
				// send
				// Client to Server
				try{
					output = new DataOutputStream(MyClient.getOutputStream());
					output.writeBytes(tmp);
					
					
					MineButton.initMessage();
					tmp = "";
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

