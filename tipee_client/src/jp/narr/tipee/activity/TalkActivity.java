package jp.narr.tipee.activity;

import java.util.ArrayList;

import jp.narr.tipee.R;
import jp.narr.tipee.command.ValueCommand;
import jp.narr.tipee.network.BluetoothManager;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.content.ActivityNotFoundException;
import android.content.Intent;
import android.os.Bundle;
import android.speech.RecognizerIntent;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

public class TalkActivity extends Activity {
	private final String TAG = "tipee";

	private static final int REQUEST_ENABLE_BT = 1234;
	private static final int REQUEST_SPEACH_RECONGIZE = 1235;
	// デモ用なので接続先サーバのBluetoothアドレスを直指定 (本来であれば接続可能なものをリストアップして選ぶ)
	private static final String SERVER_BT_ADDRESS = "00:1B:DC:06:2D:7D";
	private BluetoothManager bluetoothManager;

	private EditText inputEditText;
	private TextView messageTextView;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setContentView(R.layout.activity_talk);
		inputEditText = (EditText) findViewById(R.id.edittext_input);
		messageTextView = (TextView) findViewById(R.id.tv_message);

		bluetoothManager = new BluetoothManager();
		if (bluetoothManager.isEnabled()) {
			// BlueToothが設定でONになっている
			bluetoothManager.connect(SERVER_BT_ADDRESS);
		} else {
			// BlueToothが設定でOFFになっている
			// BlueToothをONにする
			Intent intent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
			startActivityForResult(intent, REQUEST_ENABLE_BT);
		}
	}

	public void onActivityResult(int requestCode, int resultCode, Intent data) {
		if (requestCode == REQUEST_ENABLE_BT) {
			// BlueToothの設定ON対応
			if (resultCode != 0) {
				// 「はい」が選択された
				bluetoothManager.connect(SERVER_BT_ADDRESS);
			} else {
				finish();
			}
		} else if (requestCode == REQUEST_SPEACH_RECONGIZE) {
			// 音声認識の結果が返ってきた
			if (resultCode == RESULT_OK) {
				ArrayList<String> results = data.getStringArrayListExtra(RecognizerIntent.EXTRA_RESULTS);

				if (results.size() > 0) {
					String str = results.get(0);
					Log.i(TAG, "recog message=" + str);
					processMessage(str);
				}
			}
		}
	}

	private int getCommandValue(String message) {
		if (message.contains("天気")) {
			// 明日の天気
			return 1;
		} else if( message.contains("まわり") || message.contains("周り")  || message.contains("調べて") || message.contains("ここ")) {
			// くまが出るって
			return 3;
		} else if( message.contains("さみしい") || message.contains("さびしい") || message.contains("寂しい") ) {
			// もうひとつのTIPEE
			return 5;			
		} else if( message.contains("空") || message.contains("星")) {
			// スーパームーン
			return 2;
		} else if( message.contains("山") || message.contains("登")) {
			// 標高
			return 4;
		} else if( message.contains("夜") ) {
			// 眠く
			return 6;
		}
		return 0;
	}

	private void processMessage(String message) {
		messageTextView.setText(message);
		int commandValue = getCommandValue(message);
		if( commandValue > 0 ) {
			sendValueCommand(commandValue);
		}
	}

	private void sendValueCommand(int commandValue) {
		ValueCommand command = new ValueCommand(commandValue);
		bluetoothManager.sendCommand(command);
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		bluetoothManager.cancel();
	}

	/**
	 * 音声認識ボタン押下
	 */
	public void onTalkBtnClicked(View view) {
		try {
			// 音声認識インテント作成
			Intent intent = new Intent(RecognizerIntent.ACTION_RECOGNIZE_SPEECH);
			intent.putExtra(RecognizerIntent.EXTRA_LANGUAGE_MODEL, RecognizerIntent.LANGUAGE_MODEL_FREE_FORM);
			intent.putExtra(RecognizerIntent.EXTRA_PROMPT, "Tipee");
			
			startActivityForResult(intent, REQUEST_SPEACH_RECONGIZE);
		} catch (ActivityNotFoundException e) {
			Toast.makeText(this, "ActivityNotFoundException", Toast.LENGTH_LONG).show();
		}
	}

	public void onSendBtnClicked(View view) {
		String str = inputEditText.getText().toString();
		if (!str.isEmpty()) {
			processMessage(str);
		}
	}

	// デバッグ用にメニューから直にも切り替えられる様に
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.menu_talk, menu);
		return true;
	}

	@Override
	public boolean onMenuItemSelected(int featureId, MenuItem item) {
		int itemId = item.getItemId();
		if (itemId == R.id.menu2) {
			// スーパームーン
			sendValueCommand(2);
		} else if (itemId == R.id.menu3) {
			// くま
			sendValueCommand(3);
		} else if (itemId == R.id.menu4) {
			// 標高
			sendValueCommand(4);
		} else if (itemId == R.id.menu5) {
			// もうひとつのTipee
			sendValueCommand(5);
		} else if (itemId == R.id.menu6) {
			// 夜の11時
			sendValueCommand(6);
		}
		return true;
	}
}
