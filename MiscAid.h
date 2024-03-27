#pragma once
#include <iostream>
#include <list>
#include <vector>
using namespace std;

#define BALLNUM  6  // �������Ϸ�������������
#define GLUENUM  2  // �������Ϸ�н�ˮ�������

RBTree<State> T;

uint16_t TempBinaryBalls[16];
uint64_t * AP = (uint64_t *) & (TempBinaryBalls[0]);
uint64_t * BP = (uint64_t *) & (TempBinaryBalls[4]);
uint64_t * CP = (uint64_t *) & (TempBinaryBalls[8]);
uint64_t * DP = (uint64_t *) & (TempBinaryBalls[12]);
unsigned short StoneAry[16];    // �ö����Ʊ�ʾ�����й̶�ʯͷ������

void GenerateVirtualObstaclesList();



/***********************************************************************************************************************************
 *  ������������ɽ�ˮ���������飻 ����������5��, ��ˮ������3��, ��ô������һ�׺���ͷ����β����һ�����飻�����һ�����飺����ͷ  *
 *  ����[0][1]����β����[2][3][4], �������������ʾ[0][1][2], [0][1][3]��[0][1][4]�����ֽ�ˮ�����, �����Ļ���������ʱ��һ�����ȭ *
 *  ��ֻ��Ҫ�л�2,3,4����, ͷ��0��1���Թ̶�ס���л��Խ�ʡ����Ч�ʡ���ĩβ���л����鵽[1��2]��ͷ����ϣ����Ķ�Ӧβ����[3��4]        *   
 ***********************************************************************************************************************************/
char AidAry[GLUENUM - 1]; // һ��Ϊ�����ɱ���̶�����϶���������ʱ��������
struct VariationCombine
{
    char _headAry[GLUENUM - 1];  // �����ͷ������
    char _tailAry[BALLNUM];      // �����β������
    VariationCombine() 
    {
        for (size_t i = 0; i < BALLNUM; i++) _tailAry[i] = -1;
    }
};

vector<VariationCombine> *GluesCombine = new vector<VariationCombine>(); // ȫ�ֱ���ָ��, ���������н�ˮ��ı������

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
/*******  ����������ڽ�ˮ����ϵ��㷨�Ѿ�ȫ����� *******/


// ������ɹ̶���ʯͷ
void RandGenerateStone(int stoneNum = 16)
{
    memset(StoneAry, 0, 32); // ��������е�ʯͷ

    for (int i = 0; i < stoneNum; i++)
    {
        int index = rand() % 16;
        int offset = rand() % 16;
        StoneAry[index] = (1 << offset) | StoneAry[index];
    }

    GenerateVirtualObstaclesList(); // ���Ҫ����ˢ�������ϰ�list
}
// �ֶ�����һЩʯͷ
void ManualGenerateStone(bool saveStone = false)
{
    if (!saveStone) memset(StoneAry, 0, 32);

    int maxStone = 0;
    cout << "����������Ҫ�����ʯͷ����:";
    cin >> maxStone;
    int x, y;
    for (int i = 0; i < maxStone; i++)
    {
        cout << "�������" << i + 1 << "��ʯͷ��X����:";
        cin >> x;
        cout << "�������" << i + 1 << "��ʯͷ��Y����:";
        cin >> y;
        StoneAry[y] |= (1 << x);
    }

    GenerateVirtualObstaclesList();
}

/** �����ĸ�ȫ�ֱ������飬��ʾ���Ǿ����еĹ����ϰ�ʯ **/
list<int> UpList[16];
list<int> DownList[16];
list<int> LeftList[16];
list<int> RightList[16];

