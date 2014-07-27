#include <stdio.h>
#include <cairo.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <string.h>
#include <stdlib.h>

#include <sys/time.h>

#include "String.h"
#include "Expression.h"
#include "ExpressionManager.h"
#include "BluetoothServerSocket.h"
#include "BluetoothSocket.h"
#include "Message.h"
#include "MessageManager.h"
#include "Movement.h"

#include "constants.h"

// 吹き出し型
static const int FUKIDASHI_CLOUD      = 0; // クラウド型
static const int FUKIDASHI_BIKKURI    = 1; // とげとげ
static const int FUKIDASHI_ROUND      = 2; // まるっこい吹き出し
static const int FUKIDASHI_ELIPSE     = 3; // 楕円
static const int FUKIDASHI_ROUND_RECT = 4; // 角丸
static const int FUKIDASHI_THINK      = 5; // 思ってる風
static const int FUKIDASHI_BUBBLE     = 6; // 泡風
static const int FUKIDASHI_MOWAMOWA   = 7; // もわもわ
static const int FUKIDASHI_RECT       = 8; // 角型

// 表情
static const int EXP_IDLE        = 0; // アイドリング時
static const int EXP_TALK_NORMAL = 1; // 通常会話
static const int EXP_TALK_SAD    = 2; // 悲しい会話
static const int EXP_TALK_ANGRY  = 3; // おこった会話
static const int EXP_KYOTON      = 4; // きょとん
static const int EXP_WAO         = 5; // ワオ
static const int EXP_WINK        = 6; // ウインク
static const int EXP_DELIGHT     = 7; // よろこび
static const int EXP_NIYANIYA    = 8; // ニヤニヤ
static const int EXP_JII         = 9; // じー

// コマンドタイプ
static const int COMMAND_VALUE   = 1;
static const int COMMAND_MESSAGE = 2;

#define M_PI 3.14159265358979323846

// 受信チャンネルは22
static const int BLUETOOTH_CHANNEL = 22;

#define BROW_SIZE 5
#define EYE_SIZE 7
#define MOUTH_SIZE 10
#define FUKIDASHI_SIZE 9

static GThread* thread;

static BluetoothServerSocket serverSocket;

static cairo_surface_t* brows[BROW_SIZE];
static cairo_surface_t* eyes[EYE_SIZE];
static cairo_surface_t* mouthes[MOUTH_SIZE];
static cairo_surface_t* fukidashis[FUKIDASHI_SIZE];
static cairo_surface_t* starImage;
static cairo_surface_t* heartImage;
static cairo_surface_t* cheekImage;

static ExpressionManager expressionManager;

static int debugCheckExpressionId = -1;
static MessageManager messageManager;

// 左右反転モードにするかどうか (プロジェクタでの内側投影用)
static bool horizontalFlipping = false;
static char buffer[1024];

static Movement movement;

static int windowWidth = 0;
static int windowHeight = 0;

// 各吹き出し内でのテキスト表示可能範囲
struct SpeakRegion {
	int left;
	int top;
	int width;
	int height;
};
static SpeakRegion speakRegions[] = {
	{ 89,145,390,123 },
	{ 120,99,310,181 },
	{ 38,95,426,187 },
	{ 55,89,412,209 },
	{ 31,23,447,276 },
	{ 47,93,447,204 },
	{ 88,82,344,247 },
	{ 63,86,369,247 },
	{ 18,14,473,336 },
};

/**
 * <!--  getSpeakRegionParam():  -->
 *
 * 各吹き出しのセリフ表示可能範囲取得
 */
static void getSpeakRegionParam( int fukidashiIndex, 
								 double& cx, double& cy, 
								 int& width, int& height ) {
	SpeakRegion& region = speakRegions[fukidashiIndex];
	
	cx = region.left + region.width * 0.5;
	cy = region.top + region.height * 0.5;
	width = region.width;
	height = region.height;
}

/**
 * <!--  setMessage():  -->
 *
 * 表示メッセージ設定.
 */
static void setMessage( const char* str, 
						int expressionId, 
						int fukidashi,
						double durationSec ) {
	// GTKスレッドとの排他制御
	gdk_threads_enter();
	{
		messageManager.clearMessages();
		Expression* expression = expressionManager.getExpressionById( expressionId );
		messageManager.addMessage( str, expression, fukidashi, durationSec );
	}
	gdk_threads_leave();
}

