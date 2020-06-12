#ifndef BITREE_H
#define BITREE_H

#include <iostream>
#include <list>
#include <stack>
#include <queue>

using namespace std;

/*���������*/
template <typename T>
class BiTreeNode {
public:
    T data;
    BiTreeNode<T>* left;
    BiTreeNode<T>* right;
    BiTreeNode(T data) {
        this->data = data;
        this->left = nullptr;
        this->right = nullptr;
    }
};

template <typename T>
class BiTree {
private:
    int size;/*���н��ĸ���*/
public:
    BiTreeNode<T> *root;/*������ָ��*/

    BiTree() { size = 0; root = nullptr; };
    ~BiTree() { remove(root); };

    bool insert_left(BiTreeNode<T>* node, T data);/*��data���뵽node������*/
    bool insert_right(BiTreeNode<T>* node, T data);/*��data���뵽node���Һ���*/
    bool remove(BiTreeNode<T>* &node);/*ɾ����nodeΪ��������*/
    bool preorderR(BiTreeNode<T>* node, list<T>* l);/*�ݹ�ʵ�֣����������nodeΪ����������������������浽l��*/
    bool preorderI(BiTreeNode<T>* node, list<T>* l);/*����ʵ�֣����������nodeΪ����������������������浽l��*/
    bool inorderR(BiTreeNode<T>* node, list<T>* l);/*�ݹ�ʵ�֣����������nodeΪ����������������������浽l��*/
    bool inorderI(BiTreeNode<T>* node, list<T>* l);/*����ʵ�֣����������nodeΪ����������������������浽l��*/
    bool postorderR(BiTreeNode<T>* node, list<T>* l);/*�ݹ�ʵ�֣����������nodeΪ����������������������浽l��*/
    bool postorderI(BiTreeNode<T>* node, list<T>* l);/*����ʵ�֣����������nodeΪ����������������������浽l��*/
    bool level(BiTreeNode<T>* node, list<T>* l);/*��α�����nodeΪ����������������������浽l��*/
    int height(BiTreeNode<T>* node);/*��nodeΪ�������ĸ߶�*/
};

/**************************************************************************
Function:insert_left
Description:��data���뵽node������
Input:���������node,����data
Output:�����Ƿ�ɹ�
***************************************************************************/
template <typename T>
bool BiTree<T>::insert_left(BiTreeNode<T>* node, T data) {
    if (node == nullptr) {//�������������
        if (size > 0)
            return false;
        else
            root = new BiTreeNode<T>(data);
    }
    else {
        if (node->left != nullptr)
            return false;
        else
            node->left = new BiTreeNode<T>(data);
    }
    size++;
    return true;
}
/**************************************************************************
Function:insert_right
Description:��data���뵽node���Һ���
Input:���������node,����data
Output:�����Ƿ�ɹ�
***************************************************************************/
template <typename T>
bool BiTree<T>::insert_right(BiTreeNode<T>* node, T data) {
    if (node == nullptr) {
        if (size > 0)
            return false;
        else
            root = new BiTreeNode<T>(data);
    }
    else {
        if (node->right != nullptr)
            return false;
        else
            node->right = new BiTreeNode<T>(data);
    }
    size++;
    return true;
}

/**************************************************************************
Function:remove
Description:ɾ����nodeΪ��������
Input:���������node������
Output:ɾ���Ƿ�ɹ�
***************************************************************************/
template <typename T>
bool BiTree<T>::remove(BiTreeNode<T>* &node) {
    /*֮����Ҫ����ָ������ã�����Ϊ����Ҫ�ͷ�nodeָ��Ŀռ䣬
    ͬ��Ҳ��Ҫ��node�ڱ���ĵ�ַ����Ϊnullptr*/
    if (node == nullptr)
        return true;

    if (remove(node->left) && remove(node->right)) {
        delete node;//�ͷ�pָ��Ŀռ�
        node = nullptr;//���p�ڵĵ�ַ
        size--;
        return true;
    }
    else
        return false;
}

