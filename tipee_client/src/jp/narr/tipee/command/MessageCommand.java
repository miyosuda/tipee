package jp.narr.tipee.command;

import java.io.DataOutputStream;
import java.io.IOException;

/**
 * 文字列を送るためのコマンド(未使用)
 * 
 * @author miyoshi
 */
public class MessageCommand extends Command {
	private static final byte COMMAND_MESSAGE = 2;
	
	private String msg;

	public MessageCommand(String msg) {
		this.msg = msg;
	}
	
	@Override	
	public void write(DataOutputStream out) throws IOException {
		out.write(COMMAND_MESSAGE);
		out.writeUTF(msg);
	}
}
