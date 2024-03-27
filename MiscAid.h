#pragma once
#include <iostream>
#include <list>
#include <vector>
using namespace std;

#define BALLNUM  6  // 这个是游戏中球球的总数量
#define GLUENUM  2  // 这个是游戏中胶水球的数量

RBTree<State> T;

uint16_t TempBinaryBalls[16];
uint64_t * AP = (uint64_t *) & (TempBinaryBalls[0]);
uint64_t * BP = (uint64_t *) & (TempBinaryBalls[4]);
uint64_t * CP = (uint64_t *) & (TempBinaryBalls[8]);
uint64_t * DP = (uint64_t *) & (TempBinaryBalls[12]);
unsigned short StoneAry[16];    // 用二进制表示局面中固定石头的样子

void GenerateVirtualObstaclesList();



/***********************************************************************************************************************************
 *  下面代码是生成胶水球的组合数组； 比如总球数5个, 胶水球数量3个, 那么会生成一套含有头部和尾部的一堆数组；比如第一个数组：它的头  *
 *  部是[0][1]，而尾部是[2][3][4], 它们组合起来表示[0][1][2], [0][1][3]和[0][1][4]这三种胶水球组合, 这样的话，到计算时，一套组合拳 *
 *  里只需要切换2,3,4即可, 头部0和1可以固定住免切换以节省运行效率。到末尾再切换数组到[1、2]的头部组合（它的对应尾部是[3、4]        *   
 ***********************************************************************************************************************************/
char AidAry[GLUENUM - 1]; // 一个为了生成变体固定球组合而声明的临时辅助数组
struct VariationCombine
{
    char _headAry[GLUENUM - 1];  // 这个是头部数组
    char _tailAry[BALLNUM];      // 这个是尾部数组
    VariationCombine() 
    {
        for (size_t i = 0; i < BALLNUM; i++) _tailAry[i] = -1;
    }
};

vector<VariationCombine> *GluesCombine = new vector<VariationCombine>(); // 全局变量指针, 它就是所有胶水球的变体组合

void RecursionVariation(int start, int end, int phase)
{
    if (1 == phase)
    {
        VariationCombine *temp = new VariationCombine();
        for (size_t i = 0; i < GLUENUM-1; i++)
        {
            temp->_headAry[i] = AidAry[i];
        }
        for (size_t i = start; i < end; i++)
        {
            temp->_tailAry[i-start] = i;
        }
        GluesCombine->push_back(*temp);
    }
    else if (phase > 1)
    {
        for (size_t i = start; i < end; i++)
        {
            AidAry[GLUENUM - phase] = i;
            RecursionVariation(i + 1, end + 1, phase - 1);
        }
    }
}

void GenerateVariation(int total, int variation)
{
    RecursionVariation(0, total - variation + 1, variation);
}
/*******  到了这里关于胶水球组合的算法已经全部完毕 *******/


// 随机生成固定的石头
void RandGenerateStone(int stoneNum = 16)
{
    memset(StoneAry, 0, 32); // 先清空所有的石头

    for (int i = 0; i < stoneNum; i++)
    {
        int index = rand() % 16;
        int offset = rand() % 16;
        StoneAry[index] = (1 << offset) | StoneAry[index];
    }

    GenerateVirtualObstaclesList(); // 最后不要忘了刷新虚拟障碍list
}
// 手动生成一些石头
void ManualGenerateStone(bool saveStone = false)
{
    if (!saveStone) memset(StoneAry, 0, 32);

    int maxStone = 0;
    cout << "请输入您想要的最多石头数量:";
    cin >> maxStone;
    int x, y;
    for (int i = 0; i < maxStone; i++)
    {
        cout << "请输入第" << i + 1 << "个石头的X坐标:";
        cin >> x;
        cout << "请输入第" << i + 1 << "个石头的Y坐标:";
        cin >> y;
        StoneAry[y] |= (1 << x);
    }

    GenerateVirtualObstaclesList();
}

/** 下面四个全局变量数组，表示的是局面中的公共障碍石 **/
list<int> UpList[16];
list<int> DownList[16];
list<int> LeftList[16];
list<int> RightList[16];

