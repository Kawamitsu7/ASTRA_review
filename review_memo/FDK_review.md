# ASTRA Toolbox FDK法のコードレビュー

## 今までのまとめ(2020/1/8)

- FDK法の実際の処理は．**fdk.cu(.h)**に記述されている
  - 注意：任意のcone_vecジオメトリをサポートしていない．通常のコーンジオメトリの垂直サブジオメトリ（cf. Composite Geometry Manager）のみ．
  - **ここから先の処理内容は別にメモを作る**

## いままでのまとめ(2019/12/12)

- 実際にFDK法を呼び出しているのは

```python
astra.algorithm.run(algorithm_id)
```

### 入力 `algorithm_id`

- `astra.algorithm.create(alg_cfg)`で作成
  - この時FDK法をrunするための実体が作成され，`algorithm.run()`で呼び出される
- alg_cfg : algorithm用の設定パラメタ
  - {'type' : 'FDK_CUDA', 'ProjectionDataId' : projections_id(int型), 'ReconstructionDataId' : reconstruction_id(int型)}

- 実体の内容はおそらく **CudaFDKAlgorithm3D.cpp/.h** から派生するプログラムを読む必要があり



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

## AstraObjectManager.h

### get()関数

- 使っているのはおそらくCAstraObjectManagerBase内のやつ
  - CAstraObjectManagerクラスを継承 --> **【疑問】CAstraObjectManagerクラスはどこで作ってるのだろうか**
- m_tableにstore()されたオブジェクトを探すもの
- mapにキーとしてintを,オブジェクトに紐づけて格納
- その実態はSingleton?
- class CAstraObjectManagerがtemplateとして定義されている
  - get()関数に#includeされているものなら使用可能？ --> **Algorithm.hを見てみるか**

### ちょっと待って

- 下のほうにCAlgorithmManagerを発見
  - Singleton(CAlgorithmManagerを型に指定)と，CAstraObjectManager(CAlgorithmを型に指定)の多重継承

## Algorithm.h

- class CAlgorithmを定義
- #define _AstraExport は複数ファイルから#includeされることによる重複定義を防ぐ記号定数 (Globals.hに記述？)
- run関数はここに仮想関数として書いてあるけど中身が分からん

## 【疑問】`alg.run()`の行方は？

- algはSingletonパターンで作られたmanAlgから提供される．
- manAlg.get(i)を呼び出している．
- つまり PyAlgorithmManager.pxdに記載された，`cppclass CAlgorithmManager`クラスの`CAlgorithm * get(int)`
- すなわち，**AstraObjectManager.hのget()関数を読むべき**
  --> よくわからないidの管理をしていた



## 次

- 一番最初の`astra.astra_dict('FDK_CUDA')`を追いかけてみる？

## creators.py

- `astra_dict(intype)`はここに

- 入力 : String型のintype

- 出力 : dictクラス

  - ASTRAの辞書型オブジェクト
  - というかpythonが提供するdict型っぽい？

  ---

  ### dict型とは

  - {キー(str型) : 値} でデータ保持
  - dict["(今存在しないキー)"] = hoge とすると，新しいペアが追加される

  ---

- 結局どうなった？

  - FDK_astra.py内で
    - alg_cfg に {'type' : 'FDK_CUDA', 'ProjectionDataId' : projections_id(int型), 'ReconstructionDataId' : reconstruction_id(int型)} を構成
      - idについてはdata3d.pyを追っていけば分かりそう
    - `astra.algorithm.create(alg_cfg)`でidを取得，この際にアルゴリズム(FDK)の実行オブジェクトが生成される？
       --> こいつを追ってみよう

## 再び algorithm.py --> algorithm_c.pyx

### `create(config)`

- 入力 : `config` (dict型)
- 出力 : `manAlg.store(alg)`

- 処理流れ
  - Config型 で `cfg = utils.dictToConfig(six.b('Algorithm'), config)`
    - six.b(data) : 8bit文字で構成された文字列を返す
    - `utils.dictToConfig()` --> 詳しくは後で
  - CAlgorithm型 で `alg = PyAlgorithmFactory.getSingletonPtr().create(cfg.self.getAttribute(six.b('type')))`
    - これも詳しくは後で
  - algに対して例外処理
  - cfgの消去
  - `manAlg.store(alg)`
    - これも詳しくは後で

