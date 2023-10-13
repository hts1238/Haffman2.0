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

void zip(const char* original_filename, const char* ziped_filename) {
    FileReader reader(original_filename);
    FileWriter writer(ziped_filename);
	
    std::vector<uint> numc(SYMBOLS_COUNT);
    uchar ch;

    // ������� ���-�� ���������� ������� �������
    while (reader.read_bites(ch, 8)) {
        ++numc[ch];
    }
    
    // ��������� ������ ����� �����
    numc[END_OF_FILE] = 1;

    // ������ �������������� �����
    std::vector<Damb> damb(SYMBOLS_COUNT);
    // ������ ��������
    std::vector<Tree> tree(TREE_SIZE);
    // ������� ���-�� ��������� � ������ damb
    ushort l = 0;
    // ���������� ��� �������
    for (ushort i = 0; i < SYMBOLS_COUNT; ++i) {
        // ���� �� ��������� ���� (i) ������, ��������� ��� � damb
        if (numc[i] > 0) {
            damb[l].ch = i;
            damb[l].n = numc[i];
            damb[l].tri = l;
            tree[l].ch = i;
            ++l;
        }
    }

    ushort curl = l; // ������� ���-�� ��������� � ������ damb
    ushort curi = l; // ������� ���-�� ����� � ������
    // ���� ������ damb > 1
    for (ushort i = 0; i < l - 1; ++i) {
        // ���� ��� ��������
        Damb min1 = damb[0];
        Damb min2 = damb[1];
        ushort i1 = 0;
        ushort i2 = 1;

        if (min1.n > min2.n) {
            std::swap(min2, min1);
            std::swap(i1, i2);
        }

        for (int j = 2; j < curl; ++j) {
            if (damb[j].n < min1.n) {
                min2 = min1;
                i2 = i1;
                min1 = damb[j];
                i1 = j;
            }
            else if (damb[j].n < min2.n) {
                min2 = damb[j];
                i2 = j;
            }
        }

        // ��������� ����� ������� � ������ � ��������� �����
        tree[curi].ch = 0;
        tree[curi].left = min1.tri;
        tree[curi].right = min2.tri;

        // ����� ������ �������� ��������� �� ������ ��������
        tree[min1.tri].parent = curi;
        tree[min2.tri].parent = curi;

        // �� ����� ������� �������� ������ ����� ������� (����� ���� ���������)
        damb[i1].ch = 0;
        damb[i1].n = min1.n + min2.n;
        damb[i1].tri = curi;

        // ������ ������� ������ ������� � ��������� ������� ������ damb
        std::swap(damb[i2], damb[curl - 1]);

        // ������ ������ damb ���������� �� 1
        --curl;
        // ������ ������ ���������� �� 1
        ++curi;
    }

    // ���������� ���� ������� �������
    // ����� � ����� l - ���-�� ��������� �������� � �������� �����
    std::vector<Code> codes(SYMBOLS_COUNT);
    for (ushort i = 0; i < l; ++i) {
        ushort& code = codes[tree[i].ch].code; // ����������, � ������� �������� ��� �������
        ushort& len = codes[tree[i].ch].length; // ����� ������ ��� �������� �����
        short p = tree[i].parent; // ������ �������� �������� ���� ������
        short cur = i; // ������ �������� ���� ������
        // ���� �� �� � ����� ������
        while (p != -1) {
            // ������� ���
            code <<= 1;
            // ���� ������� ���� �������� ������ ����� ��� ��������, �� ����� 1
            if (tree[p].right == cur) {
                ++code;
            }
            else {
                // ����� �������� 0 (������ �� ������)
            }
            // ����������� �������� ����� ����
            ++len;

            // ����������� �� ������ � ���������� ����
            cur = p;
            p = tree[p].parent;
        }
    }

    // ������� ������ � ���� � ������� ������ � �������
    // 0 - ���� ��� �������, 1 - ���� � ��������
    // �����, ���� ���� � ��������,
    // 0 - ������� ������, 1 - ������ ����� �����
    // �����, ���� ������� ������, �� ����� ���� ��������� ������
    std::stack<ushort> st;
    st.push(curi - 1);
    while (!st.empty()) {
        ushort cur = st.top();
        st.pop();

        // ���� ������� ���� ����� �����, �� �� ��� ������� (������� 0)
        if (tree[cur].left != -1 && tree[cur].right != -1) {
            writer.write_bites(0);
            st.push(tree[cur].right);
            st.push(tree[cur].left);
            continue;
        }
        // ����� �� � �������� (������� 1)
        writer.write_bites(1);
        // ���� ������ ����� �����, ������� 1
        if (tree[cur].ch == END_OF_FILE) {
            writer.write_bites(1);
        }
        // ����� ������� ������ (������� 0 � ����� ��� ������)
        else {
            writer.write_bites(0);
            writer.write_bites(tree[cur].ch, 8);
        }
    }

    // ��������� ������� ����, ����� ������� � �������� �� ���� ������
    reader.reopen_file();

    //uchar buffer = 0; // �����
    //uchar k = 0; // �������� ������ ������
    while (reader.read_bites(ch, 8)) {
        ushort code = codes[ch].code;
        ushort len = codes[ch].length;

        writer.write_bites(code, len, true);
    }

    // ��������� ������ ����� �����
    ushort code = codes[END_OF_FILE].code;
    ushort len = codes[END_OF_FILE].length;

    writer.write_bites(code, len, true);

    reader.~FileReader();
    writer.~FileWriter();
}
