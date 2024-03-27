#pragma once
#include "MiscAid.h"  // ����ļ������ڶ��߳�˼�������
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


        /********�����￪ʼ���̵߳Ĺ�����**************/
        {
            list<int>* headBlocksInListAlias[(GLUENUM - 1) * 4];       // �����д��������, ������һ�����ڱ��������������,��Ϊ�ڱ�����ˮ�����ѭ����
            list<int>::iterator headBlocks[(GLUENUM - 1) * 4];         // �������Ҫ������ݽ�ˮ���������ӵ�block, ʹ���Ĵ�blockList�ָ�����ԭʼģ��
            for (const VariationCombine& gluesCombine : *GluesCombine) // ������ˮ��������ÿһ�����
            {
                auto headGlues = gluesCombine._headAry;
                auto tailGlues = gluesCombine._tailAry;
                int headDelIndex = 0;

                // ����,���ʽ�ˮ��������ͷ�����, ����ͷ�����������, ��blockList��Ӷ�Ӧ����blockԪ��
                for (int i = 0; i < GLUENUM - 1; i++)
                {
                    int x = workerItem->_posX[headGlues[i]];               // ��ȡͷ��������ĳ��Ž�ˮ���X�����Y����
                    int y = workerItem->_posY[headGlues[i]];               // ���Ǹ�X����, ����ζ��Ҫ����ǵ�X�е�list


                    list<int>::iterator item = workerSBlock[x].begin();         // ��һ����������UpList: ���ȸ��ݽ�ˮ��ͷ��������ĳ��������Y����, ��
                    while (y > *item) item++;                             // ����ӦX�е�UpList��ÿ���߶�blockԪ��, ���ջ�ͣ���ڸ߶Ⱥ��ʵ�listλ����
                    headBlocks[headDelIndex] = workerSBlock[x].insert(item, y); // ,������������λ���ϲ���һ���µĸ߶�blockԪ��, ��list���ִ��µ��ϵ���������
                    headBlocksInListAlias[headDelIndex] = &workerSBlock[x];     // ��¼�������blockԪ���Լ���������list, ��Ϊ����Ҫ�Ѵ�Ԫ�شӶ�Ӧlist��ɾ��!
                    headDelIndex++;                                       // ��ֵ+1, ����ȥ���������߼�ȥ����DownList��LeftList�Լ�RightList, ����׸��

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


                for (size_t glueNum = 0; tailGlues[glueNum] >= 0; glueNum++) // ����β���Ľ�ˮ��
                {
                    int glueX = workerItem->_posX[tailGlues[glueNum]];        // ��ȡ����ŵĽ�ˮ���X����
                    int glueY = workerItem->_posY[tailGlues[glueNum]];        // ��ȡY����
                    list<int>::iterator newItem, delItem;

                    /****���������ϴ�����߼���ʼ****/
                    newItem = workerSBlock[glueX].begin();                    // ����X��list��ÿ���߶�blockԪ��, ֱ��ͣ���ڸ߶Ⱥ��ʵ�λ����Ȼ��
                    while (glueY > *newItem) newItem++;                       // �������λ�ò����µ�blockԪ��, ʹ��list���ִ��µ��ϵ���������
                    delItem = workerSBlock[glueX].insert(newItem, glueY);     // ��¼������µ�blockԪ��, ��Ϊ�����Ҫɾ��Ԫ��
                    *workerAP = *workerBP = *workerCP = *workerDP = 0;        // ������ձ�ʾ����״̬�Ķ���������
                    BallState* afterUpBalls = new BallState(workerItem);
                    for (size_t i = 0; i < BALLNUM; i++)                      // Ȼ��������������е�����
                    {
                        int posX = workerItem->_posX[i];
                        int posY = workerItem->_posY[i];                       // ��ȡ�����X�����Y����
                        list<int>::iterator item = workerSBlock[posX].begin();
                        while (posY > *item) item++;                           // ���ʶ�Ӧ��blockList, ֱ���ҳ������򴵷��Ӧ��ͣ�����ĸ��ϰ�������
                        int newY = *item;                                      // ��ȡ����ϰ����Ӧ��Y����, 
                        while (workerTempBinary[newY] & (1 << posX)) newY--;   // ���������Ӧλ���ǲ���1(�б����������Ϊ����), �����, �߶ȼ�һ, ֱ���ҵ��յ�Ϊֹ
                        workerTempBinary[newY] |= (1 << posX);                 // ��binary�����и���һ���µ�1, ����¼����Ķ����Ʊ�ʾ
                        afterUpBalls->_posY[i] = newY;                         // �����Y�������һ��
                        afterUpBalls->_posX[i] = workerItem->_posX[i];
                    }
                    workerSBlock[glueX].erase(delItem);                        // �Ȳ����ղ� ����β���̶��� ���ӵ���Ԫ��

                    bool shang = false;                                                    // �����￪ʼ������ʹ��ԭ�Ӳ����ķ�ʽ
                    while (!CASFlag.compare_exchange_weak(shang, true)) { shang = false; } // �����ܳɹ����á�shang����־����ô�߳�ԭ����������Ҫ˯�ߣ�������Ƶ����
                    if (true == T.Insert((uint64_t *)workerAP))               // ���׶�: �ȿ������������������ǰ�Ƿ���ֹ�, û
                    {
                        DeepThinkListAry[DeepThinkLv + 1].push_back(afterUpBalls);         // �оͱ����¾��浽��һ����������, �еĻ�, ��ô��ֱ
                    }
                    else delete afterUpBalls;                                              // �����������ظ�˼���ľ���, ��������ڴ�й¶��
                    CASFlag.store(false);                                                  // ����������

                    /****�������ϴ�����߼�����,��������������ƶ�,���߼�������ͬ,�������治��׸��****/


                    /****Ȼ��������´�����߼���ʼ****/
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
                    /****�������´�����߼�������****/



                    /****�ٽ������󴵷���߼���ʼ****/
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
                    /****���󴵷���߼�������****/



                    /****������Ҵ�����߼���ʼ***/
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
                    /****�������Ҵ�����߼�������***/
                }

                // �������˻ָ� ����������һ�齺ˮ����ϵ�ͷ�� ��ӵ�����obstacles
                for (size_t i = 0; i < (GLUENUM - 1) * 4; i++)  headBlocksInListAlias[i]->erase(headBlocks[i]);
            }
        }
        /********���̵߳Ĺ������˽����ˣ�**************/
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

// ���´����ǲ��Զ��߳��õģ������ԣ�ԭ�Ӳ���Ҫ����ͬ����ָͨ�����ʱ���������ָͨ���50������)
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
    cout << "���̺߳�ʱΪ" << clock() - st << endl;

    st = clock();
    std::vector<std::thread> casThreadPool;
    for (size_t i = 0; i < MAXT; i++) casThreadPool.push_back(std::move(std::thread(TestCASLockFun)));
    for (auto& worker : casThreadPool)  worker.join();
    cout << "���������̵߳ĺ�ʱΪ" << clock() - st << endl;

    st = clock();
    std::vector<std::thread> commonThreadPool;
    for (size_t i = 0; i < MAXT; i++) commonThreadPool.push_back(std::move(std::thread(TestCommonLockFun)));
    for (auto& worker : commonThreadPool)  worker.join();
    cout << "��ͨ�����̵߳ĺ�ʱΪ" << clock() - st << endl;

    st = clock();
    std::vector<std::thread> spinLockThreadPool;
    for (size_t i = 0; i < MAXT; i++) spinLockThreadPool.push_back(std::move(std::thread(TestSpinLockFun)));
    for (auto& worker : spinLockThreadPool)  worker.join();
    cout << "AI��ʾ�������������̵߳ĺ�ʱΪ" << clock() - st << endl;

    int r = 0;
    std::atomic<int> cc = 0;

    st = clock();
    for (size_t i = 0; i < 500000000; i++)
    {
        cc = i;
    }
    cout << "ԭ�Ӳ���ʱ����" << clock() - st << endl;

    st = clock();
    for (size_t i = 0; i < 500000000; i++)
    {
        r = i;
    }
    cout << "��ͨ����ʱ����" << clock() - st << endl;

    int dd = r;
}