## utils.pyx

### `utils.dictToConfig(string rootname, dc)`

- Config 型
  - Config.cpp Config.h を参考
  - `XMLNode self, XMLDocument *_doc`を所持
  - メンバ関数 : `initialize(str rootname)`
    - 多分docの初期化
- `readDict(cfg.self, dc)`
  - 実行できなければ例外
  - 中身
    - val にdcの値をひとつづついれる
      - item に dcのキーをひとつづつ入れてる
    - val の形式が何であるかを調べてそれぞれに応じた処理
    - itemが'type'なら，`root.addAttribute(type, val)`
    - これはのちにSIngletonを作るとき，getAttributeで呼び出される
      - おそらくstrで"FDK_CUDA"が呼ばれる？

- 関連ファイルメモ
  - src/Config.cpp, astra/Config.h
    - Config型定義
    - XMLNode.cpp, XMLDocument.cppも関連？もちろんヘッダも
  - python/astra/PyIncludes.pxd
    - いろんなオブジェクトの定義
    - ここからいろんなcppやhに派生？
  - Singleton関係の前述のやつら

## PyAlgorithmFactory.pxd --> astra/AstraObjectFactory.h

### `getSingletonPtr().create(cfg.self.getAttribute(six.b('type')))`

- ここで 'FDK_CUDA' を引数としてcreateメソッドが呼ばれる

### `create(str _sType)`

- TypeListからfinderにタイプを探すもの

  - typelist::functor_find型 の finderを用いて検索

- ここではタイプ名"CAlgorithm"，参照するタイプリスト"AlgorithmTypeList"として，CalgorithmFactoryクラスが定義されており，それに対してgetSingletonPtr()で実態を生成する？

  #### Typelist.h --> AlgorithmTypeList.h

  - タイプリストの定義
  - コンストラクタや追加や削除，検索などの動作も定義
  - AlgorithmTypeListに使用できるタイプが列挙

- 多分だけど，**結局CudaFDKAlgorithm3D.cpp, CudaFDKAlgorithm3D.hに行き着く**
  
  - ここで割と具体的なrunが出てきた

## CudaFDKAlgorithm3D.cpp

- `void CCudaFDKAlgorithm3D::run(int _iNrIterations)`
  - initialize確認
  - 各種データを関数に食わせる準備
    - dynamic_cast : 参考- https://programming.pc-note.net/cpp/cast.html
  - FDKを実行する命令は2ヶ所…?
    - `#if 0 --> astraCudaFDK`
      - これは過去に作ったプログラムの断片らしい
      - `#if 0` は無効にする
    - そうでない --> cgm.doFDK
      - つまりこっちだけ見ればいい
      - cgm.doFDK は CompositGeometryManager.cpp (.h)によって提供

## CompositGeometryManager.cpp

- `doFDK()`
  - dynamic_cast できない場合はエラー
  - 変数 job にFDKのジョブ情報を作成
  - ジョブリストにプッシュ
  - `doJobs(L)`で実行
- `doJobs()`
  - 多分`dojob(iter)`で呼び出す
  - ここの`ASTRA_DEBUG`でシングルスレッドでやるかマルチスレッドでやるかが分かりそう
- `dojob()`
  - GPUのメモリアロケーションとかやってる
  - `switch(j.eType)`で，`case CCompositeGeometryManager::Sjob::JOB_FDK`にアクセス
  - `ok = astraCUDA3d::FDK()`でFDKを実行に移す
    - どうもサブタスクに分割(sub-FDK)して実行している？ (l.1530)
    - astra3d.cu(.h)を呼び出しそいつがfdk.cu(.h)を呼び出していると考えられる

## CudaFDKAlgorithm3D.cpp --> CudaFDKAlgorithm3D.h

### `CCudaFDKAlgorithm3D::run(int _iNrIterations)`

- 多分これが呼ばれる
- initializeされたかどうか確認
- pSinoMem，pReconMem，pFilterDataには対応するデータポインタをdynamic_castして代入
- filter にpFilterDataから`getDataConst()`でフィルタデータを読み出す？
- `cgm.doFDK(m_projector,pReconMem,pSinoMem,m_bShortScan, filter)`で次へ行く？

## やること

- m_projector, m_bshortScanってなんやねん
- CCompositeGeometryManager.h から doFDKを見つける