// 根据局面中固定石头的状态，来生成上下左右四个方面的虚拟障碍物
void GenerateVirtualObstaclesList()
{
    // 因为是重新生成所有的virtualobstacles, 所以这里先清空以前生成的virtualobstacles
    for (size_t i = 0; i < 16; i++) 
    {
        UpList[i].clear();
        DownList[i].clear();
        LeftList[i].clear();
        RightList[i].clear();
    }

    /******生成往上吹风和往下吹风的16条虚拟障碍链*****/
    for (int row = 0; row < 16; row++)
    {
        UpList[row].push_back(15); // 往上吹风,不管啥情况, 先虚拟出一个高度为15的虚拟障碍, 所有道都是如此(其它三个也是同理不再赘述)
        bool setFlag = false;      // 这里是往上吹风, 比如在第七道上, 有一个高度为5的实体障碍, 那么就应该添加一个高度为4的虚拟障碍物
        for (int line = 15; line >= 0; line--)
        {
            if ((StoneAry[line] & (1 << row))) // 若循环到的这个位置有一颗石头
                setFlag = true;                // 设置障碍的标志置为真（但是先不设置障碍）
            else if (setFlag == true)          // 不然就是循环的这个位置没有石头，但是设置障碍的标志为true，说明以前有过石头（这是石头之后的第一个空位）
            {
                UpList[row].push_front(line);  // 那么在这里真实设置一个障碍
                setFlag = false;               // 设置障碍的标志置为false
            }
        }

        DownList[row].push_back(0);
        setFlag = false;
        for (int line = 0; line <= 15; line++)
        {
            if ((StoneAry[line] & (1 << row))) setFlag = true;
            else if (true == setFlag) 
            {
                DownList[row].push_front(line);
                setFlag = false;
            }
        }
    }


    /******生成往左吹风和往右吹风的各16条虚拟障碍链******/
    for (int line = 0; line < 16; line++)
    {
        LeftList[line].push_back(15);
        bool setFlag = false;
        for (int row = 15; row >= 0; row--)
        {
            if ((StoneAry[line] & (1 << row))) setFlag = true;
            else if (true == setFlag)
            {
                LeftList[line].push_front(row);
                setFlag = false;
            }
        }

        RightList[line].push_back(0);
        setFlag = false;
        for (int row = 0; row < 16; row++)
        {
            if ((StoneAry[line] & (1 << row))) setFlag = true;
            else if (true == setFlag)
            {
                RightList[line].push_front(row);
                setFlag = false;
            }
        }
    }
}

// 这个函数是在控制台中看虚拟障碍物的, mode是看哪个方向的; 总体来说此函数用的不多, 可以考虑删除掉
void ShowVirtualObstacle(int mode)
{
    string tu[16];
    for (size_t i = 0; i < 16; i++) tu[i] = "________________________________ 看虚拟阻碍" + to_string(i);
    switch (mode)
    {
    case 0:
        cout << "F E D C B A 9 8 7 6 5 4 3 2 1 0" << endl;
        for (size_t i = 0; i < 16; i++)
        {
            for (list<int>::iterator item = UpList[i].begin(); item != UpList[i].end(); item++) {
                tu[*item].replace(30 - i * 2, 2, "上");
            }
        }

        for (int i = 15; i >= 0; i--) cout << tu[i] << endl;
        cout << "F E D C B A 9 8 7 6 5 4 3 2 1 0" << endl;
        break;

    case 1:
        cout << "F E D C B A 9 8 7 6 5 4 3 2 1 0" << endl;
        for (int i = 0; i < 16; i++) {
            for (list<int>::iterator item = DownList[i].begin(); item != DownList[i].end(); item++) {
                tu[*item].replace(30 - i * 2, 2, "下");
            }
        }

        for (int i = 15; i >= 0; i--) cout << tu[i] << endl;
        cout << "F E D C B A 9 8 7 6 5 4 3 2 1 0" << endl;
        break;

    case 2:
        cout << "F E D C B A 9 8 7 6 5 4 3 2 1 0" << endl;
        for (int i = 0; i < 16; i++)
            for (list<int>::iterator item = LeftList[i].begin(); item != LeftList[i].end(); item++)
                tu[i].replace(30 - *item * 2, 2, "左");

        for (int i = 15; i >= 0; i--) cout << tu[i] << endl;
        cout << "F E D C B A 9 8 7 6 5 4 3 2 1 0" << endl;
        break;

    case 3:
        cout << "F E D C B A 9 8 7 6 5 4 3 2 1 0" << endl;
        for (int i = 0; i < 16; i++)
            for (list<int>::iterator item = RightList[i].begin(); item != RightList[i].end(); item++)
                tu[i].replace(30 - *item * 2, 2, "右");

        for (int i = 15; i >= 0; i--) cout << tu[i] << endl;
        cout << "F E D C B A 9 8 7 6 5 4 3 2 1 0" << endl;
        break;
    default:
        break;
    }
}


struct BallState
{
    uint8_t _posX[BALLNUM];
    uint8_t _posY[BALLNUM];
    BallState* _parent;

    BallState(BallState* p) : _parent(p) {}

    // 下面这个是一个比较函数, 用other的球球们状态, 比较本球球们的状态, 看它们是否一样
    bool Compare(BallState* otherP)
    {
        uint16_t myAry[16];        // 请注意，要比较俩局面是否相同的话，不能单纯靠球球们是否位置都一样来判断，比如A局面两个球球，1球在X1位置，2球在X2位
        uint16_t otherAry[16];     // 置，然后B局面的1球在X2位置，2球在X1位置，用上面方法比较的话， 会判断俩局面不相等，这显然是错的，因为实际俩局面相等
        uint64_t* ap, * bp, * cp, * dp;
        uint64_t* oap, * obp, * ocp, * odp;
        ap = (uint64_t*)myAry;
        bp = (uint64_t*)&myAry[4];
        cp = (uint64_t*)&myAry[8];
        dp = (uint64_t*)&myAry[12];
        oap = (uint64_t*)&otherAry[0];
        obp = (uint64_t*)&otherAry[4];
        ocp = (uint64_t*)&otherAry[8];
        odp = (uint64_t*)&otherAry[12];
        *ap = *bp = *cp = *dp = *oap = *obp = *ocp = *odp = 0;

        for (size_t i = 0; i < BALLNUM; i++)
        {
            myAry[_posY[i]] |= (1 << _posX[i]);
            otherAry[otherP->_posY[i]] |= (1 << otherP->_posX[i]);
        }

        if (*ap != *oap) return false;
        if (*bp != *obp) return false;
        if (*cp != *ocp) return false;
        if (*dp != *odp) return false;
        return true;
    }
};


