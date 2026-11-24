#define NOMINMAX
#include <windows.h>
#include <iostream>
#include <random>
#include <chrono>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
using namespace chrono;

int isLower(char c) { return c >= 'a' && c <= 'z'; }
int isUpper(char c) { return c >= 'A' && c <= 'Z'; }
int isDigit(char c) { return c >= '0' && c <= '9'; }
int isLetter(char c) { return isLower(c) || isUpper(c); }
int isPunct(char c) {
    return c == '.' || c == ',' || c == '!' || c == '?' || c == ';' || c == ':' || c == '-';
}
char toUpperCh(char c) { return isLower(c) ? (char)(c - 'a' + 'A') : c; }
char toLowerCh(char c) { return isUpper(c) ? (char)(c - 'A' + 'a') : c; }

void printWords(const vector<string>& w) {
    for (int i = 0; i < (int)w.size(); ++i) {
        cout << w[i];
        if (i + 1 < (int)w.size()) cout << " ";
    }
    cout << endl;
}

int lessWord(const string& a, const string& b) {
    int i = 0;
    while (i < (int)a.length() && i < (int)b.length()) {
        char x = toLowerCh(a[i]);
        char y = toLowerCh(b[i]);
        if (x != y) return x < y;
        ++i;
    }
    return a.length() < b.length();
}

int sameWord(const string& a, const string& b) {
    if (a.length() != b.length()) return 0;
    for (int i = 0; i < (int)a.length(); ++i)
        if (a[i] != b[i]) return 0;
    return 1;
}

// Задание 1
string checkRawInput(const string& s) {
    int end = (int)s.length() - 1;
    while (end >= 0 && s[end] == ' ') --end;
    if (end < 0) return "Пустая строка.";
    if (s[end] != '.') return "После последнего слова должна стоять точка.";
    for (int i = 0; i <= end; ++i) {
        char c = s[i];
        if (!(isLower(c) || isDigit(c) || c == ' ' || isPunct(c)))
            return "Недопустимый символ (разрешены только строчные латинские буквы, цифры, пробелы и знаки препинания).";
    }
    return "";
}

string fixCase(const string& w) {
    string res = "";
    for (int i = 0; i < (int)w.length(); ++i) {
        if (i == 0) res += toUpperCh(w[i]);
        else res += toLowerCh(w[i]);
    }
    return res;
}

// Задание 2
string cleanText(const string& s) {
    string res = "";
    int n = s.length();
    int needSpace = 0;  
    int i = 0;
    while (i < n) {
        char c = s[i];
        if (c == ' ') {
            if (res.length() > 0) needSpace = 1;
            ++i;
        } else if (isPunct(c)) {
            int j = i, cnt = 0;
            int allDots = 1;
            while (j < n && (isPunct(s[j]) || s[j] == ' ')) {
                if (isPunct(s[j])) {
                    ++cnt;
                    if (s[j] != '.') allDots = 0;
                }
                ++j;
            }

            if (cnt == 3 && allDots) res += "...";   
            else if (j >= n) res += '.';            
            else res += c;                           
            needSpace = 1;                          
            i = j;
        } else {
            if (needSpace) {
                res += ' ';
                needSpace = 0;
            }
            res += c;
            ++i;
        }
    }
    return res;
}

string fixTextCase(const string& s) {
    string res = "";
    string cur = "";
    for (int i = 0; i < (int)s.length(); ++i) {
        char c = s[i];
        if (c == ' ' || isPunct(c)) {
            if (cur.length() > 0) {
                res += fixCase(cur);
                cur = "";
            }
            res += c;
        } else {
            cur += c;
        }
    }
    if (cur.length() > 0) res += fixCase(cur);
    return res;
}

vector<string> splitWords(const string& s) {
    vector<string> words;
    string cur = "";
    for (int i = 0; i < (int)s.length(); ++i) {
        char c = s[i];
        if (c == ' ' || isPunct(c)) {
            if (cur.length() > 0) {
                words.push_back(cur);
                cur = "";
            }
        } else {
            cur += c;
        }
    }
    if (cur.length() > 0) words.push_back(cur);
    return words;
}

