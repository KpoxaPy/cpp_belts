#pragma once

#include <cstdlib>
#include <deque>
#include <vector>

template <typename It>
class Range {
public:
  using ValueType = typename std::iterator_traits<It>::value_type;

  Range(It begin, It end) : begin_(begin), end_(end) {}
  It begin() const { return begin_; }
  It end() const { return end_; }

private:
  It begin_;
  It end_;
};

namespace Graph {

  using VertexId = size_t;
  using EdgeId = size_t;

  template <typename Weight, typename Extra>
  struct Edge {
    VertexId from;
    VertexId to;
    Weight weight;
    Extra extra;
  };

  template <typename Weight, typename Extra>
  class DirectedWeightedGraph {
  private:
    using IncidenceList = std::vector<EdgeId>;
    using IncidentEdgesRange = Range<typename IncidenceList::const_iterator>;
    using Edge = Edge<Weight, Extra>;

  public:
    DirectedWeightedGraph(size_t vertex_count);
    EdgeId AddEdge(const Edge& edge);

    size_t GetVertexCount() const;
    size_t GetEdgeCount() const;
    const Edge& GetEdge(EdgeId edge_id) const;
    IncidentEdgesRange GetIncidentEdges(VertexId vertex) const;

  private:
    std::vector<Edge> edges_;
    std::vector<IncidenceList> incidence_lists_;
  };


  template <typename Weight, typename Extra>
  DirectedWeightedGraph<Weight, Extra>::DirectedWeightedGraph(size_t vertex_count) : incidence_lists_(vertex_count) {}

  template <typename Weight, typename Extra>
  EdgeId DirectedWeightedGraph<Weight, Extra>::AddEdge(const Edge& edge) {
    edges_.push_back(edge);
    const EdgeId id = edges_.size() - 1;
    incidence_lists_[edge.from].push_back(id);
    return id;
  }

  template <typename Weight, typename Extra>
  size_t DirectedWeightedGraph<Weight, Extra>::GetVertexCount() const {
    return incidence_lists_.size();
  }

  template <typename Weight, typename Extra>
  size_t DirectedWeightedGraph<Weight, Extra>::GetEdgeCount() const {
    return edges_.size();
  }

  template <typename Weight, typename Extra>
  const Edge<Weight, Extra>& DirectedWeightedGraph<Weight, Extra>::GetEdge(EdgeId edge_id) const {
    return edges_[edge_id];
  }

  template <typename Weight, typename Extra>
  typename DirectedWeightedGraph<Weight, Extra>::IncidentEdgesRange
  DirectedWeightedGraph<Weight, Extra>::GetIncidentEdges(VertexId vertex) const {
    const auto& edges = incidence_lists_[vertex];
    return {std::begin(edges), std::end(edges)};
  }
}
