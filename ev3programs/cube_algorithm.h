#ifndef CUBE_ALGORITHM_H
#define CUBE_ALGORITHM_H

#include <string>

using namespace std;

class cube_algorithm {
    public:
        cube_algorithm();
        ~cube_algorithm() {};
        string solve();
        cube_algorithm& set_input(string input);
        cube_algorithm& set_input(char* buf);
        cube_algorithm& set_input(int argc, char** argv);
        cube_algorithm& set_mode_o(int mode);
        static const string err_prefix;
    private:
        string compactstep(string movestep);
        void rollorspin(int step, const int ord[], int faces_mc[], int faces_cm[]);
        string tosrt(int mi, int step, int faces_mc[], int faces_cm[]);
        void cycle(char* p, const char* a);
        void twist(int i, int a);
        int permtonum(char* p);
        int getposition(int t);
        void domove(int m);
        int searchphase(int movesleft, int movesdone, int lastmove);

        int _mode_o;
        string _output;

        static const int CHAROFFSET = 65;
        static const char* order;
        static const char* bithash;
        static const char* perm;
        static const char val[20];
        static const int imap[48];
        static const int order_r[4];
        static const int order_s[4];
        static const char table0[];
        static const char table1[];
        static const char table2[];
        static const char table3[];
        static const char table4[];
        static const char table5[];
        static const char table6[];
        static const char table7[];
        static const int tablesize[8];
        const char* tables[8];

        char pos[20];
        char ori[20];
        int move[20];
        int moveamount[20];
        int phase;
};

#endif
