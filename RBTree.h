#pragma once
#include <iostream>
#include <string>
using namespace std;
#define STEPLEN 16
typedef enum { RED = 0, BLACK } Color;

class State
{
public:
    long long unsigned int _a, _b, _c, _d;
    State() { _a = 0; _b = 0; _c = 0; _d = 0; }
    State(long long unsigned int a, long long unsigned int b, long long unsigned int c, long long unsigned int d) : _a(a), _b(b), _c(c), _d(d) {}
};

int Compare(const State& l, uint64_t* rp)
{
    if (l._a > rp[0]) return -1;     // 返回-1表示左边的关键值大一点
    else if (l._a < rp[0]) return 1; // 右边关键值大

    if (l._b > rp[1]) return -1;
    else if (l._b < rp[1]) return 1;

    if (l._c > rp[2]) return -1;
    else if (l._c < rp[2]) return 1;

    if (l._d > rp[3]) return -1;
    else if (l._d < rp[3]) return 1;

    return 0; // 左右两个关键值一摸一样
}
int Compare(const State& l, const State& r) // 请注意:所有参加这种红黑树节点的关键值类型, 都必须定义一个这种对应的同名双参函数(红黑树算法里面要用到)
{
    if (l._a > r._a) return -1;     // 返回-1表示左边的关键值大一点
    else if (l._a < r._a) return 1; // 右边关键值大

    if (l._b > r._b) return -1;
    else if (l._b < r._b) return 1;

    if (l._c > r._c) return -1;
    else if (l._c < r._c) return 1;

    if (l._d > r._d) return -1;
    else if (l._d < r._d) return 1;

    return 0; // 左右两个关键值一摸一样
}
int Compare(int l, int r) // 同上, 普通的int类型重写了大于小于等于的算法
{
    if (l < r) return 1;
    else if (l > r) return -1;
    else return 0;
}


string ConversionStr(const State& val)  // 请注意:所有参加这种红黑树节点的关键值类型, 都必须定义一个转换为字符串的函数(红黑树算法里面要用到)
{
    string str = std::to_string(val._a);
    str += std::to_string(val._b);
    str += std::to_string(val._c);
    str += std::to_string(val._d);
    return str;
}
string ConversionStr(const int val) // 同上
{
    return std::to_string(val);
}

// 红黑树每个节点的定义
template <typename T>
struct RBNode
{
    Color _color;    // 节点颜色
    T _val;          // 节点关键值
    RBNode* _left;   // 节点的左下枝指针
    RBNode* _right;  // 右下枝指针
    RBNode* _parent; // 父节点指针
};

// 红黑树
template <typename T>
class RBTree
{
public:
    RBNode<T>* _root;       // 红黑树的树根之顶的节点
    RBNode<T>* _nil;        // 一个末端红叶子的模板节点
    int _pyramidHeight = 0; // 这颗红黑树最多有多少层(主要用于打印红黑树外观用的)

    // 下面是所有函数的定义
    RBTree() {
        _nil = GenerateLeaf();
        _root = _nil;
        _nil->_color = BLACK;
    }
    ~RBTree() {
        Destory(_root);
        delete _nil;
        _nil = nullptr;
    }

    bool Insert(uint64_t* binaryArySize256)
    {
        RBNode<T>* p = _nil;
        RBNode<T>* s = _root;

        while (s != _nil) {
            p = s;
            switch (Compare(s->_val, binaryArySize256))
            {
            case 1:             // 若右边的关键值比较大
                s = s->_right;
                break;
            case 0:             // 若两边的关键值相等
                return false;
                break;
            case -1:           // 若左边的关键值比较大
                s = s->_left;
                break;
            }
        }

        {
            s = new RBNode<T>();
            s->_color = RED;
            s->_left = s->_right = s->_parent = _nil;
            s->_val._a = binaryArySize256[0];
            s->_val._b = binaryArySize256[1];
            s->_val._c = binaryArySize256[2];
            s->_val._d = binaryArySize256[3];
            
        }// 至此, 肯定要生成一个末端红色叶子节点
        if (p == _nil)
        {
            _root = s;
            _root->_parent = p;
        }
        else
        {
            if (-1 == Compare(p->_val, binaryArySize256)) p->_left = s;
            else p->_right = s;

            s->_parent = p;
        }

        InsertFixup(s);
        return true;
    }

