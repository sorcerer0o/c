
int romanToInt(char* s) {
    int map[256] = {0};
    map['I'] = 1;
    map['V'] = 5;
    map['X'] = 10;
    map['L'] = 50;
    map['C'] = 100;
    map['D'] = 500;
    map['M'] = 1000;

    int res = 0;
    int prev = 0;
    char *p = s;
    while (*p) {
        p++;
    }
    p--;
    while (p >= s) {
        int curr = map[*p];
        if (curr < prev) {
            res -= curr;
        } else {
            res += curr;
        }
        prev = curr;
        p--;
    }
    return res;
}