/**
 * <!--  setMessage2():  -->
 *
 * 表示メッセージ設定(2行)
 */
static void setMessage2( const char* str0, 
						 int expressionId0, 
						 int fukidashi0,
						 double durationSec0,
						 const char* str1, 
						 int expressionId1, 
						 int fukidashi1,
						 double durationSec1 ) {
	gdk_threads_enter();
	{
		messageManager.clearMessages();
		Expression* expression0 = expressionManager.getExpressionById( expressionId0 );
		messageManager.addMessage( str0, expression0, fukidashi0, durationSec0 );
		Expression* expression1 = expressionManager.getExpressionById( expressionId1 );
		messageManager.addMessage( str1, expression1, fukidashi1, durationSec1 );
	}
	gdk_threads_leave();
}

/**
 * <!--  setMessage3():  -->
 *
 * 表示メッセージ設定(3行)
 */
static void setMessage3( const char* str0, int expressionId0, int fukidashi0, 
						 double durationSec0,
						 const char* str1, int expressionId1, int fukidashi1,
						 double durationSec1,
						 const char* str2, int expressionId2, int fukidashi2,
						 double durationSec2 ) {
	gdk_threads_enter();
	{
		messageManager.clearMessages();
		Expression* expression0 = expressionManager.getExpressionById( expressionId0 );
		messageManager.addMessage( str0, expression0, fukidashi0, durationSec0 );
		Expression* expression1 = expressionManager.getExpressionById( expressionId1 );
		messageManager.addMessage( str1, expression1, fukidashi1, durationSec1 );
		Expression* expression2 = expressionManager.getExpressionById( expressionId2 );
		messageManager.addMessage( str2, expression2, fukidashi2, durationSec2 );
	}
	gdk_threads_leave();
}

/**
 * <!--  init():  -->
 */
static void init() {
	// 画像をロード
	char buf[64];
	GError* error = NULL;
	for(int i=0; i<BROW_SIZE; ++i) {
		sprintf(buf, "images/brow/face2_brow%d.png", i);
		brows[i] = cairo_image_surface_create_from_png(buf);
	}
	for(int i=0; i<EYE_SIZE; ++i) {
		sprintf(buf, "images/eye/face2_eye%d.png", i);
		eyes[i] = cairo_image_surface_create_from_png(buf);
	}
	for(int i=0; i<MOUTH_SIZE; ++i) {
		sprintf(buf, "images/mouth/face2_mouth%d.png", i);
		mouthes[i] = cairo_image_surface_create_from_png(buf);
	}
	for(int i=0; i<FUKIDASHI_SIZE; ++i) {
		sprintf(buf, "images/fukidashi/fukidashi%d.png", i);
		fukidashis[i] = cairo_image_surface_create_from_png(buf);
	}
	
	starImage = cairo_image_surface_create_from_png("images/other/face2_star.png");
	heartImage = cairo_image_surface_create_from_png("images/other/face2_heart.png");
	cheekImage = cairo_image_surface_create_from_png("images/other/face2_cheek.png");
	
	// 表情パターンをロード
	bool ret = expressionManager.load();
	
	int expressionId = EXP_TALK_NORMAL;
	if( debugCheckExpressionId >= 0 ) {
		expressionId = debugCheckExpressionId;
	}

	// デフォルトのセリフ表示
	setMessage("やっほ〜！\nTipeeだよ〜", expressionId, FUKIDASHI_ROUND, 0.0);
}

/**
 * <!--  calcStringExtent():  -->
 *
 * 文字列の描画幅/高さを計算
 */
static void calcStringExtent( cairo_t *cr, const char* str, int& width, int& height ) {
	cairo_text_extents_t extents;
	cairo_text_extents(cr, str, &extents);
	width = extents.width;
	height = extents.height;
}

/**
 * <!--  calcWindowExtent():  -->
 *
 * 画面の幅/高さを計算
 */
static void calcWindowExtent( GtkWidget *widget, int& width, int& height ) {
	GtkWidget *win = gtk_widget_get_toplevel(widget);
	gtk_window_get_size( GTK_WINDOW(win), &width, &height );
}

/**
 * <!--  drawFace():  -->
 * 
 * 表情描画
 */