int prepare(const string& input, string& text, vector<string>& words, string& err) {
    err = checkRawInput(input);
    if (err.length() > 0) return 0;

    string cleaned = cleanText(input);   
    text = fixTextCase(cleaned);         
    words = splitWords(text);            

    if (words.size() < 1 || words.size() > 50) {
        err = "Количество слов должно быть от 1 до 50.";
        return 0;
    }
    for (int i = 0; i < (int)words.size(); ++i) {
        if (words[i].length() < 1 || words[i].length() > 10) {
            err = "Длина каждого слова должна быть от 1 до 10 символов (слово \"" + words[i] + "\").";
            return 0;
        }
    }
    return 1;
}

//Задание 5 
vector<int> naiveSearch(const string& t, const string& p) {
    vector<int> res;
    int n = t.length(), m = p.length();
    for (int i = 0; i + m <= n; ++i) {
        int j = 0;
        while (j < m && t[i + j] == p[j]) ++j;
        if (j == m) res.push_back(i);
    }
    return res;
}

vector<int> kmpSearch(const string& t, const string& p) {
    vector<int> res;
    int n = t.length(), m = p.length();
    vector<int> pi(m, 0);
    for (int i = 1; i < m; ++i) {
        int k = pi[i - 1];
        while (k > 0 && p[i] != p[k]) k = pi[k - 1];
        if (p[i] == p[k]) ++k;
        pi[i] = k;
    }
    int k = 0;
    for (int i = 0; i < n; ++i) {
        while (k > 0 && t[i] != p[k]) k = pi[k - 1];
        if (t[i] == p[k]) ++k;
        if (k == m) {
            res.push_back(i - m + 1);
            k = pi[k - 1];
        }
    }
    return res;
}

vector<int> bmSearch(const string& t, const string& p) {
    vector<int> res;
    int n = t.length(), m = p.length();

    int bad[256];
    for (int i = 0; i < 256; ++i) bad[i] = -1;
    for (int i = 0; i < m; ++i) bad[(unsigned char)p[i]] = i;

    vector<int> shift(m + 1, 0), bpos(m + 1, 0);
    int i = m, j = m + 1;
    bpos[i] = j;
    while (i > 0) {
        while (j <= m && p[i - 1] != p[j - 1]) {
            if (shift[j] == 0) shift[j] = j - i;
            j = bpos[j];
        }
        --i;
        --j;
        bpos[i] = j;
    }
    j = bpos[0];
    for (i = 0; i <= m; ++i) {
        if (shift[i] == 0) shift[i] = j;
        if (i == j) j = bpos[j];
    }

    int s = 0;
    while (s <= n - m) {
        int k = m - 1;
        while (k >= 0 && p[k] == t[s + k]) --k;
        if (k < 0) {
            res.push_back(s);
            s += shift[0];
        } else {
            int a = shift[k + 1];
            int b = k - bad[(unsigned char)t[s + k]];
            s += (a > b) ? a : b;
        }
    }
    return res;
}

double measure(vector<int> (*f)(const string&, const string&),
               const string& t, const string& p, int iters) {
    long long total = 0;
    auto start = steady_clock::now();
    for (int i = 0; i < iters; ++i) {
        vector<int> r = f(t, p);
        total += r.size(); 
    }
    auto end = steady_clock::now();
    double us = duration_cast<nanoseconds>(end - start).count() / 1000.0;
    if (total < 0) cout << total;
    return us / iters;
}

void printPositions(const string& name, const vector<int>& r) {
    cout << name << ": ";
    if (r.size() == 0) cout << "не найдено";
    for (int i = 0; i < (int)r.size(); ++i) cout << r[i] << " ";
    cout << endl;
}

void benchRow(const string& label, const string& t, const string& p) {
    const int ITER = 1000;
    double t1 = measure(naiveSearch, t, p, ITER);
    double t2 = measure(kmpSearch, t, p, ITER);
    double t3 = measure(bmSearch, t, p, ITER);
    int same = (naiveSearch(t, p) == kmpSearch(t, p)) && (kmpSearch(t, p) == bmSearch(t, p));

    string best = "Линейный";
    double bt = t1;
    if (t2 < bt) { bt = t2; best = "КМП"; }
    if (t3 < bt) { bt = t3; best = "Бойер-Мур"; }

    cout << label << "\t" << t.length() << "\t" << p.length() << "\t"
         << t1 << "\t\t" << t2 << "\t\t" << t3 << "\t\t"
         << (same ? "да" : "НЕТ") << "\t" << best << endl;
}

