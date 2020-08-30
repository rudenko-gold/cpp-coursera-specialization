#include "test_runner.h"

#include <cstddef>

struct Nucleotide {
    char Symbol;
    size_t Position;
    int ChromosomeNum;
    int GeneNum;
    bool IsMarked;
    char ServiceInfo;
};


struct CompactNucleotide {
    uint32_t Position; // 0...3 300 000 000 [4 bites]
    char ServiceInfo;
    uint8_t ChromosomeNum:6;
    uint8_t Symbol:2;
    uint16_t GeneNum:15;
    uint8_t IsMarked:1;
};

bool operator == (const Nucleotide& lhs, const Nucleotide& rhs) {
    return (lhs.Symbol == rhs.Symbol)
           && (lhs.Position == rhs.Position)
           && (lhs.ChromosomeNum == rhs.ChromosomeNum)
           && (lhs.GeneNum == rhs.GeneNum)
           && (lhs.IsMarked == rhs.IsMarked)
           && (lhs.ServiceInfo == rhs.ServiceInfo);
}


CompactNucleotide Compress(const Nucleotide& n) {
    CompactNucleotide result;
    result.Position = n.Position;
    result.GeneNum = n.GeneNum;
    result.ServiceInfo = n.ServiceInfo;
    result.ChromosomeNum = n.ChromosomeNum;

    if (n.IsMarked) {
        result.IsMarked = 1u;
    } else {
        result.IsMarked = 0u;
    }

    uint8_t symbol;
    if (n.Symbol == 'A') {
        symbol = 0u;
    } else if (n.Symbol == 'T') {
        symbol = 1u;
    } else if (n.Symbol == 'G') {
        symbol = 2u;
    } else if (n.Symbol == 'C') {
        symbol = 3u;
    }

    result.Symbol = symbol;

    return result;
};


Nucleotide Decompress(const CompactNucleotide& cn) {
    Nucleotide result;
    result.Position = cn.Position;
    result.GeneNum = cn.GeneNum;
    result.IsMarked = cn.IsMarked;

    result.ServiceInfo =  cn.ServiceInfo;
    unsigned short symbol_mask = cn.Symbol;

    if (symbol_mask == 0) {
        result.Symbol = 'A';
    } else if (symbol_mask  == 1) {
        result.Symbol = 'T';
    } else if (symbol_mask == 2) {
        result.Symbol = 'G';
    } else if (symbol_mask == 3) {
        result.Symbol = 'C';
    }

    result.ChromosomeNum = cn.ChromosomeNum;

    return result;
}

void TestSize() {
    ASSERT(sizeof(CompactNucleotide) <= 8);
}

void TestCompressDecompress() {
    Nucleotide source;

    source.Symbol = 'T';
    source.Position = 3'300'000'000;
    source.ChromosomeNum = 46;
    source.GeneNum = 25'000;
    source.IsMarked = true;
    source.ServiceInfo = 'C';

    CompactNucleotide compressedSource = Compress(source);
    Nucleotide decompressedSource = Decompress(compressedSource);
    ASSERT(source == decompressedSource);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestSize);
    RUN_TEST(tr, TestCompressDecompress);

    return 0;
}
