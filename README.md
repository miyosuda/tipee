tipee
=====

Camp Hack Day 2014 "喋るテントTipee" ソース

# 動作環境
サーバ環境
- Raspberry Pi (2014-06-20-wheezy-raspbian)
- Bluetooth ドングル (デモ時はBuffalo BSHSBD04BKを利用)

クライアント環境
- Nexus5 (Android 4.4.4)


# サーバ設定

## ライブラリインストール
Bluetoothツール,ライブラリインストール

    $ apt-get install -y bluetooth bluez-utils blueman libbluetooth-dev libgtk-3-dev

GTKインストール

    $ apt-get install -y libgtk-3-dev

## Bluetooth設定
/etc/rc.local に下記追加して起動時にBluetooth Channel22で SPPに. スキャン可能に設定.

    sdptool add --channel=22 SP
    hciconfig hci0 piscan
    hciconfig hci0 name ‘pi’
    
## フォント設定
http://d.hatena.ne.jp/itouhiro/20120226
より丸文字フォント、rounded-mplus-2c-bold.ttf を取得.
/usr/share/fonts/truetype/
以下に
rounded ディレクトリを作成し，中に
rounded-mplus-2c-bold.ttf
を配置

    $ fc-cache -f -v

でフォントキャッシュを更新

## 解像度変更
解像度が高いままだと、画面の更新が重かったので、解像度を下げる.

/boot/config.txt

に、

    hdmi_group=2
    hdmi_mode=14

上記を設定. 解像度を848x480に変更.
リブート (sudo reboot)