string genText(int len) {
    string s = "";
    unsigned int x = 12345;
    for (int i = 0; i < len; ++i) {
        x = x * 1103515245u + 12345u;
        s += (char)('a' + (x >> 16) % 4);
    }
    return s;
}

// Задание 2
void showText(const string& text) {
    cout << "\nОтредактированный текст:\n" << text << endl;
}

// Задание 3.1
void reverseWords(const vector<string>& words) {
    cout << "\n--- Задание 3.1: обратный порядок ---" << endl;
    for (int i = (int)words.size() - 1; i >= 0; --i) cout << words[i] << " ";
    cout << endl;
}

// Задание 3.2
void sortWords(const vector<string>& words) {
    cout << "\n--- Задание 3.2: алфавитный порядок ---" << endl;
    vector<string> sorted = words;
    int n = sorted.size();
    for (int i = 0; i < n - 1; ++i) {           
        for (int j = 0; j < n - 1 - i; ++j) {
            if (lessWord(sorted[j + 1], sorted[j])) {
                string tmp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = tmp;
            }
        }
    }
    printWords(sorted);
}

// Задание 3.3
void wordsWithoutDigits(const vector<string>& words) {
    cout << "\n--- Задание 3.3: слова без цифр ---" << endl;
    for (int i = 0; i < (int)words.size(); ++i) {
        int hasDigit = 0;
        for (int j = 0; j < (int)words[i].length(); ++j)
            if (isDigit(words[i][j])) hasDigit = 1;
        if (!hasDigit) cout << words[i] << " ";
    }
    cout << endl;
}

// Задание 3.4
void wordsWithRepeatedLetters(const vector<string>& words) {
    cout << "\n--- Задание 3.4: слова с одинаковыми буквами ---" << endl;
    for (int i = 0; i < (int)words.size(); ++i) {
        int found = 0;
        int len = words[i].length();
        for (int a = 0; a < len; ++a) {
            for (int b = a + 1; b < len; ++b) {
                if (isLetter(words[i][a]) && toLowerCh(words[i][a]) == toLowerCh(words[i][b]))
                    found = 1;
            }
        }
        if (found) cout << words[i] << " ";
    }
    cout << endl;
}

// Задание 3.5
void wordsFirstLetterRepeats(const vector<string>& words) {
    cout << "\n--- Задание 3.5: первая буква встречается ещё раз ---" << endl;
    for (int i = 0; i < (int)words.size(); ++i) {
        int found = 0;
        if (isLetter(words[i][0])) {
            for (int j = 1; j < (int)words[i].length(); ++j)
                if (toLowerCh(words[i][0]) == toLowerCh(words[i][j])) found = 1;
        }
        if (found) cout << words[i] << " ";
    }
    cout << endl;
}

// Задание 3.6
void groupWords(const vector<string>& words) {
    cout << "\n--- Задание 3.6: сначала только буквы, затем только цифры, затем смешанные ---" << endl;
    vector<string> onlyL, onlyD, mixed;
    for (int i = 0; i < (int)words.size(); ++i) {
        int hasL = 0, hasD = 0;
        for (int j = 0; j < (int)words[i].length(); ++j) {
            if (isLetter(words[i][j])) hasL = 1;
            if (isDigit(words[i][j])) hasD = 1;
        }
        if (hasL && !hasD) onlyL.push_back(words[i]);
        else if (hasD && !hasL) onlyD.push_back(words[i]);
        else mixed.push_back(words[i]);
    }
    for (int i = 0; i < (int)onlyL.size(); ++i) cout << onlyL[i] << " ";
    for (int i = 0; i < (int)onlyD.size(); ++i) cout << onlyD[i] << " ";
    for (int i = 0; i < (int)mixed.size(); ++i) cout << mixed[i] << " ";
    cout << endl;
}

// Задание 4.1
void capitalizeFirst(const vector<string>& words) {
    cout << "\n--- Задание 4.1: первая буква - прописная ---" << endl;
    for (int i = 0; i < (int)words.size(); ++i) {
        string w = words[i];
        w[0] = toUpperCh(w[0]);
        cout << w << " ";
    }
    cout << endl;
}