static void drawFace( cairo_t *cr ) {
	Message* currentMessage = messageManager.getCurrentMessage();
	if( currentMessage == NULL ) {
		return;
	}

	Expression* expression = currentMessage->getExpression();
	expression->update();
	int brow;
	int eye;
	int mouth;
	bool cheek;
	bool star;
	bool heart;
	expression->getExpression( brow, eye, mouth, cheek, star, heart );

	if( horizontalFlipping ) {
		// 画面を左右反転する場合
		cairo_translate(cr, windowWidth, 0.0);	
		cairo_scale(cr, -1.0, 1.0);
	}

	cairo_scale(cr, 0.6, 0.6);
	cairo_translate(cr, 50, 250.0);

	// 表情の各パーツを表示
	cairo_set_source_surface(cr, brows[brow], 52, 55);
	cairo_paint(cr);
	
	cairo_set_source_surface(cr, eyes[eye], 52, 178);
	cairo_paint(cr);
	
	cairo_set_source_surface(cr, mouthes[mouth], 145, 311);
	cairo_paint(cr);
	
	if( star ) {
		cairo_set_source_surface(cr, starImage, 39, 156);
		cairo_paint(cr);
	}
	if( heart ) {
		cairo_set_source_surface(cr, heartImage, 156, 334);
		cairo_paint(cr);
	}
	if( cheek ) {
		cairo_set_source_surface(cr, cheekImage, 64, 254);
		cairo_paint(cr);
		cairo_set_source_surface(cr, cheekImage, 336, 254);
		cairo_paint(cr);
	}
}

/**
 * <!--  drawMessage():  -->
 * 
 * セリフと吹き出し表示
 */
static void drawMessage( GtkWidget *widget,
						 cairo_t *cr ) {
	Message* currentMessage = messageManager.getCurrentMessage();
	if( currentMessage == NULL ) {
		return;
	}

	// 吹き出しの動きを計算
	movement.update();

	int fukidashiIndex = currentMessage->getFukidashi();

	double dx, dy;
	movement.getParam(dx, dy);
	
	cairo_identity_matrix(cr);

	if( horizontalFlipping ) {
		// 画面を左右反転する場合
		cairo_translate(cr, windowWidth, 0.0);	
		cairo_scale(cr, -1.0, 1.0);
	}

	cairo_scale(cr, 0.75, 0.75);
	cairo_translate(cr, 500.0 + dx, 50.0 + dy);
	
	// 吹き出し表示
	cairo_set_source_surface(cr, 
							 fukidashis[fukidashiIndex], 0, 0);
	cairo_paint(cr);

	// フォント選択
	const char* fontName = "Rounded M+ 2c";
	cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
	cairo_select_font_face(cr, fontName,
						   CAIRO_FONT_SLANT_NORMAL,
						   CAIRO_FONT_WEIGHT_BOLD);
	
	// 吹き出し内の文字列表示
	// 吹き出し内に収まる様にフォント幅、位置等を設定
	double cx;
	double cy;
	int cw;
	int ch;
	getSpeakRegionParam(fukidashiIndex, cx, cy, cw, ch);

	const String& str = currentMessage->getString();

	int lineSize = str.getLineSize();
	int maxLineSize = str.getMaxUTF8LineLength();
	int fontSize = (int)((double)cw / (double)maxLineSize);
	cairo_set_font_size(cr, fontSize);

	float spanY = fontSize * 1.2f;
	
	for(int i=0; i<lineSize; ++i) {	
		const char* line = str.getLineBuffer(i);
		int lineWidth;
		int lineHeight;
		calcStringExtent( cr, line, lineWidth, lineHeight );

		double posX = cx - lineWidth * 0.5;
		double baseY = cy + lineHeight * 0.5;
		double posY = baseY - (spanY * 0.5) * (lineSize-1) + spanY * i;
		cairo_move_to(cr, posX, posY);
		cairo_show_text(cr, line);
	}
}

/**
 * <!--  getTime():  -->
 */
static long long getTime() {
	struct timeval ts;
	gettimeofday(&ts, NULL);
	long long time = ts.tv_sec *1000L+ ts.tv_usec / 1000L;
	return time;
}

static long long lastTime = 0L;

/**
 * <!--  debugDumpTime():  -->
 */
static void debugDumpTime() {
	long long time = getTime();
	int dt = (int)(time - lastTime);
	printf("dt=%d\n", dt);
	lastTime = time;
}

