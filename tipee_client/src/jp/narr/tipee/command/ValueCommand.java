package jp.narr.tipee.command;

import java.io.DataOutputStream;
import java.io.IOException;

/**
 * 値を送るコマンド
 * 
 * @author miyoshi
 */
public class ValueCommand extends Command {
private static final byte COMMAND_VALUE = 1;
	private int value;

	public ValueCommand(int value) {
		this.value = value;
	}
	
	public void write(DataOutputStream out) throws IOException {
		out.write(COMMAND_VALUE);
		out.writeShort(value);
	}
}
