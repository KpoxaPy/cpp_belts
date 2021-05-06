#pragma once

#include <cstdlib>
#include <deque>
#include <vector>

#include "transport_catalog.pb.h"

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
    DirectedWeightedGraph(const SpravSerialize::Graph& m);

    EdgeId AddEdge(const Edge& edge);

    size_t GetVertexCount() const;
    size_t GetEdgeCount() const;
    const Edge& GetEdge(EdgeId edge_id) const;
    IncidentEdgesRange GetIncidentEdges(VertexId vertex) const;

    void Serialize(SpravSerialize::Graph& m);

  private:
    std::vector<Edge> edges_;
    std::vector<IncidenceList> incidence_lists_;

    void ParseFrom(const SpravSerialize::Graph& m);
  };


  template <typename Weight, typename Extra>
  DirectedWeightedGraph<Weight, Extra>::DirectedWeightedGraph(size_t vertex_count)
    : incidence_lists_(vertex_count)
  {}

  template <typename Weight, typename Extra>
  DirectedWeightedGraph<Weight, Extra>::DirectedWeightedGraph(const SpravSerialize::Graph& m) {
    ParseFrom(m);
  }

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

  template <typename Weight, typename Extra>
  void DirectedWeightedGraph<Weight, Extra>::Serialize(SpravSerialize::Graph& m) {
    for (const auto& edge : edges_) {
      auto& m_edge = *m.add_edge();
      m_edge.set_from(edge.from);
      m_edge.set_to(edge.to);
      m_edge.set_weight(edge.weight);
      edge.extra.Serialize(*m_edge.mutable_extra());
    }
    m.set_vertex_count(incidence_lists_.size());
  }

  template <typename Weight, typename Extra>
  void DirectedWeightedGraph<Weight, Extra>::ParseFrom(const SpravSerialize::Graph& m) {
    edges_.reserve(m.edge().size());
    incidence_lists_.resize(m.vertex_count());
    for (const auto& m_edge : m.edge()) {
      Edge edge;
      edge.from = m_edge.from();
      edge.to = m_edge.to();
      edge.weight = m_edge.weight();
      edge.extra = Extra::ParseFrom(m_edge.extra());
      edges_.push_back(std::move(edge));
      incidence_lists_[edge.from].push_back(edges_.size() - 1);
    }
  }
}