    bool Insert(const T& value)
    {
        RBNode<T>* p = _nil;
        RBNode<T>* s = _root;

        while (s != _nil) {
            p = s;
            switch (Compare(s->_val, value))
            {
            case 1:             // 若右边的关键值比较大
                s = s->_right;
                break;
            case 0:             // 若两边的关键值相等
                return false;
                break;
            case -1:           // 若左边的关键值比较大
                s = s->_left;
                break;
            }
        }

        s = GenerateLeaf(value); // 至此, 肯定要生成一个末端红色叶子节点
        if (p == _nil)
        {
            _root = s;
            _root->_parent = p;
        }
        else
        {
            if (-1 == Compare(p->_val, value)) p->_left = s;
            else p->_right = s;

            s->_parent = p;
        }

        InsertFixup(s);
        return true;
    }

    void Remove(T key)  // 根据关键值删除树中的某个节点
    {
        RBNode<T>* t;
        if ((t = Search(_root, key)) != _nil) Remove(t);
        else std::cout << "Key is not exist!" << std::endl;
    }


    RBNode<T>* GenerateLeaf(const T& x = T()) // 在末端生成叶子尾巴(红色节点)
    {
        RBNode<T>* s = new RBNode<T>();
        s->_color = RED;
        s->_left = s->_right = s->_parent = _nil;
        s->_val = x;
        return s;
    }

    void LeftRotate(RBNode<T>* z) // 调整树型的左旋
    {
        RBNode<T>* y = z->_right;
        z->_right = y->_left;
        if (y->_left != _nil) y->_left->_parent = z;
        y->_parent = z->_parent;

        if (_root == z) _root = y;
        else if (z == z->_parent->_left) z->_parent->_left = y;
        else z->_parent->_right = y;

        y->_left = z;
        z->_parent = y;
    }

    void RightRotate(RBNode<T>* z)  // 右旋
    {
        RBNode<T>* y = z->_left;
        z->_left = y->_right;
        if (y->_right != _nil) y->_right->_parent = z;
        y->_parent = z->_parent;

        if (_root == z) _root = y;
        else if (z == z->_parent->_left) z->_parent->_left = y;
        else z->_parent->_right = y;

        y->_right = z;
        z->_parent = y;
    }


    void InsertFixup(RBNode<T>* s)
    {
        RBNode<T>* uncle;
        while (s->_parent->_color == RED)
        {
            if (s->_parent == s->_parent->_parent->_left)
            {
                uncle = s->_parent->_parent->_right;
                if (RED == uncle->_color)
                {
                    s->_parent->_color = BLACK;
                    uncle->_color = BLACK;
                    s->_parent->_parent->_color = RED;
                    s = s->_parent->_parent;
                }
                else
                {
                    if (s == s->_parent->_right)
                    {
                        s = s->_parent;
                        LeftRotate(s);
                    }
                    s->_parent->_color = BLACK;
                    s->_parent->_parent->_color = RED;
                    RightRotate(s->_parent->_parent);
                }
            }
            else
            {
                if (s->_parent == s->_parent->_parent->_right)
                {
                    uncle = s->_parent->_parent->_left;
                    if (RED == uncle->_color)
                    {
                        s->_parent->_color = BLACK;
                        uncle->_color = BLACK;
                        s->_parent->_parent->_color = RED;
                        s = s->_parent->_parent;
                    }
                    else
                    {
                        if (s == s->_parent->_left)
                        {
                            s = s->_parent;
                            RightRotate(s);
                        }
                        s->_parent->_color = BLACK;
                        s->_parent->_parent->_color = RED;
                        LeftRotate(s->_parent->_parent);
                    }
                }
            }
        }
        _root->_color = BLACK;
    }

    RBNode<T>* Search(RBNode<T>* root, T key) const
    {
        if (root == _nil) return _nil;
        switch (Compare(root->_val, key))
        {
        case 1:
            return Search(root->_right, key);
            break;
        case 0:
            return root;
            break;
        case -1:
            return Search(root->_left, key);
            break;
        }
    }

    void Transplant(RBNode<T>* u, RBNode<T>* v)
    {
        if (u->_parent == _nil) _root = v;
        else if (u == u->_parent->_left) u->_parent->_left = v;
        else u->_parent->_right = v;
        v->_parent = u->_parent;
    }

    RBNode<T>* GetMinNode(RBNode<T>* x)
    {
        if (x->_left == _nil) return x;
        return GetMinNode(x->_left);
    }

