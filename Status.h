#pragma once
#include "MiscAid.h"  // 这个文件是用于多线程思考局面的
#include <thread>
#include <mutex>
#include <queue>
#include <Windows.h>
#define MAXTHREAD 8

std::mutex PoolLock;
std::mutex TreeLock;
std::queue<BallState*> PoolQueue;
std::atomic_flag SpinLock = ATOMIC_FLAG_INIT;
std::atomic<bool> CASFlag;



void ParallelWorker()
{
    BallState* workerItem = nullptr;
    list<int> workerSBlock[16], workerXBlock[16], workerZBlock[16], workerYBlock[16];
    for (size_t i = 0; i < 16; i++)
    {
        for (int value : UpList[i])      workerSBlock[i].push_back(value);
        for (int value : DownList[i])    workerXBlock[i].push_back(value);
        for (int value : LeftList[i])    workerZBlock[i].push_back(value);
        for (int value : RightList[i])   workerYBlock[i].push_back(value);
    }

    uint16_t workerTempBinary[16];
    uint64_t * workerAP = (uint64_t *) & (workerTempBinary[0]);
    uint64_t * workerBP = (uint64_t *) & (workerTempBinary[4]);
    uint64_t * workerCP = (uint64_t *) & (workerTempBinary[8]);
    uint64_t * workerDP = (uint64_t *) & (workerTempBinary[12]);
    //cout << GetCurrentThreadId() << endl;

    while (true)
    {
        PoolLock.lock();
        if (PoolQueue.empty()) {
            PoolLock.unlock();
            break;
        }
        workerItem = PoolQueue.front();
        PoolQueue.pop();
        PoolLock.unlock();


        /********在这里开始多线程的工作！**************/
        {
            list<int>* headBlocksInListAlias[(GLUENUM - 1) * 4];       // 这两行代码的作用, 是声明一个用于标记清除的配对数组,因为在遍历胶水球组合循环的
            list<int>::iterator headBlocks[(GLUENUM - 1) * 4];         // 最后我们要清除根据胶水球组合新添加的block, 使得四大blockList恢复它们原始模样
            for (const VariationCombine& gluesCombine : *GluesCombine) // 遍历胶水球组合里的每一种组合
            {
                auto headGlues = gluesCombine._headAry;
                auto tailGlues = gluesCombine._tailAry;
                int headDelIndex = 0;

                // 首先,访问胶水球组合里的头部组合, 根据头部组合里的序号, 给blockList添加对应的新block元素
                for (int i = 0; i < GLUENUM - 1; i++)
                {
                    int x = workerItem->_posX[headGlues[i]];               // 提取头部数组中某序号胶水球的X坐标和Y坐标
                    int y = workerItem->_posY[headGlues[i]];               // 而那个X坐标, 就意味着要搞的是第X行的list


                    list<int>::iterator item = workerSBlock[x].begin();         // 第一个操作的是UpList: 首先根据胶水球头部组合里的某个序号球的Y坐标, 遍
                    while (y > *item) item++;                             // 历对应X行的UpList的每个高度block元素, 最终会停留在高度合适的list位置上
                    headBlocks[headDelIndex] = workerSBlock[x].insert(item, y); // ,最后在这个合适位置上插入一个新的高度block元素, 让list保持从下到上的增序排序
                    headBlocksInListAlias[headDelIndex] = &workerSBlock[x];     // 记录下这个新block元素以及所操作的list, 因为后面要把此元素从对应list中删掉!
                    headDelIndex++;                                       // 此值+1, 接下去依照类似逻辑去操作DownList、LeftList以及RightList, 不再赘述

                    item = workerXBlock[x].begin();
                    while (y < *item) item++;
                    headBlocks[headDelIndex] = workerXBlock[x].insert(item, y);
                    headBlocksInListAlias[headDelIndex] = &workerXBlock[x];
                    headDelIndex++;

                    item = workerZBlock[y].begin();
                    while (x > *item) item++;
                    headBlocks[headDelIndex] = workerZBlock[y].insert(item, x);
                    headBlocksInListAlias[headDelIndex] = &workerZBlock[y];
                    headDelIndex++;

                    item = workerYBlock[y].begin();
                    while (x < *item) item++;
                    headBlocks[headDelIndex] = workerYBlock[y].insert(item, x);
                    headBlocksInListAlias[headDelIndex] = &workerYBlock[y];
                    headDelIndex++;
                }


                for (size_t glueNum = 0; tailGlues[glueNum] >= 0; glueNum++) // 遍历尾部的胶水球
                {
                    int glueX = workerItem->_posX[tailGlues[glueNum]];        // 提取该序号的胶水球的X坐标
                    int glueY = workerItem->_posY[tailGlues[glueNum]];        // 提取Y坐标
                    list<int>::iterator newItem, delItem;

                    /****首先是往上吹风的逻辑开始****/
                    newItem = workerSBlock[glueX].begin();                    // 遍历X行list的每个高度block元素, 直到停留在高度合适的位置上然后
                    while (glueY > *newItem) newItem++;                       // 在这个新位置插入新的block元素, 使得list保持从下到上的增序排序
                    delItem = workerSBlock[glueX].insert(newItem, glueY);     // 记录下这个新的block元素, 因为吹风后要删改元素
                    *workerAP = *workerBP = *workerCP = *workerDP = 0;        // 首先清空表示球球状态的二进制数组
                    BallState* afterUpBalls = new BallState(workerItem);
                    for (size_t i = 0; i < BALLNUM; i++)                      // 然后遍历局面中所有的球球
                    {
                        int posX = workerItem->_posX[i];
                        int posY = workerItem->_posY[i];                       // 提取球球的X坐标和Y坐标
                        list<int>::iterator item = workerSBlock[posX].begin();
                        while (posY > *item) item++;                           // 访问对应的blockList, 直到找出该球球吹风后应该停留在哪个障碍物那里
                        int newY = *item;                                      // 获取这个障碍物对应的Y坐标, 
                        while (workerTempBinary[newY] & (1 << posX)) newY--;   // 看看这个对应位置是不是1(有别的球球先入为主了), 如果是, 高度减一, 直到找到空地为止
                        workerTempBinary[newY] |= (1 << posX);                 // 在binary数组中更新一个新的1, 来记录局面的二进制表示
                        afterUpBalls->_posY[i] = newY;                         // 球球的Y坐标更新一下
                        afterUpBalls->_posX[i] = workerItem->_posX[i];
                    }
                    workerSBlock[glueX].erase(delItem);                        // 先擦除刚才 根据尾部固定球 增加的新元素

                    bool shang = false;                                                    // 在这里开始加锁，使用原子操作的方式
                    while (!CASFlag.compare_exchange_weak(shang, true)) { shang = false; } // 若不能成功设置“shang”标志，那么线程原地自旋（不要睡眠！竞争较频繁）
                    if (true == T.Insert((uint64_t *)workerAP))               // 最后阶段: 先看看球球二进制阵型以前是否出现过, 没
                    {
                        DeepThinkListAry[DeepThinkLv + 1].push_back(afterUpBalls);         // 有就保存新局面到下一步的链表里, 有的话, 那么就直
                    }
                    else delete afterUpBalls;                                              // 接清理掉这个重复思考的局面, 以免造成内存泄露！
                    CASFlag.store(false);                                                  // 解锁自旋锁

                    /****至此往上吹风的逻辑结束,后面的往下左右移动,其逻辑本质相同,所以下面不再赘述****/


                    /****然后接着往下吹风的逻辑开始****/
                    newItem = workerXBlock[glueX].begin();
                    while (glueY < *newItem) newItem++;
                    delItem = workerXBlock[glueX].insert(newItem, glueY);
                    *workerAP = *workerBP = *workerCP = *workerDP = 0;
                    BallState* afterDownBalls = new BallState(workerItem);
                    for (size_t i = 0; i < BALLNUM; i++)
                    {
                        int posX = workerItem->_posX[i];
                        int posY = workerItem->_posY[i];
                        list<int>::iterator item = workerXBlock[posX].begin();
                        while (posY < *item) item++;
                        int newY = *item;
                        while (workerTempBinary[newY] & (1 << posX)) newY++;
                        workerTempBinary[newY] |= (1 << posX);
                        afterDownBalls->_posY[i] = newY;
                        afterDownBalls->_posX[i] = workerItem->_posX[i];
                    }
                    workerXBlock[glueX].erase(delItem);

                    bool xia = false;
                    while (!CASFlag.compare_exchange_weak(xia, true)) { xia = false; }
                    if (true == T.Insert((uint64_t *)workerTempBinary))
                    {
                        DeepThinkListAry[DeepThinkLv + 1].push_back(afterDownBalls);
                    }
                    else delete afterDownBalls;
                    CASFlag.store(false);
                    /****至此往下吹风的逻辑结束了****/



                    /****再接着往左吹风的逻辑开始****/
                    newItem = workerZBlock[glueY].begin();
                    while (glueX > *newItem) newItem++;
                    delItem = workerZBlock[glueY].insert(newItem, glueX);
                    *workerAP = *workerBP = *workerCP = *workerDP = 0;
                    BallState* afterLeftBalls = new BallState(workerItem);
                    for (size_t i = 0; i < BALLNUM; i++)
                    {
                        int posX = workerItem->_posX[i];
                        int posY = workerItem->_posY[i];
                        list<int>::iterator item = workerZBlock[posY].begin();
                        while (posX > *item) item++;
                        int newX = *item;
                        while (workerTempBinary[posY] & (1 << newX)) newX--;
                        workerTempBinary[posY] |= (1 << newX);
                        afterLeftBalls->_posX[i] = newX;
                        afterLeftBalls->_posY[i] = workerItem->_posY[i];
                    }
                    workerZBlock[glueY].erase(delItem);

                    bool zuo = false;
                    while (!CASFlag.compare_exchange_weak(zuo, true)) { zuo = false; }
                    if (true == T.Insert((uint64_t *)workerTempBinary))
                    {
                        DeepThinkListAry[DeepThinkLv + 1].push_back(afterLeftBalls);
                    }
                    else delete afterLeftBalls;
                    CASFlag.store(false);
                    /****往左吹风的逻辑结束了****/



                    /****最后往右吹风的逻辑开始***/
                    newItem = workerYBlock[glueY].begin();
                    while (glueX < *newItem) newItem++;
                    delItem = workerYBlock[glueY].insert(newItem, glueX);
                    *workerAP = *workerBP = *workerCP = *workerDP = 0;
                    BallState* afterRightBalls = new BallState(workerItem);
                    for (size_t i = 0; i < BALLNUM; i++)
                    {
                        int posX = workerItem->_posX[i];
                        int posY = workerItem->_posY[i];
                        list<int>::iterator item = workerYBlock[posY].begin();
                        while (posX < *item) item++;
                        int newX = *item;
                        while (workerTempBinary[posY] & (1 << newX)) newX++;
                        workerTempBinary[posY] |= (1 << newX);
                        afterRightBalls->_posX[i] = newX;
                        afterRightBalls->_posY[i] = workerItem->_posY[i];
                    }
                    workerYBlock[glueY].erase(delItem);

                    //TreeLock.lock();
                    bool you = false;
                    while (!CASFlag.compare_exchange_weak(you, true)) { you = false; }
                    if (true == T.Insert((uint64_t *)workerTempBinary))
                    {
                        DeepThinkListAry[DeepThinkLv + 1].push_back(afterRightBalls);
                    }
                    else delete afterRightBalls;
                    CASFlag.store(false);
                    /****至此往右吹风的逻辑结束了***/
                }

                // 最后别忘了恢复 根据我们这一组胶水球组合的头部 添加的虚拟obstacles
                for (size_t i = 0; i < (GLUENUM - 1) * 4; i++)  headBlocksInListAlias[i]->erase(headBlocks[i]);
            }
        }
        /********多线程的工作至此结束了！**************/
    }
}

