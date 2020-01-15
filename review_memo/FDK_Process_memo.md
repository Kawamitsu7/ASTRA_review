# ASTRA Toolbox FDK処理 レビューメモ

## 処理が記述された場所

- astra-toolbox-master/cuda/3d/ **fdk.cu**

- astra-toolbox-master/include/astra/cuda/3d/ **fdk.h**
  - 上記2つの`bool FDK()`によって実施

## TODO

- 入力データ洗い出し
- 出力データ洗い出し
- 関数を追いかける

## メモ

- fdk.cu はスタンドアローン向けコンパイルを想定したコードが後半部分に書いてあるらしい…？
  - 変数などはこれを参考にすることもできる？

## 関数を追いかける

### fdu.cu(.h)

- まず，座標軸周りのパラメタを計算
- 次に以下の関数にかける
  - `FDK_PreWeight()`
  - `FDK_Filter()`
    - 上記2つはfdk.cu内に記述
  - `ConeBP()`
    - astra/cuda/3d/cone_bp.h
    - つまりcone_bp.cuに記述？
- 全てできたならtrueを返す