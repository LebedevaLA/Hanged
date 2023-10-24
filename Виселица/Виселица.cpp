// Виселица.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//


#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <locale.h>
#include <stdio.h> 
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include<string>
#include <vector>
#include<Windows.h>
using namespace std;

class Hangman
{
private:

    ifstream fp1;//файл с зашифрованными словами
    vector <bool> done;//список разгаданных слов
    const int count = 6;//количество попыток
    int ost;//осталось попыток
    double timer;//время на угадывание
    vector<char> word;//искомое слово 
    int index;//индекс загаданного слова в списке
    vector <char> categoria;//категория искомого слова
    vector<vector<char>> wordlist;//зашифрованные слова из файла в двумерный массив (вектор векторов) 
    int colword;//количество слов в одной категории в зашифр. файле
    int colcat;//количество категорий
    vector<char> letters;//буквы которые выбирал пользователь
    vector<char>pos;//текущая позиция
    vector <char> Deshifr(vector<char> a)//функция дишифратор
    {
        int sdvig = 3;
        vector<char> result;
        for (int i = 0; i < a.size(); i++)
        {
            char ABC[32] = { 'А','Б','В','Г','Д','Е','Ж','З','И',
                    'Й','К','Л','М','Н','О','П','Р','С',
                    'Т','У','Ф','Х','Ц','Ч','Ш','Щ','Ъ',
                    'Ы','Ь','Э','Ю','Я' };
            char abc[32] = { 'а','б','в','г','д','е','ж','з','и',
                    'й','к','л','м','н','о','п','р','с',
                    'т','у','ф','х','ц','ч','ш','щ','ъ',
                    'ы','ь','э','ю','я' };
            if (a[i] == ' ') result.push_back(' ');
            else
            {
                for (int j = 0; j < 32; j++)
                {
                    if (a[i] == ABC[j])
                    {
                        if (j <= 2)
                        {
                            if (j == 2) result.push_back(ABC[31]);
                            if (j == 1) result.push_back(ABC[30]);
                            if (j == 0) result.push_back(ABC[29]);
                        }
                        else result.push_back(ABC[j - sdvig]);
                    }
                    if (a[i] == abc[j])
                    {
                        if (j <= 2)
                        {
                            if (j == 2) result.push_back(abc[31]);
                            if (j == 1) result.push_back(abc[30]);
                            if (j == 0) result.push_back(abc[29]);
                        }
                        else result.push_back(abc[j - sdvig]);
                    }
                }
            }
        }
        return(result);
    };
public:
    Hangman()
    {
        char ch;
        fp1.open("Words.txt");
        if (fp1.is_open())
        {
            vector<char> wordtmp;
            while (fp1.get(ch))
            {
                if (ch != '\n')
                {
                    wordtmp.push_back(ch);//записываем слово
                }
                else
                {
                    wordlist.push_back(wordtmp);//записываем слово в вектор векторов
                    wordtmp.clear();//очищаем вектор для записи новых слов
                    vector<char>().swap(wordtmp);//очищаем память для вектора-слова
                }
            }
        }
        else
        {
            cout << "Not found";
            return;
        }
        colword = 0;
        colcat = 0;
        for (int i = 0; i < wordlist[0].size(); i++)// в первом векторе-количество слов в категории
        {
            colword += (wordlist[0][i] - '0') * pow(10, wordlist[0].size() - 1 - i);
        }//запомнили сколько слов в одной категории
        wordlist.erase(wordlist.begin());//убрали из списка количество слов
        for (int i = 0; i < wordlist[0].size(); i++)// в первом векторе-количество слов в категории
        {
            colcat += (wordlist[0][i] - '0') * pow(10, wordlist[0].size() - 1 - i);
        }//запомнили сколько категорий
        wordlist.erase(wordlist.begin());//убрали количество категорий
        fp1.close();
        done.resize(wordlist.size());//по количеству слов в списке включая категориии, потому что надо сохранить индексацию
        for (int i = 0; i < done.size(); i++)
        {
            if (i == 0 || i % (colword + 1) == 0) done[i] = 1;//это категория
            else (done[i] = 0);//пока ни одного слова не отгадали
        }
        timer = 0.0;
        for (;;)
        {
            int v;
            cout << "Играем ?" << endl << "1-да" << endl << "2-нет" << endl;
            do
            {
                cin >> v;
            } while (v != 1 && v != 2);
            switch (v)
            {
            case 1:
                word.clear();//чистим вектор-слово, чтобы загадать новое
                vector<char>().swap(word);
                letters.clear();//чистим вектор-буквы
                vector<char>().swap(letters);
                pos.clear();//чистим вектор-позицию
                vector<char>().swap(pos);
                Choose_Word();//заново выбрали слово
                Define_Category();//заново определили категорию
                Game();
                break;
            case 2:
                int count = 0;
                for (int i = 0; i < done.size(); i++)
                {
                    if (done[i] == 1)count++;
                }
                cout << "Всего вы отгадали " << count - colcat << " слов";
                return;
                break;
            }

        }
    };