list<BallState*> DeepThinkListAry[100]; // 程序思考过的所有球球们局面, 保存进这里
uint8_t DeepThinkLv = 0;                // 这个数值代表程序思考了多少层, 为0说明程序还没有思考呢

list<BallState*> FileStatesList;


// 随机生成球球们
void RandGenerateBalls()
{
    *AP = *BP = *CP = *DP = 0;
    BallState* balls = new BallState(nullptr);
    for (size_t i = 0; i < BALLNUM; i++)
    {
        start:
        int randX = rand() % 16;
        int randY = rand() % 16;
        if (StoneAry[randY] & (1 << randX)) goto start;

        balls->_posX[i] = rand() % 16;
        balls->_posY[i] = rand() % 16;
        TempBinaryBalls[balls->_posY[i]] |= (1 << TempBinaryBalls[balls->_posX[i]]);
    }

    T.Insert((uint64_t*) AP);
    DeepThinkListAry[0].clear();
    DeepThinkListAry[0].push_back(balls);
}

// 在控制台上展示局面的总状态, 包括石头们以及球球们的位置
void ShowAllState(const BallState * balls)
{
    string tu[16];
    for (size_t i = 0; i < 16; i++) tu[i] = "________________________________墙" + to_string(i);
    
    for (int y = 15; y >= 0; y--)
        for (int x = 15; x >= 0; x--)
            if (((1 << x) & StoneAry[y]) > 0) tu[y].replace(30 - x * 2, 2, "固");

    for (size_t i = 0; i < BALLNUM; i++)
    {
        int x = 30 - 2 * balls->_posX[i];
        int y = balls->_posY[i];
        if (x < 0 || x >= tu[y].size())
            cout << "下标超限, 开始坐标: " << x << ", 行所在高度: " << y << "; 而这一行最大长度为" << tu[y].size() << endl;
        else tu[y].replace(x, 2, to_string(i) + to_string(i));
    }
        

    cout << "F E D C B A 9 8 7 6 5 4 3 2 1 0" << endl;
    for (int i = 15; i >= 0; i--) cout << tu[i] << endl;
    cout << "F E D C B A 9 8 7 6 5 4 3 2 1 0" << endl;
}

// 标准化初始球球们的状态, 球球数量应该>=4, 比如4个球球, 那么局面里的四个角, 每个角都有一个球球
void StandardInitializeBallsState()
{
    if (BALLNUM < 4) {
        cout << "球球数量太少, 程序拒绝标准化初始化局面, 这里建议您手动输入局面" << endl;
        return;
    }
    BallState* balls = new BallState(nullptr);
    balls->_posX[0] = 0; balls->_posY[0] = 0;
    balls->_posX[1] = 15; balls->_posY[1] = 0;
    balls->_posX[2] = 0; balls->_posY[2] = 15;
    balls->_posX[3] = 15; balls->_posY[3] = 15;

    for (int i = 4; i < BALLNUM; i++) // 如果局面里的球球数量大于4个，那么第5个球球的标准初始化位置在它的下标方那里，以此类推
    {
        balls->_posX[i] = i;
        balls->_posY[i] = i;
    }

    DeepThinkListAry[0].clear();
    DeepThinkListAry[0].push_back(balls);

    *AP = *BP = *CP = *DP = 0;
    for (size_t i = 0; i < GLUENUM; i++)
        TempBinaryBalls[balls->_posY[i]] |= (1 << TempBinaryBalls[balls->_posX[i]]);

    T.Insert((uint64_t *)AP);
}

// 手工生成球球们
void ManualGenerateBalls()
{
    int x, y;
    BallState* balls = new BallState(nullptr);
    for (size_t i = 0; i < BALLNUM; i++)
    {
        cout << "请输入第" << i + 1 << "个球的X坐标:";
        cin >> x;
        cout << "请输入第" << i + 1 << "个球的Y坐标:";
        cin >> y;
        balls->_posX[i] = x;
        balls->_posY[i] = y;
    }

    DeepThinkListAry[0].clear();
    DeepThinkListAry[0].push_back(balls);

    *AP = *BP = *CP = *DP = 0;
    for (size_t i = 0; i < GLUENUM; i++)
        TempBinaryBalls[balls->_posY[i]] |= (1 << TempBinaryBalls[balls->_posX[i]]);

    T.Insert((uint64_t*)AP);
}