    void Remove(RBNode<T>* z)
    {
        RBNode<T>* x = _nil;
        RBNode<T>* y = z;
        Color ycolor = y->_color;

        if (z->_left == _nil)
        {
            x = z->_right;
            Transplant(z, z->_right);
        }
        else if (z->_right == _nil)
        {
            x = z->_left;
            Transplant(z, z->_left);
        }
        else
        {
            y = GetMinNode(z->_right);
            ycolor = y->_color;
            x = y->_right;
            if (y->_parent == z) x->_parent = y;
            else
            {
                Transplant(y, y->_right);
                y->_right = z->_right;
                y->_right->_parent = y;
            }
            Transplant(z, y);
            y->_left = z->_left;
            z->_left->_parent = y;
            y->_color = z->_color;
        }

        if (ycolor == BLACK) RemoveFixup(x);
    }

    void RemoveFixup(RBNode<T>* x)
    {
        while (x != _root && x->_color == BLACK)
        {
            if (x == x->_parent->_left)
            {
                RBNode<T>* w = x->_parent->_right;
                if (RED == w->_color)
                {
                    w->_color = BLACK;
                    x->_parent->_color = RED;
                    LeftRotate(x->_parent);
                    w = x->_parent->_right;
                }
                if (w->_left->_color == BLACK && w->_right->_color == BLACK)
                {
                    w->_color = RED;
                    x = x->_parent;
                }
                else
                {
                    if (w->_right->_color == BLACK)
                    {
                        w->_color = RED;
                        w->_left->_color = BLACK;
                        RightRotate(w);
                        w = x->_parent->_right;
                    }

                    w->_color = w->_parent->_color;
                    x->_parent->_color = BLACK;
                    w->_right.color = BLACK;
                    LeftRotate(x->_parent);
                    x = _root;
                }
            }
            else
            {
                RBNode<T>* w = x->_parent->_left;
                if (RED == w->_color)
                {
                    w->_parent->_color = RED;
                    w->_color = BLACK;
                    RightRotate(x->_parent);
                    w = x->_parent->_left;
                }
                if (w->_right->_color == BLACK && w->_left->_color == BLACK)
                {
                    w->_color = RED;
                    x = x->_parent;
                }
                else
                {
                    if (w->_left->_color == BLACK)
                    {
                        w->_right->_color = BLACK;
                        w->_color = RED;
                        LeftRotate(w);
                        w = x->_parent->_left;
                    }

                    w->_color = x->_parent->_color;
                    x->_parent->_color = BLACK;
                    w->_left->_color = BLACK;
                    RightRotate(x->_parent);
                    x = _root;
                }
            }
        }
        x->_color = BLACK;
    }

    void Destory(RBNode<T>*& root)
    {
        if (root == _nil || root == nullptr) return;
        if (root->_left != _nil) 
            Destory(root->_left);
        if (root->_right != _nil)
            Destory(root->_right);
        delete root;
        root = nullptr;
    }

    // 更新红黑树外观
    void UpdatePyramid(RBNode<T>* node, int level, std::string strArr[], int pos = 48)
    {
        if (_root == node)
        {
            for (size_t i = 0; i < 100; i++)
                strArr[i] = "                                                                                                                             ";

            std::string tempStr = ConversionStr(_root->_val);
            tempStr += (BLACK == _root->_color) ? "b " : "r ";
            strArr[0].replace(48, tempStr.length(), tempStr);
        }

        int pyramidLv = level * 2;
        if (_pyramidHeight < pyramidLv) _pyramidHeight = pyramidLv;

        std::string temp = ConversionStr(node->_val);
        temp += (BLACK == node->_color) ? "b" : "r";
        strArr[pyramidLv].replace(pos, temp.length(), temp);

        if (_nil != node->_left)
        {
            int leftOffset = pos - STEPLEN / (1 + level);
            strArr[pyramidLv + 1].replace((pos + leftOffset) / 2, 1, "/");
            UpdatePyramid(node->_left, level + 1, strArr, leftOffset);
        }
        if (_nil != node->_right)
        {
            int rightOffset = pos + STEPLEN / (1 + level);
            strArr[pyramidLv + 1].replace((rightOffset + pos) / 2, 1, "\\");
            UpdatePyramid(node->_right, level + 1, strArr, rightOffset);
        }
    }

    void ShowRBTreePyramid(string strList[]) // 显示红黑树外观
    {
        for (size_t i = 0; i <= this->_pyramidHeight; i++) std::cout << strList[i] << std::endl;
    }
};