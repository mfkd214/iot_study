# ESP8266にMicroPythonを入れてWebREPLを使えるようにする

## 参考

http://docs.micropython.org/en/latest/esp8266/tutorial/intro.html

## ファームウェアの入手

http://micropython.org/resources/firmware/esp8266-20190125-v1.10.bin

## ファームウェアのデプロイ

1.esptoolのインストール
    $ pip3 install esptool

2.フラッシュの消去
    $ esptool.py --port /dev/tty.usbserial-DN02B447 erase_flash

3.ファームウェアの書き込み

    $ esptool.py --port /dev/tty.usbserial-DN02B447 --baud 460800 write_flash --flash_size=detect 0 esp8266-20190125-v1.10.bin

    インストールが成功すると、WiFiアクセスポイント（AP）として設定されます。ESSIDはMicroPython-xxxxxxという形式。WiFiのパスワードはmicropythoNです（大文字のNに注意してください）。ネットワークに接続すると、IPアドレスは192.168.4.1になります。

4.REPL

REPLはRead Evaluate Print Loopの略で、ESP8266でアクセスできる対話型のMicroPythonプロンプトに付けられた名前です。

    $ screen /dev/tty.usbserial-DN02B447 115200


5.WebREPL

- 使う前に、シリアルポート経由で有効にする必要がある

    >>> import webrepl_setup
    WebREPL daemon auto-start status: disabled

    Would you like to (E)nable or (D)isable it running on boot?
    (Empty line to quit)
    > E
    To enable WebREPL, you must set password for it
    New password (4-9 chars):<your password>
    Confirm password: <your password>
    Changes will be activated after reboot
    Would you like to reboot now? (y/n)
    y

- WebREPLクライアントをローカルにクローン

    $ git clone https://github.com/micropython/webrepl.git

- webrepl.htmlをchromeまたはfirefoxで開く