void StoneSiege(unsigned int width, unsigned int height)
{
    memset(StoneAry, 0, 32);

    cout << width << "," << height << endl;

    for (size_t i = 0; i < 16; i++) StoneAry[i] = -1;

    unsigned short tempVal = 0;
    for (size_t i = 0; i < width; i++) tempVal += (1 << i);
    tempVal = ~tempVal;

    for (size_t i = 0; i < height; i++) StoneAry[i] = tempVal;

    GenerateVirtualObstaclesList();
}


// 程序的思考函数, 在main函数里配合一个for循环使用（目前暂时不支持并发模式，以后若有需要，改成多线程并发模式）
void Ponder()
{
    list<int>* headBlocksInListAlias[(GLUENUM - 1)*4]; // 这两行代码的作用, 是声明一个用于标记清除的配对数组,因为在遍历胶水球组合循环的
    list<int>::iterator headBlocks[(GLUENUM - 1) * 4]; // 最后我们要清除根据胶水球组合新添加的block, 使得四大blockList恢复它们原始模样
    for (BallState* stateItem : DeepThinkListAry[DeepThinkLv])          // 遍历当前层数的所有局面，用其中的每一个局面来生成该局面可扩散的所有分支变化
    {
        for (const VariationCombine& gluesCombine : *GluesCombine)      // 遍历胶水球组合里的每一种组合
        {
            auto headGlues = gluesCombine._headAry;
            auto tailGlues = gluesCombine._tailAry;
            int headDelIndex = 0;

            // 首先,访问胶水球组合里的头部组合, 根据头部组合里的序号, 给blockList添加对应的新block元素
            for (int i = 0; i < GLUENUM - 1; i++)
            {
                int x = stateItem->_posX[headGlues[i]];               // 提取头部数组中某序号胶水球的X坐标和Y坐标
                int y = stateItem->_posY[headGlues[i]];               // 而那个X坐标, 就意味着要搞的是第X行的list


                list<int>::iterator item = UpList[x].begin();         // 第一个操作的是UpList: 首先根据胶水球头部组合里的某个序号球的Y坐标, 遍
                while (y > *item) item++;                             // 历对应X行的UpList的每个高度block元素, 最终会停留在高度合适的list位置上
                headBlocks[headDelIndex] = UpList[x].insert(item, y); // ,最后在这个合适位置上插入一个新的高度block元素, 让list保持从下到上的增序排序
                headBlocksInListAlias[headDelIndex] = &UpList[x];     // 记录下这个新block元素以及所操作的list, 因为后面要把此元素从对应list中删掉!
                headDelIndex++;                                       // 此值+1, 接下去依照类似逻辑去操作DownList、LeftList以及RightList, 不再赘述

                item = DownList[x].begin();
                while (y < *item) item++;
                headBlocks[headDelIndex] = DownList[x].insert(item, y);
                headBlocksInListAlias[headDelIndex] = &DownList[x];
                headDelIndex++;

                item = LeftList[y].begin();
                while (x > *item) item++;
                headBlocks[headDelIndex] = LeftList[y].insert(item, x);
                headBlocksInListAlias[headDelIndex] = &LeftList[y];
                headDelIndex++;

                item = RightList[y].begin();
                while (x < *item) item++;
                headBlocks[headDelIndex] = RightList[y].insert(item, x);
                headBlocksInListAlias[headDelIndex] = &RightList[y];
                headDelIndex++;
            }


            for (size_t glueNum = 0; tailGlues[glueNum] >= 0; glueNum++) // 遍历尾部的胶水球
            {
                int glueX = stateItem->_posX[tailGlues[glueNum]];        // 提取该序号的胶水球的X坐标
                int glueY = stateItem->_posY[tailGlues[glueNum]];        // 提取Y坐标
                list<int>::iterator newItem, delItem;

                /****首先是往上吹风的逻辑开始****/
                newItem = UpList[glueX].begin();                         // 遍历X行list的每个高度block元素, 直到停留在高度合适的位置上然后
                while (glueY > *newItem) newItem++;                      // 在这个新位置插入新的block元素, 使得list保持从下到上的增序排序
                delItem = UpList[glueX].insert(newItem, glueY);          // 记录下这个新的block元素, 因为吹风后要删除改元素
                *AP = *BP = *CP = *DP = 0;                               // 首先清空表示球球状态的二进制数组
                BallState* afterUpBalls = new BallState(stateItem);
                for (size_t i = 0; i < BALLNUM; i++)                     // 然后遍历局面中所有的球球
                {
                    int posX = stateItem->_posX[i];
                    int posY = stateItem->_posY[i];                      // 提取球球的X坐标和Y坐标
                    list<int>::iterator item = UpList[posX].begin();
                    while (posY > *item) item++;                         // 访问对应的blockList, 直到找出该球球吹风后应该停留在哪个障碍物那里
                    int newY = *item;                                    // 获取这个障碍物对应的Y坐标, 
                    while (TempBinaryBalls[newY] & (1 << posX)) newY--;  // 看看这个对应位置是不是1(有别的球球先入为主了), 如果是, 高度减一, 直到找到空地为止
                    TempBinaryBalls[newY] |= (1 << posX);                // 在binary数组中更新一个新的1, 来记录局面的二进制表示
                    afterUpBalls->_posY[i] = newY;                       // 球球的Y坐标更新一下
                    afterUpBalls->_posX[i] = stateItem->_posX[i];
                }
                UpList[glueX].erase(delItem);                            // 先擦除刚才 根据尾部胶水球 增加的新元素
                if (true == T.Insert((uint64_t *)AP))      // 最后阶段: 先看看球球二进制阵型以前是否出现过, 没
                    DeepThinkListAry[DeepThinkLv + 1].push_back(afterUpBalls);        // 有就保存新局面到下一步的链表里, 有的话, 那么就直
                else delete afterUpBalls;                                // 接清理掉刚才的局面, 以免造成内存泄露
                /****至此往上吹风的逻辑结束了, 下面的往下往右等, 都跟这里的逻辑差不多, 所以下面就不再赘述了****/


                /****然后接着往下吹风的逻辑开始****/
                newItem = DownList[glueX].begin();
                while (glueY < *newItem) newItem++;
                delItem = DownList[glueX].insert(newItem, glueY);
                *AP = *BP = *CP = *DP = 0;
                BallState* afterDownBalls = new BallState(stateItem);
                for (size_t i = 0; i < BALLNUM; i++)
                {
                    int posX = stateItem->_posX[i];
                    int posY = stateItem->_posY[i];
                    list<int>::iterator item = DownList[posX].begin();
                    while (posY < *item) item++;
                    int newY = *item;
                    while (TempBinaryBalls[newY] & (1 << posX)) newY++;
                    TempBinaryBalls[newY] |= (1 << posX);
                    afterDownBalls->_posY[i] = newY;
                    afterDownBalls->_posX[i] = stateItem->_posX[i];
                }
                DownList[glueX].erase(delItem);
                if (true == T.Insert((uint64_t*)TempBinaryBalls)) 
                    DeepThinkListAry[DeepThinkLv + 1].push_back(afterDownBalls);
                else delete afterDownBalls;
                /****至此往下吹风的逻辑结束了****/



                /****再接着往左吹风的逻辑开始****/
                newItem = LeftList[glueY].begin();
                while (glueX > *newItem) newItem++;
                delItem = LeftList[glueY].insert(newItem, glueX);
                *AP = *BP = *CP = *DP = 0;
                BallState* afterLeftBalls = new BallState(stateItem);
                for (size_t i = 0; i < BALLNUM; i++)
                {
                    int posX = stateItem->_posX[i];
                    int posY = stateItem->_posY[i];
                    list<int>::iterator item = LeftList[posY].begin();
                    while (posX > *item) item++;
                    int newX = *item;
                    while (TempBinaryBalls[posY] & (1 << newX)) newX--;
                    TempBinaryBalls[posY] |= (1 << newX);
                    afterLeftBalls->_posX[i] = newX;
                    afterLeftBalls->_posY[i] = stateItem->_posY[i];
                }
                LeftList[glueY].erase(delItem);
                if (true == T.Insert((uint64_t*)TempBinaryBalls)) DeepThinkListAry[DeepThinkLv + 1].push_back(afterLeftBalls);
                else delete afterLeftBalls;
                /****往左吹风的逻辑结束了****/



                /****最后往右吹风的逻辑开始***/
                newItem = RightList[glueY].begin();
                while (glueX < *newItem) newItem++;
                delItem = RightList[glueY].insert(newItem, glueX);
                *AP = *BP = *CP = *DP = 0;
                BallState* afterRightBalls = new BallState(stateItem);
                for (size_t i = 0; i < BALLNUM; i++)
                {
                    int posX = stateItem->_posX[i];
                    int posY = stateItem->_posY[i];
                    list<int>::iterator item = RightList[posY].begin();
                    while (posX < *item) item++;
                    int newX = *item;
                    while (TempBinaryBalls[posY] & (1 << newX)) newX++;
                    TempBinaryBalls[posY] |= (1 << newX);
                    afterRightBalls->_posX[i] = newX;
                    afterRightBalls->_posY[i] = stateItem->_posY[i];
                }
                RightList[glueY].erase(delItem);
                if (true == T.Insert((uint64_t*)TempBinaryBalls)) DeepThinkListAry[DeepThinkLv + 1].push_back(afterRightBalls);
                else delete afterRightBalls;
                /****至此往右吹风的逻辑结束了***/
            }

            // 最后别忘了恢复 根据我们这一组胶水球组合的头部 添加的虚拟obstacles
            for (size_t i = 0; i < (GLUENUM - 1) * 4; i++)  headBlocksInListAlias[i]->erase(headBlocks[i]);
        }
    }
}

