#pragma once
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#pragma warning(disable : 4996)
#pragma warning(disable : 6031)
#pragma warning(disable : 6295)
#include <stdio.h>
#include <iostream>
#include <cstdio>
#include <vector>
#include <stack>
#include "models.h"
#include "reader.h"
#include "writer.h"

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

//void read_short(FILE* inf, short& n) {
//    uchar ch;
//    fscanf(inf, "%c", &ch);
//    n = ch;
//    fscanf(inf, "%c", &ch);
//    n += (short)ch << 8;
//}
//
//void read_ushort(FILE* inf, ushort& n) {
//    uchar ch;
//    fscanf(inf, "%c", &ch);
//    n = ch;
//    fscanf(inf, "%c", &ch);
//    n += (ushort)ch << 8;
//}

void unzip(const char* ziped_filename, const char* unziped_filename) {
    // ��������� ����� � ���������� ������
    //FILE* inf = fopen(ziped_filename, "rb");
    //FILE* outf = fopen(unziped_filename, "wb");

    FileReader reader(ziped_filename);
    FileWriter writer(unziped_filename);

    // ��������� ������ ������ (2 �����)
    //ushort n;
    //read_ushort(inf, n);

    //cout << "Tree size = " << n << endl;

    // ������� � ��������� ������
    //vector<Tree> tree(TREE_SIZE);
    //for (ushort i = 0; i < n; ++i) {
    //    uchar ch;
    //    fscanf(inf, "%c", &ch);
    //    short left, right;
    //    read_short(inf, left);
    //    read_short(inf, right);

    //    tree[i].ch = ch;
    //    tree[i].left = left;
    //    tree[i].right = right;

    //    if (left >= 0 && right >= 0) {
    //        tree[left].parent = i;
    //        tree[right].parent = i;
    //    }

    //    // ��������� ������ ����� ����������� ��� char
    //    // ��� �����, ���� ��������� ������������, ���������� 
    //    /*if (tree[i].left < -1) {
    //        tree[i].left += 256;
    //    }
    //    if (tree[i].right < -1) {
    //        tree[i].right += 256;
    //    }
    //    if (tree[i].parent < -1) {
    //        tree[i].parent += 256;
    //    }*/
    //}

    std::vector<Tree> tree(TREE_SIZE);
    std::stack<ushort> st;
    ushort tree_size = 1;
    st.push(0);

    // 0 - ���� ��� �������, 1 - ���� � ��������
    // �����, ���� ���� � ��������,
    // 0 - ������� ������, 1 - ������ ����� �����
    // �����, ���� ������� ������, �� ����� ���� ��������� ������
    while (!st.empty()) {
        ushort cur = st.top();
        st.pop();

        uchar ch;
        reader.read_bites(ch);

        if (ch == 1) {
            reader.read_bites(ch);

            if (ch == 1) {
                tree[cur].ch = END_OF_FILE;
            }
            else {
                reader.read_bites(ch, 8);
                tree[cur].ch = ch;
            }
        }
        else {
            tree[cur].left = tree_size++;
            tree[cur].right = tree_size++;
            tree[tree[cur].left].parent = cur;
            tree[tree[cur].right].parent = cur;

            st.push(tree[cur].right);
            st.push(tree[cur].left);
        }
    }

    // ��������� ����� ���� � ���������� ��� ����
    uchar ch;
    ushort cur = 0; // ������ �������� ���� ������ (�������� � ����� ������)
    while (reader.read_bites(ch, 1)) {
        // ������� �� ������ ���
        // ���� �� ����� 1, �� ���� � ������� ����
        if (ch == 1) {
            cur = tree[cur].right;
        }
        // ����� ���� � ������ ����
        else {
            cur = tree[cur].left;
        }
        // ���� � ������� ���� - ���� (� ���� ��� �����)
        if (tree[cur].left == -1 && tree[cur].right == -1) {
            // ���� ���� ������ �������� �������� ����� ����� (��� ��� == END_OF_FILE),
            // �� �� ������� ���, � ��������� ������ 
            if (tree[cur].ch == END_OF_FILE) {
                break;
            }
            // ����� ������� ������, ������� � ������� ����
            writer.write_bites(tree[cur].ch, 8);
            // ������������ � ������ ������
            cur = 0;
        }
    }
}
