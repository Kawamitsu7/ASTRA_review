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

#### `FDK_PreWeight`

- 投影データに係数乗算
- 係数 = 光線の中央線(X,z)=(0,0)とその検出器までの線との$\cos$
- `devFDK_ParkerWeight()`で実行

#### `FDK_Filter`

- Filterインスタンスの生成
- サイノグラム1枚ずつ処理と書いてある (l.279)
- FFT --> Filter適用 --> IFFT の処理を行う

#### `ConeBP`

- 投影を配列に変換？(l.342)
- -->`ConeBP_Array()`-->`dev_cone_BP()`or`dev_cone_BP_SS()`

##### `dev_cone_BP()`

- いわゆる逆投影処理を行ってるっぽい…？