// 这个是思考无胶水球的函数
void PonderWithZeroGlues()
{
    for (BallState* stateItem : DeepThinkListAry[DeepThinkLv])
    {
        *AP = *BP = *CP = *DP = 0;                               // 首先清空表示球球状态的二进制数组
        BallState* afterUpBalls = new BallState(stateItem);
        for (size_t i = 0; i < BALLNUM; i++)                     // 然后遍历局面中所有的球球
        {
            int posX = stateItem->_posX[i];
            int posY = stateItem->_posY[i];                      // 提取球球的X坐标和Y坐标
            list<int>::iterator item = UpList[posX].begin();
            while (posY > *item) item++;                         // 访问对应的blockList, 直到找出该球球吹风后应该停留在哪个障碍物那里
            int newY = *item;                                    // 获取这个障碍物对应的Y坐标, 
            while (TempBinaryBalls[newY] & (1 << posX)) newY--;  // 看看这个对应位置是不是1(有别的球球先入为主了), 如果是, 高度减一, 直到找到空地为止
            TempBinaryBalls[newY] |= (1 << posX);                // 在binary数组中更新一个新的1, 来记录局面的二进制表示
            afterUpBalls->_posY[i] = newY;                       // 球球的Y坐标更新一下
            afterUpBalls->_posX[i] = stateItem->_posX[i];
        }
        if (true == T.Insert((uint64_t*)AP))      // 最后阶段: 先看看球球二进制阵型以前是否出现过, 没
            DeepThinkListAry[DeepThinkLv + 1].push_back(afterUpBalls);        // 有就保存新局面到下一步的链表里, 有的话, 那么就直
        else delete afterUpBalls;                                // 接清理掉刚才的局面, 以免造成内存泄露


        /****然后接着往下吹风的逻辑开始****/
        *AP = *BP = *CP = *DP = 0;
        BallState* afterDownBalls = new BallState(stateItem);
        for (size_t i = 0; i < BALLNUM; i++)
        {
            int posX = stateItem->_posX[i];
            int posY = stateItem->_posY[i];
            list<int>::iterator item = DownList[posX].begin();
            while (posY < *item) item++;
            int newY = *item;
            while (TempBinaryBalls[newY] & (1 << posX)) newY++;
            TempBinaryBalls[newY] |= (1 << posX);
            afterDownBalls->_posY[i] = newY;
            afterDownBalls->_posX[i] = stateItem->_posX[i];
        }
        if (true == T.Insert((uint64_t*)TempBinaryBalls))
            DeepThinkListAry[DeepThinkLv + 1].push_back(afterDownBalls);
        else delete afterDownBalls;
        /****至此往下吹风的逻辑结束了****/

        *AP = *BP = *CP = *DP = 0;
        BallState* afterLeftBalls = new BallState(stateItem);
        for (size_t i = 0; i < BALLNUM; i++)
        {
            int posX = stateItem->_posX[i];
            int posY = stateItem->_posY[i];
            list<int>::iterator item = LeftList[posY].begin();
            while (posX > *item) item++;
            int newX = *item;
            while (TempBinaryBalls[posY] & (1 << newX)) newX--;
            TempBinaryBalls[posY] |= (1 << newX);
            afterLeftBalls->_posX[i] = newX;
            afterLeftBalls->_posY[i] = stateItem->_posY[i];
        }
        if (true == T.Insert((uint64_t*)TempBinaryBalls)) DeepThinkListAry[DeepThinkLv + 1].push_back(afterLeftBalls);
        else delete afterLeftBalls;


        *AP = *BP = *CP = *DP = 0;
        BallState* afterRightBalls = new BallState(stateItem);
        for (size_t i = 0; i < BALLNUM; i++)
        {
            int posX = stateItem->_posX[i];
            int posY = stateItem->_posY[i];
            list<int>::iterator item = RightList[posY].begin();
            while (posX < *item) item++;
            int newX = *item;
            while (TempBinaryBalls[posY] & (1 << newX)) newX++;
            TempBinaryBalls[posY] |= (1 << newX);
            afterRightBalls->_posX[i] = newX;
            afterRightBalls->_posY[i] = stateItem->_posY[i];
        }
        if (true == T.Insert((uint64_t*)TempBinaryBalls)) DeepThinkListAry[DeepThinkLv + 1].push_back(afterRightBalls);
        else delete afterRightBalls;
    }
}

