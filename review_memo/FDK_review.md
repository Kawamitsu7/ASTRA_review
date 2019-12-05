# ASTRA Toolbox FDK法のコードレビュー

## FDK法の呼び出し方

- 実際に呼び出しているのは

```python
astra.algorithm.run(algorithm_id)
```

- 入力 `algorithm_id`
  - `astra.algorithm.create(alg_cfg)`で作成
  - alg_cfg : algrithm用の設定パラメタ
    - まず`astra.astra_dict('FDK_CUDA')`でざっくり設定
    - `ProjectionDataId,ReconstructionDataId`をそれぞれ事前に作成した`projections_id,reconstruction_id`に設定
- 出力
  - 3dData
  - 結果は`reconstruction_id`に指定された領域に生成される
    - `astra.data3d.get(reconstruction_id)`で抜き出すことができる

## `astra.algorithm.run(algorithm_id)`を追いかける

### algorithm.py

- `from . import algorithm_c as a`
  - 同ディレクトリの algorithm_c.pyx インポートしている？
- `def run(i, iterations=1)`
  - 実行部分
  - パラメータ
    - i : オブジェクトのID (int 型?)
    - iterations : 繰り返し回数 (int型)
  - `return a.run(i,iterations)`で呼び出し

### algorithm_c.pyx

- pyxとは
  - Cythonのスクリプトファイル
  - Cython は、**C言語によるPythonの拡張モジュールの作成の労力を軽減**することを目的として開発されたプログラミング言語である。その**言語仕様はほとんど Python のものと同じ (上位互換) だが、Cの関数を直接呼び出したり、C言語の変数の型やクラスを宣言できる**などの拡張が行われている。Cython の処理系ではソースファイルを C のコードに変換し、コンパイルすれば Python の拡張モジュールになるようにして出力する。
    Wikipediaより
- `def run(i, iterations=0)`
  - <CAlgorithm *>型(?)で`alg`に`getAlg(i)`の結果を代入
  - itsの分だけGILなしで`alg.run(its)`を実行
- `cdef CAlgorithm * getAlg(i) except NULL:`
  - <CAlgorithm *>型(?)で`alg`に`manAlg.get(i)`の結果を代入
  - algの例外処理を実行
  - algを返す
- `cdef CAlgorithmManager * manAlg = <CAlgorithmManager * >PyAlgorithmManager.getSingletonPtr()`
  - PyalgorithmManagerをcimportしている

## PyalgrithmManager.pxd

- `cdef extern from ~~~`
  - Cythonが生成するCのファイル上に\~\~\~を#includeするような分を出力
  - 関数内はPythonのまま実行？
- つまり？
  - astra-toolbox-master/include/astra 内の AstraObjectManager.h を参照している
- `cdef cppclass CAlgorithmManager:`
  - クラス宣言
  - ヘッダファイルから必要なメンバ関数を取得
- `cdef CAlgrithmManager* getSingletonPtr`
  - 多分シングルトン型をコンストラクトしてる？

## 【疑問】CAlgorithm型とは?

## 【疑問】`alg.run()`の行方は？

- algはSingletonパターンで作られたmanAlgから提供される．
- manAlg.get(i)を呼び出している．
- つまり PyAlgorithmManager.pxdに記載された，`cppclass CAlgorithmManager`クラスの`CAlgorithm * get(int)`
- すなわち，**AstraObjectManager.hのget()関数を読むべき**