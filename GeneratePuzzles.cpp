#include "RBTree.h"
#include "MiscAid.h"
#include "Status.h"
#include <string>
#include <time.h>
#include <fstream>
#include <stack>


std::string BallGlueStr = std::to_string(BALLNUM) + "球" + to_string(GLUENUM) + "胶";
int main()
{
    //testParallel(); // 测试多线程执行速度的，感觉不需要了可以删掉！
    srand((unsigned int)time(NULL));
    
    // 这里是测试红黑树插入查询的, 这里暂时废弃掉
    if (false)
    {
        std::string str[100];
        for (size_t i = 0; i < 15; i++)
        {
            long long unsigned int a, b, c, d;
            a = rand() % 10;
            b = rand() % 10;
            c = rand() % 10;
            d = rand() % 10;
            cout << "随机生成的数值是:" << a << "," << b << "," << c << "," << d << endl;
            State* temp = new State(a, b, c, d);
            T.Insert(*temp);
        }

        T.UpdatePyramid(T._root, 0, str);
        T.ShowRBTreePyramid(str);

        {
            long long unsigned int a, b, c, d;
            cin >> a;
            cin >> b;
            cin >> c;
            cin >> d;
            State temp = State(a, b, c, d);
            T.Insert(temp);
            cout << "红黑树已经更新, 新的红黑树外观如下:" << endl;
            T.UpdatePyramid(T._root, 0, str);
            T.ShowRBTreePyramid(str);
        }
    }


    GenerateVariation(BALLNUM, GLUENUM); // 首先生成胶水球们的各种组合（这非常重要，请不要注释这句代码）

    // 然后打印这些胶水球组合, 测试用，这里暂时废弃掉
    if (false)
    {
        int count = 0;
        cout << "所有的变体固定球组合有多少种呢? 答案如右: " << GluesCombine->size() << endl;
        for (vector<VariationCombine>::iterator item = GluesCombine->begin(); item != GluesCombine->end(); item++)
        {
            cout << count << ":   ";
            for (size_t i = 0; i < GLUENUM - 1; i++)
            {
                cout << (int)item->_headAry[i];
            }
            cout << " | ";
            for (size_t i = 0; i < BALLNUM; i++)
            {
                if (-1 < item->_tailAry[i]) cout << (int)item->_tailAry[i] << ",";
                else break;

            }
            cout << endl;
            count++;
        }
    } 

    programStart:
    RandGenerateStone();            // 随机生成石头们
    GenerateVirtualObstaclesList(); // 生成完石头们后就要生成所有的virtual obstacles
    RandGenerateBalls();            // 随机生成球球们
   
    auto startTime = clock();
    int op = 0;
    string fileName = "B" + to_string(BALLNUM) + "G" + to_string(GLUENUM) + ".txt"; // 在switch体内要用到的文件名
    {
    switchStart:
        cout << endl << "********************* "<<BallGlueStr<<" **************************" << endl;
        cout << "现在程序处于switch Start阶段, 局面:" << endl;
        ShowAllState(*DeepThinkListAry[0].begin()); // 展示局面的状态, 包括球球和石头
        cout << "输入0： 程序单线程搜索所有的可变局面（详细说明执行此操作后可见）\n输入1： 标准对角式初始化局面（只针对>=4球，球太少请手动录入）;\n输入2： 手动编辑石头们的位置（详细说明执行此操作后可见）;\n输入3： 手动编辑球球们的位置（详细说明执行此操作后可见）;\n输入4： 让程序查找您指定的局面（详细说明执行此操作后可见）;\n输入5： 让程序列出某一层的某一段的思考局面（详细说明执行此操作后可见）;\n输入6： 让程序展示您指定的某一层的某一下标的局面（详细说明执行此操作后可见）;\n输入7： 程序会把标准局面下的搜索结果重新写入到对应文件里（在执行该操作前，务必确认程序当前思考的是标准局面下的所有变化，不然写入文件里的数据不对头，有可能会影响到第8和第9功能）\n输入8： 程序会在当前局面变化的某一层里（输入8之后还要求您再输入一个层数），一个一个的列出所有不同于标准局面所变化出的搜索局面；\n输入9： 程序随机给出一个不同于标准变化的局面；\n输入10：让程序随机给用户初始化一个局面；\n输入11：自定义围城的大小，还需要您输入高度和宽度。请注意！重定义围城大小后，您应重新定义那些球球的位置（执行操作3），因为石头可能会覆盖掉原先的球球；\n输入12：程序回到switch的之前，并清理掉所有的成果（如果不小心操作乱套，可以执行此操作）； \n输入99：看看初始局面一共能衍变出多少层的变化；\n输入255：让程序多线程思考（不建议用，速度较慢，因为红黑树的锁域太大，等把红黑树换成哈希表后再搞）；\n 现在, 请输入您的操作:";
        cin >> op;
        switch (op)
        {
        case 255:
        {
            cout << "现在您有一次改变胶水球数量的机会： 输入0, 没有胶水球; 输入1, 只有一个胶水球; 输入其它任何正整数, 程序按原计划推演N层(N = 您的输入); 请输入:";
            int glueNum = 0;
            cin >> glueNum;
            void (*ponderFunPtr)() = Ponder;
            if (0 == glueNum)
            {
                ponderFunPtr = PonderWithZeroGlues;
                glueNum = 100;
            }
            else if (1 == glueNum)
            {
                ponderFunPtr = PonderWithOneGlue;
                glueNum = 100;
            }
            else ponderFunPtr = LoopStartParallel;
            if (0 == DeepThinkLv) // 若程序还没有思考过局面(DeepThinkLv为0说明程序没有思考过)
            {
                auto totalUseTime = clock();
                for (DeepThinkLv = 0; DeepThinkLv < glueNum; DeepThinkLv++)
                {
                    auto layerUseTime = clock();
                    ponderFunPtr();
                    cout << "多线程思考：第" << (int)DeepThinkLv << "层搜索完毕，用时（毫秒）：" << clock() - layerUseTime << "；" << "搜出新局面：" << DeepThinkListAry[DeepThinkLv + 1].size() << endl;
                }
                cout << "至此已经搜满了您需要搜索的层数的所有局面,用时" << clock() - totalUseTime << endl;
            }
            else cout << "程序已经思考过了, 请不要让它重复思考" << endl;
            goto switchStart;
        }
            break;
        case 0:
        {
            cout << "现在您有一次变更胶水球数量的机会, 输入0, 没有胶水球; 输入1, 只有一个胶水球; 输入其它任何正整数, 程序按原计划思考N层(N = 您的输入); 请输入:";
            int glueNum = 0;
            cin >> glueNum;
            void (*ponderFunPtr)() = Ponder;
            if (0 == glueNum) 
            {
                ponderFunPtr = PonderWithZeroGlues;
                glueNum = 100;
            }
            else if (1 == glueNum)
            {
                ponderFunPtr = PonderWithOneGlue;
                glueNum = 100;
            }
            if (0 == DeepThinkLv) // 若程序还没有思考过局面(DeepThinkLv为0说明程序没有思考过)
            {
                auto totalUseTime = clock();
                for (DeepThinkLv = 0; DeepThinkLv < glueNum; DeepThinkLv++)
                {
                    auto layerUseTime = clock();
                    ponderFunPtr();
                    cout <<"单线程思考：第" << (int)DeepThinkLv << "层搜索完毕，用时（毫秒）：" <<clock()-layerUseTime<<"；"<<"搜出新局面："<<DeepThinkListAry[DeepThinkLv+1].size()<< endl;
                }
                cout << "至此已经搜满了您需要搜索的层数的所有局面,用时" << clock() - totalUseTime << endl;
            }
            else cout << "程序已经思考过了, 请不要让它重复思考" << endl;
            goto switchStart;
        }
            break;

        case 1:  // 第一功能：用标准来初始化局面（四个球球分别位于四个角上，局面内不存在石头）
        {
            memset(StoneAry, 0, 32);        // 清空所有的石头
            GenerateVirtualObstaclesList(); // 重新生成虚拟障碍物链表
            StandardInitializeBallsState(); // 标准化局面
            goto switchStart;
            break;
        }

        case 2:  // 第2功能: 用户手动输入石头们的局面
        {
            int op2 = 0;
            cout << "现在请输入一个数字，如果数字是0，那么程序会先清除所有已经存在的石头；如果是1，那么程序会保留以前的石头：" << endl;
            cin >> op2;
            ManualGenerateStone(op2 == 1);
            goto switchStart;
            break;
        }

        case 3:  // 第3功能: 用户手动输入球球们的局面
        {
            ManualGenerateBalls();
            goto switchStart;
            break;
        }

        case 4: // 第4功能: 用户在控制台上手动输入一个局面的样子, 然后程序在它的思考结果中搜索, 看能不能找到相同的局面
            if (0 < DeepThinkLv)
            {
                BallState* target = new BallState(nullptr);
                int puzzleLv; int cnm;
                for (int i = 0; i < BALLNUM; i++)
                {
                    cout << "请输入" << i << "号球球的X坐标:";
                    cin >> cnm;
                    target->_posX[i] = cnm; // 请注意!!!这里不能直接输入target_>posX[], 因为它是uint8类型, 在cin输入里赋值转换时会抹掉末24位, 只保留高8位!
                    cout << "请输入" << i << "号球球的Y坐标:";
                    cin >> cnm;
                    target->_posY[i] = cnm; // 同上切记!!!
                }
                for (puzzleLv = 0; puzzleLv < DeepThinkLv; puzzleLv++)
                {
                    for (BallState* stateItem : DeepThinkListAry[puzzleLv])
                    {
                        if (true == target->Compare(stateItem))
                        {
                            cout << "已找到目标局面, 玩家至少需要操作" << puzzleLv << "步" << endl;
                            target = stateItem;
                            goto showStep;
                        }
                    }
                }
                cout << "在思考结果中找不到这个局面, 这意味着玩家在" << DeepThinkLv << "个步骤之内, 无法将局面移动成这个样子!" << endl;
                goto switchStart;
            showStep:
                std::stack<BallState*> stepStack;
                while (nullptr != target)
                {
                    stepStack.push(target);
                    target = target->_parent;
                    puzzleLv--;
                }
                while (!stepStack.empty()) {
                    cout << BallGlueStr<<":现在为您展开解谜步骤(按下enter键查看下一步), 下面的是第" << ++puzzleLv << "步的走法" << endl;
                    auto stepState = stepStack.top();
                    ShowAllState(stepState);
                    stepStack.pop();
                    system("pause");
                }
                goto switchStart;
            }
            else cout << "程序还没有思考呢, 您无法查看局面" << endl;
            system("pause");
            goto switchStart;
            break;

        case 5:  // 第5功能: 在控制台上查看某一层的所有可能局面, 用户手动输入某一层层数, 然后程序列出那一层的所有变化局面
            if (0 < DeepThinkLv)
            {
                int outputLv = 0;
                uint32_t curIndex = 0, startIndex, endIndex;
                cout << "请输入您要查看第几层的局面:";
                cin >> outputLv;
                if (0 == DeepThinkListAry[outputLv+1].size()) {
                    cout << "抱歉，根据您输入的层数，在当前层数下没有任何新局面，程序无法展示，所以现在回到switch start处" << endl;
                    goto switchStart;
                }
                cout << "您要查看的是第"<<outputLv<<"层的局面；现在请输入您想从该层的第几个局面开始查看：";
                cin >> startIndex;
                cout << "那么您想查到第几个局面结束呢？请输入（一般控制台只能看四五百个局面）：";
                cin >> endIndex;
                for (const BallState* stateItem : DeepThinkListAry[outputLv+1])
                {
                    if (curIndex >= startIndex && curIndex <= endIndex) {
                        cout << "这是第" << outputLv << "层的某个第" << curIndex << "个局面" << endl;
                        ShowAllState(stateItem);
                    }
                    curIndex++;
                }
            }
            else cout << "程序还没有思考呢, 您无法查看局面, 所以回到switchStart" << endl;
            system("pause");
            goto switchStart;
            break;

        case 6:  // 第6功能: 用户查询某个具体谜题的解法, 需要用户输入谜题所在的层数, 以及谜题所在那一层的第几个局面(对程序而言)
            if (0 < DeepThinkLv)
            {
                int lv, index;
                cout << "请输入您要找的层数:";
                cin >> lv;
                if (lv > DeepThinkLv) {
                    cout << "您输入的层数太多了，程序根本没思考到这里,现在程序回到switch开始的地方！\n" << endl;
                    goto switchStart;
                }
                cout << "您要找第" << lv << "层, 那么请输入下标:";
                cin >> index;
                BallState* focusState = nullptr;
                for (BallState* tempState : DeepThinkListAry[lv]) // 遍历这一层思考结果中所有的局面
                {
                    if (0 == index) // 由于是用户手动输入特定局面在某一层中的下标，所以这里挨个查找即可（index为0时就是那个局面啦）
                    {
                        focusState = tempState;
                        break;
                    }
                    index--;
                }

                std::stack<BallState *> stepStack;
                while (focusState != nullptr) // 找到了那个用户要查找的局面，根据她的parent指针，逐步剥开谜题链
                {
                    stepStack.push(focusState);
                    focusState = focusState->_parent;
                    lv--;
                }
                while (!stepStack.empty()) {
                    cout << BallGlueStr<<":现在为您展开解谜步骤, 下面的是第" << ++lv << "步的局面" << endl;
                    ShowAllState(stepStack.top());
                    stepStack.pop();
                    system("pause");
                }
            }
            else cout << "程序还没有思考呢, 现在回到开始, 请重新操作" << endl;
            goto switchStart;
            break;

        case 7:  // 第7功能： 向一个文件中写入程序思考的局面
        {
            cout << "请注意，现在重新向特定文件内写入标准局面所能变化出的所有局面，所以您应该确定您当前的思考结果是标准局面的思考结果！" << endl;
            ofstream output(fileName, ios::out); // 以清除文件里原先内容 + 追加写入到文件里的方式打开此文件
            auto writeStart = clock();
            for (int lv = 0; DeepThinkListAry[lv].size() > 0; lv++)
            {
                for (BallState* state : DeepThinkListAry[lv])
                {
                    if (nullptr != state)
                    {
                        for (int i = 0; i < BALLNUM; i++)
                            output << (char)('0' + state->_posX[i]) << (char)('0' + state->_posY[i]);
                        output << '\n';
                    }
                }
            }
            output.close();
            cout << "标准局面变化出的所有局面写入到文件完毕，共用时：" << clock() - writeStart << endl;
            goto switchStart;
        }
        break;

        case 8: // 第8功能： 从文件中读取所有的局面，并把它们都保存进一个全局链表里（方便以后拿某个搜索的局面与这个链表局面们比较）
        {
            ifstream file(fileName);
            string str;
            while (getline(file, str))
            {
                BallState* balls = new BallState(nullptr);
                for (size_t i = 0; i < str.length(); i+=2)
                {
                    uint8_t x = str[i] - '0';
                    uint8_t y = str[i + 1] - '0';
                    balls->_posX[i / 2] = x;
                    balls->_posY[i / 2] = y;
                }
                FileStatesList.push_back(balls);
            }
            cout << "请注意，现在您是在查看在某一层里那些不同于标准变化的局面，这意味着您要自己保证您的文件里保存的是正确的标准局面变化的信息，如若不然您所做的这些都将毫无意义。现在，请输入您的层数：";
            int searchLv = 0;
            cin >> searchLv;
            if (0 >= DeepThinkListAry[searchLv].size())
            {
                cout << "对不起，您要查找的局面程序没有思考过这里，所以现在回到游戏开始的地方，请重新执行操作" << endl;
                goto switchStart;
            }
            for (BallState* ballState : DeepThinkListAry[searchLv])
            {
                for (BallState* fileState : FileStatesList)
                {
                    if (ballState->Compare(fileState) == true)
                    {
                        cout << "这是一个在标准变化里出现过的局面,所以程序把她忽略掉，重新考虑下一个局面" << endl;
                        goto loopEnd;
                    }
                }
                cout << "找到了一个没有在标准变化里出现过的局面，模样如下：" << endl;
                ShowAllState(ballState);
                cout << "现在请输入您的操作，如果您输入0按下回车，那么程序继续寻找该层的下一个没有在标准变化里出现过的局面；若您输入1按下回车，那么程序会一步一步的剥开当前谜题是怎么走的；若您按下-1回车，那么程序回到switch的开始" << endl;
                int op8;
                cin >> op8;
                switch (op8)
                {
                case -1:
                    goto switchStart;
                    break;
                case 0:
                    break;
                case 1:
                {
                    BallState* curentState = ballState;
                    while (nullptr != curentState)
                    {
                        cout << "现在是处于第8功能里的查找非标准变化局面，现在来一下一下剥开某个非标准变化局面：" << endl;
                        ShowAllState(curentState);
                        curentState = curentState->_parent;
                        system("pause");
                    }
                }
                }

            loopEnd:;
            }
        }
            break;

        case 9:
        {
            ifstream file(fileName);
            string str;
            FileStatesList.clear();
            while (getline(file, str))
            {
                BallState* balls = new BallState(nullptr);
                for (size_t i = 0; i < str.length(); i += 2)
                {
                    uint8_t x = str[i] - '0';
                    uint8_t y = str[i + 1] - '0';
                    balls->_posX[i / 2] = x;
                    balls->_posY[i / 2] = y;
                }
                FileStatesList.push_back(balls);
            }
            cout << "请注意，现在您是在随机查看一个不同于标准变化的某个局面，所以请保证您的文件里保存的是正确的标准局面变化的信息，如若不然您所做的这些都将毫无意义！" << endl;
            int randLv, randIndex;
            randStart:
            randLv = rand() % DeepThinkLv;
            randIndex = rand() % DeepThinkListAry[randLv].size();
            if (0 >= DeepThinkListAry[randLv].size())
            {
                cout << "对不起，随机出的层数里，根本就没有思考过得局面，所以程序现在回到start处" << endl;
                goto switchStart;
            }
            int i = 0;
            BallState* randBalls = nullptr;
            for (BallState* balls : DeepThinkListAry[randLv])
            {
                if (i == randIndex)
                {
                    randBalls = balls;
                    break;
                }
                i++;
            }
            for (BallState* fileState : FileStatesList)
            {
                if (randBalls->Compare(fileState)) goto randStart;
            }
            cout << "找到了一个不同于标准变化的局面，它处于第" << randLv << "层的第" << randIndex << "个位置，它的模样如下：" << endl;
            ShowAllState(randBalls);
            cout << "现在，按下0再回车，程序会重新找一个不同于标准变化的局面；按下1回车程序会一层一层剥开当前局面的解密步骤；输入其它任何数值再回车程序会回到开始的地方；请输入您的操作" << endl;
            int op9;
            cin >> op9;
            switch (op9)
            {
            case 0:
                goto randStart;
                break;
            case 1:
                BallState * currentBalls = randBalls;
                while (nullptr != currentBalls)
                {
                    ShowAllState(currentBalls);
                    currentBalls = currentBalls->_parent;
                }
                break;
            }
            goto switchStart;
        }
            break;

        case 10:  // 第10功能：给用户随便初始化一个局面,不过需要用户手动输入局面中石头的最多数量
        {
            int stoneNum = 0;
            cout << "请输入局面中最多有多少颗石头：" << stoneNum << endl;
            cin >> stoneNum;
            RandGenerateStone(stoneNum);
            RandGenerateBalls();
            goto switchStart;
            break;
        }

        case 11:  // 第11功能：让用户自己定义游戏空间的宽高，不过执行该操作之后，应该也有执行第3操作（重新定义所有球球的位置）
        {
            unsigned int width, height;
            cout << "请先输入剩余空白的宽度：";
            cin >> width;
            cout << "然后再输入剩余空白的高度：";
            cin >> height;
            StoneSiege(width, height);
            cout << "现在游戏已经重新定义了宽高，程序先回到start那里，您可以看下效果。。。" << endl;
            goto switchStart;
        }

        case 12: // 第12功能：重新回到程序开始的地方（清空已有的思索成果，思索层变量置零，以及清空浇筑的红黑树）
        {
            for (size_t i = 0; i < DeepThinkLv; i++) {
                for (auto curItem = DeepThinkListAry[i].begin(); curItem != DeepThinkListAry[i].end(); ++curItem) {
                    delete *curItem;
                }
                DeepThinkListAry[i].clear();
            }
            DeepThinkLv = 0;
            T.Destory(T._root);         // 请注意，调用此函数销毁T时，在里面把红黑树T的根节点给nullptr的，所以下面应该重新初始化红黑树根节点，不然后面没法使用！
            T._nil = T.GenerateLeaf();  // 以下这3句代码其实就是RBTree的类构造函数里面的代码
            T._root = T._nil;
            T._nil->_color = BLACK;
            goto programStart;
            break;
        }

        case 99:  // 最后功能: 告诉用户从初始局面开始 最多能走多少步, 就把所有可能的变化局面都演变完了
            if (0 < DeepThinkLv)
            {
                int lv = 0;
                for (lv = 0; lv < DeepThinkLv; lv++)
                {
                    if (DeepThinkListAry[lv].size() == 0)
                    {
                        cout << "开始的局面最多能演变成" << lv-1 << "层" << endl;
                        goto nineEnd;
                    }
                }
            }
            else cout << "程序还没有思考呢, 所以现在回到开始, 请重新操作" << endl;
            cout << "抱歉, 您让程序思考的层数太浅了, 并不能确定初始局面能演变多少层!" << endl;
        nineEnd:
            system("pause");
            cout << "现在按任意键让程序回到switch循环开始的地方" << endl;
            goto switchStart;
            break;


        default:
            cout << "游戏无处处理您的输入, 现在程序退出" << endl;
            exit(0);
            break;
        }
    }
    cout << "程序至此已经完全执行完毕, 现在准备退出" << endl;
    return 0;
}