// 这个是思考只有一个胶水球的函数
void PonderWithOneGlue()
{
    for (BallState* stateItem : DeepThinkListAry[DeepThinkLv])
    {
        for (size_t glueNum = 0; glueNum < BALLNUM; glueNum++) // 遍历尾部的胶水球
        {
            int glueX = stateItem->_posX[glueNum];        // 提取该序号的胶水球的X坐标
            int glueY = stateItem->_posY[glueNum];        // 提取Y坐标
            list<int>::iterator newItem, delItem;

            /****首先是往上吹风的逻辑开始****/
            newItem = UpList[glueX].begin();                        // 遍历X行list的每个高度block元素, 直到停留在高度合适的位置上然后
            while (glueY > *newItem) newItem++;
            delItem = UpList[glueX].insert(newItem, glueY);         // 记录下这个新的block元素, 因为吹风后要删除改元素

            *AP = *BP = *CP = *DP = 0;                               // 首先清空表示球球状态的二进制数组
            BallState* afterUpBalls = new BallState(stateItem);
            for (size_t i = 0; i < BALLNUM; i++)                     // 然后遍历局面中所有的球球
            {
                int posX = stateItem->_posX[i];
                int posY = stateItem->_posY[i];                      // 提取球球的X坐标和Y坐标
                list<int>::iterator item = UpList[posX].begin();
                while (posY > *item) item++;                         // 访问对应的blockList, 直到找出该球球吹风后应该停留在哪个障碍物那里
                int newY = *item;                                    // 获取这个障碍物对应的Y坐标, 
                while (TempBinaryBalls[newY] & (1 << posX)) newY--;  // 看看这个对应位置是不是1(有别的球球先入为主了), 如果是, 高度减一, 直到找到空地为止
                TempBinaryBalls[newY] |= (1 << posX);                // 在binary数组中更新一个新的1, 来记录局面的二进制表示
                afterUpBalls->_posY[i] = newY;                       // 球球的Y坐标更新一下
                afterUpBalls->_posX[i] = stateItem->_posX[i];
            }
            if (true == T.Insert((uint64_t*)AP))      // 最后阶段: 先看看球球二进制阵型以前是否出现过, 没
                DeepThinkListAry[DeepThinkLv + 1].push_back(afterUpBalls);        // 有就保存新局面到下一步的链表里, 有的话, 那么就直
            else delete afterUpBalls;                                // 接清理掉刚才的局面, 以免造成内存泄露

            UpList[glueX].erase(delItem);
            /****往上吹风的逻辑结束了****/



            /****然后接着往下吹风的逻辑开始****/
            newItem = DownList[glueX].begin();
            while (glueY < *newItem) newItem++;
            delItem = DownList[glueX].insert(newItem, glueY);

            *AP = *BP = *CP = *DP = 0;
            BallState* afterDownBalls = new BallState(stateItem);
            for (size_t i = 0; i < BALLNUM; i++)
            {
                int posX = stateItem->_posX[i];
                int posY = stateItem->_posY[i];
                list<int>::iterator item = DownList[posX].begin();
                while (posY < *item) item++;
                int newY = *item;
                while (TempBinaryBalls[newY] & (1 << posX)) newY++;
                TempBinaryBalls[newY] |= (1 << posX);
                afterDownBalls->_posY[i] = newY;
                afterDownBalls->_posX[i] = stateItem->_posX[i];
            }
            if (true == T.Insert((uint64_t*)TempBinaryBalls))
                DeepThinkListAry[DeepThinkLv + 1].push_back(afterDownBalls);
            else delete afterDownBalls;
             
            DownList[glueX].erase(delItem);
            /****至此往下吹风的逻辑结束了****/



            /****再接着往左吹风的逻辑开始****/
            newItem = LeftList[glueY].begin();
            while (glueX > *newItem) newItem++;
            delItem = LeftList[glueY].insert(newItem, glueX);

            *AP = *BP = *CP = *DP = 0;
            BallState* afterLeftBalls = new BallState(stateItem);
            for (size_t i = 0; i < BALLNUM; i++)
            {
                int posX = stateItem->_posX[i];
                int posY = stateItem->_posY[i];
                list<int>::iterator item = LeftList[posY].begin();
                while (posX > *item) item++;
                int newX = *item;
                while (TempBinaryBalls[posY] & (1 << newX)) newX--;
                TempBinaryBalls[posY] |= (1 << newX);
                afterLeftBalls->_posX[i] = newX;
                afterLeftBalls->_posY[i] = stateItem->_posY[i];
            }
            if (true == T.Insert((uint64_t*)TempBinaryBalls)) DeepThinkListAry[DeepThinkLv + 1].push_back(afterLeftBalls);
            else delete afterLeftBalls;

            LeftList[glueY].erase(delItem);
            /****往左吹风的逻辑结束了****/



            /****最后往右吹风的逻辑开始***/
            newItem = RightList[glueY].begin();
            while (glueX < *newItem) newItem++;
            delItem = RightList[glueY].insert(newItem, glueX);

            *AP = *BP = *CP = *DP = 0;
            BallState* afterRightBalls = new BallState(stateItem);
            for (size_t i = 0; i < BALLNUM; i++)
            {
                int posX = stateItem->_posX[i];
                int posY = stateItem->_posY[i];
                list<int>::iterator item = RightList[posY].begin();
                while (posX < *item) item++;
                int newX = *item;
                while (TempBinaryBalls[posY] & (1 << newX)) newX++;
                TempBinaryBalls[posY] |= (1 << newX);
                afterRightBalls->_posX[i] = newX;
                afterRightBalls->_posY[i] = stateItem->_posY[i];
            }
            if (true == T.Insert((uint64_t*)TempBinaryBalls)) DeepThinkListAry[DeepThinkLv + 1].push_back(afterRightBalls);
            else delete afterRightBalls;

            RightList[glueY].erase(delItem);
        }
    }
}





