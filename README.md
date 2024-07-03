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