// ���ݾ����й̶�ʯͷ��״̬�����������������ĸ�����������ϰ���
void GenerateVirtualObstaclesList()
{
    // ��Ϊ�������������е�virtualobstacles, ���������������ǰ���ɵ�virtualobstacles
    for (size_t i = 0; i < 16; i++) 
    {
        UpList[i].clear();
        DownList[i].clear();
        LeftList[i].clear();
        RightList[i].clear();
    }

    /******�������ϴ�������´����16�������ϰ���*****/
    for (int row = 0; row < 16; row++)
    {
        UpList[row].push_back(15); // ���ϴ���,����ɶ���, �������һ���߶�Ϊ15�������ϰ�, ���е��������(��������Ҳ��ͬ����׸��)
        bool setFlag = false;      // ���������ϴ���, �����ڵ��ߵ���, ��һ���߶�Ϊ5��ʵ���ϰ�, ��ô��Ӧ�����һ���߶�Ϊ4�������ϰ���
        for (int line = 15; line >= 0; line--)
        {
            if ((StoneAry[line] & (1 << row))) // ��ѭ���������λ����һ��ʯͷ
                setFlag = true;                // �����ϰ��ı�־��Ϊ�棨�����Ȳ������ϰ���
            else if (setFlag == true)          // ��Ȼ����ѭ�������λ��û��ʯͷ�����������ϰ��ı�־Ϊtrue��˵����ǰ�й�ʯͷ������ʯͷ֮��ĵ�һ����λ��
            {
                UpList[row].push_front(line);  // ��ô��������ʵ����һ���ϰ�
                setFlag = false;               // �����ϰ��ı�־��Ϊfalse
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


    /******�������󴵷�����Ҵ���ĸ�16�������ϰ���******/
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

// ����������ڿ���̨�п������ϰ����, mode�ǿ��ĸ������; ������˵�˺����õĲ���, ���Կ���ɾ����
void ShowVirtualObstacle(int mode)
{
    string tu[16];
    for (size_t i = 0; i < 16; i++) tu[i] = "________________________________ �������谭" + to_string(i);
    switch (mode)
    {
    case 0:
        cout << "F E D C B A 9 8 7 6 5 4 3 2 1 0" << endl;
        for (size_t i = 0; i < 16; i++)
        {
            for (list<int>::iterator item = UpList[i].begin(); item != UpList[i].end(); item++) {
                tu[*item].replace(30 - i * 2, 2, "��");
            }
        }

        for (int i = 15; i >= 0; i--) cout << tu[i] << endl;
        cout << "F E D C B A 9 8 7 6 5 4 3 2 1 0" << endl;
        break;

    case 1:
        cout << "F E D C B A 9 8 7 6 5 4 3 2 1 0" << endl;
        for (int i = 0; i < 16; i++) {
            for (list<int>::iterator item = DownList[i].begin(); item != DownList[i].end(); item++) {
                tu[*item].replace(30 - i * 2, 2, "��");
            }
        }

        for (int i = 15; i >= 0; i--) cout << tu[i] << endl;
        cout << "F E D C B A 9 8 7 6 5 4 3 2 1 0" << endl;
        break;

    case 2:
        cout << "F E D C B A 9 8 7 6 5 4 3 2 1 0" << endl;
        for (int i = 0; i < 16; i++)
            for (list<int>::iterator item = LeftList[i].begin(); item != LeftList[i].end(); item++)
                tu[i].replace(30 - *item * 2, 2, "��");

        for (int i = 15; i >= 0; i--) cout << tu[i] << endl;
        cout << "F E D C B A 9 8 7 6 5 4 3 2 1 0" << endl;
        break;

    case 3:
        cout << "F E D C B A 9 8 7 6 5 4 3 2 1 0" << endl;
        for (int i = 0; i < 16; i++)
            for (list<int>::iterator item = RightList[i].begin(); item != RightList[i].end(); item++)
                tu[i].replace(30 - *item * 2, 2, "��");

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

    // ���������һ���ȽϺ���, ��other��������״̬, �Ƚϱ������ǵ�״̬, �������Ƿ�һ��
    bool Compare(BallState* otherP)
    {
        uint16_t myAry[16];        // ��ע�⣬Ҫ�Ƚ��������Ƿ���ͬ�Ļ������ܵ������������Ƿ�λ�ö�һ�����жϣ�����A������������1����X1λ�ã�2����X2λ
        uint16_t otherAry[16];     // �ã�Ȼ��B�����1����X2λ�ã�2����X1λ�ã������淽���ȽϵĻ��� ���ж������治��ȣ�����Ȼ�Ǵ�ģ���Ϊʵ�����������
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


list<BallState*> DeepThinkListAry[100]; // ����˼���������������Ǿ���, ���������
uint8_t DeepThinkLv = 0;                // �����ֵ�������˼���˶��ٲ�, Ϊ0˵������û��˼����

list<BallState*> FileStatesList;


// �������������
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

// �ڿ���̨��չʾ�������״̬, ����ʯͷ���Լ������ǵ�λ��
void ShowAllState(const BallState * balls)
{
    string tu[16];
    for (size_t i = 0; i < 16; i++) tu[i] = "________________________________ǽ" + to_string(i);
    
    for (int y = 15; y >= 0; y--)
        for (int x = 15; x >= 0; x--)
            if (((1 << x) & StoneAry[y]) > 0) tu[y].replace(30 - x * 2, 2, "��");

    for (size_t i = 0; i < BALLNUM; i++)
    {
        int x = 30 - 2 * balls->_posX[i];
        int y = balls->_posY[i];
        if (x < 0 || x >= tu[y].size())
            cout << "�±곬��, ��ʼ����: " << x << ", �����ڸ߶�: " << y << "; ����һ����󳤶�Ϊ" << tu[y].size() << endl;
        else tu[y].replace(x, 2, to_string(i) + to_string(i));
    }
        

    cout << "F E D C B A 9 8 7 6 5 4 3 2 1 0" << endl;
    for (int i = 15; i >= 0; i--) cout << tu[i] << endl;
    cout << "F E D C B A 9 8 7 6 5 4 3 2 1 0" << endl;
}

// ��׼����ʼ�����ǵ�״̬, ��������Ӧ��>=4, ����4������, ��ô��������ĸ���, ÿ���Ƕ���һ������
void StandardInitializeBallsState()
{
    if (BALLNUM < 4) {
        cout << "��������̫��, ����ܾ���׼����ʼ������, ���ｨ�����ֶ��������" << endl;
        return;
    }
    BallState* balls = new BallState(nullptr);
    balls->_posX[0] = 0; balls->_posY[0] = 0;
    balls->_posX[1] = 15; balls->_posY[1] = 0;
    balls->_posX[2] = 0; balls->_posY[2] = 15;
    balls->_posX[3] = 15; balls->_posY[3] = 15;

    for (int i = 4; i < BALLNUM; i++) // ����������������������4������ô��5������ı�׼��ʼ��λ���������±귽����Դ�����
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

// �ֹ�����������
void ManualGenerateBalls()
{
    int x, y;
    BallState* balls = new BallState(nullptr);
    for (size_t i = 0; i < BALLNUM; i++)
    {
        cout << "�������" << i + 1 << "�����X����:";
        cin >> x;
        cout << "�������" << i + 1 << "�����Y����:";
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


// �����˼������, ��main���������һ��forѭ��ʹ�ã�Ŀǰ��ʱ��֧�ֲ���ģʽ���Ժ�������Ҫ���ĳɶ��̲߳���ģʽ��
void Ponder()
{
    list<int>* headBlocksInListAlias[(GLUENUM - 1)*4]; // �����д��������, ������һ�����ڱ��������������,��Ϊ�ڱ�����ˮ�����ѭ����
    list<int>::iterator headBlocks[(GLUENUM - 1) * 4]; // �������Ҫ������ݽ�ˮ���������ӵ�block, ʹ���Ĵ�blockList�ָ�����ԭʼģ��
    for (BallState* stateItem : DeepThinkListAry[DeepThinkLv])          // ������ǰ���������о��棬�����е�ÿһ�����������ɸþ������ɢ�����з�֧�仯
    {
        for (const VariationCombine& gluesCombine : *GluesCombine)      // ������ˮ��������ÿһ�����
        {
            auto headGlues = gluesCombine._headAry;
            auto tailGlues = gluesCombine._tailAry;
            int headDelIndex = 0;

            // ����,���ʽ�ˮ��������ͷ�����, ����ͷ�����������, ��blockList��Ӷ�Ӧ����blockԪ��
            for (int i = 0; i < GLUENUM - 1; i++)
            {
                int x = stateItem->_posX[headGlues[i]];               // ��ȡͷ��������ĳ��Ž�ˮ���X�����Y����
                int y = stateItem->_posY[headGlues[i]];               // ���Ǹ�X����, ����ζ��Ҫ����ǵ�X�е�list


                list<int>::iterator item = UpList[x].begin();         // ��һ����������UpList: ���ȸ��ݽ�ˮ��ͷ��������ĳ��������Y����, ��
                while (y > *item) item++;                             // ����ӦX�е�UpList��ÿ���߶�blockԪ��, ���ջ�ͣ���ڸ߶Ⱥ��ʵ�listλ����
                headBlocks[headDelIndex] = UpList[x].insert(item, y); // ,������������λ���ϲ���һ���µĸ߶�blockԪ��, ��list���ִ��µ��ϵ���������
                headBlocksInListAlias[headDelIndex] = &UpList[x];     // ��¼�������blockԪ���Լ���������list, ��Ϊ����Ҫ�Ѵ�Ԫ�شӶ�Ӧlist��ɾ��!
                headDelIndex++;                                       // ��ֵ+1, ����ȥ���������߼�ȥ����DownList��LeftList�Լ�RightList, ����׸��

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


            for (size_t glueNum = 0; tailGlues[glueNum] >= 0; glueNum++) // ����β���Ľ�ˮ��
            {
                int glueX = stateItem->_posX[tailGlues[glueNum]];        // ��ȡ����ŵĽ�ˮ���X����
                int glueY = stateItem->_posY[tailGlues[glueNum]];        // ��ȡY����
                list<int>::iterator newItem, delItem;

                /****���������ϴ�����߼���ʼ****/
                newItem = UpList[glueX].begin();                         // ����X��list��ÿ���߶�blockԪ��, ֱ��ͣ���ڸ߶Ⱥ��ʵ�λ����Ȼ��
                while (glueY > *newItem) newItem++;                      // �������λ�ò����µ�blockԪ��, ʹ��list���ִ��µ��ϵ���������
                delItem = UpList[glueX].insert(newItem, glueY);          // ��¼������µ�blockԪ��, ��Ϊ�����Ҫɾ����Ԫ��
                *AP = *BP = *CP = *DP = 0;                               // ������ձ�ʾ����״̬�Ķ���������
                BallState* afterUpBalls = new BallState(stateItem);
                for (size_t i = 0; i < BALLNUM; i++)                     // Ȼ��������������е�����
                {
                    int posX = stateItem->_posX[i];
                    int posY = stateItem->_posY[i];                      // ��ȡ�����X�����Y����
                    list<int>::iterator item = UpList[posX].begin();
                    while (posY > *item) item++;                         // ���ʶ�Ӧ��blockList, ֱ���ҳ������򴵷��Ӧ��ͣ�����ĸ��ϰ�������
                    int newY = *item;                                    // ��ȡ����ϰ����Ӧ��Y����, 
                    while (TempBinaryBalls[newY] & (1 << posX)) newY--;  // ���������Ӧλ���ǲ���1(�б����������Ϊ����), �����, �߶ȼ�һ, ֱ���ҵ��յ�Ϊֹ
                    TempBinaryBalls[newY] |= (1 << posX);                // ��binary�����и���һ���µ�1, ����¼����Ķ����Ʊ�ʾ
                    afterUpBalls->_posY[i] = newY;                       // �����Y�������һ��
                    afterUpBalls->_posX[i] = stateItem->_posX[i];
                }
                UpList[glueX].erase(delItem);                            // �Ȳ����ղ� ����β����ˮ�� ���ӵ���Ԫ��
                if (true == T.Insert((uint64_t *)AP))      // ���׶�: �ȿ������������������ǰ�Ƿ���ֹ�, û
                    DeepThinkListAry[DeepThinkLv + 1].push_back(afterUpBalls);        // �оͱ����¾��浽��һ����������, �еĻ�, ��ô��ֱ
                else delete afterUpBalls;                                // ��������ղŵľ���, ��������ڴ�й¶
                /****�������ϴ�����߼�������, ������������ҵ�, ����������߼����, ��������Ͳ���׸����****/


                /****Ȼ��������´�����߼���ʼ****/
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
                /****�������´�����߼�������****/



                /****�ٽ������󴵷���߼���ʼ****/
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
                /****���󴵷���߼�������****/



                /****������Ҵ�����߼���ʼ***/
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
                /****�������Ҵ�����߼�������***/
            }

            // �������˻ָ� ����������һ�齺ˮ����ϵ�ͷ�� ��ӵ�����obstacles
            for (size_t i = 0; i < (GLUENUM - 1) * 4; i++)  headBlocksInListAlias[i]->erase(headBlocks[i]);
        }
    }
}

// �����˼���޽�ˮ��ĺ���
void PonderWithZeroGlues()
{
    for (BallState* stateItem : DeepThinkListAry[DeepThinkLv])
    {
        *AP = *BP = *CP = *DP = 0;                               // ������ձ�ʾ����״̬�Ķ���������
        BallState* afterUpBalls = new BallState(stateItem);
        for (size_t i = 0; i < BALLNUM; i++)                     // Ȼ��������������е�����
        {
            int posX = stateItem->_posX[i];
            int posY = stateItem->_posY[i];                      // ��ȡ�����X�����Y����
            list<int>::iterator item = UpList[posX].begin();
            while (posY > *item) item++;                         // ���ʶ�Ӧ��blockList, ֱ���ҳ������򴵷��Ӧ��ͣ�����ĸ��ϰ�������
            int newY = *item;                                    // ��ȡ����ϰ����Ӧ��Y����, 
            while (TempBinaryBalls[newY] & (1 << posX)) newY--;  // ���������Ӧλ���ǲ���1(�б����������Ϊ����), �����, �߶ȼ�һ, ֱ���ҵ��յ�Ϊֹ
            TempBinaryBalls[newY] |= (1 << posX);                // ��binary�����и���һ���µ�1, ����¼����Ķ����Ʊ�ʾ
            afterUpBalls->_posY[i] = newY;                       // �����Y�������һ��
            afterUpBalls->_posX[i] = stateItem->_posX[i];
        }
        if (true == T.Insert((uint64_t*)AP))      // ���׶�: �ȿ������������������ǰ�Ƿ���ֹ�, û
            DeepThinkListAry[DeepThinkLv + 1].push_back(afterUpBalls);        // �оͱ����¾��浽��һ����������, �еĻ�, ��ô��ֱ
        else delete afterUpBalls;                                // ��������ղŵľ���, ��������ڴ�й¶


        /****Ȼ��������´�����߼���ʼ****/
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
        /****�������´�����߼�������****/

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

// �����˼��ֻ��һ����ˮ��ĺ���
void PonderWithOneGlue()
{
    for (BallState* stateItem : DeepThinkListAry[DeepThinkLv])
    {
        for (size_t glueNum = 0; glueNum < BALLNUM; glueNum++) // ����β���Ľ�ˮ��
        {
            int glueX = stateItem->_posX[glueNum];        // ��ȡ����ŵĽ�ˮ���X����
            int glueY = stateItem->_posY[glueNum];        // ��ȡY����
            list<int>::iterator newItem, delItem;

            /****���������ϴ�����߼���ʼ****/
            newItem = UpList[glueX].begin();                        // ����X��list��ÿ���߶�blockԪ��, ֱ��ͣ���ڸ߶Ⱥ��ʵ�λ����Ȼ��
            while (glueY > *newItem) newItem++;
            delItem = UpList[glueX].insert(newItem, glueY);         // ��¼������µ�blockԪ��, ��Ϊ�����Ҫɾ����Ԫ��

            *AP = *BP = *CP = *DP = 0;                               // ������ձ�ʾ����״̬�Ķ���������
            BallState* afterUpBalls = new BallState(stateItem);
            for (size_t i = 0; i < BALLNUM; i++)                     // Ȼ��������������е�����
            {
                int posX = stateItem->_posX[i];
                int posY = stateItem->_posY[i];                      // ��ȡ�����X�����Y����
                list<int>::iterator item = UpList[posX].begin();
                while (posY > *item) item++;                         // ���ʶ�Ӧ��blockList, ֱ���ҳ������򴵷��Ӧ��ͣ�����ĸ��ϰ�������
                int newY = *item;                                    // ��ȡ����ϰ����Ӧ��Y����, 
                while (TempBinaryBalls[newY] & (1 << posX)) newY--;  // ���������Ӧλ���ǲ���1(�б����������Ϊ����), �����, �߶ȼ�һ, ֱ���ҵ��յ�Ϊֹ
                TempBinaryBalls[newY] |= (1 << posX);                // ��binary�����и���һ���µ�1, ����¼����Ķ����Ʊ�ʾ
                afterUpBalls->_posY[i] = newY;                       // �����Y�������һ��
                afterUpBalls->_posX[i] = stateItem->_posX[i];
            }
            if (true == T.Insert((uint64_t*)AP))      // ���׶�: �ȿ������������������ǰ�Ƿ���ֹ�, û
                DeepThinkListAry[DeepThinkLv + 1].push_back(afterUpBalls);        // �оͱ����¾��浽��һ����������, �еĻ�, ��ô��ֱ
            else delete afterUpBalls;                                // ��������ղŵľ���, ��������ڴ�й¶

            UpList[glueX].erase(delItem);
            /****���ϴ�����߼�������****/



            /****Ȼ��������´�����߼���ʼ****/
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
            /****�������´�����߼�������****/



            /****�ٽ������󴵷���߼���ʼ****/
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
            /****���󴵷���߼�������****/



            /****������Ҵ�����߼���ʼ***/
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
 * ��ע��: ����ĺ��������߼����ǱȽϿ��, ��������һ��������ȱ��: �޷��������µ��ƶ�, ������Ҫ��  *     
 * ƽת���ſ���, ��֮��д��������, ������������߼�����, �����ٶ�̫���Ļ�, ��ת��ʹ��������߼���  *
 ***************************************************************************************************/


/*** ��ȡһ�����Ķ����Ʊ�ʾ�����ұߵ��Ǹ�1, Ҳ����: ��ĳ����ĳһ�������ƶ�ʱ, �������Ӧ��ͣ����λ��; ***/
uint16_t GetRightBinary(int64_t obstacleVal, int64_t ballVal)
{
    obstacleVal &= ~((int64_t)-1 + ballVal); // ���������ȴ���һ��obstacleval, ʹ������ɴ�ballValλ��ʼ���ҵ�����λͳͳΪ0

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
// ��ȡһ�������Ʊ�ʾ������ߵ��Ǹ�1, Ҳ����ĳ����ĳһ�������ƶ�ʱ, �������Ӧ��ͣ����λ��
uint16_t GetLeftBinary(int64_t obstacleVal, int64_t ballVal)
{
    obstacleVal &= (int64_t)-1 + ballVal; // ���������ȴ���һ��obstacleval, ʹ������ɴ�ballValλ��ʼ���������λͳͳΪ0

    uint16_t retVal = obstacleVal - 1;
    retVal ^= obstacleVal;
    return (retVal + 1) >> 2;
}


/********************************************************************************************************************
 * �����ƶ��������������ĸ��죻�����и�ͬ����ȱ�㣺ֻ�ܴ������ҵ��ƶ��������µ��ƶ����˾���Ե��ƺ�ûɶ�ð취���� *
 * PS��ά���ƺ����ü�����14*14��ֻ��궨���farleft����ֵ���������һ�������Ƿ�ĳ������ߣ��������Ӧλ������ʯͷ *
 ********************************************************************************************************************/
constexpr auto FARLEFT = (uint64_t(1) << 15) + (uint64_t(1) << 31) + (uint64_t(1) << 47) + (uint64_t(1) << 63);      // ����һ�����������ϰ�����ֵ��������������Ƿ��ƶ�����Ӧλ��;
uint64_t LeftMove(uint64_t obstacleVal, uint64_t ballsVal)  // ��������������������������һ�������ƶ���obstacle��ʯͷ��balls�������ǣ�
{
    if (ballsVal & FARLEFT) {                 // ������������������һ����������һˮƽ�ߵ�����λ��
        obstacleVal |= (ballsVal & FARLEFT);  // ��ôֱ�Ӱ����������Ϊʯͷ���һԱ
        ballsVal -= (ballsVal & FARLEFT);     // �޳��������
    }
    uint64_t overlapVal = 0;
    while (ballsVal > 0) {                            // �����滹�����ƶ�������
        overlapVal = obstacleVal & (ballsVal << 1);   // ��ô���ȿ�������������α�ƶ�һ��֮���Ƿ�����ص��ϰ�ʯͷ�� 
        if (overlapVal) {                             // �����ص��Ļ�
            obstacleVal |= (overlapVal >> 1);         // �ϰ�ʯͷ��Ķ�Ӧλ����������һ��
            ballsVal ^= (overlapVal >> 1);            // ���������Ƕ�Ӧλ�õ������޳���
        }else {                                       // ��Ȼ����û���ص�
            ballsVal <<= 1;                           // ��ô��������������ʵ�ƶ�
            if (ballsVal & FARLEFT) {
                obstacleVal |= (ballsVal & FARLEFT);  // ��ôֱ�Ӱ����������Ϊʯͷ���һԱ
                ballsVal -= (ballsVal & FARLEFT);
            }
        }
    }
    return obstacleVal;
}

constexpr auto FARRIGHT = 1 + (1<<17) + (1<<33) + (1<<49);
uint64_t RightMove(uint64_t obstacleVal, uint64_t ballsVal) // ͬ�������ƶ�
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
 * ����һ�ַ�����죬������Ҫ�ܴ�Ĺ����������ǰѴ���ֵ���ֳ�8λ����ֵ���������г����������ֵ��Ȼ��ֱ����switch��  *
 * ��ֵ��ת���������μ��ɣ�����16*16�ľ����һ��д������8������ֵ���ɣ�Ȼ����ҵ��������Ҫ��λ���ж��㷨����֮   *
 * �����ȼ�¼һ�±���                                                                                               *
 ********************************************************************************************************************/