/***************************************************************************************************
 * 请注意: 下面的函数计算逻辑都是比较快的, 但它们有一个致命的缺点: 无法处理上下的移动, 必须需要躺  *     
 * 平转换才可以, 总之先写下来备忘, 如果按照正常逻辑计算, 发现速度太慢的话, 再转换使用下面的逻辑吧  *
 ***************************************************************************************************/


/*** 获取一个数的二进制表示中最右边的那个1, 也就是: 当某球在某一行往右移动时, 该球最后应该停留的位置; ***/
uint16_t GetRightBinary(int64_t obstacleVal, int64_t ballVal)
{
    obstacleVal &= ~((int64_t)-1 + ballVal); // 在这里首先处理一下obstacleval, 使得它变成从ballVal位开始往右的所有位统统为0

    int16_t maskAry[4];
    maskAry[0] = 2;      // 10
    maskAry[1] = 0xC;    // 1100
    maskAry[2] = 0xF0;   // 1111 0000
    maskAry[3] = 0xFF00; // 1111 1111 0000 0000

    uint8_t offsetIndex = 0;
    for (int i = 3; i >= 0; i--)
    {
        if (obstacleVal & (maskAry[i] << offsetIndex))
        {
            obstacleVal &= (maskAry[i] << offsetIndex);
            offsetIndex += (1 << i);
        }
    }
    return 1 << (offsetIndex + 1);
}
// 获取一个二进制表示中最左边的那个1, 也就是某球在某一行往左移动时, 该球最后应该停留的位置
uint16_t GetLeftBinary(int64_t obstacleVal, int64_t ballVal)
{
    obstacleVal &= (int64_t)-1 + ballVal; // 在这里首先处理一下obstacleval, 使得它变成从ballVal位开始往左的所有位统统为0

    uint16_t retVal = obstacleVal - 1;
    retVal ^= obstacleVal;
    return (retVal + 1) >> 2;
}


