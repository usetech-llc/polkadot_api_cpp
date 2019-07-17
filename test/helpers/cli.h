string getNodeUrlParam(int argc, char *argv[]) {
    string url = "";
    if (argc >= 2) {
        url = argv[1];
    }
    return url;
}

string getBlockHashParam(int argc, char *argv[]) {
    string bh = "";
    if (argc >= 3) {
        bh = argv[2];
    }
    return bh;
}
