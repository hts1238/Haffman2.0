#pragma once

// Элемент списка необработанных узлов
struct Damb {
    unsigned short ch; // Символ
    unsigned int n; // Кол-во символов в файле
    unsigned short tri; // Индексная ссылка на соответствующий элемент в дереве
};

// Элемент дерева Хаффмана
struct Tree {
    unsigned short ch = 0; // Символ (256 - символ конца файла)
    short left = -1; // Левый сын
    short right = -1; // Правый сын
    short parent = -1; // Родитель
};

// Элемент списка кодов символов
struct Code {
    unsigned short code = 0; // Код (перевернутый)
    unsigned short length = 0; // Его полезная длина
};

const size_t SYMBOLS_COUNT = 257;
const size_t TREE_SIZE = 514;

const short END_OF_FILE = 256;