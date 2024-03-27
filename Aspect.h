#pragma once
#include <iostream>
class StateAry
{
public:
	static StateAry* StartNode;
private:
	StateAry* _leftPoint;
	StateAry* _rightPoint;
	long long unsigned int _num[4];

public:
	StateAry(long long unsigned int ary[]) {
		for (int i = 0; i < 4; i++) _num[i] = ary[i];
		_leftPoint = _rightPoint = nullptr;
	};

	int Compare(long long unsigned int ary[4])
	{
		for (int i = 3; i >= 0; i--) {
			if (_num[i] < ary[i]) return 1;
			else if (_num[i] > ary[i]) return -1;
		}
		return 0;
	}

	static bool InsertNode(long long unsigned int newAry[4]) {
		StateAry* tempNode = StartNode;
		while (1) {
			switch (tempNode->Compare(newAry))
			{
			case 1:
				if (tempNode->_rightPoint) tempNode = tempNode->_rightPoint;
				else 
				{
					tempNode->_rightPoint = new StateAry(newAry);
					return true;
				}
				break;

			case 0:
				return false;
				break;

			case -1:
				if (tempNode->_leftPoint) tempNode = tempNode->_leftPoint;
				else 
				{
					tempNode->_leftPoint = new StateAry(newAry);
					return true;
				}
			}
		}
	}

	void test(StateAry& other) {
		std::cout << other._num[0] << std::endl;
	}
};

