/*****************　　　　　　　　　　Alg1: 15Puzzle　C言語解答例　　　　　　　　*************************/
/*この解答例は誰もが仕組みを理解しやすいように作成した簡易的なプログラムである。　　　　　　　　　　　　　　　　　　*/
/*Alg1ではバックトラック手法を学ぶために8パズルや15パズルといった問題を出題している。　　　　　　　　　　　　　　　*/
/*そのため、このプログラムは反復深化のみを使用したプログラムとなっている。　　　　　　　　　　　　　　　　　　　　　*/
/*あくまでAOJの審査に通るための必要最低限のものだけ使用した。　　　　　　　　　　　　　　　　　　　　　　　　　　　*/
/*つまり計算速度はそこまで速くない。(テストケース#28の場合、0.11sかかった。)　　　　　　　　　　　　　　　　　　　*/
/*優先度付きキュー等用いて計算速度を速くしたいのであればこのプログラムに追加して実装すると良い。　　　　　　　　　　*/


/*****************　　　　　　　 　　     　　　解説　　　 　　     　　　　　　　 ***********************/
/*①まず、ヒューリスティック数と最低コストをマンハッタン距離の総数で初期化する。　　　             　　　　　　　*/
/*②パズルの空白の場所を特定する。　　　　　　　　　　　　　　　　　　　　　　　　　　             　　　　　　　*/
/*③最短手数が見つかるまで深さ優先探索を繰り返す。　　　　　　　　　　　　　　　　　　　             　　　　　　*/
/*　この際、（深さ＋コスト）がヒューリスティック数を超えたら計算しないようにする。　　　                  　　　*/
/*④DFSを抜けてもfewestStepsが見つからなかったらヒューリスティック数分の探索では見つからなかったことを意味する。*/
/*　よってDFSが終わったらヒューリスティック数を+1して更新する。     　　　　　　　　　　　　　　　　        　　*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//縦(LENGTH)と横(WIDHT)の長さを定義する。
#define LENGTH 4
#define WIDTH 4

//INFの値(適応でよい)
#define INF 9999999

//15パズル(今回は全てグローバル変数として定義)
//なぜ一次元配列かというとmemcpyやmemcmp等の既存関数を使って簡単にコピー、判定処理をしたかったからである。
int ansPuzzle[LENGTH*WIDTH];//15パズルの解答を入れる1次元配列
int problemPuzzle[LENGTH*WIDTH];//15パズルの問題を入れる1次元配列

//マスを動かす方向の情報を入れた配列
//それぞれ{右, 上, 左, 下}となっている。
int dx[4]= {1, 0, -1, 0};//x軸(横軸)に対する配列
int dy[4]= {0, -1, 0, 1};//y軸(縦軸)に対する配列

int heuristic;//ヒューリスティック値
int fewestSteps = -1;//最短手数(初期値を-1とする)

int calcManhattanDist(int, int);//マンハッタン距離を計算する関数
void calcHeuristic();//最初のヒューリスティック数(今回は最低コストを使用)を求める関数
void findMoveableDirection(int*, int*);//行動可能な方向を見つける関数
int depthFirstSearch(int*, int, int, int, int);//深さ優先探索で最短手数を求める関数


//メインでは主にパズルの入力や解答の計算、出力を行っている。
int main(){

    int zeroLocation;//空白(0)の位置の要素数
    int mimimumCost;//問題の最低コスト(マンハッタン距離の総数)

    for(int i=0; i<LENGTH*WIDTH; i++){
        scanf("%d", &problemPuzzle[i]);//配列に問題のパズルを入れる。
        ansPuzzle[i] = (i+1)%(LENGTH*WIDTH);//パズルの答えを初期化する。
    }

    //ヒューリスティック数及び最低コストの初期化。
    calcHeuristic();
    mimimumCost = heuristic;

    //０の要素番号を特定する。
    for(int i=0; i<WIDTH*LENGTH; i++){
        if(problemPuzzle[i]==0){
            zeroLocation = i;
            break;
        }
    }

    //最短手数を求める。
    while(fewestSteps==-1){

        depthFirstSearch(problemPuzzle, 0, mimimumCost, -1, zeroLocation);

        //DFSを抜けてもfewestStepsが見つからなかったらヒューリスティック数分の探索では見つからなかったことを意味する。
        //よってDFSが終わったらヒューリスティック数を+1して更新する。
        heuristic+=1;
    }

    printf("%d\n", fewestSteps);

    return 0;
}


//マンハッタン距離を求める関数
//マンハッタン距離は本来の場所まで戻るまでの総距離のことを指す。
//左右に対する残りの距離　＝　(現在のマスにある数-1)%(パズルの横の長さ)-(本来あるべき数-1)％(パズルの横の長さ)の絶対値
//上下に対する残りの距離　＝　(現在のマスにある数-1)/(パズルの縦の長さ)-(本来あるべき数-1)/(パズルの縦の長さ)の絶対値
int calcManhattanDist(int puzzleNum, int ansNum){
    puzzleNum-=1;
    return abs( puzzleNum%WIDTH - ansNum%WIDTH ) + abs( puzzleNum/LENGTH - ansNum/LENGTH );
}


//ヒューリスティックを計算する関数
//今回はヒューリスティック値としてパズルを完成させるために必要な最低コストを使う。
void calcHeuristic(){

    for(int i=0; i<LENGTH*WIDTH; i++){
        //空いているマス以外でヒューリスティックを計算する。
        if(problemPuzzle[i]!=0){
            heuristic += calcManhattanDist(problemPuzzle[i], i);   
        }
    }
}

//移動可能な方向を見つける関数
void findMoveableDirection(int* puzzle, int* direction){
    for(int i=0; i<LENGTH*WIDTH; i++) {
        if(puzzle[i]==0){
            //j = 0:右、1:上、2:左、3:下
            for(int j=0; j<4; j++){
                //上下左右で動けるかどうか判定
                if((i%WIDTH)+dx[j]>=0
                && (i%WIDTH)+dx[j]<WIDTH 
                && (i/WIDTH)+dy[j]>=0 
                && (i/WIDTH)+dy[j]<LENGTH
                ) direction[j]=1;
                else direction[j]=0;
            }
            return;//breakでも可
        }
    }
}

//深さ優先探索で最短手数を求める関数。
//もし最短手数が見つかったら１を返す。ダメなら0を返す。
int depthFirstSearch(int* currentPuzzle, int depth, int currentCost, int moveDict, int zeroLocation){
    
    int tempIndex;
    int tempPuzzle[LENGTH*WIDTH];
    int direction[4];//それぞれの方向に動けるかどうかの情報を入れる配列{要素0: 右, 要素1: 上, 要素2: 左, 要素3: 下}
    int moveDictArray[4];//それぞれの方向に動いた際の最短コストの変動を入れる配列。動けない場合はINFを代入

    //現在のパズルと解答が一致するかどうかを調べる。
    //memcmp()は配列の中身を比較する関数(同じなら0返す)。
    //ProgCの第４回「ポインタ（1)」で習ったstrcmp()と使い方が似ている。
    if(memcmp(currentPuzzle, ansPuzzle, sizeof(int)*(LENGTH*WIDTH))==0) {
        fewestSteps = depth; //深さ(探索回数)を代入する。
        return 1; //一致したら1を返す。
    }

    //もし深さがヒューリスティック数を超えたら0を返す。
    if(depth > heuristic) return 0;

    //どこに動くことが可能かを調べる。
    findMoveableDirection(currentPuzzle, direction);

    //それぞれの方向に動いた際の最短コストの変動を計算する。
    //大体は1つ動かすとマンハッタン数が1増えるか減るかのどちらかである。
    for(int i=0; i<4; i++){

        //もし前回動いた方向の反対方向か動けない方向の場合はINFを入れる。
        if(moveDict==(i+2)%4||direction[i]==0) {

            moveDictArray[i] = INF;

        } else {

            moveDictArray[i] = calcManhattanDist(
                                    currentPuzzle[zeroLocation+dx[i]+WIDTH*dy[i]], //0の位置から上下左右に１マス動いた場所の値
                                    zeroLocation//0の位置の要素番号(上記の値が動く場所の要素番号)
                                )
                                - calcManhattanDist(
                                    currentPuzzle[zeroLocation+dx[i]+WIDTH*dy[i]],//0の位置から上下左右に１マス動いた場所の値
                                    zeroLocation+dx[i]+WIDTH*dy[i]//元々いた場所の要素番号
                                );
        }
    }

    for(int i=0; i<4; i++){

        //もし距離の変動がINFの場合、飛ばす。
        if(moveDictArray[i] == INF) continue;

        //もし(深さ＋動いたあとの合計コスト)がヒューリスティック数を超える場合、飛ばす。
        if(depth+currentCost+moveDictArray[i]> heuristic) continue;

        //memcpy()は第二引数の配列の中身を第一引数の配列に第三引数分コピーする関数。
        //ProgCの第４回「ポインタ（1)」で習ったstrcpy()と使い方が似ている。
        memcpy(tempPuzzle, currentPuzzle, sizeof(int)*(LENGTH*WIDTH));//仮のパズルに現在の情報をコピーする。

        //値の入れ替え
        tempIndex = zeroLocation+dx[i]+WIDTH*dy[i];
        tempPuzzle[zeroLocation] = currentPuzzle[tempIndex];
        tempPuzzle[tempIndex] = currentPuzzle[zeroLocation];

        //再帰
        if(depthFirstSearch(tempPuzzle, depth+1, currentCost+moveDictArray[i], i, tempIndex)==1) return 1;

    }
    return 0;
}
