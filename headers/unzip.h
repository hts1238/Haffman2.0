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

void unzip(const char* ziped_filename, const char* unziped_filename) {
    FileReader reader(ziped_filename);
    FileWriter writer(unziped_filename);

    std::vector<Tree> tree(TREE_SIZE);
    std::stack<ushort> st;
    ushort tree_size = 1;
    st.push(0);

    // 0 - узел без символа, 1 - узел с символом
    // Далее, если узел с символом,
    // 0 - простой символ, 1 - символ конца файла
    // Далее, если простой символ, то целый байт описывает символ
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

    // Считываем целый байт и перебираем его биты
    uchar ch;
    ushort cur = 0; // Индекс текущего узла дерева (начинаем с корня дерева)
    while (reader.read_bites(ch, 1)) {
        // Смотрим на нужный бит
        // Если он равен 1, то идем в правого сына
        if (ch == 1) {
            cur = tree[cur].right;
        }
        // Иначе идем в левого сына
        else {
            cur = tree[cur].left;
        }
        // Если в текущем узле - лист (у него нет детей)
        if (tree[cur].left == -1 && tree[cur].right == -1) {
            // Если этот символ является символом конца файла (его код == END_OF_FILE),
            // то не выводим его, а завершаем работу 
            if (tree[cur].ch == END_OF_FILE) {
                break;
            }
            // Иначе выводим символ, который в текущем узле
            writer.write_bites(tree[cur].ch, 8);
            // Возвращаемся в корень дерева
            cur = 0;
        }
    }

    reader.~FileReader();
    writer.~FileWriter();
}
