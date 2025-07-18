# elf2x68k サンプルプログラム

### hello

* printf()とシンプルなIOCSコール呼び出しのサンプルです
* makeすると hello.x というファイルができます。Human68kでそのまま実行できます

### hellosys

* 上記 sample/hello と同じ内容を、Human68k を使わずに実行するサンプルです
* makeすると hellosys.sys というバイナリが生成されます。このファイルは以下の手順で起動できます。
  1. 実機でフォーマット済みのフロッピーディスク、またはエミュレータでフォーマット済みディスクイメージを用意します
  2. hellosys.sys を **human.sys という名前で** ディスクにコピーします
      * ディスクには他のファイルを置かないでください。ブートセクタから起動できるファイルはディスクの連続したセクタに配置されている必要があるためです。
  3. 作成したディスク(イメージ)を実機またはエミュレータにセットし、リセットします。

### fileio

* newlibのファイルI/O周りのAPIをテストするサンプルです

### xsp

* よっしん氏 (@yosshin4004) 作の [スプライト管理ライブラリ XSP](https://github.com/yosshin4004/x68k_xsp) のサンプルコードを elf2x68k 環境でビルドできるようにしたサンプルです

### defsptool

* Human68k システムディスクに含まれているスプライトパターンエディタ DEFSPTOOL.BAS を C に変換できるようにパッチを当てて m68k-xelf-bas でコンパイルするサンプルです

### hat

* メキシカンハットを表示するサンプルです。make を実行すると X-BASIC で書かれたプログラムを C に変換してコンパイルします。
* 参照: https://togetter.com/li/2390318

### hellocpp

* C++の機能を使ったサンプルです
* スマートポインタやラムダ式などのModern C++の機能も使用できます

### pthread

* POSIXスレッドライブラリのサンプルです
* 複数のスレッドを使って排他制御、条件変数、バリア、読み書きロック、スピンロックのテストを行います
* 同一のソースコードから、X68kで動作するバイナリとクロス開発環境側で動作するバイナリの両方が得られます

### httpclient

* ソケットライブラリを用いた簡単なHTTPクライアントのサンプルです
* httpclient.x <ホスト名> <パス> <ポート> の形式で実行します
  * 例: `httpclient www.retropc.net /x68000/ 80`
* HTMLタグはほぼ無視して改行のみを行います
* シフトJIS以外の文字コードは表示できません

### ntptest

* ソケットライブラリを用いてNTPサーバーから時刻を取得するサンプルです
* デフォルトでは `ntp.nict.jp` を使用しますが、コマンドライン引数で別のNTPサーバーを指定することもできます