void LoopStartParallel()
{
    PoolLock.lock();
    for (BallState* item : DeepThinkListAry[DeepThinkLv]) PoolQueue.push(item);
    PoolLock.unlock();

    std::vector<std::thread> threadPool;
    for (size_t i = 0; i < MAXTHREAD; i++) threadPool.push_back(std::move(std::thread(ParallelWorker)));
    for (auto& worker : threadPool) worker.join();
}

// 以下代码是测试多线程用的（经测试，原子操作要比相同的普通指令更耗时，大概是普通指令的50倍左右)
std::atomic<bool> TestCASLock = ATOMIC_VAR_INIT(0);
#define MAXNUM 1200000
#define MAXT 4
#define MAXA 150
#define MAXB 2

void TestCASLockFun()
{
    for (size_t i = 0; i < MAXNUM / MAXT; i++)
    {
        for (int a = 0; a < MAXA; a++);

        bool expect = false;
        while (!TestCASLock.compare_exchange_weak(expect, true)) { expect = false; }
        for (int a = 0; a < MAXB; a++);
        TestCASLock.store(false);
    }
}

void TestCommonLockFun()
{
    for (size_t i = 0; i < MAXNUM / MAXT; i++)
    {
        for (int a = 0; a < MAXA; a++);

        TreeLock.lock();
        for (int a = 0; a < MAXB; a++);
        TreeLock.unlock();
    }
}

