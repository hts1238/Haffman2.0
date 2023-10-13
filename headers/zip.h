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

    // Считаем кол-во вхожднений каждого символа
    while (reader.read_bites(ch, 8)) {
        ++numc[ch];
    }
    
    // Добавляем символ конца файла
    numc[END_OF_FILE] = 1;

    // Список необработанных узлов
    std::vector<Damb> damb(SYMBOLS_COUNT);
    // Дерево Хаффмана
    std::vector<Tree> tree(TREE_SIZE);
    // Текущее кол-во элементов в списке damb
    ushort l = 0;
    // Перебираем все символы
    for (ushort i = 0; i < SYMBOLS_COUNT; ++i) {
        // Если мы встречали этот (i) символ, добавляем его в damb
        if (numc[i] > 0) {
            damb[l].ch = i;
            damb[l].n = numc[i];
            damb[l].tri = l;
            tree[l].ch = i;
            ++l;
        }
    }

    ushort curl = l; // Текущее кол-во элементов в списке damb
    ushort curi = l; // Текущее кол-во узлов в дереве
    // Пока размер damb > 1
    for (ushort i = 0; i < l - 1; ++i) {
        // Ищем два минимума
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

        // Добавляем новый элемент в дерево и указываем детей
        tree[curi].ch = 0;
        tree[curi].left = min1.tri;
        tree[curi].right = min2.tri;

        // Детям нового элемента указываем их нового родителя
        tree[min1.tri].parent = curi;
        tree[min2.tri].parent = curi;

        // На место первого минимума ставим новый элемент (сумма двух минимумов)
        damb[i1].ch = 0;
        damb[i1].n = min1.n + min2.n;
        damb[i1].tri = curi;

        // Меняем местами второй минимум и последний элемент списка damb
        std::swap(damb[i2], damb[curl - 1]);

        // Размер списка damb уменьшился на 1
        --curl;
        // Размер дерева увеличился на 1
        ++curi;
    }

    // Составляем коды каждого символа
    // Здесь и далее l - кол-во различных символов в исходном файле
    std::vector<Code> codes(SYMBOLS_COUNT);
    for (ushort i = 0; i < l; ++i) {
        ushort& code = codes[tree[i].ch].code; // Переменная, в которой собираем код символа
        ushort& len = codes[tree[i].ch].length; // Здесь храним его полезную длину
        short p = tree[i].parent; // Индекс родителя текущего узла дерева
        short cur = i; // Индекс текущего узла дерева
        // Пока мы не в корне дерева
        while (p != -1) {
            // Двигаем код
            code <<= 1;
            // Если текущий узел является правым сыном его родителя, то пишем 1
            if (tree[p].right == cur) {
                ++code;
            }
            else {
                // Иначе оставлем 0 (ничего не делаем)
            }
            // Увеличиваем полезную длину кода
            ++len;

            // Поднимаемся по дереву к следующему узлу
            cur = p;
            p = tree[p].parent;
        }
    }

    // Выводим дерево в файл в порядке обхода в глубину
    // 0 - узел без символа, 1 - узел с символом
    // Далее, если узел с символом,
    // 0 - простой символ, 1 - символ конца файла
    // Далее, если простой символ, то целый байт описывает символ
    std::stack<ushort> st;
    st.push(curi - 1);
    while (!st.empty()) {
        ushort cur = st.top();
        st.pop();

        // Если текущий узел имеет детей, то он без символа (выводим 0)
        if (tree[cur].left != -1 && tree[cur].right != -1) {
            writer.write_bites(0);
            st.push(tree[cur].right);
            st.push(tree[cur].left);
            continue;
        }
        // Иначе он с символом (выводим 1)
        writer.write_bites(1);
        // Если символ конца файла, выводим 1
        if (tree[cur].ch == END_OF_FILE) {
            writer.write_bites(1);
        }
        // Иначе обычный символ (выводим 0 и далее сам символ)
        else {
            writer.write_bites(0);
            writer.write_bites(tree[cur].ch, 8);
        }
    }

    // Закрываем входной файл, чтобы открыть и пройтись по нему заново
    reader.reopen_file();

    //uchar buffer = 0; // Буфер
    //uchar k = 0; // Полезный размер буфера
    while (reader.read_bites(ch, 8)) {
        ushort code = codes[ch].code;
        ushort len = codes[ch].length;

        writer.write_bites(code, len, true);
    }

    // Добавляем символ конца файла
    ushort code = codes[END_OF_FILE].code;
    ushort len = codes[END_OF_FILE].length;

    writer.write_bites(code, len, true);

    reader.~FileReader();
    writer.~FileWriter();
}