// Задание 4.2
void wordLengths(const vector<string>& words) {
    cout << "\n--- Задание 4.2: количество символов в словах ---" << endl;
    for (int i = 0; i < (int)words.size(); ++i)
        cout << words[i] << " - " << words[i].length() << endl;
}

// Задание 4.3
void removeChars(const vector<string>& words) {
    cout << "\n--- Задание 4.3: удаление набора букв и (или) цифр ---" << endl;
    string del;
    cout << "Введите символы для удаления (без пробелов): ";
    cin >> del;
    for (int i = 0; i < (int)words.size(); ++i) {
        string w = "";
        for (int j = 0; j < (int)words[i].length(); ++j) {
            int remove = 0;
            for (int k = 0; k < (int)del.length(); ++k)
                if (words[i][j] == del[k]) remove = 1;
            if (!remove) w += words[i][j];
        }
        if (w.length() > 0) cout << w << " ";
    }
    cout << endl;
}

// Задание 4.4
void digitsToLetters(const vector<string>& words) {
    cout << "\n--- Задание 4.4: цифры -> буквы (1->a, ..., 9->i) ---" << endl;
    for (int i = 0; i < (int)words.size(); ++i) {
        string w = "";
        for (int j = 0; j < (int)words[i].length(); ++j) {
            char c = words[i][j];
            if (c >= '1' && c <= '9') w += (char)('a' + (c - '1'));
            else w += c;   
        }
        cout << w << " ";
    }
    cout << endl;
}

// Задание 4.5
void digitsToEnd(const vector<string>& words) {
    cout << "\n--- Задание 4.5: цифры в конец слова ---" << endl;
    for (int i = 0; i < (int)words.size(); ++i) {
        string letters = "", digits = "";
        for (int j = 0; j < (int)words[i].length(); ++j) {
            if (isDigit(words[i][j])) digits += words[i][j];
            else letters += words[i][j];
        }
        cout << letters + digits << " ";
    }
    cout << endl;
}

// Задание 4.6
void printVertical(const vector<string>& words) {
    cout << "\n--- Задание 4.6: вертикальный вывод ---" << endl;
    for (int i = 0; i < (int)words.size(); ++i) {
        for (int j = 0; j < (int)words[i].length(); ++j)
            cout << words[i][j] << endl;
        cout << endl;  
    }
}

// Задание 4.7
void printColumns(const vector<string>& words) {
    cout << "\n--- Задание 4.7: две или три колонки, выравнивание по правому краю ---" << endl;
    int n = words.size();
    int cols = (n <= 10) ? 2 : 3;   
    cout << "Слов: " << n << ", колонок: " << cols << endl;
    int rows = (n + cols - 1) / cols;
    int width[3] = {0, 0, 0};
    for (int i = 0; i < n; ++i) {
        int c = i % cols;
        if ((int)words[i].length() > width[c]) width[c] = words[i].length();
    }
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            int idx = r * cols + c;
            if (idx >= n) break;
            int pad = width[c] - words[idx].length();
            for (int k = 0; k < pad; ++k) cout << ' ';
            cout << words[idx] << "  ";
        }
        cout << endl;
    }
}

// Задание 5
void substringSearch(const string& text) {
    cout << "\n--- Задание 5: поиск подстроки ---" << endl;
    string pattern;
    cout << "Введите подстроку: ";
    cin >> pattern;

    vector<int> r1 = naiveSearch(text, pattern);
    vector<int> r2 = kmpSearch(text, pattern);
    vector<int> r3 = bmSearch(text, pattern);

    cout << "Позиции (нумерация с 0):" << endl;
    printPositions("Линейный поиск", r1);
    printPositions("КМП           ", r2);
    printPositions("Бойер-Мур     ", r3);
    if (r1 == r2 && r2 == r3) cout << "Результаты всех алгоритмов совпадают." << endl;
    else cout << "ОШИБКА: результаты различаются!" << endl;

    cout << "\nСравнение скорости (мкс, среднее по 1000 запусков):" << endl;
    cout << "Тест\tТекст\tОбразец\tЛинейный\tКМП\t\tБойер-Мур\tСовпали\tЛучший" << endl;
    benchRow("ваш", text, pattern);
    int lens[4][2] = { {1000, 5}, {10000, 10}, {100000, 10}, {100000, 50} };
    for (int k = 0; k < 4; ++k) {
        string t = genText(lens[k][0]);
        string p = "";
        int start = lens[k][0] / 2;
        for (int i = 0; i < lens[k][1]; ++i) p += t[start + i];
        benchRow("тест", t, p);
    }
    cout << "\nВывод: на коротких текстах разница между алгоритмами мала (накладные расходы"
         << "\nна подготовку таблиц КМП и Бойера-Мура сравнимы со временем самого поиска)."
         << "\nС ростом длины текста и образца КМП и Бойер-Мур обычно выигрывают у линейного"
         << "\nпоиска; конкретный лучший алгоритм для каждого теста указан в последнем столбце." << endl;
}

