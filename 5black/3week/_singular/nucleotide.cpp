#include <cstddef>
#include <cstdint>


struct Nucleotide {
  char Symbol;
  size_t Position;
  int ChromosomeNum;
  int GeneNum;
  bool IsMarked;
  char ServiceInfo;
};


struct CompactNucleotide {
  uint64_t Symbol:2;
  uint64_t Position:32;
  uint64_t ChromosomeNum:6;
  uint64_t GeneNum:15;
  uint64_t IsMarked:1;
  uint64_t ServiceInfo:8;
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
  return CompactNucleotide{
    .Symbol = (n.Symbol == 'C' ? 0u : n.Symbol == 'T' ? 1u : n.Symbol == 'A' ? 2u : 3u),
    .Position = n.Position,
    .ChromosomeNum = static_cast<uint64_t>(n.ChromosomeNum),
    .GeneNum = static_cast<uint64_t>(n.GeneNum),
    .IsMarked = n.IsMarked,
    .ServiceInfo = static_cast<uint64_t>(n.ServiceInfo)
  };
};


Nucleotide Decompress(const CompactNucleotide& cn) {
  return Nucleotide{
    .Symbol = (cn.Symbol == 0u ? 'C' : cn.Symbol == 1u ? 'T' : cn.Symbol == 2u ? 'A' : 'G'),
    .Position = cn.Position,
    .ChromosomeNum = static_cast<int>(cn.ChromosomeNum),
    .GeneNum = static_cast<int>(cn.GeneNum),
    .IsMarked = cn.IsMarked == 1 ? true : false,
    .ServiceInfo = static_cast<char>(cn.ServiceInfo)
  };
}
