struct Era {
    long long eraProgress;
    long long eraLength;
};

struct SessionOrEpoch {
    bool isEpoch;

    long long sessionLength;
    long long sessionProgress;

    long long epochProgress;
    long long epochLength;
};