    int Find_word(vector<bool> a)//рандомно выбрать из тех слов, что не отгадали
    {
        vector <int> tmp;
        for (int i = 0; i < a.size(); i++)
        {
            if (a[i] == 0) tmp.push_back(i);//запомнить индексы неотгаданных
        }
        int random_index;
        random_index = rand() % tmp.size();//рандомно выбрать ячейку из tmp
        int word_index = tmp[random_index];//вернуть индекс неотгаданного слова, которое будем загадывать
        tmp.clear();//не очень шарю как работает векторы, поэтому удалю памать на всякий
        vector<int>().swap(tmp);
        return(word_index);
    };


    void Choose_Word()
    {
        int counter = 0;//счётчик отгаданных слов
        for (int i = 0; i < done.size(); i++)
        {
            if (done[i] == 1)
            {
                counter++;
            }
            else break;
        }
        if (counter == done.size())
        {
            cout << endl << "\t\t" << "Вы угадали все слова!!!" << endl;
            exit(0);//заканчиваем игру
        }
        index = Find_word(done);
        word = wordlist[index];//выбрали слово, закинули в поле класса
        word = Deshifr(word);//расшифруем слово
        pos.resize(word.size());
        for (int i = 0; i < pos.size(); i++)
        {
            (word[i] == ' ') ? pos[i] = ' ' : pos[i] = '_';
        }
    };

    void Define_Category()//определить категорию загаданного слова
    {
        int index_cat = 0;
        for (int i = index; i >= 0; i--)//идём от слова до начала списка, как только нашли индекс который делится на количество слов в категории +1 нашли индекс категории
        {
            if (i % (colword + 1) == 0)
            {
                index_cat = i;
                break;
            }
        }
        categoria = wordlist[index_cat];
    };

    void Print_category()
    {
        cout << endl;
        for (int i = 0; i < categoria.size(); i++)
        {
            cout << categoria[i];
        }
        cout << endl;
    };

