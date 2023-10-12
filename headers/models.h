#pragma once

// ������� ������ �������������� �����
struct Damb {
    unsigned short ch; // ������
    unsigned int n; // ���-�� �������� � �����
    unsigned short tri; // ��������� ������ �� ��������������� ������� � ������
};

// ������� ������ ��������
struct Tree {
    unsigned short ch = 0; // ������ (256 - ������ ����� �����)
    short left = -1; // ����� ���
    short right = -1; // ������ ���
    short parent = -1; // ��������
};

// ������� ������ ����� ��������
struct Code {
    unsigned short code = 0; // ��� (������������)
    unsigned short length = 0; // ��� �������� �����
};

const size_t SYMBOLS_COUNT = 257;
const size_t TREE_SIZE = 514;

const short END_OF_FILE = 256;