// Задание на выбор. Удаление дубликатов слов
void removeDuplicates(const vector<string>& words) {
    cout << "\n--- Задание 6: удаление слов, встречающихся более одного раза ---" << endl;
    vector<string> unique;
    for (int i = 0; i < (int)words.size(); ++i) {
        int seen = 0;
        for (int j = 0; j < (int)unique.size(); ++j)
            if (sameWord(unique[j], words[i])) seen = 1;
        if (!seen) unique.push_back(words[i]);
    }
    printWords(unique);
}

// Меню
void printMenu() {
    cout << "\n===== МЕНЮ =====\n"
         << "1. Отредактированный текст\n"
         << "2. Слова в обратном порядке\n"
         << "3. Слова по алфавиту\n"
         << "4. Слова без цифр\n"
         << "5. Слова с одинаковыми буквами\n"
         << "6. Слова, где первая буква встречается ещё раз\n"
         << "7. Сначала только буквы, затем только цифры, затем смешанные\n"
         << "8. Первая буква - прописная\n"
         << "9. Количество символов в каждом слове\n"
         << "10. Удалить заданные буквы/цифры\n"
         << "11. Цифры -> буквы (1->a, ..., 9->i)\n"
         << "12. Цифры в конец слов\n"
         << "13. Слова вертикально\n"
         << "14. Слова в колонки (выравнивание вправо)\n"
         << "15. Поиск подстроки (линейный, КМП, Бойер-Мур)\n"
         << "16. Удаление слов, встречающихся более одного раза\n"
         << "0. Выход\n"
         << "Выбор: ";
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    // Задание 1
    string input = "";
    string text = "";
    vector<string> words;
    string err;
    int mode;
    cout << "Ввод: 1 - с клавиатуры, 2 - из файла: ";
    cin >> mode;
    cin.ignore(10000, '\n');

    if (mode == 2) {
        string fname;
        cout << "Имя файла: ";
        getline(cin, fname);
        ifstream fin(fname.c_str());
        if (!fin) {
            cout << "Не удалось открыть файл" << endl;
            return 1;
        }
        string line;
        while (getline(fin, line)) {
            input += line;
            input += ' ';
        }
        // Задание 2 
        if (!prepare(input, text, words, err)) {
            cout << "Ошибка во входных данных: " << err << endl;
            return 1;
        }
    } else {
        int ok = 0;
        while (!ok) {
            cout << "Введите от 1 до 50 слов (слово - до 10 латинских букв и цифр, в конце точка):" << endl;
            getline(cin, input);
            if (!cin) return 1;
            ok = prepare(input, text, words, err);
            if (!ok) cout << "Ошибка: " << err << " Повторите ввод." << endl;
        }
    }

    // Меню
    while (cin) {
        printMenu();
        int choice;
        cin >> choice;
        if (!cin) break;
        switch (choice) {
            case 1:  showText(text); break;
            case 2:  reverseWords(words); break;
            case 3:  sortWords(words); break;
            case 4:  wordsWithoutDigits(words); break;
            case 5:  wordsWithRepeatedLetters(words); break;
            case 6:  wordsFirstLetterRepeats(words); break;
            case 7:  groupWords(words); break;
            case 8:  capitalizeFirst(words); break;
            case 9:  wordLengths(words); break;
            case 10: removeChars(words); break;
            case 11: digitsToLetters(words); break;
            case 12: digitsToEnd(words); break;
            case 13: printVertical(words); break;
            case 14: printColumns(words); break;
            case 15: substringSearch(text); break;
            case 16: removeDuplicates(words); break;
            case 0: return 0;
            default: cout << "Нет такого пункта" << endl;
        }
    }
    return 0;
}
