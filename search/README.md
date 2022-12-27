## beam-10k.cpp/beam-2k.cpp
### usage

- Create input files

please refer to create_input_files.py.
```
python create_input_files.py
```

- Run
```
g++ beam-10k.cpp -o beam-10k.out -O3 -std=c++14
./beam-10k.out < input.txt > output.txt
```

    - ビームサーチ的な探索をして、grid上のパスに対応するコスト最適なconfigを見つける
    - コスト最適なもののみをサーチするので、多少の追加コストを許すサーチはc-numberさんのコードでやる
    - 初期幅10000(beam-10k.cpp)/2000(beam-2k.cpp)、評価関数はアームのうち現在位置が角にあるものの本数（２方向に動けて自由度が高いため）。同スコアも多いためスコア上位のstateから一様乱数で選び、次のstate候補を作成する
    - 64-armの位置は、最初にpositionの遷移を先読みしてDP、遷移不可能なものは除外する
    - 有効なconfigが見つからなかった場合、256ステップ前に戻ってビーム幅を2倍にする
        - 幅に適当な上限つき、幅が上限になっても乱数でstateを選んでいるので探索は続く
    - ゴール時点でのconfigを設定する機能は特にないため、始点から終点まで一発では計算できない
        - 四隅のconfigは一意に決まるので、start->corner, goal->cornerの二本を計算すればOK
        - 分割してTSPしていた名残なので、後で使いやすく直したい
    - Stateを保持するのでメモリ使用量が大きい。大体 steps * width 個のlong long

### inputデータの例
```
5
64 0 -32 0 -16 0 -8 0 -4 0 -2 0 -1 0 -1 0 
0 0
0 1
0 2
0 3
0 4
```

一行目：パスの長さ
二行目：始点のconfig
三行目以降：Pathに対応するposition (Cartesian, x in [-128, 128], y in [-128, 128])

### outputデータの例
```
64 64 32 32 16 16 8 8 4 4 2 2 1 0 1 1 
64 64 32 32 16 16 8 8 4 4 2 1 1 0 1 1 
64 64 32 32 16 15 8 8 4 4 2 1 1 0 1 1 
64 63 32 32 16 15 8 8 4 4 2 1 1 0 1 1 
64 62 32 32 16 15 8 8 4 4 2 1 1 0 1 1 
```
各行：config (inputの逆順)

### cerr
- エラー出力はログ
```
step: x y: n_next_state
```