void TestSpinLockFun()
{
    for (size_t i = 0; i < MAXNUM / MAXT; i++)
    {
        for (int a = 0; a < MAXA; a++);

        while (SpinLock.test_and_set(std::memory_order_acquire));
        for (int a = 0; a < MAXB; a++);
        SpinLock.clear(std::memory_order_release);
    }
}

void TestSingleThread()
{
    for (size_t i = 0; i < MAXNUM; i++)
    {
        for (int a = 0; a < MAXA; a++);
        for (int a = 0; a < MAXB; a++);
    }
}

void testParallel()
{
    auto st = clock();
    TestSingleThread();
    cout << "单线程耗时为" << clock() - st << endl;

    st = clock();
    std::vector<std::thread> casThreadPool;
    for (size_t i = 0; i < MAXT; i++) casThreadPool.push_back(std::move(std::thread(TestCASLockFun)));
    for (auto& worker : casThreadPool)  worker.join();
    cout << "自旋锁多线程的耗时为" << clock() - st << endl;

    st = clock();
    std::vector<std::thread> commonThreadPool;
    for (size_t i = 0; i < MAXT; i++) commonThreadPool.push_back(std::move(std::thread(TestCommonLockFun)));
    for (auto& worker : commonThreadPool)  worker.join();
    cout << "普通锁多线程的耗时为" << clock() - st << endl;

    st = clock();
    std::vector<std::thread> spinLockThreadPool;
    for (size_t i = 0; i < MAXT; i++) spinLockThreadPool.push_back(std::move(std::thread(TestSpinLockFun)));
    for (auto& worker : spinLockThreadPool)  worker.join();
    cout << "AI提示的自旋锁锁多线程的耗时为" << clock() - st << endl;

    int r = 0;
    std::atomic<int> cc = 0;

    st = clock();
    for (size_t i = 0; i < 500000000; i++)
    {
        cc = i;
    }
    cout << "原子操作时间是" << clock() - st << endl;

    st = clock();
    for (size_t i = 0; i < 500000000; i++)
    {
        r = i;
    }
    cout << "普通运算时间是" << clock() - st << endl;

    int dd = r;
}