/**************************************************************************
Function:preorderR��preorderI
Description:���������nodeΪ����������������������浽l��
Input:���������node���б�l
Output:�����Ƿ�ɹ�
***************************************************************************/
template <typename T>
bool BiTree<T>::preorderR(BiTreeNode<T>* node, list<T>* l) {
    if (node == nullptr || l == nullptr)
        return false;
    l->push_back(node->data);
    preorderR(node->left, l);
    preorderR(node->right, l);
    return true;
}
template <typename T>
bool BiTree<T>::preorderI(BiTreeNode<T>* node, list<T>* l) {
    if (node == nullptr || l == nullptr)
        return false;
    stack<BiTreeNode<T>*> s;
    BiTreeNode<T>* p = node;
    while (p != nullptr || !s.empty()) {
        if (p != nullptr) {
            //ѹ��ջʱ���
            l->push_back(p->data);
            s.push(p);
            p = p->left;
        }
        else {
            p = s.top();
            s.pop();
            p = p->right;
        }
    }
    return true;
}
/**************************************************************************
Function:inorderR��inorderI
Description:���������nodeΪ����������������������浽l��
Input:���������node���б�l
Output:�����Ƿ�ɹ�
***************************************************************************/
template <typename T>
bool BiTree<T>::inorderR(BiTreeNode<T>* node, list<T>* l) {
    if (node == nullptr || l == nullptr)
        return false;
    inorderR(node->left, l);
    l->push_back(node->data);
    inorderR(node->right, l);
    return true;
}
template <typename T>
bool BiTree<T>::inorderI(BiTreeNode<T>* node, list<T>* l) {
    if (node == nullptr || l == nullptr)
        return false;
    stack<BiTreeNode<T>*> s;
    BiTreeNode<T>* p = node;
    while (!s.empty() || p != nullptr) {
        if (p != nullptr) {
            s.push(p);
            p = p->left;//��һ���÷��ʵĽ��
        }
        else {
            p = s.top();
            s.pop();//Ҫ���ʾ͵���
            l->push_back(p->data);//����ջʱ���
            p = p->right;//��һ���÷��ʵĽ��
        }
    }
    return true;
}
/**************************************************************************
Function:postorderR��postorderI
Description:���������nodeΪ����������������������浽l��
Input:���������node���б�l
Output:�����Ƿ�ɹ�
***************************************************************************/
template <typename T>
bool BiTree<T>::postorderR(BiTreeNode<T>* node, list<T>* l) {
    if (node == nullptr || l == nullptr)
        return false;
    postorderR(node->left, l);
    postorderR(node->right, l);
    l->push_back(node->data);
    return true;
}
template <typename T>
bool BiTree<T>::postorderI(BiTreeNode<T>* node, list<T>* l) {
    if (node == nullptr || l == nullptr)
        return false;
    stack<BiTreeNode<T>*> s;
    BiTreeNode<T>* p = node;
    BiTreeNode<T>* pre = nullptr;//���ڱ����ǰ���ʹ��Ľ��
    while (!s.empty() || p != nullptr) {
        if (p != nullptr) {
            s.push(p);
            p = p->left;
        }
        else {
            BiTreeNode<T>* right = s.top()->right;
            if (right != nullptr && right != pre) {
                p = right;
            }
            else {
                //��p==nullptr��right!=nullptr&&right!=preʱ��
                //˵��Ҫ����ջ�������
                pre = s.top();
                l->push_back(pre->data);
                s.pop();
            }
        }
    }
    return true;
}
/**************************************************************************
Function:level
Description:��α�����nodeΪ����������������������浽l��
Input:���������node���б�l
Output:�����Ƿ�ɹ�
***************************************************************************/
template <typename T>
bool BiTree<T>::level(BiTreeNode<T>* node, list<T>* l) {
    if (node == nullptr || l == nullptr)
        return false;
    queue<BiTreeNode<T>*> q;
    q.push(node);
    BiTreeNode<T>* p = nullptr;
    while (!q.empty()) {
        p = q.front();
        q.pop();
        l->push_back(p->data);
        if (p->left != nullptr)q.push(p->left);
        if (p->right != nullptr)q.push(p->right);
    }
    return true;
}

/**************************************************************************
Function:height
Description:��nodeΪ�������ĸ߶�
Input:���������node
Output:���ĸ߶�
***************************************************************************/
template <typename T>
int BiTree<T>::height(BiTreeNode<T>* node) {
    if (node == nullptr)
        return 0;
    int left_height = height(node->left);
    int right_height = height(node->right);
    return max(left_height, right_height) + 1;
}

#endif // !BITREE_H