/********************************************************************************************************************
 * 并行移动法，这个比上面的更快；但她有个同样的缺点：只能处理左右的移动，而上下的移动除了镜像对调似乎没啥好办法处理 *
 * PS：维度似乎不用减少至14*14，只需宏定义个farleft掩码值，用来检测一下球球是否到某行最左边，若是则对应位置生成石头 *
 ********************************************************************************************************************/
constexpr auto FARLEFT = (uint64_t(1) << 15) + (uint64_t(1) << 31) + (uint64_t(1) << 47) + (uint64_t(1) << 63);      // 定以一个最左隐藏障碍掩码值，用来检测球球是否移动到对应位置;
uint64_t LeftMove(uint64_t obstacleVal, uint64_t ballsVal)  // 局面中所有满足条件的球球们一起向左移动（obstacle是石头阵，balls是球球们）
{
    if (ballsVal & FARLEFT) {                 // 若自由球球们有任意一颗球在任意一水平线的最左位置
        obstacleVal |= (ballsVal & FARLEFT);  // 那么直接把这颗球设置为石头阵的一员
        ballsVal -= (ballsVal & FARLEFT);     // 剔除掉这颗球
    }
    uint64_t overlapVal = 0;
    while (ballsVal > 0) {                            // 若里面还有能移动的球球
        overlapVal = obstacleVal & (ballsVal << 1);   // 那么首先看看球球们往左伪移动一下之后，是否可以重叠障碍石头阵 
        if (overlapVal) {                             // 若有重叠的话
            obstacleVal |= (overlapVal >> 1);         // 障碍石头阵的对应位置往右扩充一下
            ballsVal ^= (overlapVal >> 1);            // 自由球球们对应位置的球球剔除掉
        }else {                                       // 不然就是没有重叠
            ballsVal <<= 1;                           // 那么让球球们往左真实移动
            if (ballsVal & FARLEFT) {
                obstacleVal |= (ballsVal & FARLEFT);  // 那么直接把这颗球设置为石头阵的一员
                ballsVal -= (ballsVal & FARLEFT);
            }
        }
    }
    return obstacleVal;
}

constexpr auto FARRIGHT = 1 + (1<<17) + (1<<33) + (1<<49);
uint64_t RightMove(uint64_t obstacleVal, uint64_t ballsVal) // 同上往右移动
{
    uint64_t overlapVal = 0;
    while (ballsVal > 0)
    {
        overlapVal = obstacleVal & (ballsVal >> 1);
        if (overlapVal)
        {
            obstacleVal |= (overlapVal << 1);
            ballsVal ^= (overlapVal << 1);
        } else {
            if (ballsVal & FARRIGHT) {
                obstacleVal |= (ballsVal & FARRIGHT);
                ballsVal ^= (ballsVal & FARRIGHT);
            }
            ballsVal >>= 1;
        }
    }
    return obstacleVal;
}




/********************************************************************************************************************
 * 还有一种方法最快，但是需要很大的工作量：就是把大数值划分成8位区域值，尽可能列出多的排列数值，然后直接用switch的  *
 * 据值跳转机制跳两次即可，比如16*16的矩形里，一行写成两个8进制数值即可，然后从右到左可能需要进位的判定算法，总之   *
 * 这里先记录一下备忘                                                                                               *
 ********************************************************************************************************************/