/**
 * <!--  onDrawEvent():  -->
 */
static gboolean onDrawEvent( GtkWidget *widget,
							 cairo_t *cr,
							 gpointer user_data ) {
	calcWindowExtent( widget, windowWidth, windowHeight );
	
	messageManager.update();
	Message* currentMessage = messageManager.getCurrentMessage();
	if( currentMessage == NULL ) {
		return FALSE;
	}

	drawFace(cr);
	drawMessage(widget, cr);
	
	return FALSE;
}

/**
 * <!--  timerHandler():  -->
 */
static gboolean timerHandler(GtkWidget *widget) {
	gtk_widget_queue_draw(widget);
	return TRUE;
}

/**
 * writeValueBuffer():
 *
 * バッファへのvalue書き出し
 */
template <class T>
inline void *writeValueBuffer(const T& value, void *buffer) {
	memcpy(buffer, reinterpret_cast<const void*>(&value) , sizeof(T));
	buffer = reinterpret_cast<void*>(reinterpret_cast<char*>(buffer) + sizeof(T));
	return buffer;	
};

/**
 * readValue():
 *
 * バッファからのvalue読み出し
 */
template <class T>
inline const void *readValueBuffer(T& value, const void *buffer) {
	memcpy(reinterpret_cast<void*>(&value), buffer, sizeof(T));
	buffer = reinterpret_cast<const void*>
		(reinterpret_cast<const char*>(buffer)+ sizeof(T));
	return buffer;
};

/**
 * <!--  btol():  -->
 *  
 * エンディアン変換 (short)
 */
static inline short btol( short value ) {
	return ( ( (value >> 8) & 0x000000ff ) |
			 ( (value << 8) & 0x0000ff00 ) );
}

/**
 * <!--  btol():  -->
 *  
 * エンディアン変換 (int)
 */
static inline int btol( int value ) {
	return
		( (value >> 24) & 0x000000ff ) |
		( (value >> 8 ) & 0x0000ff00 ) |
		( (value << 8 ) & 0x00ff0000 ) |
		( (value << 24) & 0xff000000 );
}

/**
 * <!--  processCommandValue():  -->
 */
static void processCommandValue(int value) {
	if( value == 1 ) {
		// tenki
		setMessage2("明日は昼から\n雨みたい。。。",
					EXP_TALK_SAD, FUKIDASHI_MOWAMOWA, 5.0,
					"朝の間に\nおさんぽ行こう！", 
					EXP_TALK_NORMAL, FUKIDASHI_ROUND, 0.0 );
	} else if( value == 2 ) {
		// super moon
		setMessage2("今日\nスーパームーン\nだって!", 
					EXP_TALK_ANGRY, FUKIDASHI_BIKKURI, 3.0,
					"出てる？出てる？", 
					EXP_TALK_ANGRY, FUKIDASHI_BIKKURI, 0.0 );
	} else if( value == 3 ) {
		// bear
		setMessage("ここクマ\n出るんだって〜", 
				   EXP_KYOTON, FUKIDASHI_BUBBLE, 0.0 );
	} else if( value == 4 ) {
		// mountain
		setMessage2("ここ標高\n3000mあるよ？", 
					EXP_WAO, FUKIDASHI_ROUND_RECT, 2.0,
					"空気薄くない？？", 
					EXP_WAO, FUKIDASHI_ROUND_RECT, 0.0 );
	} else if( value == 5 ) {
		// one more tipee
		setMessage2("あっちにも\nTipeeいるよ！", 
					EXP_WINK, FUKIDASHI_RECT, 3.0,
					"女の子の\nグループだって!", 
					EXP_DELIGHT, FUKIDASHI_BUBBLE, 0.0 );
	} else if( value == 6 ) {
		// sleepy
		setMessage2("急に\nねむけが。。。", 
					EXP_NIYANIYA, FUKIDASHI_MOWAMOWA, 4.0,
					"Zzz...", 
					EXP_JII, FUKIDASHI_THINK, 0.0 );
	}
}

/**
 * processServer():
 *
 * Bluetoothの待ち受け&コマンド読み取り処理
 */
