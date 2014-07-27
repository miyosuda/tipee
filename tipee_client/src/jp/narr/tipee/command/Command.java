package jp.narr.tipee.command;

import java.io.DataOutputStream;
import java.io.IOException;

public abstract class Command {
	public abstract void write(DataOutputStream out) throws IOException;
}
