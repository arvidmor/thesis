
int hexval(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return 0;
}

int ishexdigit(char c) {
    return 
        (c >= '0' && c <= '9') || 
        (c >= 'a' && c <= 'f') || 
        (c >= 'A' && c <= 'F');
}

int axtoi(const char *num) {
    long value = 0;
    while (*num && ishexdigit(*num))
        value = value * 16 + hexval(*num++);
    return value;
}
