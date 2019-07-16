string getNodeUrlParam(int argc, char *argv[]) {
    string url = "";
    if (argc >= 2) {
        url = argv[1];
    }
    return url;
}

string getBlockHashParam(int argc, char *argv[]) {
    string bh = "0xdcd1346701ca8396496e52aa2785b1748deb6db09551b72159dcb3e08991025b";
    if (argc >= 3) {
        bh = argv[2];
    }
    return bh;
}