static gpointer processServer(gpointer data) {
	bool ret;
	// チャンネルは22で待ち受け
	ret = serverSocket.bind(BLUETOOTH_CHANNEL);
	if( !ret ) {
		printf("bluetooth server socket bind failed\n");
		return NULL;
	}

	while(true) {
		BluetoothSocket socket;
		ret = serverSocket.accept(socket);
		if( !ret ) {
			printf("bluetooth server socket accept failed\n");
			return NULL;
		}

		// クライアントが接続してきたら、セリフ表示を変更
		setMessage("今日は遠くまで\n来たね〜！", EXP_TALK_NORMAL, FUKIDASHI_ROUND, 0.0);
		
		int r;
		while(true) {
			// command type
			r = socket.read(buffer, 1);
			if( r <= 0 ) {
				printf("bluetooth socket read failed\n");
				break;
			}
			int commandType = buffer[0];

			if( commandType == COMMAND_VALUE ) {
				// 数字だけのコマンドの場合
				r = socket.read(buffer, 2);
				if( r <= 0 ) {
					printf("bluetooth socket read failed\n");
					break;
				}
				// 16bitの値を読み取り
				short value;
				readValueBuffer(value, buffer);
				value = btol(value);
				processCommandValue(value);
				
			} else if( commandType == COMMAND_MESSAGE ) {
				// 文字列を含んだコマンドの場合 (結局未使用)
				// utf message length
				r = socket.read(buffer, 2);
				if( r <= 0 ) {
					printf("bluetooth socket read failed\n");
					break;
				}
				// 文字列長を読み取り
				short length;
				readValueBuffer(length, buffer);
				length = btol(length);
				printf("utf length=%d\n", length);
				// UTF8の文字列を読み取り
				r = socket.read(buffer, length);
				if( r <= 0 ) {
					printf("bluetooth socket read failed\n");
					break;
				}
				buffer[length] = '\0';
				//..setMessage(buffer, "talk_normal");
			}
		}

		// クライアントとの接続が切れた
	}
	
	return NULL;
}

/**
 * <!--  parseOptions():  -->
 *
 * 起動オプション引数のパース
 */
static void parseOptions( int argc, char *argv[] ) {
	for(int i=1; i<argc; ++i) {
		const char* option = argv[i];
		if( strcmp(option, "-f" ) == 0 ) {
			// "-f"で左右反転
			horizontalFlipping = true;
		} else if( strcmp(option, "-e" ) == 0 ) {
			// "-e"でexpressionチェック用
			if( i < argc-1 ) {
				char* e;
				debugCheckExpressionId = strtol(argv[i+1], &e, 10);
				printf("debugCheckExpressionId=%d\n", debugCheckExpressionId);
			}
		}
	}
}

/**
 * <!--  main():  -->
 */
int main(int argc, char *argv[]) {
	// 起動オプション引数のパース
	parseOptions( argc, argv );

	gtk_init( &argc, NULL );
	GtkWidget *window = gtk_window_new( GTK_WINDOW_TOPLEVEL );

	// 画像のロード
	init();

	// 画面背景色を真っ黒に(各色16bitで色指定)
	GdkColor color;
	color.red = 0x0000;
	color.green = 0x0000;
	color.blue = 0x0000;
	gtk_widget_modify_bg( window, GTK_STATE_NORMAL, &color );
	
	g_signal_connect( window, "delete-event",
					  G_CALLBACK(gtk_main_quit), NULL );
	
	GtkWidget* darea = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(window), darea);
	g_signal_connect(G_OBJECT(darea), "draw", 
					 G_CALLBACK(onDrawEvent), NULL); 

	gtk_window_set_title(GTK_WINDOW(window), " ");
	
	// 一定間隔で画面を更新
	g_timeout_add(INTERVAL_MILLIS, 
				  (GSourceFunc)timerHandler, 
				  (gpointer)darea);

	// フルスクリーン表示
	gtk_window_fullscreen( GTK_WINDOW(window) );
	gtk_widget_show_all(window);

	// カーソル非表示
	GdkWindow* gdk_window = gtk_widget_get_window(window);
	GdkCursor* cursor = gdk_cursor_new( GDK_BLANK_CURSOR );
	gdk_window_set_cursor( gdk_window, cursor );
  
	// Bluetooth通信処理を別スレッドで起動
	g_thread_init(NULL);
	gdk_threads_init();
	gdk_threads_enter();
	{
		thread = g_thread_create( processServer, NULL, FALSE, NULL );
		gtk_main();
	}
	gdk_threads_leave();
	return 0;
}