    bool Correct_letter(char l)//пользователь корректно ввёл символ
    {
        char ABC[64] = { 'А','Б','В','Г','Д','Е','Ж','З','И',
                    'Й','К','Л','М','Н','О','П','Р','С',
                    'Т','У','Ф','Х','Ц','Ч','Ш','Щ','Ъ',
                    'Ы','Ь','Э','Ю','Я','а','б','в','г','д','е','ж','з','и',
                    'й','к','л','м','н','о','п','р','с',
                    'т','у','ф','х','ц','ч','ш','щ','ъ',
                    'ы','ь','э','ю','я' };
        for (int i = 0; i < 64; i++)
        {
            if (l == ABC[i]) return(1);
        }
        return 0;
    };
    bool Find_letter(char l)//поиск буквы
    {
        bool flag = 0;
        vector <char>tmp = pos;
        for (int i = 0; i < word.size(); i++)
        {
            int difference_let = 'а' - 'А';//разница в таблице аски между строчной и заглавной
            if (l == word[i] || l + difference_let == word[i] || l - difference_let == word[i])//даже если ввели заглавную нам надо открыть и строчные тоже
                // 3 варианта word[i] загалавная и l-заглавная сразу совпали, word [i] строчная l заглавная и наоборот
            {
                flag = 1;
                pos[i] = word[i];
            }
        }
        if (pos == tmp)//пользователь ввёл повторно две одинаковые буквы
        {
            flag = 0;
            tmp.clear();
            vector<char>().swap(tmp);
        }
        if (flag == 0) {
            cout << " Такой буквы в слове нет( " << endl;
            return false;
        }
        return true;

    };

    bool Win(vector<char> pos)//проверка на победу
    {
        for (int i = 0; i < pos.size(); i++)
        {
            if (pos[i] == '_')//если осталась хоть одна черта-не выиграли
            {
                Statistic();
                return false;
            }
        }
        done[index] = 1;
        Statistic();
        cout << "На отгадывание слова вы потратили " << timer << " секунд" << endl;
        return true;
    };
    void Statistic()//статистика для одного хода-использованные буквы
    {
        cout << "Вы использовали буквы ";
        for (int i = 0; i < letters.size(); i++)
        {
            cout << letters[i] << "  ";
        }
    };
    void Game()
    {
        clock_t begin = clock();
        Print_category();
        cout << endl;
        for (int i = 0; i < word.size(); i++)
        {
            if (word[i] != ' ')cout << "_ ";
            else cout << " ";
        }
        cout << endl << "У вас " << count << " попыток" << endl;
        ost = count;
        char ABC[32] = { 'А','Б','В','Г','Д','Е','Ж','З','И',
                    'Й','К','Л','М','Н','О','П','Р','С',
                    'Т','У','Ф','Х','Ц','Ч','Ш','Щ','Ъ',
                    'Ы','Ь','Э','Ю','Я' };
        for (;;)
        {
            char letter;
            if (ost != count) cout << "Осталось " << ost << "  попыток" << endl << endl;
            do
            {
                cout << "Выберите букву " << endl;
                for (int i = 0; i < 32; i++)
                {
                    cout << ABC[i] << " ";
                    if (i % 4 == 0) cout << endl;
                }
                cout << endl << "Ваша буква  ";
                cin >> letter;
            } while (Correct_letter(letter) == 0);
            timer = (double)(clock() - begin) / CLOCKS_PER_SEC;
            letters.push_back(letter);
            if (Find_letter(letter)==false) ost--;//если не отгадали- попытки уменьшаются
            cout << endl;
            for (int i = 0; i < pos.size(); i++)
            {
                cout << pos[i] << " ";
            }
            cout << endl;

            if (Win(pos) == 1)
            {
                cout << "Вы угадали слово!" << endl;
                cout << "Вы угадали слово с " << letters.size() << " попытки" << endl;
                break;
            }
            if (ost == 0)
            {
                cout << " Попытки кончились, вы не отгадали слово( ";
                Statistic();
                cout << "На отгадывание слова вы потратили " << timer << " секунд" << endl;
                break;
            }
        }
    };

    ~Hangman()
    {
        word.clear();//чистим вектор-слово, чтобы загадать новое
        vector<char>().swap(word);
        letters.clear();//чистим вектор-буквы
        vector<char>().swap(letters);
        pos.clear();//чистим вектор-позицию
        vector<char>().swap(pos);
        done.clear();
        vector<bool>().swap(done);
    }
};
int main()
{
    setlocale(LC_ALL, "Rus");
    SetConsoleOutputCP(1251);//функция для корректного отображения русских символов
    SetConsoleCP(1251);
    srand(time(NULL));
    Hangman a;
}
// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
